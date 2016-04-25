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
#include "Mesh.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// --------------------------------------------------------------------- Terrain --------------------------------------------------------------------- //

// ** Terrain::kChunkSize
s32 Terrain::kChunkSize = 32;

// ** Terrain::kMaxSize
s32 Terrain::kMaxSize = 2048;

// ** Terrain::Terrain
Terrain::Terrain( u32 size )
	: m_heightmap( size ), m_maxHeight( static_cast<f32>( size ) )
{
	DC_BREAK_IF( (size % kChunkSize) != 0, "terrain size should be a multiple of chunk size" );
}

// ** Terrain::size
u32 Terrain::size( void ) const
{
	return m_heightmap.size();
}

// ** Terrain::maxHeight
f32 Terrain::maxHeight( void ) const
{
	return m_maxHeight;
}

// ** Terrain::chunkCount
u32 Terrain::chunkCount( void ) const
{
	return m_heightmap.size() / kChunkSize;
}

// ** Terrain::hasVertex
bool Terrain::hasVertex( s32 x, s32 z ) const
{
	bool invalid = x < 0 || z < 0 || x > static_cast<s32>( size() ) || z > static_cast<s32>( size() );
	return !invalid;
}

// ** Terrain::heightAtVertex
f32 Terrain::heightAtVertex( s32 x, s32 z ) const
{
	return f32( m_heightmap.height( x, z ) ) / m_heightmap.maxValue() * m_maxHeight;
}

// ** Terrain::height
f32 Terrain::height( f32 x, f32 z ) const
{
	if( x < 0.0f || x > size() ) return -1.0f;
	if( z < 0.0f || z > size() ) return -1.0f;

	// Convert to cell coordinates
	s32 hx = static_cast<s32>( floor( x ) );
	s32 hz = static_cast<s32>( floor( z ) );

	// Make sure that we are using right indices
	if( hx + 1 >= static_cast<s32>( size() ) ) hx--;
	if( hz + 1 >= static_cast<s32>( size() ) ) hz--;

	// Calculate the fraction values
	f32 fx = x - hx;
	f32 fz = z - hz;

	// Calculate interpolated terrain height
	f32 height = 0.0f;

	height += heightAtVertex( hx    , hz     ) * (1.0f - fx) * (1.0f - fz);
	height += heightAtVertex( hx + 1, hz     ) * (       fx) * (1.0f - fz);
	height += heightAtVertex( hx    , hz + 1 ) * (1.0f - fx) * (       fz);
	height += heightAtVertex( hx + 1, hz + 1 ) * (       fx) * (       fz);

	return height;
}

// ** Terrain::rayMarch
Vec3 Terrain::rayMarch( const Ray& ray, f32 epsilon ) const
{
	f32 dt		= 0.5f;
	f32 scalar	= 0.5f;
	f32 step	= 0.25f;
	Vec3 point;

	do {
		// Get the point on ray
		point = ray.origin() + ray.direction() * 1000.0f * scalar;

		// Get height at point
		f32 height = this->height( point.x, point.z );

		// Check the height difference, if it's less than epsilon - intersection point is found.
		if( fabs( height - point.y ) <= epsilon ) {
			break;
		}

		// Select the next segment of a ray based on height difference
		if( height < point.y ) {
			f32 newScalar = scalar + step;
			dt = newScalar - scalar;
			scalar = newScalar;
		}
		else if( height > point.y ) {
			f32 newScalar = scalar - step;
			dt = scalar - newScalar;
			scalar = newScalar;
		}

		step *= 0.5f;
	} while( dt >= 0.00001f );

	return point;
}

// ** Terrain::heightmap
const Heightmap& Terrain::heightmap( void ) const
{
	return m_heightmap;
}

// ** Terrain::heightmap
Heightmap& Terrain::heightmap( void )
{
	return m_heightmap;
}

