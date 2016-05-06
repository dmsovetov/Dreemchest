/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#include "Rvm.h"
#include "Commands.h"
#include "RenderFrame.h"
#include "Ubershader.h"
#include "RenderingContext.h"
#include "../../Components/Rendering.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// -------------------------------------------------------------------- Rvm::IntermediateTargetStack ------------------------------------------------------------------- //

/*!
    Intermediate target stack is used to convert local indices that are
    stored in commands to a global index of an intermediate render target.
*/
class Rvm::IntermediateTargetStack {
public:

    //! A maximum number of intermediate render targets that can be hold by a single stack frame.
    enum { StackFrameSize = 8 };

    //! A maximum number of stack frames that can be pushed during rendering.
    enum { MaxStackFrames = 8 };

    //! A total size of an intermediate stack size.
    enum { MaxStackSize = MaxStackFrames * StackFrameSize };

                                //! Constructs an IntermediateTargetStack instance.
                                IntermediateTargetStack( RenderingContextWPtr context );

    //! Pushes a new stack frame.
    void                        pushFrame( void );

    //! Pops an active stack frame.
    void                        popFrame( void );

    //! Returns a render target by a local index.
    Renderer::RenderTargetWPtr  get( u8 index ) const;

    //! Acquires an intermediate target with a specified parameters and loads it to a local slot.
    void                        acquire( u8 index, u16 width, u16 height, Renderer::PixelFormat format );

    //! Releases an intermediate target.
    void                        release( u8 index );

private:

    RenderingContextWPtr        m_context;                      //!< A parent rendering context.
    RenderResource*             m_stackFrame;                   //!< An active render target stack frame.
    RenderResource              m_identifiers[MaxStackSize];    //!< An array of intermediate render target handles.
};

// ** Rvm::IntermediateTargetStack::IntermediateTargetStack
Rvm::IntermediateTargetStack::IntermediateTargetStack( RenderingContextWPtr context )
    : m_context( context )
    , m_stackFrame( m_identifiers )
{
    memset( m_identifiers, 0, sizeof m_identifiers );
}

// ** Rvm::IntermediateTargetStack::pushFrame
void Rvm::IntermediateTargetStack::pushFrame( void )
{
    DC_ABORT_IF( (m_stackFrame + StackFrameSize) > (m_identifiers + MaxStackSize), "frame stack overflow" );
    m_stackFrame += StackFrameSize;
}

// ** Rvm::IntermediateTargetStack::popFrame
void Rvm::IntermediateTargetStack::popFrame( void )
{
    DC_ABORT_IF( m_stackFrame == m_identifiers, "stack underflow" );

    // Ensure that all render targets were released
    for( s32 i = 0; i < StackFrameSize; i++ ) {
        if( m_stackFrame[i] ) {
            LogWarning( "rvm", "an intermediate render target was not released before popping a stack frame\n" );
        }
    }

    // Pop a stack frame
    m_stackFrame -= StackFrameSize;
}

// ** Rvm::IntermediateTargetStack::get
Renderer::RenderTargetWPtr Rvm::IntermediateTargetStack::get( u8 index ) const
{
    DC_ABORT_IF( index == 0, "invalid render target index" );
    return m_context->intermediateRenderTarget( m_stackFrame[index - 1] );
}

// ** Rvm::IntermediateTargetStack::acquire
void Rvm::IntermediateTargetStack::acquire( u8 index, u16 width, u16 height, Renderer::PixelFormat format )
{
    DC_ABORT_IF( index == 0, "invalid render target index" );
    m_stackFrame[index - 1] = m_context->acquireRenderTarget( width, height, format );
}

// ** Rvm::IntermediateTargetStack::release
void Rvm::IntermediateTargetStack::release( u8 index )
{
    DC_ABORT_IF( index == 0, "invalid render target index" );
    m_context->releaseRenderTarget( m_stackFrame[index - 1] );
    m_stackFrame[index - 1] = 0;
}

// -------------------------------------------------------------------------------- Rvm -------------------------------------------------------------------------------- //

