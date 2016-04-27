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

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( Renderer::HalWPtr hal )
    : m_hal( hal )
{
}

// ** RenderingContext::hal
Renderer::HalWPtr RenderingContext::hal( void ) const
{
    return m_hal;
}

// ** RenderingContext::constructResources
void RenderingContext::constructResources( void )
{
    // A table of construction functions
    static ResourceConstructor::Function kConstructors[ResourceConstructor::TotalConstructors + 1] = {
          &RenderingContext::constructInputLayout
        , &RenderingContext::constructVertexBuffer
        , &RenderingContext::constructIndexBuffer
        , &RenderingContext::constructConstantBuffer
        , &RenderingContext::constructTexture
        , NULL
    };

    // Construct all resources
    for( ResourceConstructors::const_iterator i = m_resourceConstructors.begin(), end = m_resourceConstructors.end(); i != end; ++i ) {
        DC_ABORT_IF( kConstructors[i->type] == NULL, "unhandled resource constructor type" );
        ( this->*kConstructors[i->type] )( *i );
    }

    // Clear a resource constructor list
    m_resourceConstructors.clear();
}

// ** RenderingContext::constructInputLayout
void RenderingContext::constructInputLayout( const ResourceConstructor& constructor )
{
    DC_BREAK_IF( m_inputLayoutPool[constructor.id - 1].valid(), "resource was already constructed" );

    // Create an input layout vertex format
    VertexFormat format( constructor.inputLayout.format );

    // Create an input layout
    Renderer::InputLayoutPtr inputLayout = m_hal->createInputLayout( format.vertexSize() );

    // Add vertex attributes to an input layout
    if( format & VertexFormat::Position ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Position, 3, format.attributeOffset( VertexFormat::Position ) );
    }
    if( format & VertexFormat::Color ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Color, 4, format.attributeOffset( VertexFormat::Color ) );
    }
    if( format & VertexFormat::Normal ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Normal, 3, format.attributeOffset( VertexFormat::Normal ) );
    }
    if( format & VertexFormat::Uv0 ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Uv0, 2, format.attributeOffset( VertexFormat::Uv0 ) );
    }
    if( format & VertexFormat::Uv1 ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Uv1, 2, format.attributeOffset( VertexFormat::Uv1 ) );
    }

    // Save an input layout to a pool
    m_inputLayoutPool[constructor.id - 1] = inputLayout;
}

// ** RenderingContext::constructVertexBuffer
void RenderingContext::constructVertexBuffer( const ResourceConstructor& constructor )
{
    DC_BREAK_IF( m_vertexBufferPool[constructor.id - 1].valid(), "resource was already constructed" );

    // Create a vertex buffer instance
    Renderer::VertexBufferPtr vertexBuffer = m_hal->createVertexBuffer( constructor.buffer.size );

    // Upload data to a GPU buffer
    if( constructor.buffer.data ) {
        memcpy( vertexBuffer->lock(), constructor.buffer.data, constructor.buffer.size );
        vertexBuffer->unlock();
    }

    // Save a vertex buffer to a pool
    m_vertexBufferPool[constructor.id - 1] = vertexBuffer;
}

// ** RenderingContext::constructIndexBuffer
void RenderingContext::constructIndexBuffer( const ResourceConstructor& constructor )
{
    // Create an index buffer instance
    Renderer::IndexBufferPtr indexBuffer = m_hal->createIndexBuffer( constructor.buffer.size );

    // Upload data to a GPU buffer
    if( constructor.buffer.data ) {
        memcpy( indexBuffer->lock(), constructor.buffer.data, constructor.buffer.size );
        indexBuffer->unlock();
    }

    // Save a index buffer to a pool
    m_indexBufferPool[constructor.id - 1] = indexBuffer;
}

