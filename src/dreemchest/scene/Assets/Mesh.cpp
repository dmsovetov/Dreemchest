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

#include <renderer/Hal.h>
#include "Mesh.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ** Mesh::Mesh
Mesh::Mesh( AssetBundle* bundle, const String& name ) : Asset( bundle, Asset::Mesh, name )
{
}

// ** Mesh::addChunk
void Mesh::addChunk( const renderer::VertexBufferPtr& vertexBuffer, const renderer::IndexBufferPtr& indexBuffer)
{
	Chunk chunk;
	chunk.m_vertexBuffer = vertexBuffer;
	chunk.m_indexBuffer  = indexBuffer;
	DC_BREAK_IF( chunk == false );

	m_chunks.push_back( chunk );
}

// ** Mesh::chunkCount
u32 Mesh::chunkCount( void ) const
{
	return ( u32 )m_chunks.size();
}

// ** Mesh::chunk
const Mesh::Chunk& Mesh::chunk( u32 index ) const
{
	static Chunk Invalid;
	return index < chunkCount() ? m_chunks[index] : Invalid;
}

// ** Mesh::create
MeshPtr Mesh::create( const String& name )
{
	return MeshPtr( DC_NEW Mesh( NULL, name ) );
}

// ** Mesh::createRectangular
MeshPtr Mesh::createRectangular( const String& name, renderer::Hal* hal, f32 width, f32 height )
{
	MeshPtr mesh = create( name );

	//! Rectangular mesh vertex
	struct Vertex {
		f32 position[3];
		f32 texCoord[2];
	} vertices[4] = {
		{ -width * 0.5f,  height * 0.5f, 0.0f,		0.0f, 0.0f },
		{  width * 0.5f,  height * 0.5f, 0.0f,		1.0f, 0.0f },
		{  width * 0.5f, -height * 0.5f, 0.0f,		1.0f, 1.0f },
		{ -width * 0.5f, -height * 0.5f, 0.0f,		0.0f, 1.0f },
	};

	//! Mesh indices
	u16 indices[6] = {
		0, 1, 2,
		0, 2, 3
	};

	renderer::VertexDeclaration* vertexDeclaration = hal->createVertexDeclaration( "P3:T0" );
	
	renderer::VertexBufferPtr vb = hal->createVertexBuffer( vertexDeclaration, 4, false );
	memcpy( vb->lock(), vertices, sizeof( Vertex ) * 4 );
	vb->unlock();

	renderer::IndexBufferPtr ib = hal->createIndexBuffer( 6, false );
	memcpy( ib->lock(), indices, sizeof( u16 ) * 6 );
	ib->unlock();

	mesh->addChunk( vb, ib );

	return mesh;
}

} // namespace scene

DC_END_DREEMCHEST