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

// ** RenderingContext::createInputLayout
RenderResource RenderingContext::createInputLayout( const VertexFormat& format )
{
    ResourceConstructor constructor = ResourceConstructor::InputLayout;
    constructor.id      = m_inputLayoutPool.push( NULL ) + 1;
    constructor.inputLayout.format = format;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
}

// ** RenderingContext::createVertexBuffer
RenderResource RenderingContext::createVertexBuffer( const void* data, s32 size )
{
    ResourceConstructor constructor = ResourceConstructor::VertexBuffer;
    constructor.id          = m_vertexBufferPool.push( NULL ) + 1;
    constructor.buffer.size = size;
    constructor.buffer.data = data;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
}

// ** RenderingContext::createIndexBuffer
RenderResource RenderingContext::createIndexBuffer( const void* data, s32 size )
{
    ResourceConstructor constructor = ResourceConstructor::IndexBuffer;
    constructor.id          = m_indexBufferPool.push( NULL ) + 1;
    constructor.buffer.size = size;
    constructor.buffer.data = data;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
}

// ** RenderingContext::createConstantBuffer
RenderResource RenderingContext::createConstantBuffer( const void* data, s32 size, const Renderer::ConstantBufferLayout* layout )
{
    ResourceConstructor constructor = ResourceConstructor::ConstantBuffer;
    constructor.id          = m_constantBufferPool.push( NULL ) + 1;
    constructor.buffer.size = size;
    constructor.buffer.data = data;
    constructor.buffer.userData = layout;

    m_resourceConstructors.push_back( constructor );
    return constructor.id;
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

// ** RenderingContext::internTexture
s32 RenderingContext::internTexture( Renderer::TexturePtr texture )
{
    return m_textures.add( texture );
}
        
// ** RenderingContext::texture
const Renderer::TexturePtr& RenderingContext::texture( s32 identifier ) const
{
    return m_textures.resolve( identifier );
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