// ** Terrain::chunkVertexBuffer
Array<Terrain::Vertex> Terrain::chunkVertexBuffer( u32 x, u32 z ) const
{
	DC_ABORT_IF( x >= chunkCount(), "invalid chunk coordinate" )
	DC_ABORT_IF( z >= chunkCount(), "invalid chunk coordinate" )

	// UV tiling
	f32 uvSize = 1.0f / m_heightmap.size();

	// Vertex stride
	s32 stride = kChunkSize + 1;

	// Construct vertex buffer
	Array<Vertex> vertices;
	vertices.resize( stride * stride );

	// Fill vertex buffer
	for( s32 i = 0; i <= kChunkSize; i++ ) {
		for( s32 j = 0; j <= kChunkSize; j++ ) {
			Vertex& vertex = vertices[i * stride + j];
			f32		height = heightAtVertex( x * kChunkSize + j, z * kChunkSize + i );

			vertex.position = Vec3( ( f32 )j, ( f32 )height, ( f32 )i );
			vertex.normal   = m_heightmap.normal( x * kChunkSize + j, z * kChunkSize + i );
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
	for( s32 i = 0; i < kChunkSize; i++ ) {
		for( s32 j = 0; j < kChunkSize; j++ ) {
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

// ** Terrain::createMesh
Mesh Terrain::createMesh( void ) const
{
	// Create an empty mesh
	Mesh mesh;

	// Set the number of mesh chunks
	mesh.setChunkCount( chunkCount() * chunkCount() );

	// Construct chunks
	u32 chunk = 0;

	for( u32 i = 0; i < chunkCount(); i++ ) {
		for( u32 j = 0; j < chunkCount(); j++ ) {
			// Get the chunk buffers
			VertexBuffer vertices = chunkVertexBuffer( i, j );
			IndexBuffer  indices  = chunkIndexBuffer();

			// Set chunk mesh
			setMeshChunk( mesh, chunk, vertices, indices, i * kChunkSize, j * kChunkSize );

			// Increase the chunk index
			chunk++;
		}
	}

	// Now update the mesh bounding box
	mesh.updateBounds();

	return mesh;
}

// ** Terrain::createChunkMesh
Mesh Terrain::createChunkMesh( u32 x, u32 z ) const
{
	// Create an empty mesh
	Mesh mesh;

	// Get the chunk buffers
	VertexBuffer vertices = chunkVertexBuffer( x, z );
	IndexBuffer  indices  = chunkIndexBuffer();

	// Set the number of mesh chunks
	mesh.setChunkCount( 1 );

	// Set the chunk data
	setMeshChunk( mesh, 0, vertices, indices );

	// Now update the mesh bounding box
	mesh.updateBounds();

	return mesh;
}

// ** Terrain::setMeshChunk
void Terrain::setMeshChunk( Mesh& mesh, u32 chunk, const VertexBuffer& vertices, const IndexBuffer& indices, u32 x, u32 z ) const
{
#if 0
	// Set the chunk data
	Mesh::VertexBuffer vb;
	vb.resize( vertices.size() );

	for( u32 i = 0, n = ( u32 )vertices.size(); i < n; i++ ) {
		vb[i].position = vertices[i].position + Vec3( x, 0, z );
		vb[i].normal = vertices[i].normal;
		vb[i].uv[0] = vertices[i].uv;
	}

	mesh.setVertexBuffer( vb );
	mesh.setIndexBuffer( indices );
#else
    DC_NOT_IMPLEMENTED
#endif
}

// --------------------------------------------------------------------- Heightmap ---------------------------------------------------------------------- //

// ** Heightmap::Heightmap
Heightmap::Heightmap( u32 size ) : m_size( size )
{
	// Allocate the heightmap.
	m_buffer.resize( (size + 1) * (size + 1) );
	memset( &m_buffer[0], 0, sizeof( u16 ) * m_buffer.size() );
}

// ** Heightmap::size
u32 Heightmap::size( void ) const
{
	return m_size;
}

// ** Heightmap::maxValue
Heightmap::Type Heightmap::maxValue( void ) const
{
	Type value = ~static_cast<Type>( 0 );
	DC_BREAK_IF( value <= 0, "invalid maximum value" );
	return value;
}

// ** Heightmap::height
Heightmap::Type Heightmap::height( u32 x, u32 z ) const
{
	DC_ABORT_IF( x > m_size || z > m_size, "index is out of range" );
	return m_buffer[z * (m_size + 1) + x];
}

// ** Heightmap::setHeight
void Heightmap::setHeight( u32 x, u32 z, Type value )
{
	DC_ABORT_IF( x > m_size || z > m_size, "index is out of range" );
	m_buffer[z * (m_size + 1) + x] = value;
}

// ** Heightmap::normal
Vec3 Heightmap::normal( u32 x, u32 z ) const
{
	// Compute the X gradient
    f32 dx = static_cast<f32>( height( x < m_size - 1 ? x + 1 : x, z ) - height( x > 0 ? x - 1 : x, z ) );

    if( x == 0 || x == m_size - 1 ) {
        dx *= 2;
	}

	// Compute the Z gradient
    f32 dz = static_cast<f32>( height( x, z < m_size-1 ? z + 1 : z ) - height( x, z > 0 ?  z - 1 : z ) );

    if( z == 0 || z == m_size - 1 ) {
        dz *= 2;
	}

	// Normalize
	Vec3 normal( -dx, 2.0f, dz );
	normal.normalize();

	return normal;
}

// ** Heightmap::set
void Heightmap::set( StrongPtr<Generator> generator )
{
	// Generate heightmap
	for( u32 z = 0; z <= m_size; z++ ) {
		for( u32 x = 0; x <= m_size; x++ ) {
			setHeight( x, z, generator->calculate( x, z ) );
		}
	}
}

} // namespace Scene

DC_END_DREEMCHEST