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

#include "Renderable.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Renderable::Renderable
Renderable::Renderable( void )
    : m_primitiveType( Renderer::PrimTriangles )
{
}

// ** Renderable::chunkCount
s32 Renderable::chunkCount( void ) const
{
    return static_cast<s32>( m_chunks.size() );
}

// ** Renderable::primitiveType
Renderer::PrimitiveType Renderable::primitiveType( void ) const
{
    return m_primitiveType;
}

// ** Renderable::setPrimitiveType
void Renderable::setPrimitiveType( Renderer::PrimitiveType value )
{
    m_primitiveType = value;
}

// ** Renderable::vertexBuffer
Renderer::VertexBufferWPtr Renderable::vertexBuffer( s32 chunk ) const
{
    DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
    return m_chunks[chunk].vertexBuffer;
}

// ** Renderable::setVertexBuffer
void Renderable::setVertexBuffer( s32 chunk, Renderer::VertexBufferPtr value )
{
    if( chunk >= chunkCount() ) {
        m_chunks.resize( chunk + 1 );
    }

    m_chunks[chunk].vertexBuffer = value;
}

// ** Renderable::indexBuffer
Renderer::IndexBufferWPtr Renderable::indexBuffer( s32 chunk ) const
{
    DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
    return m_chunks[chunk].indexBuffer;
}

// ** Renderable::setIndexBuffer
void Renderable::setIndexBuffer( s32 chunk, Renderer::IndexBufferPtr value )
{
    if( chunk >= chunkCount() ) {
        m_chunks.resize( chunk + 1 );
    }

    m_chunks[chunk].indexBuffer = value;
}

// ** Texture::texture
Renderer::TextureWPtr Texture::texture( void ) const
{
    return m_texture;
}

// ** Texture::setTexture
void Texture::setTexture( Renderer::TexturePtr value )
{
    m_texture = value;
}

} // namespace Scene

DC_END_DREEMCHEST