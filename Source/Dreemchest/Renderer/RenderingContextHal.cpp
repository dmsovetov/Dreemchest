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
#include "Commands/CommandBuffer.h"
#include "RenderFrame.h"
#include "RenderingContext.h"
#include "VertexFormat.h"
#include "VertexBufferLayout.h"
#include "Hal.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
// ** createDeprecatedRenderingContext
RenderingContextPtr createDeprecatedRenderingContext(RenderViewPtr view, HalPtr hal)
{
    return DC_NEW RenderingContextHal(view, hal);
}

// ** RenderingContextHal::RenderingContextHal
RenderingContextHal::RenderingContextHal(RenderViewPtr view, HalPtr hal)
    : RenderingContext(view)
    , m_hal(hal)
{
    // Reset all state switchers
    memset( m_stateSwitches, 0, sizeof( m_stateSwitches ) );

    // Setup render state switchers
    m_stateSwitches[State::AlphaTest]           = &RenderingContextHal::switchAlphaTest;
    m_stateSwitches[State::DepthState]          = &RenderingContextHal::switchDepthState;
    m_stateSwitches[State::Blending]            = &RenderingContextHal::switchBlending;
    m_stateSwitches[State::BindProgram]         = &RenderingContextHal::switchShader;
    m_stateSwitches[State::BindConstantBuffer]  = &RenderingContextHal::switchConstantBuffer;
    m_stateSwitches[State::BindVertexBuffer]    = &RenderingContextHal::switchVertexBuffer;
    m_stateSwitches[State::BindIndexBuffer]     = &RenderingContextHal::switchIndexBuffer;
    m_stateSwitches[State::SetInputLayout]      = &RenderingContextHal::switchInputLayout;
    m_stateSwitches[State::SetFeatureLayout]    = &RenderingContextHal::switchPipelineFeatureLayout;
    m_stateSwitches[State::BindTexture]         = &RenderingContextHal::switchTexture;
    m_stateSwitches[State::CullFace]            = &RenderingContextHal::switchCullFace;
    m_stateSwitches[State::PolygonOffset]       = &RenderingContextHal::switchPolygonOffset;
}

// ** RenderingContextHal::renderToTarget
void RenderingContextHal::renderToTarget( const RenderFrame& frame, u8 renderTarget, const NormalizedViewport* viewport, const CommandBuffer& commands )
{
#if 0
    Rect prevViewport;
    m_hal->getViewport(prevViewport);
    
    // Push a render target state
    if( renderTarget )
    {
        const Renderer::RenderTargetWPtr rt = intermediateRenderTarget(transientTarget( renderTarget ));
        m_hal->setRenderTarget( rt );
        m_hal->setViewport( viewport->x * rt->width(), viewport->y * rt->height(), viewport->width * rt->width(), viewport->height * rt->height() );
    }
    else
    {
        //!! denormalize the viewport here
        m_hal->setViewport(viewport->x, viewport->y, viewport->width, viewport->height);
    }

    // Execute an attached command buffer
    execute( frame, commands );

    // Pop a render target state
    if( renderTarget )
    {
        m_hal->setRenderTarget( NULL );
    }

    // Pop a viewport
    m_hal->setViewport(prevViewport);
#endif  //  #if 0
}