// ** Rvm::Rvm
Rvm::Rvm( RenderingContextWPtr context )
    : m_context( context )
    , m_hal( context->hal() )
    , m_intermediateTargets( DC_NEW IntermediateTargetStack( context ) )
{
    // Reset all state switchers
    memset( m_stateSwitches, 0, sizeof m_stateSwitches );

    // Setup render state switchers
    m_stateSwitches[RenderState::AlphaTest]         = &Rvm::switchAlphaTest;
    m_stateSwitches[RenderState::DepthState]        = &Rvm::switchDepthState;
    m_stateSwitches[RenderState::Blending]          = &Rvm::switchBlending;
    m_stateSwitches[RenderState::Shader]            = &Rvm::switchShader;
    m_stateSwitches[RenderState::ConstantBuffer]    = &Rvm::switchConstantBuffer;
    m_stateSwitches[RenderState::VertexBuffer]      = &Rvm::switchVertexBuffer;
    m_stateSwitches[RenderState::IndexBuffer]       = &Rvm::switchIndexBuffer;
    m_stateSwitches[RenderState::InputLayout]       = &Rvm::switchInputLayout;
    m_stateSwitches[RenderState::Texture]           = &Rvm::switchTexture;
    m_stateSwitches[RenderState::CullFace]          = &Rvm::switchCullFace;
}

// ** Rvm::create
RvmPtr Rvm::create( RenderingContextWPtr context )
{
    return DC_NEW Rvm( context );
}

// ** Rvm::display
void Rvm::display( const RenderFrameUPtr& frame )
{
    // Construct all resources before rendering a frame
    m_context->constructResources();

    // Execute an entry point command buffer
    execute( *frame.get(), frame->entryPoint() );

    // Reset rendering states
    reset();
}

// ** Rvm::renderToTarget
void Rvm::renderToTarget( const RenderFrame& frame, u8 renderTarget, const u32* viewport, const RenderCommandBuffer& commands )
{
    // Push a render target state
    if( renderTarget ) {
        const Renderer::RenderTargetWPtr rt = m_intermediateTargets->get( renderTarget );
        m_hal->setRenderTarget( rt );
        m_hal->setViewport( viewport[0] * rt->width(), viewport[1] * rt->height(), viewport[2] * rt->width(), viewport[3] * rt->height() );
    } else {
        m_hal->setViewport( viewport[0], viewport[1], viewport[2], viewport[3] );
    }

    // Set a viewport before executing an attached command buffer
    m_viewportStack.push( viewport );

    // Execute an attached command buffer
    execute( frame, commands );

    // Pop a render target state
    if( renderTarget ) {
        m_hal->setRenderTarget( NULL );
    }

    // Pop a viewport
    m_viewportStack.pop();

    if( m_viewportStack.size() ) {
        const u32* prev = m_viewportStack.top();
        m_hal->setViewport( prev[0], prev[1], prev[2], prev[3] );
    }
}

// ** Rvm::execute
void Rvm::execute( const RenderFrame& frame, const RenderCommandBuffer& commands )
{
    // Push a new frame to an intermediate target stack
    m_intermediateTargets->pushFrame();

    // Execute all commands inside a buffer
    for( s32 i = 0, n = commands.size(); i < n; i++ ) {
        // Get a render operation at specified index
        const RenderCommandBuffer::OpCode& opCode = commands.opCodeAt( i );

        // Perform a draw call
        switch( opCode.type ) {
        case RenderCommandBuffer::OpCode::Clear:                clear( opCode.clear.color, opCode.clear.depth, opCode.clear.stencil, opCode.clear.mask );
                                                                break;
        case RenderCommandBuffer::OpCode::Execute:              execute( frame, *opCode.execute.commands );
                                                                break;
        case RenderCommandBuffer::OpCode::UploadConstantBuffer: uploadConstantBuffer( opCode.upload.id, opCode.upload.data, opCode.upload.size );
                                                                break;
        case RenderCommandBuffer::OpCode::RenderTarget:         renderToTarget( frame, opCode.renderTarget.index, opCode.renderTarget.viewport, *opCode.renderTarget.commands );
                                                                break;
        case RenderCommandBuffer::OpCode::AcquireRenderTarget:  m_intermediateTargets->acquire( opCode.intermediateRenderTarget.index, opCode.intermediateRenderTarget.width, opCode.intermediateRenderTarget.height, opCode.intermediateRenderTarget.format );
                                                                break;
        case RenderCommandBuffer::OpCode::ReleaseRenderTarget:  m_intermediateTargets->release( opCode.intermediateRenderTarget.index );
                                                                break;
        case RenderCommandBuffer::OpCode::DrawIndexed:          {
                                                                    // Apply rendering states from a stack
                                                                    applyStates( frame, opCode.drawCall.states, MaxStateStackDepth );

                                                                    // Perform an actual draw call
                                                                    m_hal->renderIndexed( opCode.drawCall.primitives, opCode.drawCall.first, opCode.drawCall.count );
                                                                }
                                                                break;
        case RenderCommandBuffer::OpCode::DrawPrimitives:       {
                                                                    // Apply rendering states from a stack
                                                                    applyStates( frame, opCode.drawCall.states, MaxStateStackDepth );

                                                                    // Perform an actual draw call
                                                                    m_hal->renderPrimitives( opCode.drawCall.primitives, opCode.drawCall.first, opCode.drawCall.count );
                                                                }
                                                                break;
        default:                                                DC_NOT_IMPLEMENTED;
        }
    }

    // Pop a stack frame
    m_intermediateTargets->popFrame();
}

