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

#include "Terrain.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Terrain::kChunkSize
s32 Terrain::kChunkSize = 32;

// ** Terrain::kMaxSize
s32 Terrain::kMaxSize = 2048;

// ** Terrain::Terrain
Terrain::Terrain( AssetBundle* bundle, const String& uuid, const String& name, u32 size )
	: Asset( bundle, Asset::Terrain, uuid, name ), m_size( size )
{
	DC_BREAK_IF( (size % kChunkSize) != 0 )

	m_heightmap.resize( (size + 1) * (size + 1) );
	memset( &m_heightmap[0], 0, m_heightmap.size() * sizeof( u16 ) );
}

// ** Terrain::size
u32 Terrain::size( void ) const
{
	return m_size;
}

// ** Terrain::chunkCount
u32 Terrain::chunkCount( void ) const
{
	return m_size / kChunkSize;
}

// ** Terrain::height
u16 Terrain::height( u32 x, u32 z ) const
{
	return m_heightmap[z * (m_size + 1) + x];
}

// ** Terrain::chunkVertexBuffer
Array<Terrain::Vertex> Terrain::chunkVertexBuffer( u32 x, u32 z ) const
{
	DC_BREAK_IF( x >= chunkCount() )
	DC_BREAK_IF( z >= chunkCount() )

	// UV tiling
	f32 uvSize = 1.0f / m_size;

	// Vertex stride
	s32 stride = kChunkSize + 1;

	// Construct vertex buffer
	Array<Vertex> vertices;
	vertices.resize( stride * stride );

	// Fill vertex buffer
	for( s32 i = 0; i <= kChunkSize; i++ ) {
		for( s32 j = 0; j <= kChunkSize; j++ ) {
			Vertex& vertex = vertices[i * stride + j];
			u16		height = this->height( x * kChunkSize + j, z * kChunkSize + i );

			vertex.position = Vec3( ( f32 )j, ( f32 )height, ( f32 )i );
			vertex.normal   = Vec3( 0.0f, 1.0f, 0.0f );
			vertex.uv		= Vec2( vertex.position.x, vertex.position.z ) * uvSize;
		}
	}

	return vertices;
}

// ** Terrain::chunkIndexBuffer
Array<u16> Terrain::chunkIndexBuffer( void ) const
{
	// Index offset
	u16 idx = 0;

	// Vertex stride
	s32 stride = kChunkSize + 1;

	// Construct index buffer
	Array<u16> indices;
	indices.resize( stride * stride * 6 );

	// Fill index buffer
	for( s32 i = 0; i <= kChunkSize; i++ ) {
		for( s32 j = 0; j <= kChunkSize; j++ ) {
			indices[idx + 0] = (i    ) * stride + (j    );
			indices[idx + 1] = (i + 1) * stride + (j    );
			indices[idx + 2] = (i    ) * stride + (j + 1);

			indices[idx + 3] = (i    ) * stride + (j + 1);
			indices[idx + 4] = (i + 1) * stride + (j    );
			indices[idx + 5] = (i + 1) * stride + (j + 1);

			idx += 6;
		}
	}

	return indices;
}

} // namespace Scene

DC_END_DREEMCHEST