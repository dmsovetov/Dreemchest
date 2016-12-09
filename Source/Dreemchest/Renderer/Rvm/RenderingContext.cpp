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
#include "VertexFormat.h"
#include "RenderState.h"
#include "Ubershader.h"
#include "../../Io/DiskFileSystem.h"
#include "../Hal.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

#if DEV_DEPRECATED_HAL
// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( Renderer::HalWPtr hal )
    : m_hal( hal )
{
    memset( m_inputLayouts, 0, sizeof m_inputLayouts );
}

// ** RenderingContext::hal
Renderer::HalWPtr RenderingContext::hal( void ) const
{
    return m_hal;
}
#else
// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( void )
{
    memset( m_inputLayouts, 0, sizeof m_inputLayouts );
}
#endif  /*  #if DEV_DEPRECATED_HAL  */

// ** RenderingContext::constructResources
void RenderingContext::constructResources( void )
{
    // A table of construction functions
    static ResourceConstructor::Function kConstructors[ResourceConstructor::TotalConstructors + 1] =
    {
          &RenderingContext::constructInputLayout
        , &RenderingContext::constructVertexBuffer
        , &RenderingContext::constructIndexBuffer
        , &RenderingContext::constructConstantBuffer
        , &RenderingContext::constructTexture
        , NULL
    };

    // Construct all resources
    for( ResourceConstructors::const_iterator i = m_resourceConstructors.begin(), end = m_resourceConstructors.end(); i != end; ++i )
    {
        NIMBLE_ABORT_IF( kConstructors[i->type] == NULL, "unhandled resource constructor type" );
        ( this->*kConstructors[i->type] )( *i );
    }

    // Clear a resource constructor list
    m_resourceConstructors.clear();
}

