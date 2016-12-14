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

#include "RenderingContextHal.h"
#include "CommandBuffer.h"
#include "RenderFrame.h"
#include "Ubershader.h"
#include "RenderingContext.h"
#include "VertexFormat.h"
#include "VertexBufferLayout.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** RenderingContextHal::RenderingContextHal
RenderingContextHal::RenderingContextHal( HalWPtr hal )
    : m_hal( hal )
{
    // Reset all state switchers
    memset( m_stateSwitches, 0, sizeof( m_stateSwitches ) );

    // Setup render state switchers
    m_stateSwitches[State::AlphaTest]         = &RenderingContextHal::switchAlphaTest;
    m_stateSwitches[State::DepthState]        = &RenderingContextHal::switchDepthState;
    m_stateSwitches[State::Blending]          = &RenderingContextHal::switchBlending;
    m_stateSwitches[State::Shader]            = &RenderingContextHal::switchShader;
    m_stateSwitches[State::ConstantBuffer]    = &RenderingContextHal::switchConstantBuffer;
    m_stateSwitches[State::VertexBuffer]      = &RenderingContextHal::switchVertexBuffer;
    m_stateSwitches[State::IndexBuffer]       = &RenderingContextHal::switchIndexBuffer;
    m_stateSwitches[State::InputLayout]       = &RenderingContextHal::switchInputLayout;
    m_stateSwitches[State::FeatureLayout]     = &RenderingContextHal::switchPipelineFeatureLayout;
    m_stateSwitches[State::Texture]           = &RenderingContextHal::switchTexture;
    m_stateSwitches[State::CullFace]          = &RenderingContextHal::switchCullFace;
    m_stateSwitches[State::PolygonOffset]     = &RenderingContextHal::switchPolygonOffset;
}

// ** RenderingContextHal::renderToTarget
void RenderingContextHal::renderToTarget( const RenderFrame& frame, u8 renderTarget, const f32* viewport, const CommandBuffer& commands )
{
    // Push a render target state
    if( renderTarget )
    {
        const Renderer::RenderTargetWPtr rt = intermediateRenderTarget(intermediateTarget( renderTarget ));
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
}

// ** RenderingContextHal::executeCommandBuffer
void RenderingContextHal::executeCommandBuffer( const RenderFrame& frame, const CommandBuffer& commands )
{
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
        case CommandBuffer::OpCode::AcquireRenderTarget:    {
                                                                TransientRenderTarget id = acquireRenderTarget(opCode.intermediateRenderTarget.width, opCode.intermediateRenderTarget.height, opCode.intermediateRenderTarget.format);
                                                                loadTransientTarget(opCode.intermediateRenderTarget.index, id);
                                                            }
                                                            break;
        case CommandBuffer::OpCode::ReleaseRenderTarget:    {
                                                                TransientRenderTarget id = intermediateTarget(opCode.intermediateRenderTarget.index);
                                                                releaseRenderTarget(id);
                                                                unloadTransientTarget(opCode.intermediateRenderTarget.index);
                                                            }
                                                            break;
        case CommandBuffer::OpCode::DrawIndexed:            {
                                                                // Apply rendering states from a stack
                                                                PipelineFeatures features = applyStates( frame, opCode.drawCall.states, MaxStateStackDepth );
            
                                                                // Now activate a matching shader permutation
                                                                activateShaderPermutation(features);

                                                                // Perform an actual draw call
                                                                m_hal->renderIndexed( opCode.drawCall.primitives, opCode.drawCall.first, opCode.drawCall.count );
                                                            }
                                                            break;
        case CommandBuffer::OpCode::DrawPrimitives:         {
                                                                // Apply rendering states from a stack
                                                                PipelineFeatures features = applyStates( frame, opCode.drawCall.states, MaxStateStackDepth );
            
                                                                // Now activate a matching shader permutation
                                                                activateShaderPermutation(features);

                                                                // Perform an actual draw call
                                                                m_hal->renderPrimitives( opCode.drawCall.primitives, opCode.drawCall.first, opCode.drawCall.count );
                                                            }
                                                            break;
        default:                                            NIMBLE_NOT_IMPLEMENTED;
        }
    }
}
    
// ** RenderingContextHal::applyStateBlock
PipelineFeatures RenderingContextHal::applyStateBlock(const RenderFrame& frame, const StateBlock& stateBlock)
{
    const StateBlock* blocks[] = { &stateBlock };
    return applyStates(frame, blocks, 1);
}