// ** Rvm::reset
void Rvm::reset( void )
{
	// Reset the face culling
	m_hal->setCulling( Renderer::TriangleFaceBack );

	// Set the default polygon mode
	m_hal->setPolygonMode( Renderer::PolygonFill );

	// Set the default shader
	m_hal->setShader( NULL );

	// Set the default vertex buffer
	m_hal->setVertexBuffer( NULL );

	// Set default textures
	for( s32 i = 0; i < 8; i++ ) {
		m_hal->setTexture( i, NULL );
	}

	// Enable the depth test back
	m_hal->setDepthTest( true, Renderer::LessEqual );

    // Disable the alpha test
    m_hal->setAlphaTest( Renderer::CompareDisabled );
}

// ** Rvm::clear
void Rvm::clear( const f32* color, f32 depth, s32 stencil, u8 mask )
{
    m_hal->clear( Rgba( color ), depth,stencil, mask );
}

// ** Rvm::uploadConstantBuffer
void Rvm::uploadConstantBuffer( u32 id, const void* data, s32 size )
{
    Renderer::ConstantBufferPtr constantBuffer = m_context->constantBuffer( id );
    memcpy( constantBuffer->lock(), data, size );
    constantBuffer->unlock();
}

// ** Rvm::applyStates
void Rvm::applyStates( const RenderFrame& frame, const RenderStateBlock* const * states, s32 count )
{
    u64 userFeatures = 0;
    u64 userFeaturesMask = ~0;

    // Reset all ubershader features
    m_vertexAttributeFeatures = 0;
    m_resourceFeatures        = 0;

    // A bitmask of states that were already set
    u32 activeStateMask = 0;

    for( s32 i = 0; i < count; i++ ) {
        // Get a state block at specified index
        const RenderStateBlock* block = states[i];

        // No more state blocks in a stack - break
        if( block == NULL ) {
            break;
        }

        // Update feature set
        userFeatures      = userFeatures     | block->features();
        userFeaturesMask  = userFeaturesMask & block->featureMask();

        // Skip redundant state blocks by testing a block bitmask against an active state mask
        if( (activeStateMask ^ block->mask()) == 0 ) {
            continue;
        }

        // Apply all states in a block
        for( s32 j = 0, n = block->stateCount(); j < n; j++ ) {
            // Get a render state bit
            u32 stateBit = block->stateBit( j );

            // Skip redundate state blocks by testing a state bitmask agains an active state mask
            if( activeStateMask & stateBit ) {
                continue;
            }

            // Get a render state at specified index
            const RenderState& state = block->state( j );

            // Update an active state mask
            activeStateMask = activeStateMask | stateBit;

            // Finally apply a state
            DC_ABORT_IF( m_stateSwitches[state.type] == NULL, "unhandled render state type" );
            (this->*m_stateSwitches[state.type])( frame, state );
        }
    }

    // Finally apply a shader
    DC_ABORT_IF( !m_activeShader.shader.valid(), "no valid shader set" );

    // Select a shader permutation that match an active pipeline state
    Ubershader::Bitmask supported   = m_activeShader.shader->supportedFeatures();
    Ubershader::Bitmask userDefined = (userFeatures & userFeaturesMask) << UserDefinedFeaturesOffset;
    Ubershader::Bitmask features    = (m_vertexAttributeFeatures | m_resourceFeatures | userDefined) & supported;

    if( m_activeShader.activeShader != m_activeShader.shader || m_activeShader.features != features ) {
        m_activeShader.permutation  = m_activeShader.shader->permutation( m_hal, features );
        m_activeShader.features     = features;
        m_activeShader.activeShader = m_activeShader.shader;
    }

    // Bind an active shader permutation
    m_hal->setShader( m_activeShader.permutation );
}

