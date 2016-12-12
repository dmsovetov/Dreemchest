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

#include "RenderingContext.h"
#include "CommandBuffer.h"
#include "RenderFrame.h"
#include "Ubershader.h"
#include "RenderingContext.h"
#include "VertexFormat.h"
#include "VertexBufferLayout.h"

#include "../Io/DiskFileSystem.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
// --------------------------------------------------------- RenderingContext::ConstructionCommandBuffer --------------------------------------------------------- //

//! An internal comamnd buffer type that is used for resource construction.
class RenderingContext::ConstructionCommandBuffer : public CommandBuffer
{
public:
    
    //! Emits an input layout creation command.
    InputLayout                 createInputLayout(InputLayout id, const VertexFormat& vertexFormat);
    
    //! Emits a vertex buffer creation command.
    VertexBuffer_               createVertexBuffer(VertexBuffer_ id, const void* data, s32 size);
    
    //! Emits an index buffer creation command.
    IndexBuffer_                createIndexBuffer(IndexBuffer_ id, const void* data, s32 size);
    
    //! Emits a constant buffer creation command.
    ConstantBuffer_             createConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, const ConstantBufferLayout* layout);
    
    //! Emits a texture creation command.
    Texture_                    createTexture(Texture_ id, const void* data, u16 width, u16 height, PixelFormat format);
};

// ** RenderingContext::ConstructionCommandBuffer::createVertexBuffer
InputLayout RenderingContext::ConstructionCommandBuffer::createInputLayout(InputLayout id, const VertexFormat& vertexFormat)
{
    OpCode opCode;
    opCode.type = OpCode::CreateInputLayout;
    opCode.createInputLayout.id = id;
    opCode.createInputLayout.format = vertexFormat;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createVertexBuffer
VertexBuffer_ RenderingContext::ConstructionCommandBuffer::createVertexBuffer(VertexBuffer_ id, const void *data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::CreateVertexBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.data = data;
    opCode.createBuffer.size = size;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createIndexBuffer
IndexBuffer_ RenderingContext::ConstructionCommandBuffer::createIndexBuffer(IndexBuffer_ id, const void *data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::CreateIndexBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.data = data;
    opCode.createBuffer.size = size;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createConstantBuffer
ConstantBuffer_ RenderingContext::ConstructionCommandBuffer::createConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, const ConstantBufferLayout* layout)
{
    OpCode opCode;
    opCode.type = OpCode::CreateConstantBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.data = data;
    opCode.createBuffer.size = size;
    opCode.createBuffer.userData = layout;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createTexture
Texture_ RenderingContext::ConstructionCommandBuffer::createTexture(Texture_ id, const void* data, u16 width, u16 height, PixelFormat format)
{
    OpCode opCode;
    opCode.type = OpCode::CreateTexture;
    opCode.createTexture.id = id;
    opCode.createTexture.data = data;
    opCode.createTexture.width = width;
    opCode.createTexture.height = height;
    opCode.createTexture.format = format;
    push( opCode );
    return id;
}

// -------------------------------------------------------------------- RenderingContext::IntermediateTargetStack ------------------------------------------------------------------- //

/*!
    Intermediate target stack is used to convert local indices that are
    stored in commands to a global index of an intermediate render target.
*/
class RenderingContext::IntermediateTargetStack
{
public:

    //! A maximum number of intermediate render targets that can be hold by a single stack frame.
    enum { StackFrameSize = 8 };

    //! A maximum number of stack frames that can be pushed during rendering.
    enum { MaxStackFrames = 8 };

    //! A total size of an intermediate stack size.
    enum { MaxStackSize = MaxStackFrames * StackFrameSize };

                                //! Constructs an IntermediateTargetStack instance.
                                IntermediateTargetStack( RenderingContext& context );

    //! Pushes a new stack frame.
    void                        pushFrame( void );

    //! Pops an active stack frame.
    void                        popFrame( void );

#if DEV_DEPRECATED_HAL
    //! Returns a render target by a local index.
    RenderTargetWPtr            get( u8 index ) const;
#endif  /*  #if DEV_DEPRECATED_HAL  */

    //! Acquires an intermediate target with a specified parameters and loads it to a local slot.
    void                        acquire( u8 index, u16 width, u16 height, PixelFormat format );

    //! Releases an intermediate target.
    void                        release( u8 index );

private:

    RenderingContext&           m_context;                      //!< A parent rendering context.
    IntermediateRenderTarget*   m_stackFrame;                   //!< An active render target stack frame.
    IntermediateRenderTarget    m_identifiers[MaxStackSize];    //!< An array of intermediate render target handles.
};

// ** RenderingContext::IntermediateTargetStack::IntermediateTargetStack
RenderingContext::IntermediateTargetStack::IntermediateTargetStack( RenderingContext& context )
    : m_context( context )
    , m_stackFrame( m_identifiers )
{
    memset( m_identifiers, 0, sizeof m_identifiers );
}

// ** RenderingContext::IntermediateTargetStack::pushFrame
void RenderingContext::IntermediateTargetStack::pushFrame( void )
{
    NIMBLE_ABORT_IF( (m_stackFrame + StackFrameSize) > (m_identifiers + MaxStackSize), "frame stack overflow" );
    m_stackFrame += StackFrameSize;
}

// ** RenderingContext::IntermediateTargetStack::popFrame
void RenderingContext::IntermediateTargetStack::popFrame( void )
{
    NIMBLE_ABORT_IF( m_stackFrame == m_identifiers, "stack underflow" );

    // Ensure that all render targets were released
    for( s32 i = 0; i < StackFrameSize; i++ )
    {
        if( m_stackFrame[i] )
        {
            LogWarning( "rvm", "%s", "an intermediate render target was not released before popping a stack frame\n" );
        }
    }

    // Pop a stack frame
    m_stackFrame -= StackFrameSize;
}

#if DEV_DEPRECATED_HAL
// ** RenderingContext::IntermediateTargetStack::get
RenderTargetWPtr RenderingContext::IntermediateTargetStack::get( u8 index ) const
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    return m_context.intermediateRenderTarget( m_stackFrame[index - 1] );
}
#endif  /*  #if DEV_DEPRECATED_HAL  */

// ** RenderingContext::IntermediateTargetStack::acquire
void RenderingContext::IntermediateTargetStack::acquire( u8 index, u16 width, u16 height, PixelFormat format )
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    m_stackFrame[index - 1] = m_context.acquireRenderTarget( width, height, format );
}

// ** RenderingContext::IntermediateTargetStack::release
void RenderingContext::IntermediateTargetStack::release( u8 index )
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    m_context.releaseRenderTarget( m_stackFrame[index - 1] );
    m_stackFrame[index - 1] = 0;
}