// ** RenderingContextHal::clear
void RenderingContextHal::clear( const f32* color, f32 depth, s32 stencil, u8 mask )
{
    m_hal->clear( Rgba( color ), depth,stencil, mask );
}

// ** RenderingContextHal::commandUploadConstantBuffer
void RenderingContextHal::commandUploadConstantBuffer( ConstantBuffer_ id, const void* data, s32 size )
{
    Renderer::ConstantBufferPtr constantBuffer = m_constantBuffers[id];
    memcpy( constantBuffer->lock(), data, size );
    constantBuffer->unlock();
}

// ** RenderingContextHal::commandUploadVertexBuffer
void RenderingContextHal::commandUploadVertexBuffer( VertexBuffer_ id, const void* data, s32 size )
{
    Renderer::VertexBufferPtr vertexBuffer = m_vertexBuffers[id];
    vertexBuffer->setBufferData( data, 0, size );
}
    
// ** RenderingContextHal::commandCreateInputLayout
void RenderingContextHal::commandCreateInputLayout(InputLayout id, u8 vertexFormat)
{
    //NIMBLE_BREAK_IF( m_inputLayouts[id].get(), "resource was already constructed" );
    
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
    m_inputLayouts.emplace(id, inputLayout);
    
    LogDebug( "renderingContext", "input layout %d constructed\n", id );
}

// ** RenderingContextHal::commandCreateVertexBuffer
void RenderingContextHal::commandCreateVertexBuffer(VertexBuffer_ id, const void* data, s32 size)
{
    //NIMBLE_BREAK_IF( m_vertexBuffers[id].valid(), "resource was already constructed" );
    
    // Create a vertex buffer instance
    VertexBufferPtr vertexBuffer = m_hal->createVertexBuffer( size );
    
    // Upload data to a GPU buffer
    if( data )
    {
        memcpy( vertexBuffer->lock(), data, size );
        vertexBuffer->unlock();
    }
    
    // Save a vertex buffer to a pool
    m_vertexBuffers.emplace(id, vertexBuffer);
    
    LogDebug( "renderingContext", "vertex buffer %d of size %2.2fkb constructed\n", id, size / 1024.0f );
}

// ** RenderingContextHal::commandCreateIndexBuffer
void RenderingContextHal::commandCreateIndexBuffer(IndexBuffer_ id, const void* data, s32 size)
{
    //NIMBLE_BREAK_IF( m_indexBuffers[id].valid(), "resource was already constructed" );
    
    // Create an index buffer instance
    IndexBufferPtr indexBuffer = m_hal->createIndexBuffer( size );
    
    // Upload data to a GPU buffer
    if( data )
    {
        memcpy( indexBuffer->lock(), data, size );
        indexBuffer->unlock();
    }
    
    // Save a index buffer to a pool
    m_indexBuffers.emplace(id, indexBuffer);
    
    LogDebug( "renderingContext", "index buffer %d of size %2.2fkb constructed\n", id, size / 1024.0f );
}

// ** RenderingContextHal::commandCreateConstantBuffer
void RenderingContextHal::commandCreateConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, const ConstantBufferLayout* layout)
{
    //NIMBLE_BREAK_IF( m_constantBuffers[id].valid(), "resource was already constructed" );
        
    ConstantBufferPtr constantBuffer = m_hal->createConstantBuffer( size, layout );
    
    // Upload data to a GPU buffer
    if( data )
    {
        memcpy( constantBuffer->lock(), data, size );
        constantBuffer->unlock();
    }
    
    // Save a index buffer to a pool
    m_constantBuffers.emplace(id, constantBuffer);
    
    LogDebug( "renderingContext", "constant buffer %d of size %2.2fkb constructed\n", id, size / 1024.0f );
}

// ** RenderingContextHal::commandCreateTexture
void RenderingContextHal::commandCreateTexture(Texture_ id, u16 width, u16 height, const void* data, PixelFormat format)
{
    //NIMBLE_BREAK_IF( m_textures[id].valid(), "resource was already constructed" );
        
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
    m_textures.emplace(id, texture);
}

// ** RenderingContextHal::applyStates
PipelineFeatures RenderingContextHal::applyStates( const RenderFrame& frame, const StateBlock* const * states, s32 count )
{
    PipelineFeatures userFeatures = 0;
    PipelineFeatures userFeaturesMask = ~0;

    // This will start recording pipeline changes
    m_pipeline.beginStateBlock();

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
    PipelineFeatures userDefined = PipelineFeature::mask(userFeatures & userFeaturesMask);
    
    // Apply a user defined feature mask
    m_pipeline.activateUserFeatures(userDefined);
    
    // Finish applying state blocks
    m_pipeline.endStateBlock();
    
    //return features;
    return m_pipeline.features();
}
    
