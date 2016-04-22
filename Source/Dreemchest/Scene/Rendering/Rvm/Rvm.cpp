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

// ** Rvm::Rvm
Rvm::Rvm( RenderingContextWPtr context )
    : m_context( context )
    , m_hal( context->hal() )
{
    // Reset all state switchers
    memset( m_stateSwitches, 0, sizeof m_stateSwitches );

    // Setup render state switchers
    m_stateSwitches[RenderState::AlphaTest]         = &Rvm::switchAlphaTest;
    m_stateSwitches[RenderState::DepthState]        = &Rvm::switchDepthState;
    m_stateSwitches[RenderState::Blending]          = &Rvm::switchBlending;
    m_stateSwitches[RenderState::RenderTarget]      = &Rvm::switchRenderTarget;
    m_stateSwitches[RenderState::Shader]            = &Rvm::switchShader;
    m_stateSwitches[RenderState::ConstantBuffer]    = &Rvm::switchConstantBuffer;
    m_stateSwitches[RenderState::VertexBuffer]      = &Rvm::switchVertexBuffer;
    m_stateSwitches[RenderState::IndexBuffer]       = &Rvm::switchIndexBuffer;
    m_stateSwitches[RenderState::InputLayout]       = &Rvm::switchInputLayout;
}

// ** Rvm::create
RvmPtr Rvm::create( RenderingContextWPtr context )
{
    return DC_NEW Rvm( context );
}

// ** Rvm::display
void Rvm::display( const RenderFrameUPtr& frame )
{
    // Execute an entry point command buffer
    execute( *frame.get(), frame->entryPoint() );

    // Reset rendering states
    reset();
}

// ** Rvm::execute
void Rvm::execute( const RenderFrame& frame, const RenderCommandBuffer& commands )
{
    for( s32 i = 0, n = commands.size(); i < n; i++ ) {
        // Get a render operation at specified index
        const RenderCommandBuffer::OpCode& opCode = commands.opCodeAt( i );

        // Perform a draw call
        switch( opCode.type ) {
        case RenderCommandBuffer::OpCode::Clear:                clearRenderTarget( m_context->renderTarget( opCode.renderTarget.id ), opCode.renderTarget.clearColor, opCode.renderTarget.viewport, opCode.renderTarget.clearMask );
                                                                break;
        case RenderCommandBuffer::OpCode::Execute:              execute( frame, *opCode.execute.commands );
                                                                break;
        case RenderCommandBuffer::OpCode::UploadConstantBuffer: uploadConstantBuffer( opCode.upload.id, opCode.upload.data, opCode.upload.size );
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

// ** Rvm::clearRenderTarget
void Rvm::clearRenderTarget( const RenderTargetPtr& renderTarget, const f32* color, const u32* viewport, u8 clearMask )
{
	renderTarget->begin( m_hal );
	{
		m_hal->setViewport( viewport[0], viewport[1], viewport[2], viewport[3] );
		u32 mask = ( clearMask & Camera::ClearColor ? Renderer::ClearColor : 0 ) | ( clearMask & Camera::ClearDepth ? Renderer::ClearDepth : 0 );
		m_hal->clear( Rgba( color ), 1.0f, 0, mask );
		m_hal->setViewport( renderTarget->rect() );
	}
	renderTarget->end( m_hal );
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
    m_inputLayoutFeatures = 0;

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
    Ubershader::Bitmask features = (m_inputLayoutFeatures | (userFeatures & userFeaturesMask)) & m_activeShader.shader->supportedFeatures();

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
    m_hal->setAlphaTest( state.alpha.function, state.alpha.reference );
}

// ** Rvm::switchDepthState
void Rvm::switchDepthState( const RenderFrame& frame, const RenderState& state )
{
    m_hal->setDepthTest( state.depth.write, state.depth.function );
}

// ** Rvm::switchBlending
void Rvm::switchBlending( const RenderFrame& frame, const RenderState& state )
{
    m_hal->setBlendFactors( state.blend.src, state.blend.dst );
}

// ** Rvm::switchRenderTarget
void Rvm::switchRenderTarget( const RenderFrame& frame, const RenderState& state )
{
    const RenderTargetPtr& rt = m_context->renderTarget( state.renderTarget.id );
    m_hal->setViewport( state.renderTarget.viewport[0], state.renderTarget.viewport[1], state.renderTarget.viewport[2], state.renderTarget.viewport[3] );
    rt->begin( m_hal );
}

// ** Rvm::switchShader
void Rvm::switchShader( const RenderFrame& frame, const RenderState& state )
{
    m_activeShader.shader = m_context->shader( state.id );
}

// ** Rvm::switchConstantBuffer
void Rvm::switchConstantBuffer( const RenderFrame& frame, const RenderState& state )
{
    const Renderer::ConstantBufferPtr& constantBuffer = m_context->constantBuffer( state.constantBuffer.id );
    m_hal->setConstantBuffer( constantBuffer, state.constantBuffer.type );
}

// ** Rvm::switchVertexBuffer
void Rvm::switchVertexBuffer( const RenderFrame& frame, const RenderState& state )
{
    const Renderer::VertexBufferPtr& vertexBuffer = m_context->vertexBuffer( state.id );
    m_hal->setVertexBuffer( vertexBuffer );
}

// ** Rvm::switchIndexBuffer
void Rvm::switchIndexBuffer( const RenderFrame& frame, const RenderState& state )
{
    const Renderer::IndexBufferPtr& indexBuffer = m_context->indexBuffer( state.id );
    m_hal->setIndexBuffer( indexBuffer );
}

// ** Rvm::switchInputLayout
void Rvm::switchInputLayout( const RenderFrame& frame, const RenderState& state )
{
    // Bind an input layout
    const Renderer::InputLayoutPtr& inputLayout = m_context->inputLayout( state.id );
    m_hal->setInputLayout( inputLayout );

    // Update an input layout features
    m_inputLayoutFeatures = inputLayout->features();
}

} // namespace Scene

DC_END_DREEMCHEST