// ** Rvm::switchAlphaTest
void Rvm::switchAlphaTest( const RenderFrame& frame, const RenderState& state )
{
    m_hal->setAlphaTest( static_cast<Renderer::Compare>( state.compareFunction ), state.data.alphaReference / 255.0f );
}

// ** Rvm::switchDepthState
void Rvm::switchDepthState( const RenderFrame& frame, const RenderState& state )
{
    m_hal->setDepthTest( state.data.depthWrite, static_cast<Renderer::Compare>( state.compareFunction ) );
}

// ** Rvm::switchBlending
void Rvm::switchBlending( const RenderFrame& frame, const RenderState& state )
{
    m_hal->setBlendFactors( static_cast<Renderer::BlendFactor>( state.data.blend >> 4 ), static_cast<Renderer::BlendFactor>( state.data.blend & 0xF ) );
}

// ** Rvm::switchShader
void Rvm::switchShader( const RenderFrame& frame, const RenderState& state )
{
    m_activeShader.shader = m_context->shader( state.resourceId );
}

// ** Rvm::switchConstantBuffer
void Rvm::switchConstantBuffer( const RenderFrame& frame, const RenderState& state )
{
    const Renderer::ConstantBufferPtr& constantBuffer = m_context->constantBuffer( state.resourceId );
    m_hal->setConstantBuffer( constantBuffer, state.data.index );
}

// ** Rvm::switchVertexBuffer
void Rvm::switchVertexBuffer( const RenderFrame& frame, const RenderState& state )
{
    const Renderer::VertexBufferPtr& vertexBuffer = m_context->vertexBuffer( state.resourceId );
    m_hal->setVertexBuffer( vertexBuffer );
}

// ** Rvm::switchIndexBuffer
void Rvm::switchIndexBuffer( const RenderFrame& frame, const RenderState& state )
{
    const Renderer::IndexBufferPtr& indexBuffer = m_context->indexBuffer( state.resourceId );
    m_hal->setIndexBuffer( indexBuffer );
}

// ** Rvm::switchInputLayout
void Rvm::switchInputLayout( const RenderFrame& frame, const RenderState& state )
{
    // Bind an input layout
    const Renderer::InputLayoutPtr& inputLayout = m_context->inputLayout( state.resourceId );
    m_hal->setInputLayout( inputLayout );

    // Update an input layout features
    m_vertexAttributeFeatures = inputLayout->features();
}

// ** Rvm::switchTexture
void Rvm::switchTexture( const RenderFrame& frame, const RenderState& state )
{
    // A single u64 bit constant value
    static const u64 bit = 1;

    // Convert a resource id to a signed integer
    s32 id = static_cast<s16>( state.resourceId );

    // Bind a texture to sampler
    if( id >= 0 ) {
        const Renderer::TexturePtr& texture = m_context->texture( state.resourceId );
        m_hal->setTexture( state.data.index, texture.get() );
    } else {
        DC_BREAK_IF( abs( id ) > 255, "invalid identifier" );
        Renderer::Texture2DPtr texture = m_intermediateTargets->get( -id )->attachment( static_cast<Renderer::RenderTarget::Attachment>( state.data.index >> 4 ) );
        DC_BREAK_IF( !texture.valid(), "invalid render target attachment" );
        m_hal->setTexture( state.data.index & 0xF, texture.get() );
    }

    // Update resource features
    m_resourceFeatures = m_resourceFeatures | (bit << (state.data.index + SamplerFeaturesOffset));
}

// ** Rvm::switchCullFace
void Rvm::switchCullFace( const RenderFrame& frame, const RenderState& state )
{
    m_hal->setCulling( static_cast<Renderer::TriangleFace>( state.cullFace ) );
}

} // namespace Scene

DC_END_DREEMCHEST