// ** RenderingContextHal::activateShaderPermutation
void RenderingContextHal::activateShaderPermutation( PipelineFeatures features )
{
    NIMBLE_ABORT_IF(!m_pipeline.program(), "no valid program set");
    NIMBLE_ABORT_IF(!m_pipeline.featureLayout(), "no valid feature layout set");
    
    // Do we have any pipeline changes that may invalidate an active program permutation?
    if (m_pipeline.changes())
    {
        // Get an active program handle
        Program program = m_pipeline.program();
        
        // Now compile a program permutation
        m_activeProgram = compileShaderPermutation(m_shaders[program], m_pipeline.features(), m_pipeline.featureLayout());
        
        // Accept these changes
        m_pipeline.acceptChanges();
    }
    
    // Bind an active shader permutation
    m_hal->setShader(m_activeProgram);
}
    
// ** RenderingContextHal::compileShaderPermutation
ShaderPtr RenderingContextHal::compileShaderPermutation( UbershaderPtr shader, PipelineFeatures features, const PipelineFeatureLayout* featureLayout )
{
    // First cancel all features that are not supported by a shader
    //features = features & m_supportedFeatures;
    
    // Now lookup a permutation cache
    ShaderPtr permutation = shader->findPermutation(features);
    
    if (permutation.valid())
    {
        return permutation;
    }
    
    // Generate macro definitions from features
    String macro = "";
    String debug = "";
    
    for( u32 i = 0, n = featureLayout->elementCount(); i < n; i++ ) {
        const PipelineFeatureLayout::Element& element = featureLayout->elementAt(i);
        
        if( element.mask & features )
        {
            macro += "#define " + element.name + " " + toString( (element.mask & features) >> element.offset ) + "\n";
            if( debug.length() ) debug += ", ";
            debug += element.name;
        }
    }
    
    LogVerbose( "shader", "compiling permutation %s\n", debug.empty() ? "" : ("(" + debug + ")").c_str() );
    
    // Includes
    //for( s32 i = 0, n = static_cast<s32>( m_includes.size() ); i < n; i++ ) {
    //    macro += m_includes[i];
    //}
    
    // Compile the shader
    ShaderPtr compiled = m_hal->createShader( (macro + shader->vertex()).c_str(), (macro + shader->fragment()).c_str() );
    NIMBLE_BREAK_IF( !compiled.valid() );
    
    shader->addPermutation(features, compiled);
    
    return compiled;
}

// ** RenderingContextHal::switchAlphaTest
void RenderingContextHal::switchAlphaTest( const RenderFrame& frame, const State& state )
{
    m_hal->setAlphaTest( static_cast<Renderer::Compare>( state.compareFunction ), state.data.alphaReference / 255.0f );
}

// ** RenderingContextHal::switchDepthState
void RenderingContextHal::switchDepthState( const RenderFrame& frame, const State& state )
{
    m_hal->setDepthTest( state.data.depthWrite, static_cast<Renderer::Compare>( state.compareFunction ) );
}

// ** RenderingContextHal::switchBlending
void RenderingContextHal::switchBlending( const RenderFrame& frame, const State& state )
{
    m_hal->setBlendFactors( static_cast<Renderer::BlendFactor>( state.data.blend >> 4 ), static_cast<Renderer::BlendFactor>( state.data.blend & 0xF ) );
}

// ** RenderingContextHal::switchShader
void RenderingContextHal::switchShader( const RenderFrame& frame, const State& state )
{
    m_pipeline.setProgram(state.resourceId);
}

// ** RenderingContextHal::switchConstantBuffer
void RenderingContextHal::switchConstantBuffer( const RenderFrame& frame, const State& state )
{
    const Renderer::ConstantBufferPtr& constantBuffer = m_constantBuffers[state.resourceId];
    m_hal->setConstantBuffer( constantBuffer, state.data.index );

    // Update resource features
    m_pipeline.activateConstantBuffer(state.data.index);
}

// ** RenderingContextHal::switchVertexBuffer
void RenderingContextHal::switchVertexBuffer( const RenderFrame& frame, const State& state )
{
    const Renderer::VertexBufferPtr& vertexBuffer = m_vertexBuffers[state.resourceId];
    NIMBLE_ABORT_IF(!vertexBuffer.valid(), "invalid resource id");
    m_hal->setVertexBuffer( vertexBuffer );
}

