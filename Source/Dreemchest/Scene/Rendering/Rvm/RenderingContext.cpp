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

// ** RenderingContext::createInputLayout
Renderer::InputLayoutPtr RenderingContext::createInputLayout( const VertexFormat& format )
{
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

    return inputLayout;
}

// ** RenderingContext::createVertexBuffer
Renderer::VertexBufferPtr RenderingContext::createVertexBuffer( const void* vertices, s32 count, const VertexFormat& dstFormat, const VertexFormat& srcFormat )
{
    // Create a vertex buffer instance
    Renderer::VertexBufferPtr vertexBuffer = m_hal->createVertexBuffer( count * dstFormat.vertexSize() );

    // Lock a vertex buffer
    void* locked = vertexBuffer->lock();

    // Just copy memory if vertex formats match
    if( dstFormat == srcFormat ) {
        memcpy( locked, vertices, count * dstFormat.vertexSize() );
    } else {
        // Copy all vertices to a vertex buffer
        for( s32 i = 0; i < count; i++ ) {
            dstFormat.setVertexAttribute( VertexFormat::Position, srcFormat.vertexAttribute<Vec3>( VertexFormat::Position, vertices, i ), locked, i );
            dstFormat.setVertexAttribute( VertexFormat::Color,    srcFormat.vertexAttribute<u32> ( VertexFormat::Color,    vertices, i ), locked, i );
            dstFormat.setVertexAttribute( VertexFormat::Normal,   srcFormat.vertexAttribute<Vec3>( VertexFormat::Normal,   vertices, i ), locked, i );
            dstFormat.setVertexAttribute( VertexFormat::Uv0,      srcFormat.vertexAttribute<Vec2>( VertexFormat::Uv0,      vertices, i ), locked, i );
            dstFormat.setVertexAttribute( VertexFormat::Uv1,      srcFormat.vertexAttribute<Vec2>( VertexFormat::Uv1,      vertices, i ), locked, i );
        }
    }

    // Unlock a vertex buffer.
    vertexBuffer->unlock();

    return vertexBuffer;
}

// ** RenderingContext::createIndexBuffer
Renderer::IndexBufferPtr RenderingContext::createIndexBuffer( const u16* indices, s32 count )
{
    // Create an index buffer instance
    Renderer::IndexBufferPtr indexBuffer = m_hal->createIndexBuffer( count * sizeof( u16 ) );

    // Copy memory to a GPU index buffer
    memcpy( indexBuffer->lock(), indices, count * sizeof( u16 ) );

    // Unlock an index buffer.
    indexBuffer->unlock();

    return indexBuffer;
}

// ** RenderingContext::createConstantBuffer
Renderer::ConstantBufferPtr RenderingContext::createConstantBuffer( s32 size, const Renderer::ConstantBufferLayout* layout )
{
    return m_hal->createConstantBuffer( size, layout );
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

// ** RenderingContext::internVertexBuffer
s32 RenderingContext::internVertexBuffer( Renderer::VertexBufferPtr vertexBuffer )
{
    return m_vertexBuffers.add( vertexBuffer );
}
        
// ** RenderingContext::vertexBuffer
const Renderer::VertexBufferPtr& RenderingContext::vertexBuffer( s32 identifier ) const
{
    return m_vertexBuffers.resolve( identifier );
}

// ** RenderingContext::internIndexBuffer
s32 RenderingContext::internIndexBuffer( Renderer::IndexBufferPtr indexBuffer )
{
    return m_indexBuffers.add( indexBuffer );
}
        
// ** RenderingContext::indexBuffer
const Renderer::IndexBufferPtr& RenderingContext::indexBuffer( s32 identifier ) const
{
    return m_indexBuffers.resolve( identifier );
}

// ** RenderingContext::internConstantBuffer
s32 RenderingContext::internConstantBuffer( Renderer::ConstantBufferPtr constantBuffer )
{
    return m_constantBuffers.add( constantBuffer );
}

// ** RenderingContext::constantBuffer
const Renderer::ConstantBufferPtr& RenderingContext::constantBuffer( s32 identifier ) const
{
    return m_constantBuffers.resolve( identifier );
}

// ** RenderingContext::internInputLayout
s32 RenderingContext::internInputLayout( Renderer::InputLayoutPtr inputLayout )
{
    return m_inputLayouts.add( inputLayout );
}

// ** RenderingContext::inputLayout
const Renderer::InputLayoutPtr& RenderingContext::inputLayout( s32 identifier ) const
{
    return m_inputLayouts.resolve( identifier );
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