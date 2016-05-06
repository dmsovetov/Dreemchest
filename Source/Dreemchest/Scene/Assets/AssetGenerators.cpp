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

#include "AssetGenerators.h"
#include "Mesh.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** MeshPlaneGenerator::MeshPlaneGenerator
MeshPlaneGenerator::MeshPlaneGenerator( const Vec3& u, const Vec3& v, f32 size )
    : m_size( size )
    , m_u( u )
    , m_v( v )
{
}

// ** MeshPlaneGenerator::generate
bool MeshPlaneGenerator::generate( Assets::Assets& assets, Mesh& mesh )
{
    // Calculate a plane normal
    Vec3 normal = m_v % m_u;

    // Vertex texture coordinates
    f32 uv[] = {
          0.0f, 0.0f
        , 1.0f, 0.0f
        , 1.0f, 1.0f
        , 0.0f, 1.0f
    };

    // Vertex positions
    Vec3 pos[] = {
          -m_u * m_size - m_v * m_size
        ,  m_u * m_size - m_v * m_size
        ,  m_u * m_size + m_v * m_size
        , -m_u * m_size + m_v * m_size
    };

    // Index buffer
    Mesh::IndexBuffer ib;
    ib.push_back( 2 );
    ib.push_back( 1 );
    ib.push_back( 0 );
    ib.push_back( 3 );
    ib.push_back( 2 );
    ib.push_back( 0 );

    // Create all vertices
    Mesh::VertexBuffer vb;

    for( s32 i = 0; i < 4; i++ ) {
        Mesh::Vertex v;
        v.normal   = normal;
        v.position = pos[i];

        for( s32 j = 0; j < Mesh::Vertex::MaxTexCoords; j++ ) {
            v.uv[j] = Vec2( uv + i * 2 );
        }

        vb.push_back( v );
    }

    // Setup mesh asset
    mesh.setChunkCount( 1 );
    mesh.setVertexBuffer( vb );
    mesh.setIndexBuffer( ib );

    return true;
}

} // namespace Scene

DC_END_DREEMCHEST