// ** RenderingContext::constructConstantBuffer
void RenderingContext::constructConstantBuffer( const ResourceConstructor& constructor )
{
    Renderer::ConstantBufferPtr constantBuffer = m_hal->createConstantBuffer( constructor.buffer.size, reinterpret_cast<const Renderer::ConstantBufferLayout*>( constructor.buffer.userData ) );

    // Upload data to a GPU buffer
    if( constructor.buffer.data ) {
        memcpy( constantBuffer->lock(), constructor.buffer.data, constructor.buffer.size );
        constantBuffer->unlock();
    }

    // Save a index buffer to a pool
    m_constantBufferPool[constructor.id - 1] = constantBuffer;
}

void RenderingContext::constructTexture( const ResourceConstructor& constructor )
{
    Renderer::Texture2DPtr texture = m_hal->createTexture2D( constructor.texture.width, constructor.texture.height, constructor.texture.channels == 3 ? Renderer::PixelRgb8 : Renderer::PixelRgba8 );

    // Upload data to a GPU buffer
    if( constructor.texture.data ) {
        u32 size;
        void* ptr = texture->lock( 0, size );
        memcpy( ptr, constructor.texture.data, size );
        texture->unlock();
    }

    // Save a index buffer to a pool
    m_texturePool[constructor.id - 1] = texture;
}

// ** RenderingContext::requestInputLayout
RenderResource RenderingContext::requestInputLayout( const VertexFormat& format )
{
    ResourceConstructor constructor = ResourceConstructor::InputLayout;
    constructor.id      = m_inputLayoutPool.push( NULL ) + 1;
    constructor.inputLayout.format = format;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
}

// ** RenderingContext::requestVertexBuffer
RenderResource RenderingContext::requestVertexBuffer( const void* data, s32 size )
{
    ResourceConstructor constructor = ResourceConstructor::VertexBuffer;
    constructor.id          = m_vertexBufferPool.push( NULL ) + 1;
    constructor.buffer.size = size;
    constructor.buffer.data = data;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
}

// ** RenderingContext::requestIndexBuffer
RenderResource RenderingContext::requestIndexBuffer( const void* data, s32 size )
{
    ResourceConstructor constructor = ResourceConstructor::IndexBuffer;
    constructor.id          = m_indexBufferPool.push( NULL ) + 1;
    constructor.buffer.size = size;
    constructor.buffer.data = data;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
}

// ** RenderingContext::requestConstantBuffer
RenderResource RenderingContext::requestConstantBuffer( const void* data, s32 size, const Renderer::ConstantBufferLayout* layout )
{
    ResourceConstructor constructor = ResourceConstructor::ConstantBuffer;
    constructor.id          = m_constantBufferPool.push( NULL ) + 1;
    constructor.buffer.size = size;
    constructor.buffer.data = data;
    constructor.buffer.userData = layout;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
}