// ** RenderingContext::constructInputLayout
void RenderingContext::constructInputLayout( const ResourceConstructor& constructor )
{
#if DEV_DEPRECATED_HAL
    NIMBLE_BREAK_IF( m_inputLayoutPool[constructor.id - 1].valid(), "resource was already constructed" );

    // Create an input layout vertex format
    VertexFormat format( constructor.inputLayout.format );

    // Create an input layout
    InputLayoutPtr inputLayout = m_hal->createInputLayout( format.vertexSize() );

    // Add vertex attributes to an input layout
    if( format & VertexFormat::Position )
    {
        inputLayout->attributeLocation( InputLayout::Position, 3, format.attributeOffset( VertexFormat::Position ) );
    }
    if( format & VertexFormat::Color )
    {
        inputLayout->attributeLocation( InputLayout::Color, 4, format.attributeOffset( VertexFormat::Color ) );
    }
    if( format & VertexFormat::Normal )
    {
        inputLayout->attributeLocation( InputLayout::Normal, 3, format.attributeOffset( VertexFormat::Normal ) );
    }
    if( format & VertexFormat::Uv0 )
    {
        inputLayout->attributeLocation( InputLayout::Uv0, 2, format.attributeOffset( VertexFormat::Uv0 ) );
    }
    if( format & VertexFormat::Uv1 )
    {
        inputLayout->attributeLocation( InputLayout::Uv1, 2, format.attributeOffset( VertexFormat::Uv1 ) );
    }

    // Save an input layout to a pool
    m_inputLayoutPool[constructor.id - 1] = inputLayout;
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::constructVertexBuffer
void RenderingContext::constructVertexBuffer( const ResourceConstructor& constructor )
{
#if DEV_DEPRECATED_HAL
    NIMBLE_BREAK_IF( m_vertexBufferPool[constructor.id - 1].valid(), "resource was already constructed" );

    // Create a vertex buffer instance
    VertexBufferPtr vertexBuffer = m_hal->createVertexBuffer( constructor.buffer.size );

    // Upload data to a GPU buffer
    if( constructor.buffer.data )
    {
        memcpy( vertexBuffer->lock(), constructor.buffer.data, constructor.buffer.size );
        vertexBuffer->unlock();
    }

    // Save a vertex buffer to a pool
    m_vertexBufferPool[constructor.id - 1] = vertexBuffer;

    LogDebug( "renderingContext", "vertex buffer %d of size %2.2fkb constructed\n", constructor.id, constructor.buffer.size / 1024.0f );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::constructIndexBuffer
void RenderingContext::constructIndexBuffer( const ResourceConstructor& constructor )
{
#if DEV_DEPRECATED_HAL
    // Create an index buffer instance
    IndexBufferPtr indexBuffer = m_hal->createIndexBuffer( constructor.buffer.size );

    // Upload data to a GPU buffer
    if( constructor.buffer.data )
    {
        memcpy( indexBuffer->lock(), constructor.buffer.data, constructor.buffer.size );
        indexBuffer->unlock();
    }

    // Save a index buffer to a pool
    m_indexBufferPool[constructor.id - 1] = indexBuffer;

    LogDebug( "renderingContext", "index buffer %d of size %2.2fkb constructed\n", constructor.id, constructor.buffer.size / 1024.0f );
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::constructConstantBuffer
void RenderingContext::constructConstantBuffer( const ResourceConstructor& constructor )
{
#if DEV_DEPRECATED_HAL
    ConstantBufferPtr constantBuffer = m_hal->createConstantBuffer( constructor.buffer.size, reinterpret_cast<const ConstantBufferLayout*>( constructor.buffer.userData ) );

    // Upload data to a GPU buffer
    if( constructor.buffer.data )
    {
        memcpy( constantBuffer->lock(), constructor.buffer.data, constructor.buffer.size );
        constantBuffer->unlock();
    }

    // Save a index buffer to a pool
    m_constantBufferPool[constructor.id - 1] = constantBuffer;
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::constructTexture
void RenderingContext::constructTexture( const ResourceConstructor& constructor )
{
#if DEV_DEPRECATED_HAL
    Texture2DPtr texture = m_hal->createTexture2D( constructor.texture.width, constructor.texture.height, constructor.texture.format );

    // Upload data to a GPU buffer
    if( constructor.texture.data )
    {
        u32 size;
        void* ptr = texture->lock( 0, size );
        memcpy( ptr, constructor.texture.data, size );
        texture->unlock();
    }

    // Save a index buffer to a pool
    m_texturePool[constructor.id - 1] = texture;
#else
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::requestInputLayout
RenderId RenderingContext::requestInputLayout( const VertexFormat& format )
{
#if DEV_DEPRECATED_HAL
    // First lookup a previously constucted input layout
    RenderId id = m_inputLayouts[format];
    if( id )
    {
        return id;
    }

    // Nothing found - construct a new one
    ResourceConstructor constructor = ResourceConstructor::InputLayout;
    constructor.id      = m_inputLayoutPool.push( NULL ) + 1;
    constructor.inputLayout.format = format;
    m_resourceConstructors.push_back( constructor );

    // Save this input layout id in a lookup table
    m_inputLayouts[format] = constructor.id;

    return constructor.id;
#else
    NIMBLE_NOT_IMPLEMENTED
    return 0;
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::requestVertexBuffer
RenderId RenderingContext::requestVertexBuffer( const void* data, s32 size )
{
#if DEV_DEPRECATED_HAL
    ResourceConstructor constructor = ResourceConstructor::VertexBuffer;
    constructor.id          = m_vertexBufferPool.push( NULL ) + 1;
    constructor.buffer.size = size;
    constructor.buffer.data = data;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
#else
    NIMBLE_NOT_IMPLEMENTED
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::requestIndexBuffer
RenderId RenderingContext::requestIndexBuffer( const void* data, s32 size )
{
#if DEV_DEPRECATED_HAL
    ResourceConstructor constructor = ResourceConstructor::IndexBuffer;
    constructor.id          = m_indexBufferPool.push( NULL ) + 1;
    constructor.buffer.size = size;
    constructor.buffer.data = data;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
#else
    NIMBLE_NOT_IMPLEMENTED
    return 0;
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::requestConstantBuffer
RenderId RenderingContext::requestConstantBuffer( const void* data, s32 size, const Renderer::ConstantBufferLayout* layout )
{
#if DEV_DEPRECATED_HAL
    ResourceConstructor constructor = ResourceConstructor::ConstantBuffer;
    constructor.id          = m_constantBufferPool.push( NULL ) + 1;
    constructor.buffer.size = size;
    constructor.buffer.data = data;
    constructor.buffer.userData = layout;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
#else
    NIMBLE_NOT_IMPLEMENTED
    return 0;
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::requestConstantBuffer
RenderId RenderingContext::requestTexture( const void* data, s32 width, s32 height, Renderer::PixelFormat format )
{
#if DEV_DEPRECATED_HAL
    ResourceConstructor constructor = ResourceConstructor::Texture;
    constructor.id          = m_texturePool.push( NULL ) + 1;
    constructor.texture.data = data;
    constructor.texture.width = width;
    constructor.texture.height = height;
    constructor.texture.format = format;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
#else
    NIMBLE_NOT_IMPLEMENTED
    return 0;
#endif  /*  #if DEV_DEPRECATED_HAL  */
}

// ** RenderingContext::createShader
UbershaderPtr RenderingContext::createShader( const String& fileName ) const
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
            shader->addFeature( masks[value[1]], value[0] );
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
}

// ** RenderingContext::acquireRenderTarget
RenderId RenderingContext::acquireRenderTarget( u16 width, u16 height, PixelFormat format )
{
    // Perform a linear search of a render target
    for( s32 i = 0, n = static_cast<s32>( m_renderTargets.size() ); i < n; i++ )
    {
        IntermediateRenderTarget& intermediate = m_renderTargets[i];
        
        if( intermediate.isFree && intermediate.width == width && intermediate.height == height && intermediate.format == format )
        {
            intermediate.isFree = false;
            return i + 1;
        }
    }

#if DEV_DEPRECATED_HAL
    // Not found - create a new one
    IntermediateRenderTarget intermediate;
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
void RenderingContext::releaseRenderTarget( RenderId id )
{
    m_renderTargets[id - 1].isFree = true;
}

#if DEV_DEPRECATED_HAL
// ** RenderingContext::intermediateRenderTarget
RenderTargetWPtr RenderingContext::intermediateRenderTarget( RenderId id ) const
{
    return m_renderTargets[id - 1].renderTarget;
}

// ** RenderingContext::vertexBuffer
const Renderer::VertexBufferPtr& RenderingContext::vertexBuffer( RenderId identifier ) const
{
    NIMBLE_ABORT_IF( identifier <= 0, "invalid identifier" );
    return m_vertexBufferPool[identifier - 1];
}
        
// ** RenderingContext::indexBuffer
const IndexBufferPtr& RenderingContext::indexBuffer( RenderId identifier ) const
{
    NIMBLE_ABORT_IF( identifier <= 0, "invalid identifier" );
    return m_indexBufferPool[identifier - 1];
}

// ** RenderingContext::constantBuffer
const ConstantBufferPtr& RenderingContext::constantBuffer( RenderId identifier ) const
{
    NIMBLE_ABORT_IF( identifier <= 0, "invalid identifier" );
    return m_constantBufferPool[identifier - 1];
}

// ** RenderingContext::inputLayout
const InputLayoutPtr& RenderingContext::inputLayout( RenderId identifier ) const
{
    NIMBLE_ABORT_IF( identifier <= 0, "invalid identifier" );
    return m_inputLayoutPool[identifier - 1];
}
        
// ** RenderingContext::texture
const TexturePtr& RenderingContext::texture( s32 identifier ) const
{
    NIMBLE_ABORT_IF( identifier <= 0, "invalid identifier" );
    return m_texturePool[identifier - 1];
}
    
// ** RenderingContext::internShader
s32 RenderingContext::internShader( UbershaderPtr shader )
{
    return m_shaders.add( shader );
}

// ** RenderingContext::shader
const UbershaderPtr& RenderingContext::shader( s32 identifier ) const
{
    return m_shaders.resolve( identifier );
}
#else
// ** RenderingContext::internShader
s32 RenderingContext::internShader( UbershaderPtr shader )
{
    return 0;
}
#endif  /*  #if DEV_DEPRECATED_HAL  */

#if DEV_DEPRECATED_HAL
// ** RenderingContext::create
RenderingContextPtr RenderingContext::create( HalWPtr hal )
{
    return DC_NEW RenderingContext( hal );
}
#else
// ** RenderingContext::create
RenderingContextPtr RenderingContext::create( void )
{
    return DC_NEW RenderingContext;
}
#endif  /*  #if DEV_DEPRECATED_HAL  */

} // namespace Scene

DC_END_DREEMCHEST