// ** RenderingContextHal::switchIndexBuffer
void RenderingContextHal::switchIndexBuffer( const RenderFrame& frame, const State& state )
{
    const Renderer::IndexBufferPtr& indexBuffer = m_indexBuffers[state.resourceId];
    NIMBLE_ABORT_IF(!indexBuffer.valid(), "invalid resource id");
    m_hal->setIndexBuffer( indexBuffer );
}

// ** RenderingContextHal::switchInputLayout
void RenderingContextHal::switchInputLayout( const RenderFrame& frame, const State& state )
{
    // Bind an input layout
    const VertexBufferLayout* inputLayout = m_inputLayouts[state.resourceId].get();
    NIMBLE_ABORT_IF(!inputLayout, "invalid resource id");
    m_hal->setInputLayout( inputLayout );

    // Update an input layout features
    m_pipeline.activateVertexAttributes(inputLayout->features());
}
    
// ** RenderingContextHal::switchPipelineFeatureLayout
void RenderingContextHal::switchPipelineFeatureLayout( const RenderFrame& frame, const State& state )
{
    // Get a feature layout by it's id
    const PipelineFeatureLayoutUPtr& featureLayout = m_pipelineFeatureLayouts[state.resourceId];
    NIMBLE_ABORT_IF(!featureLayout.get(), "invalid resource id");
    
    // Make this feature layout active
    m_pipeline.setFeatureLayout(featureLayout.get());
}

// ** RenderingContextHal::switchTexture
void RenderingContextHal::switchTexture( const RenderFrame& frame, const State& state )
{
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
        Renderer::Texture2DPtr texture = intermediateRenderTarget(intermediateTarget( -id ))->attachment( static_cast<Renderer::RenderTargetAttachment>( state.data.index >> 4 ) );
        NIMBLE_BREAK_IF( !texture.valid(), "invalid render target attachment" );
        m_hal->setTexture( samplerIndex, texture.get() );
    }

    // Update resource features
    m_pipeline.activateSampler(samplerIndex);
}

// ** RenderingContextHal::switchCullFace
void RenderingContextHal::switchCullFace( const RenderFrame& frame, const State& state )
{
    m_hal->setCulling( static_cast<Renderer::TriangleFace>( state.cullFace ) );
}

// ** RenderingContextHal::switchPolygonOffset
void RenderingContextHal::switchPolygonOffset( const RenderFrame& frame, const State& state )
{
    m_hal->setPolygonOffset( state.polygonOffset.factor / 128.0f, state.polygonOffset.units / 128.0f );
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

// ** RenderingContextHal::acquireRenderTarget
TransientRenderTarget RenderingContextHal::acquireRenderTarget( u16 width, u16 height, PixelFormat format )
{
    // Perform a linear search of a render target
    for( s32 i = 0, n = static_cast<s32>( m_renderTargets.size() ); i < n; i++ )
    {
        TransientRenderTarget_& intermediate = m_renderTargets[i];
        
        if( intermediate.isFree && intermediate.width == width && intermediate.height == height && intermediate.format == format )
        {
            intermediate.isFree = false;
            return i + 1;
        }
    }
    
    // Not found - create a new one
    TransientRenderTarget_ intermediate;
    intermediate.isFree         = false;
    intermediate.width          = width;
    intermediate.height         = height;
    intermediate.format         = format;
    intermediate.renderTarget   = m_hal->createRenderTarget( width, height );
    if( format == Renderer::PixelD24S8 || format == Renderer::PixelD24X8 )
    {
        intermediate.renderTarget->setAttachment( format, Renderer::RenderTargetDepth );
    }
    else
    {
        intermediate.renderTarget->setAttachment( format, Renderer::RenderTargetColor0 );
    }
    intermediate.renderTarget->setDepth( Renderer::PixelD24X8 );
    m_renderTargets.push_back( intermediate );
    
    LogVerbose( "renderingContext", "%dx%d render target created\n", width, height );
    
    return m_renderTargets.size();
}

// ** RenderingContextHal::releaseRenderTarget
void RenderingContextHal::releaseRenderTarget( TransientRenderTarget id )
{
    m_renderTargets[static_cast<s32>(id) - 1].isFree = true;
}

// ** RenderingContextHal::intermediateRenderTarget
RenderTargetWPtr RenderingContextHal::intermediateRenderTarget( TransientRenderTarget id ) const
{
    return m_renderTargets[static_cast<s32>(id) - 1].renderTarget;
}

} // namespace Renderer

DC_END_DREEMCHEST