// ** RenderingContext::requestConstantBuffer
RenderResource RenderingContext::requestTexture( const void* data, s32 width, s32 height, s32 channels )
{
    ResourceConstructor constructor = ResourceConstructor::Texture;
    constructor.id          = m_texturePool.push( NULL ) + 1;
    constructor.texture.data = data;
    constructor.texture.width = width;
    constructor.texture.height = height;
    constructor.texture.channels = channels;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
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
    masks["pointLight"]     = static_cast<u64>( ShaderPointLight )    << UserDefinedFeaturesOffset;
    masks["linearDepth"]    = static_cast<u64>( ShaderLinearDepth )   << UserDefinedFeaturesOffset;

    // Create a shader instance
    UbershaderPtr shader = DC_NEW Ubershader;

	// Read the code from an input stream
	String code = Io::DiskFileSystem::readTextFile( fileName );

	// Extract vertex/fragment shader code blocks
	u32 vertexBegin = code.find( vertexShaderMarker );
	u32 fragmentBegin = code.find( fragmentShaderMarker );
    u32 featuresBegin = code.find( featuresMarker );

	if( vertexBegin == String::npos && fragmentBegin == String::npos ) {
		return false;
	}

    if( featuresBegin != String::npos ) {
        u32 featuresCodeStart = featuresBegin + strlen( featuresMarker );
        Array<String> features = split( code.substr( featuresCodeStart, vertexBegin - featuresCodeStart ), "\r\n" );

        for( Array<String>::const_iterator i = features.begin(), end = features.end(); i != end; ++i ) {
            Array<String> value = split( *i, " \t=" );
            shader->addFeature( masks[value[1]], value[0] );
            LogVerbose( "shader", "feature %s = %s (0x%x) added\n", value[0].c_str(), value[1].c_str(), masks[value[1]] );
        }
    }

	if( vertexBegin != String::npos ) {
		u32 vertexCodeStart = vertexBegin + strlen( vertexShaderMarker );
		String vertex = code.substr( vertexCodeStart, fragmentBegin > vertexBegin ? fragmentBegin - vertexCodeStart : String::npos );
        shader->setVertex( vertex );
	}

	if( fragmentBegin != String::npos ) {
		u32 fragmentCodeStart = fragmentBegin + strlen( fragmentShaderMarker );
		String fragment = code.substr( fragmentCodeStart, vertexBegin > fragmentBegin ? vertexBegin - fragmentCodeStart : String::npos );
        shader->setFragment( fragment );
	}

    // ----------------------------------------------------
    shader->addInclude(
            "                                                       \n\
                struct CBufferScene    { vec4 ambient; };           \n\
                struct CBufferView     { mat4 transform; float near; float far; };         \n\
                struct CBufferInstance { mat4 transform; };         \n\
                struct CBufferMaterial { vec4 diffuse; vec4 specular; vec4 emission; };         \n\
                struct CBufferLight    { vec3 position; float range; vec3 color; float intensity; };         \n\
                uniform CBufferScene    Scene;                      \n\
                uniform CBufferView     View;                       \n\
                uniform CBufferInstance Instance;                   \n\
                uniform CBufferMaterial Material;                   \n\
                uniform CBufferLight    Light;                      \n\
                #define u_DiffuseTexture Texture0                   \n\
            "
        );

	return shader;
}

// ** RenderingContext::internRenderTarget
s32 RenderingContext::internRenderTarget( RenderTargetPtr renderTarget )
{
    return m_renderTargets.add( renderTarget );
}

// ** RenderingContext::renderTarget
const RenderTargetPtr& RenderingContext::renderTarget( s32 identifier ) const
{
    return m_renderTargets.resolve( identifier );
}

// ** RenderingContext::vertexBuffer
const Renderer::VertexBufferPtr& RenderingContext::vertexBuffer( RenderResource identifier ) const
{
    DC_ABORT_IF( identifier <= 0, "invalid identifier" );
    return m_vertexBufferPool[identifier - 1];
}
        
// ** RenderingContext::indexBuffer
const Renderer::IndexBufferPtr& RenderingContext::indexBuffer( RenderResource identifier ) const
{
    DC_ABORT_IF( identifier <= 0, "invalid identifier" );
    return m_indexBufferPool[identifier - 1];
}

// ** RenderingContext::constantBuffer
const Renderer::ConstantBufferPtr& RenderingContext::constantBuffer( RenderResource identifier ) const
{
    DC_ABORT_IF( identifier <= 0, "invalid identifier" );
    return m_constantBufferPool[identifier - 1];
}

// ** RenderingContext::inputLayout
const Renderer::InputLayoutPtr& RenderingContext::inputLayout( RenderResource identifier ) const
{
    DC_ABORT_IF( identifier <= 0, "invalid identifier" );
    return m_inputLayoutPool[identifier - 1];
}
        
// ** RenderingContext::texture
const Renderer::TexturePtr& RenderingContext::texture( s32 identifier ) const
{
    DC_ABORT_IF( identifier <= 0, "invalid identifier" );
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

// ** RenderingContext::create
RenderingContextPtr RenderingContext::create( Renderer::HalWPtr hal )
{
    return DC_NEW RenderingContext( hal );
}

} // namespace Scene

DC_END_DREEMCHEST