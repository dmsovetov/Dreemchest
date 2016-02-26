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

#include "Mesh.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Mesh::Mesh
Mesh::Mesh( void ) : Asset( NULL, Asset::Mesh, "", "" )
{
	setFormat( AssetFormatMesh );
}

// ** Mesh::chunkCount
s32 Mesh::chunkCount( void ) const
{
	return ( s32 )m_chunks.size();
}

// ** Mesh::setChunkCount
void Mesh::setChunkCount( s32 value )
{
	m_chunks.resize( value );
}

// ** Mesh::chunkId
const RenderingAssetId& Mesh::chunkId( s32 chunk ) const
{
	DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
	return m_chunks[chunk].id;
}

// ** Mesh::setChunkId
void Mesh::setChunkId( s32 chunk, const RenderingAssetId& value )
{
	DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
	m_chunks[chunk].id = value;
}

// ** Mesh::setTexture
void Mesh::setTexture( s32 chunk, const String& value )
{
	DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
	m_chunks[chunk].texture = value;
}

// ** Mesh::texture
const String& Mesh::texture( s32 chunk ) const
{
	DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
	return m_chunks[chunk].texture;
}

// ** Mesh::vertexBuffer
const Mesh::VertexBuffer& Mesh::vertexBuffer( s32 chunk ) const
{
	DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
	return m_chunks[chunk].vertices;
}

// ** Mesh::setVertexBuffer
void Mesh::setVertexBuffer( s32 chunk, const VertexBuffer& value )
{
	DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
	m_chunks[chunk].vertices = value;
}

// ** Mesh::indexBuffer
const Mesh::IndexBuffer& Mesh::indexBuffer( s32 chunk ) const
{
	DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
	return m_chunks[chunk].indices;
}

// ** Mesh::setIndexBuffer
void Mesh::setIndexBuffer( s32 chunk, const IndexBuffer& value )
{
	DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
	m_chunks[chunk].indices = value;
}

// ** Mesh::bounds
const Bounds& Mesh::bounds( void ) const
{
	return m_bounds;
}

// ** Mesh::updateBounds
void Mesh::updateBounds( void )
{
	m_bounds = Bounds();

	// For each chunk
	for( s32 i = 0, n = chunkCount(); i < n; i++ ) {
		// Get the chunk's vertex buffer
		const VertexBuffer& vertices = vertexBuffer( i );

		// For each vertex
		for( s32 j = 0, jn = ( s32 )vertices.size(); j < jn; j++ ) {
			m_bounds << vertices[j].position;
		}
	}
}

// ** Mesh::create
MeshPtr Mesh::create( void )
{
	return MeshPtr( DC_NEW Mesh );
}

} // namespace Scene

DC_END_DREEMCHEST