// -------------------------------------------------------------------------------- RenderingContext -------------------------------------------------------------------------------- //

// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( HalWPtr hal )
#if DEV_DEPRECATED_HAL
    : m_constructionCommandBuffer(new ConstructionCommandBuffer)
    , m_hal( hal )
#else
    : m_context( context )
#endif  /*  #if DEV_DEPRECATED_HAL */
    , m_intermediateTargets( DC_NEW IntermediateTargetStack( *this ) )
{
    // Reset all state switchers
    memset( m_stateSwitches, 0, sizeof( m_stateSwitches ) );
    
    // Reset input layout cache
    memset( m_inputLayoutCache, 0, sizeof( m_inputLayoutCache ) );

    // Setup render state switchers
    m_stateSwitches[State::AlphaTest]         = &RenderingContext::switchAlphaTest;
    m_stateSwitches[State::DepthState]        = &RenderingContext::switchDepthState;
    m_stateSwitches[State::Blending]          = &RenderingContext::switchBlending;
    m_stateSwitches[State::Shader]            = &RenderingContext::switchShader;
    m_stateSwitches[State::ConstantBuffer]    = &RenderingContext::switchConstantBuffer;
    m_stateSwitches[State::VertexBuffer]      = &RenderingContext::switchVertexBuffer;
    m_stateSwitches[State::IndexBuffer]       = &RenderingContext::switchIndexBuffer;
    m_stateSwitches[State::InputLayout]       = &RenderingContext::switchInputLayout;
    m_stateSwitches[State::FeatureLayout]     = &RenderingContext::switchFeatureLayout;
    m_stateSwitches[State::Texture]           = &RenderingContext::switchTexture;
    m_stateSwitches[State::CullFace]          = &RenderingContext::switchCullFace;
    m_stateSwitches[State::PolygonOffset]     = &RenderingContext::switchPolygonOffset;

    // Allocate an invalid item placeholders.
    m_inputLayouts.push( NULL );
    m_vertexBuffers.push( NULL );
    m_indexBuffers.push( NULL );
    m_constantBuffers.push( NULL );
    m_textures.push( NULL );
    m_featureLayouts.push( NULL );
    m_shaders.push( NULL );
}

#if DEV_DEPRECATED_HAL
// ** RenderingContext::create
RenderingContextPtr RenderingContext::create( HalWPtr hal )
{
    return DC_NEW RenderingContext( hal );
}
#endif  /*  #if DEV_DEPRECATED_HAL  */

// ** RenderingContext::display
void RenderingContext::display( RenderFrame& frame )
{
    // First execute a construction command buffer
    execute(frame, *m_constructionCommandBuffer);
    m_constructionCommandBuffer->reset();
    
    // Execute an entry point command buffer
    execute( frame, frame.entryPoint() );

    // Reset rendering states
    reset();
    
    // Clear this frame
    frame.clear();
}