// ** RenderingContextHal::executeCommandBuffer
void RenderingContextHal::executeCommandBuffer( const RenderFrame& frame, const CommandBuffer& commands )
{
/*    for( s32 i = 0, n = commands.size(); i < n; i++ )
    {
        // Get a render operation at specified index
        const OpCode& opCode = commands.opCodeAt( i );

        // Perform a draw call
        switch( opCode.type ) {
        case CommandBuffer::OpCode::Clear:                  clear( opCode.clear.color, opCode.clear.depth, opCode.clear.stencil, opCode.clear.mask );
                                                            break;
        case CommandBuffer::OpCode::Execute:                execute( frame, *opCode.execute.commands );
                                                            break;
        case CommandBuffer::OpCode::UploadConstantBuffer:   commandUploadConstantBuffer( opCode.upload.id, opCode.upload.buffer.data, opCode.upload.buffer.size );
                                                            break;
        case CommandBuffer::OpCode::UploadVertexBuffer:     commandUploadVertexBuffer( opCode.upload.id, opCode.upload.buffer.data, opCode.upload.buffer.size );
                                                            break;
        case CommandBuffer::OpCode::CreateInputLayout:      commandCreateInputLayout(opCode.createInputLayout.id, opCode.createInputLayout.format);
                                                            break;
        case CommandBuffer::OpCode::CreateTexture:          commandCreateTexture(opCode.createTexture.id, opCode.createTexture.width, opCode.createTexture.height, opCode.createTexture.buffer.data, opCode.createTexture.format);
                                                            break;
        case CommandBuffer::OpCode::CreateIndexBuffer:      commandCreateIndexBuffer(opCode.createBuffer.id, opCode.createBuffer.buffer.data, opCode.createBuffer.buffer.size);
                                                            break;
        case CommandBuffer::OpCode::CreateVertexBuffer:     commandCreateVertexBuffer(opCode.createBuffer.id, opCode.createBuffer.buffer.data, opCode.createBuffer.buffer.size);
                                                            break;
        case CommandBuffer::OpCode::CreateConstantBuffer:   {
                                                                commandCreateConstantBuffer(opCode.createBuffer.id, opCode.createBuffer.buffer.data, opCode.createBuffer.buffer.size, opCode.createBuffer.layout);
                                                            }
                                                            break;
        case CommandBuffer::OpCode::RenderToTransientTexture: renderToTarget( frame, opCode.renderToTextures.id, &opCode.renderToTextures.viewport, *opCode.renderToTextures.commands );
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
    }*/
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
    
    // Create an input layout
    VertexBufferLayoutUPtr inputLayout = createVertexBufferLayout(vertexFormat);
    
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
void RenderingContextHal::commandCreateConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, UniformLayout layout)
{
    //NIMBLE_BREAK_IF( m_constantBuffers[id].valid(), "resource was already constructed" );
        
    ConstantBufferPtr constantBuffer = m_hal->createConstantBuffer( size, &m_uniformLayouts[layout][0]);
    
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
PipelineFeatures RenderingContextHal::applyStates( const RenderFrame& frame, const StateBlock* const * stateBlocks, s32 count )
{
/*    State states[MaxStateChanges];
    PipelineFeatures userDefined;
    
    // This will notify a pipeline that we started the stage change process
    s32 stateCount = startPipelineConfiguration(stateBlocks, count, states, MaxStateChanges, userDefined);
    
    // Apply all states
    for (s32 i = 0; i < stateCount; i++)
    {
        // Get a state at specified index
        const State& state = states[i];
        
        // Invoke a state handler function
        NIMBLE_ABORT_IF( m_stateSwitches[state.type] == NULL, "unhandled render state type" );
        (this->*m_stateSwitches[state.type])(frame, state);
    }
    
    return finishPipelineConfiguration(userDefined);*/
    return 0;
}
    
// ** RenderingContextHal::activateShaderPermutation
void RenderingContextHal::activateShaderPermutation( PipelineFeatures features )
{
/*    // Get an active program
    Program program = m_pipeline.program();
    NIMBLE_ABORT_IF(!program && !m_defaultProgram, "no valid program set and no default one specified");
    
    // Use a default program if nothing was set by a user
    if (!program)
    {
        program = m_defaultProgram;
    }
    
    // Do we have any pipeline changes that may invalidate an active program permutation?
    if (m_pipeline.changes())
    {
        // Now compile a program permutation
        m_activeProgram = compileShaderPermutation(program, m_pipeline.features(), m_pipeline.featureLayout());
        
        // Accept these changes
        m_pipeline.acceptChanges();
    }
    
    // Bind an active shader permutation
    NIMBLE_ABORT_IF(!m_activeProgram.valid(), "no valie shader permutation set");
    m_hal->setShader(m_activeProgram);*/
}
    
// ** RenderingContextHal::compileShaderPermutation
ShaderPtr RenderingContextHal::compileShaderPermutation(Program program, PipelineFeatures features, const PipelineFeatureLayout* featureLayout)
{
    // Get a shader program descriptor and corresponding permutations container
    const ShaderProgramDescriptor& descriptor = m_programs[program];
    
    if (static_cast<s32>(program) >= m_permutations.count())
    {
        m_permutations.emplace(program, ProgramPermutations());
    }
    ProgramPermutations& permutations = m_permutations[program];
    
    // Now lookup a permutation cache
    ProgramPermutations::const_iterator permutation = permutations.find(features);
    
    if (permutation != permutations.end())
    {
        return permutation->second;
    }

    // Get a shader source code
    String shaderSourceCode[TotalShaderTypes];
    m_shaderLibrary.generateShaderCode(m_programs[program], features, featureLayout, shaderSourceCode);
 
    // Compile the shader
    ShaderPtr compiled = m_hal->createShader( shaderSourceCode[VertexShaderType].c_str(), shaderSourceCode[FragmentShaderType].c_str() );
    NIMBLE_BREAK_IF( !compiled.valid() );
    
    // Put it to a cache
    permutations[features] = compiled;
    
    return compiled;
}

// ** RenderingContextHal::switchAlphaTest
void RenderingContextHal::switchAlphaTest( const RenderFrame& frame, const State& state )
{
    m_hal->setAlphaTest(state.function(), state.data.ref / 255.0f );
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
    //m_pipeline.activateConstantBuffer(state.data.index);
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
    //m_pipeline.activateVertexAttributes(inputLayout->features());
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
    u8 samplerIndex = state.samplerIndex();

    // Bind a texture to sampler
    if( id >= 0 )
    {
        const Renderer::TexturePtr& texture = m_textures[state.resourceId];
        m_hal->setTexture( state.data.index, texture.get() );
    }
    else
    {
    #if 0
        NIMBLE_BREAK_IF( abs( id ) > 255, "invalid identifier" );
        Renderer::Texture2DPtr texture = intermediateRenderTarget(transientTarget( -id ))->attachment(state.attachmentIndex());
        NIMBLE_BREAK_IF( !texture.valid(), "invalid render target attachment" );
        m_hal->setTexture( samplerIndex, texture.get() );
    #endif  //  #if 0
    }

    // Update resource features
    //m_pipeline.activateSampler(samplerIndex);
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

#if 0
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
        intermediate.renderTarget->setAttachment( format, RenderTargetAttachment::Depth );
    }
    else
    {
        intermediate.renderTarget->setAttachment( format, RenderTargetAttachment::Color0 );
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
#endif  //  #if 0

} // namespace Renderer

DC_END_DREEMCHEST