// ** RenderingContext::renderToTarget
void RenderingContext::renderToTarget( const RenderFrame& frame, u8 renderTarget, const f32* viewport, const CommandBuffer& commands )
{
#if DEV_DEPRECATED_HAL
    // Push a render target state
    if( renderTarget )
    {
        const Renderer::RenderTargetWPtr rt = m_intermediateTargets->get( renderTarget );
        m_hal->setRenderTarget( rt );
        m_hal->setViewport( viewport[0] * rt->width(), viewport[1] * rt->height(), viewport[2] * rt->width(), viewport[3] * rt->height() );
    }
    else
    {
        m_hal->setViewport( viewport[0], viewport[1], viewport[2], viewport[3] );
    }

    // Set a viewport before executing an attached command buffer
    m_viewportStack.push( viewport );

    // Execute an attached command buffer
    execute( frame, commands );

    // Pop a render target state
    if( renderTarget )
    {
        m_hal->setRenderTarget( NULL );
    }

    // Pop a viewport
    m_viewportStack.pop();

    if( m_viewportStack.size() )
    {
        const f32* prev = m_viewportStack.top();
        m_hal->setViewport( prev[0], prev[1], prev[2], prev[3] );
    }
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::execute
void RenderingContext::execute( const RenderFrame& frame, const CommandBuffer& commands )
{
    // Push a new frame to an intermediate target stack
    m_intermediateTargets->pushFrame();

#if DEV_DEPRECATED_HAL
    // Execute all commands inside a buffer
    for( s32 i = 0, n = commands.size(); i < n; i++ )
    {
        // Get a render operation at specified index
        const CommandBuffer::OpCode& opCode = commands.opCodeAt( i );

        // Perform a draw call
        switch( opCode.type ) {
        case CommandBuffer::OpCode::Clear:                  clear( opCode.clear.color, opCode.clear.depth, opCode.clear.stencil, opCode.clear.mask );
                                                            break;
        case CommandBuffer::OpCode::Execute:                execute( frame, *opCode.execute.commands );
                                                            break;
        case CommandBuffer::OpCode::UploadConstantBuffer:   commandUploadConstantBuffer( opCode.upload.id, opCode.upload.data, opCode.upload.size );
                                                            break;
        case CommandBuffer::OpCode::UploadVertexBuffer:     commandUploadVertexBuffer( opCode.upload.id, opCode.upload.data, opCode.upload.size );
                                                            break;
        case CommandBuffer::OpCode::CreateInputLayout:      commandCreateInputLayout(opCode.createInputLayout.id, opCode.createInputLayout.format);
                                                            break;
        case CommandBuffer::OpCode::CreateTexture:          commandCreateTexture(opCode.createTexture.id, opCode.createTexture.width, opCode.createTexture.height, opCode.createTexture.data, opCode.createTexture.format);
                                                            break;
        case CommandBuffer::OpCode::CreateIndexBuffer:      commandCreateIndexBuffer(opCode.createBuffer.id, opCode.createBuffer.data, opCode.createBuffer.size);
                                                            break;
        case CommandBuffer::OpCode::CreateVertexBuffer:     commandCreateVertexBuffer(opCode.createBuffer.id, opCode.createBuffer.data, opCode.createBuffer.size);
                                                            break;
        case CommandBuffer::OpCode::CreateConstantBuffer:   {
                                                                const ConstantBufferLayout* layout = reinterpret_cast<const ConstantBufferLayout*>(opCode.createBuffer.userData);
                                                                commandCreateConstantBuffer(opCode.createBuffer.id, opCode.createBuffer.data, opCode.createBuffer.size, layout);
                                                            }
                                                            break;
        case CommandBuffer::OpCode::RenderTarget:           renderToTarget( frame, opCode.renderTarget.index, opCode.renderTarget.viewport, *opCode.renderTarget.commands );
                                                            break;
        case CommandBuffer::OpCode::AcquireRenderTarget:    m_intermediateTargets->acquire( opCode.intermediateRenderTarget.index, opCode.intermediateRenderTarget.width, opCode.intermediateRenderTarget.height, opCode.intermediateRenderTarget.format );
                                                            break;
        case CommandBuffer::OpCode::ReleaseRenderTarget:    m_intermediateTargets->release( opCode.intermediateRenderTarget.index );
                                                            break;
        case CommandBuffer::OpCode::DrawIndexed:            {
                                                                // Apply rendering states from a stack
                                                                PipelineFeatures features = applyStates( frame, opCode.drawCall.states, MaxStateStackDepth );
            
                                                                // Now activate a matching shader permutation
                                                                activateShaderPermutation(features);

                                                            #if DEV_DEPRECATED_HAL
                                                                // Perform an actual draw call
                                                                m_hal->renderIndexed( opCode.drawCall.primitives, opCode.drawCall.first, opCode.drawCall.count );
                                                            #else
                                                                NIMBLE_NOT_IMPLEMENTED
                                                            #endif  /*  #if DEV_DEPRECATED_HAL  */
                                                            }
                                                            break;
        case CommandBuffer::OpCode::DrawPrimitives:         {
                                                                // Apply rendering states from a stack
                                                                PipelineFeatures features = applyStates( frame, opCode.drawCall.states, MaxStateStackDepth );
            
                                                                // Now activate a matching shader permutation
                                                                activateShaderPermutation(features);

                                                            #if DEV_DEPRECATED_HAL
                                                                // Perform an actual draw call
                                                                m_hal->renderPrimitives( opCode.drawCall.primitives, opCode.drawCall.first, opCode.drawCall.count );
                                                            #else
                                                                NIMBLE_NOT_IMPLEMENTED
                                                            #endif  /*  #if DEV_DEPRECATED_HAL  */
                                                            }
                                                            break;
        default:                                            NIMBLE_NOT_IMPLEMENTED;
        }
    }
#else
    executeCommandBuffer(frame, commands);
#endif  /*  #if DEV_DEPRECATED_HAL  */

    // Pop a stack frame
    m_intermediateTargets->popFrame();
}

#if DEV_DEPRECATED_HAL
// ** RenderingContext::reset
void RenderingContext::reset( void )
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
#endif  /*  #if DEV_DEPRECATED_HAL  */

// ** RenderingContext::clear
void RenderingContext::clear( const f32* color, f32 depth, s32 stencil, u8 mask )
{
#if DEV_DEPRECATED_HAL
    m_hal->clear( Rgba( color ), depth,stencil, mask );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::commandUploadConstantBuffer
void RenderingContext::commandUploadConstantBuffer( ConstantBuffer_ id, const void* data, s32 size )
{
#if DEV_DEPRECATED_HAL
    Renderer::ConstantBufferPtr constantBuffer = m_constantBuffers[id];
    memcpy( constantBuffer->lock(), data, size );
    constantBuffer->unlock();
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::commandUploadVertexBuffer
void RenderingContext::commandUploadVertexBuffer( VertexBuffer_ id, const void* data, s32 size )
{
#if DEV_DEPRECATED_HAL
    Renderer::VertexBufferPtr vertexBuffer = m_vertexBuffers[id];
    vertexBuffer->setBufferData( data, 0, size );
#else
#endif  /*  #if DEV_DEPRECATED_HAL  */
}
    
// ** RenderingContext::commandCreateInputLayout
void RenderingContext::commandCreateInputLayout(InputLayout id, u8 vertexFormat)
{
    NIMBLE_BREAK_IF( m_inputLayouts[id].get(), "resource was already constructed" );
    
    // Create an input layout vertex format
    VertexFormat format( vertexFormat );
    
    // Create an input layout
    VertexBufferLayout* inputLayout = DC_NEW VertexBufferLayout( format.vertexSize() );
    
    // Add vertex attributes to an input layout
    if( format & VertexFormat::Position )
    {
        inputLayout->attributeLocation( VertexBufferLayout::Position, 3, format.attributeOffset( VertexFormat::Position ) );
    }
    if( format & VertexFormat::Color )
    {
        inputLayout->attributeLocation( VertexBufferLayout::Color, 4, format.attributeOffset( VertexFormat::Color ) );
    }
    if( format & VertexFormat::Normal )
    {
        inputLayout->attributeLocation( VertexBufferLayout::Normal, 3, format.attributeOffset( VertexFormat::Normal ) );
    }
    if( format & VertexFormat::Uv0 )
    {
        inputLayout->attributeLocation( VertexBufferLayout::Uv0, 2, format.attributeOffset( VertexFormat::Uv0 ) );
    }
    if( format & VertexFormat::Uv1 )
    {
        inputLayout->attributeLocation( VertexBufferLayout::Uv1, 2, format.attributeOffset( VertexFormat::Uv1 ) );
    }
    
    // Save an input layout to a pool
    m_inputLayouts[id] = inputLayout;
    
    LogDebug( "renderingContext", "input layout %d constructed\n", id );
}

// ** RenderingContext::commandCreateVertexBuffer
void RenderingContext::commandCreateVertexBuffer(VertexBuffer_ id, const void* data, s32 size)
{
    NIMBLE_BREAK_IF( m_vertexBuffers[id].valid(), "resource was already constructed" );
    
    // Create a vertex buffer instance
    VertexBufferPtr vertexBuffer = m_hal->createVertexBuffer( size );
    
    // Upload data to a GPU buffer
    if( data )
    {
        memcpy( vertexBuffer->lock(), data, size );
        vertexBuffer->unlock();
    }
    
    // Save a vertex buffer to a pool
    m_vertexBuffers[id] = vertexBuffer;
    
    LogDebug( "renderingContext", "vertex buffer %d of size %2.2fkb constructed\n", id, size / 1024.0f );
}

// ** RenderingContext::commandCreateIndexBuffer
void RenderingContext::commandCreateIndexBuffer(IndexBuffer_ id, const void* data, s32 size)
{
    NIMBLE_BREAK_IF( m_indexBuffers[id].valid(), "resource was already constructed" );
    
    // Create an index buffer instance
    IndexBufferPtr indexBuffer = m_hal->createIndexBuffer( size );
    
    // Upload data to a GPU buffer
    if( data )
    {
        memcpy( indexBuffer->lock(), data, size );
        indexBuffer->unlock();
    }
    
    // Save a index buffer to a pool
    m_indexBuffers[id] = indexBuffer;
    
    LogDebug( "renderingContext", "index buffer %d of size %2.2fkb constructed\n", id, size / 1024.0f );
}

// ** RenderingContext::commandCreateConstantBuffer
void RenderingContext::commandCreateConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, const ConstantBufferLayout* layout)
{
    NIMBLE_BREAK_IF( m_constantBuffers[id].valid(), "resource was already constructed" );
        
    ConstantBufferPtr constantBuffer = m_hal->createConstantBuffer( size, layout );
    
    // Upload data to a GPU buffer
    if( data )
    {
        memcpy( constantBuffer->lock(), data, size );
        constantBuffer->unlock();
    }
    
    // Save a index buffer to a pool
    m_constantBuffers[id] = constantBuffer;
    
    LogDebug( "renderingContext", "constant buffer %d of size %2.2fkb constructed\n", id, size / 1024.0f );
}

// ** RenderingContext::commandCreateTexture
void RenderingContext::commandCreateTexture(Texture_ id, u16 width, u16 height, const void* data, PixelFormat format)
{
    NIMBLE_BREAK_IF( m_textures[id].valid(), "resource was already constructed" );
        
    Texture2DPtr texture = m_hal->createTexture2D( width, height, format );
    
    // Upload data to a GPU buffer
    if( data )
    {
        u32 size;
        void* ptr = texture->lock( 0, size );
        memcpy( ptr, data, size );
        texture->unlock();
    }
    
    // Save a index buffer to a pool
    m_textures[id] = texture;
}

// ** RenderingContext::applyStates
PipelineFeatures RenderingContext::applyStates( const RenderFrame& frame, const StateBlock* const * states, s32 count )
{
    PipelineFeatures userFeatures = 0;
    PipelineFeatures userFeaturesMask = ~0;

    // Reset all ubershader features
    m_vertexAttributeFeatures = 0;
    m_resourceFeatures        = 0;

    // A bitmask of states that were already set
    u32 activeStateMask = 0;

    for( s32 i = 0; i < count; i++ )
    {
        // Get a state block at specified index
        const StateBlock* block = states[i];

        // No more state blocks in a stack - break
        if( block == NULL )
        {
            break;
        }

        // Update feature set
        userFeatures      = userFeatures     | block->features();
        userFeaturesMask  = userFeaturesMask & block->featureMask();

        // Skip redundant state blocks by testing a block bitmask against an active state mask
        if( (activeStateMask ^ block->mask()) == 0 )
        {
            continue;
        }

        // Apply all states in a block
        for( s32 j = 0, n = block->stateCount(); j < n; j++ )
        {
            // Get a render state bit
            u32 stateBit = block->stateBit( j );

            // Skip redundate state blocks by testing a state bitmask agains an active state mask
            if( activeStateMask & stateBit )
            {
                continue;
            }

            // Get a render state at specified index
            const State& state = block->state( j );

            // Update an active state mask
            activeStateMask = activeStateMask | stateBit;

            // Finally apply a state
            NIMBLE_ABORT_IF( m_stateSwitches[state.type] == NULL, "unhandled render state type" );
            (this->*m_stateSwitches[state.type])( frame, state );
        }
    }
    
    // Compose a user defined feature mask
    PipelineFeatures userDefined = (userFeatures & userFeaturesMask) << UserDefinedFeaturesOffset;
    
    // Compose a new bitmaks with pipeline features
    PipelineFeatures features = (m_vertexAttributeFeatures | m_resourceFeatures | userDefined);
    
    return features;
}
    
// ** RenderingContext::activateShaderPermutation
void RenderingContext::activateShaderPermutation( PipelineFeatures features )
{
    // Finally apply a shader
    NIMBLE_ABORT_IF( !m_activeShader.shader.valid(), "no valid shader set" );
    NIMBLE_ABORT_IF( !m_activeShader.featureLayout, "no valid feature layout set" );
    
    // Select a shader permutation that match an active pipeline state
    PipelineFeatures supported   = m_activeShader.featureLayout->mask();
    features = features & supported;
    //PipelineFeatures userDefined = userFeatures << UserDefinedFeaturesOffset;
    //PipelineFeatures features    = (m_vertexAttributeFeatures | m_resourceFeatures | userDefined) & supported;
    
    if( m_activeShader.activeShader != m_activeShader.shader || m_activeShader.features != features || m_activeShader.featureLayout != m_activeShader.activeFeatureLayout)
    {
#if DEV_DEPRECATED_HAL
        m_activeShader.permutation  = m_activeShader.shader->permutation( m_hal, features, m_activeShader.featureLayout );
#else
        NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
        m_activeShader.features     = features;
        m_activeShader.activeShader = m_activeShader.shader;
        m_activeShader.activeFeatureLayout = m_activeShader.featureLayout;
    }
    
#if DEV_DEPRECATED_HAL
    // Bind an active shader permutation
    m_hal->setShader( m_activeShader.permutation );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchAlphaTest
void RenderingContext::switchAlphaTest( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    m_hal->setAlphaTest( static_cast<Renderer::Compare>( state.compareFunction ), state.data.alphaReference / 255.0f );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchDepthState
void RenderingContext::switchDepthState( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    m_hal->setDepthTest( state.data.depthWrite, static_cast<Renderer::Compare>( state.compareFunction ) );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchBlending
void RenderingContext::switchBlending( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    m_hal->setBlendFactors( static_cast<Renderer::BlendFactor>( state.data.blend >> 4 ), static_cast<Renderer::BlendFactor>( state.data.blend & 0xF ) );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchShader
void RenderingContext::switchShader( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    m_activeShader.shader = m_shaders[state.resourceId];
    NIMBLE_ABORT_IF(!m_activeShader.shader.valid(), "invalid resource id");
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchConstantBuffer
void RenderingContext::switchConstantBuffer( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    const Renderer::ConstantBufferPtr& constantBuffer = m_constantBuffers[state.resourceId];
    m_hal->setConstantBuffer( constantBuffer, state.data.index );

    // A single u64 bit constant value
    static const u64 bit = 1;

    // Update resource features
    m_resourceFeatures = m_resourceFeatures | (bit << (state.data.index + CBufferFeaturesOffset));
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchVertexBuffer
void RenderingContext::switchVertexBuffer( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    const Renderer::VertexBufferPtr& vertexBuffer = m_vertexBuffers[state.resourceId];
    NIMBLE_ABORT_IF(!vertexBuffer.valid(), "invalid resource id");
    m_hal->setVertexBuffer( vertexBuffer );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchIndexBuffer
void RenderingContext::switchIndexBuffer( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    const Renderer::IndexBufferPtr& indexBuffer = m_indexBuffers[state.resourceId];
    NIMBLE_ABORT_IF(!indexBuffer.valid(), "invalid resource id");
    m_hal->setIndexBuffer( indexBuffer );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchInputLayout
void RenderingContext::switchInputLayout( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    // Bind an input layout
    const Renderer::VertexBufferLayoutPtr& inputLayout = m_inputLayouts[state.resourceId];
    NIMBLE_ABORT_IF(!inputLayout.valid(), "invalid resource id");
    m_hal->setInputLayout( inputLayout );

    // Update an input layout features
    m_vertexAttributeFeatures = inputLayout->features();
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}
    
// ** RenderingContext::switchFeatureLayout
void RenderingContext::switchFeatureLayout( const RenderFrame& frame, const State& state )
{
    // Get a feature layout by it's id
    const ShaderFeatureLayoutUPtr& featureLayout = m_featureLayouts[state.resourceId];
    NIMBLE_ABORT_IF(!featureLayout.get(), "invalid resource id");
    
    // Make this feature layout active
    m_activeShader.featureLayout = featureLayout.get();
}

// ** RenderingContext::switchTexture
void RenderingContext::switchTexture( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    // A single u64 bit constant value
    static const u64 bit = 1;

    // Convert a resource id to a signed integer
    s32 id = static_cast<s16>( state.resourceId );

    // Get a sampler index
    u8 samplerIndex = state.data.index & 0xF;

    // Bind a texture to sampler
    if( id >= 0 )
    {
        const Renderer::TexturePtr& texture = m_textures[state.resourceId];
        m_hal->setTexture( state.data.index, texture.get() );
    }
    else
    {
        NIMBLE_BREAK_IF( abs( id ) > 255, "invalid identifier" );
        Renderer::Texture2DPtr texture = m_intermediateTargets->get( -id )->attachment( static_cast<Renderer::RenderTarget::Attachment>( state.data.index >> 4 ) );
        NIMBLE_BREAK_IF( !texture.valid(), "invalid render target attachment" );
        m_hal->setTexture( samplerIndex, texture.get() );
    }

    // Update resource features
    m_resourceFeatures = m_resourceFeatures | (bit << (samplerIndex + SamplerFeaturesOffset));
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchCullFace
void RenderingContext::switchCullFace( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    m_hal->setCulling( static_cast<Renderer::TriangleFace>( state.cullFace ) );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::switchPolygonOffset
void RenderingContext::switchPolygonOffset( const RenderFrame& frame, const State& state )
{
#if DEV_DEPRECATED_HAL
    m_hal->setPolygonOffset( state.polygonOffset.factor / 128.0f, state.polygonOffset.units / 128.0f );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::requestInputLayout
InputLayout RenderingContext::requestInputLayout( const VertexFormat& format )
{
    // First lookup a previously constucted input layout
    InputLayout id = m_inputLayoutCache[format];
    
    if( id )
    {
        return id;
    }
    
    // Nothing found - construct a new one
    id = m_constructionCommandBuffer->createInputLayout(allocateInputLayout(), format);
    
    // Now put a new input layout to cache
    m_inputLayoutCache[format] = id;
    
    return id;
}

// ** RenderingContext::requestFeatureLayout
FeatureLayout RenderingContext::requestFeatureLayout(const ShaderFeature* features)
{
    // Create a shader feature layout
    ShaderFeatureLayout* layout = DC_NEW ShaderFeatureLayout;
    
    // Populate it with feature mappings
    for (; features->name; features++)
    {
        layout->addFeature(features->name.value(), features->mask);
    }
    
    // Put this layout instance to a pool
    FeatureLayout id = m_featureLayouts.push(layout);
    
    return id;
}

// ** RenderingContext::allocateInputLayout
InputLayout RenderingContext::allocateInputLayout( void )
{
    return m_inputLayouts.push( NULL );
}

// ** RenderingContext::allocateTexture
Texture_ RenderingContext::allocateTexture( void )
{
    return m_textures.push( NULL );
}

// ** RenderingContext::allocateConstantBuffer
ConstantBuffer_ RenderingContext::allocateConstantBuffer( void )
{
    return m_constantBuffers.push( NULL );
}

// ** RenderingContext::allocateIndexBuffer
IndexBuffer_ RenderingContext::allocateIndexBuffer( void )
{
    return m_indexBuffers.push( NULL );
}

// ** RenderingContext::allocateVertexBuffer
VertexBuffer_ RenderingContext::allocateVertexBuffer( void )
{
    return m_vertexBuffers.push( NULL );
}

// ** RenderingContext::requestVertexBuffer
VertexBuffer_ RenderingContext::requestVertexBuffer( const void* data, s32 size )
{
    return m_constructionCommandBuffer->createVertexBuffer(allocateVertexBuffer(), data, size);
}

// ** RenderingContext::requestIndexBuffer
IndexBuffer_ RenderingContext::requestIndexBuffer( const void* data, s32 size )
{
    return m_constructionCommandBuffer->createIndexBuffer(allocateIndexBuffer(), data, size);
}

// ** RenderingContext::requestConstantBuffer
ConstantBuffer_ RenderingContext::requestConstantBuffer( const void* data, s32 size, const Renderer::ConstantBufferLayout* layout )
{
    return m_constructionCommandBuffer->createConstantBuffer(allocateConstantBuffer(), data, size, layout);
}

// ** RenderingContext::requestConstantBuffer
Texture_ RenderingContext::requestTexture( const void* data, u16 width, u16 height, Renderer::PixelFormat format )
{
    return m_constructionCommandBuffer->createTexture(allocateTexture(), data, width, height, format);
}

// ** RenderingContext::requestShader
Program RenderingContext::requestShader(const String& fileName)
{
    static CString vertexShaderMarker   = "[VertexShader]";
    static CString fragmentShaderMarker = "[FragmentShader]";
    
    // Read the code from an input stream
    String code = Io::DiskFileSystem::readTextFile( fileName );
    NIMBLE_ABORT_IF(code.empty(), "a shader source is empty or file not found");
    
    // Extract vertex/fragment shader code blocks
    size_t vertexBegin   = code.find( vertexShaderMarker );
    size_t fragmentBegin = code.find( fragmentShaderMarker );
    
    if( vertexBegin == String::npos && fragmentBegin == String::npos )
    {
        return 0;
    }
    
    String vertexShader, fragmentShader;
    
    if( vertexBegin != String::npos )
    {
        u32 vertexCodeStart = vertexBegin + strlen( vertexShaderMarker );
        vertexShader = code.substr( vertexCodeStart, fragmentBegin > vertexBegin ? fragmentBegin - vertexCodeStart : String::npos );
    }
    
    if( fragmentBegin != String::npos )
    {
        u32 fragmentCodeStart = fragmentBegin + strlen( fragmentShaderMarker );
        fragmentShader = code.substr( fragmentCodeStart, vertexBegin > fragmentBegin ? vertexBegin - fragmentCodeStart : String::npos );
    }
    
    return requestShader(vertexShader, fragmentShader);
}
    
// ** RenderingContext::requestShader
Program RenderingContext::requestShader(const String& vertex, const String& fragment)
{
    // Create a shader instance
    UbershaderPtr shader = DC_NEW Ubershader;
    
    // Set vertex and fragment shader code
    shader->setVertex(vertex);
    shader->setFragment(fragment);
    
    // ----------------------------------------------------
    shader->addInclude(
                       "                                                       \n\
                       struct CBufferScene    { vec4 ambient; };           \n\
                       struct CBufferView     { mat4 transform; float near; float far; vec3 position; };         \n\
                       struct CBufferInstance { mat4 transform; };         \n\
                       struct CBufferMaterial { vec4 diffuse; vec4 specular; vec4 emission; struct { vec3 color; float factor; float start; float end; } rim; };   \n\
                       struct CBufferLight    { vec3 position; float range; vec3 color; float intensity; vec3 direction; float cutoff; };         \n\
                       struct CBufferShadow   { mat4 transform; float invSize; };         \n\
                       struct CBufferClipPlanes { vec4 equation[6]; };         \n\
                       uniform CBufferScene    Scene;                      \n\
                       uniform CBufferView     View;                       \n\
                       uniform CBufferInstance Instance;                   \n\
                       uniform CBufferMaterial Material;                   \n\
                       uniform CBufferLight    Light;                      \n\
                       uniform CBufferShadow   Shadow;                     \n\
                       uniform CBufferClipPlanes ClipPlanes;               \n\
                       #define u_DiffuseTexture Texture0                   \n\
                       #define u_ShadowTexture  Texture1                   \n\
                       "
                       );
    
    // Put this shader to a pool
    Program id = m_shaders.push(shader);
    
    return id;
}
    
// ** RenderingContext::createShader
/*UbershaderPtr RenderingContext::createShader( const String& fileName ) const
{
    static CString vertexShaderMarker   = "[VertexShader]";
    static CString fragmentShaderMarker = "[FragmentShader]";
    static CString featuresMarker       = "[Features]";
    
    Map<String, u64> masks;
    
    masks["vertexNormal"]        = BIT( VertexNormal );
    masks["vertexColor"]         = BIT( VertexColor );
    masks["vertexTangent"]       = BIT( VertexTangent );
    masks["vertexBitangent"]     = BIT( VertexBitangent );
    masks["vertexPointSize"]     = BIT( VertexPointSize );
    masks["vertexTexCoord0"]     = BIT( VertexTexCoord0 );
    masks["vertexTexCoord1"]     = BIT( VertexTexCoord1 );
    masks["vertexTexCoord2"]     = BIT( VertexTexCoord2 );
    masks["vertexTexCoord3"]     = BIT( VertexTexCoord3 );
    masks["vertexTexCoord4"]     = BIT( VertexTexCoord4 );
    masks["vertexTexCoord5"]     = BIT( VertexTexCoord5 );
    masks["vertexTexCoord6"]     = BIT( VertexTexCoord6 );
    masks["vertexTexCoord7"]     = BIT( VertexTexCoord7 );
    
    masks["texture0"]       = BIT( TextureSampler0 + SamplerFeaturesOffset );
    masks["texture1"]       = BIT( TextureSampler1 + SamplerFeaturesOffset );
    masks["texture2"]       = BIT( TextureSampler2 + SamplerFeaturesOffset );
    masks["texture3"]       = BIT( TextureSampler3 + SamplerFeaturesOffset );
    masks["texture4"]       = BIT( TextureSampler4 + SamplerFeaturesOffset );
    masks["texture5"]       = BIT( TextureSampler5 + SamplerFeaturesOffset );
    masks["texture6"]       = BIT( TextureSampler6 + SamplerFeaturesOffset );
    masks["texture7"]       = BIT( TextureSampler7 + SamplerFeaturesOffset );
    
    masks["cbuffer0"]       = BIT( ConstantBuffer0 + CBufferFeaturesOffset );
    masks["cbuffer1"]       = BIT( ConstantBuffer1 + CBufferFeaturesOffset );
    masks["cbuffer2"]       = BIT( ConstantBuffer2 + CBufferFeaturesOffset );
    masks["cbuffer3"]       = BIT( ConstantBuffer3 + CBufferFeaturesOffset );
    masks["cbuffer4"]       = BIT( ConstantBuffer4 + CBufferFeaturesOffset );
    masks["cbuffer5"]       = BIT( ConstantBuffer5 + CBufferFeaturesOffset );
    masks["cbuffer6"]       = BIT( ConstantBuffer6 + CBufferFeaturesOffset );
    masks["cbuffer7"]       = BIT( ConstantBuffer7 + CBufferFeaturesOffset );
    
    masks["ambientColor"]   = static_cast<u64>( ShaderAmbientColor )  << UserDefinedFeaturesOffset;
    masks["emissionColor"]  = static_cast<u64>( ShaderEmissionColor ) << UserDefinedFeaturesOffset;
    masks["specularColor"]  = static_cast<u64>( ShaderSpecularColor ) << UserDefinedFeaturesOffset;
    masks["lightType"]      = static_cast<u64>( ShaderPointLight | ShaderDirectionalLight | ShaderSpotLight ) << UserDefinedFeaturesOffset;
    masks["shadowFiltering"]= static_cast<u64>( ShaderShadowFiltering1 | ShaderShadowFiltering2 | ShaderShadowFiltering3 ) << UserDefinedFeaturesOffset;
    masks["rimLight"]       = static_cast<u64>( ShaderRimLight ) << UserDefinedFeaturesOffset;
    
    // Create a shader instance
    UbershaderPtr shader = DC_NEW Ubershader;
    
    // Read the code from an input stream
    String code = Io::DiskFileSystem::readTextFile( fileName );
    NIMBLE_ABORT_IF(code.empty(), "a shader source is empty or file not found");
    
    // Extract vertex/fragment shader code blocks
    size_t vertexBegin   = code.find( vertexShaderMarker );
    size_t fragmentBegin = code.find( fragmentShaderMarker );
    size_t featuresBegin = code.find( featuresMarker );
    
    if( vertexBegin == String::npos && fragmentBegin == String::npos )
    {
        return UbershaderPtr();
    }
    
    if( featuresBegin != String::npos )
    {
        u32 featuresCodeStart = featuresBegin + strlen( featuresMarker );
        Array<String> features = split( code.substr( featuresCodeStart, vertexBegin - featuresCodeStart ), "\r\n" );
        
        for( Array<String>::const_iterator i = features.begin(), end = features.end(); i != end; ++i )
        {
            Array<String> value = split( *i, " \t=" );
            //shader->addFeature( masks[value[1]], value[0] );
            LogVerbose( "shader", "feature %s = %s (0x%x) added\n", value[0].c_str(), value[1].c_str(), masks[value[1]] );
        }
    }
    
    if( vertexBegin != String::npos )
    {
        u32 vertexCodeStart = vertexBegin + strlen( vertexShaderMarker );
        String vertex = code.substr( vertexCodeStart, fragmentBegin > vertexBegin ? fragmentBegin - vertexCodeStart : String::npos );
        shader->setVertex( vertex );
    }
    
    if( fragmentBegin != String::npos )
    {
        u32 fragmentCodeStart = fragmentBegin + strlen( fragmentShaderMarker );
        String fragment = code.substr( fragmentCodeStart, vertexBegin > fragmentBegin ? vertexBegin - fragmentCodeStart : String::npos );
        shader->setFragment( fragment );
    }
    
    // ----------------------------------------------------
    shader->addInclude(
                       "                                                       \n\
                       struct CBufferScene    { vec4 ambient; };           \n\
                       struct CBufferView     { mat4 transform; float near; float far; vec3 position; };         \n\
                       struct CBufferInstance { mat4 transform; };         \n\
                       struct CBufferMaterial { vec4 diffuse; vec4 specular; vec4 emission; struct { vec3 color; float factor; float start; float end; } rim; };   \n\
                       struct CBufferLight    { vec3 position; float range; vec3 color; float intensity; vec3 direction; float cutoff; };         \n\
                       struct CBufferShadow   { mat4 transform; float invSize; };         \n\
                       struct CBufferClipPlanes { vec4 equation[6]; };         \n\
                       uniform CBufferScene    Scene;                      \n\
                       uniform CBufferView     View;                       \n\
                       uniform CBufferInstance Instance;                   \n\
                       uniform CBufferMaterial Material;                   \n\
                       uniform CBufferLight    Light;                      \n\
                       uniform CBufferShadow   Shadow;                     \n\
                       uniform CBufferClipPlanes ClipPlanes;               \n\
                       #define u_DiffuseTexture Texture0                   \n\
                       #define u_ShadowTexture  Texture1                   \n\
                       "
                       );
    
    return shader;
}*/

// ** RenderingContext::acquireRenderTarget
IntermediateRenderTarget RenderingContext::acquireRenderTarget( u16 width, u16 height, PixelFormat format )
{
    // Perform a linear search of a render target
    for( s32 i = 0, n = static_cast<s32>( m_renderTargets.size() ); i < n; i++ )
    {
        IntermediateRenderTarget_& intermediate = m_renderTargets[i];
        
        if( intermediate.isFree && intermediate.width == width && intermediate.height == height && intermediate.format == format )
        {
            intermediate.isFree = false;
            return i + 1;
        }
    }
    
#if DEV_DEPRECATED_HAL
    // Not found - create a new one
    IntermediateRenderTarget_ intermediate;
    intermediate.isFree         = false;
    intermediate.width          = width;
    intermediate.height         = height;
    intermediate.format         = format;
    intermediate.renderTarget   = m_hal->createRenderTarget( width, height );
    if( format == Renderer::PixelD24S8 || format == Renderer::PixelD24X8 )
    {
        intermediate.renderTarget->setAttachment( format, Renderer::RenderTarget::Depth );
    }
    else
    {
        intermediate.renderTarget->setAttachment( format, Renderer::RenderTarget::Color0 );
    }
    intermediate.renderTarget->setDepth( Renderer::PixelD24X8 );
    m_renderTargets.push_back( intermediate );
    
    LogVerbose( "renderingContext", "%dx%d render target created\n", width, height );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
    
    return m_renderTargets.size();
}

// ** RenderingContext::releaseRenderTarget
void RenderingContext::releaseRenderTarget( IntermediateRenderTarget id )
{
    m_renderTargets[static_cast<s32>(id) - 1].isFree = true;
}

#if DEV_DEPRECATED_HAL
// ** RenderingContext::intermediateRenderTarget
RenderTargetWPtr RenderingContext::intermediateRenderTarget( IntermediateRenderTarget id ) const
{
    return m_renderTargets[static_cast<s32>(id) - 1].renderTarget;
}
#endif  //  #if DEV_DEPRECATED_HAL

} // namespace Renderer

DC_END_DREEMCHEST
