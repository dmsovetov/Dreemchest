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

#include "MeshImporter.h"

DC_BEGIN_COMPOSER

namespace Importers {

// ------------------------------------------------------ MeshImporter ------------------------------------------------------ //

// ** MeshImporter::import
bool MeshImporter::import( FileSystemWPtr fs, const Io::Path& sourceFileName, const Io::Path& destinationFileName )
{
	// Import mesh nodes from file
	if( !importNodes( fs, sourceFileName ) ) {
		return false;
	}

	// Write nodes to file
	Io::StreamPtr stream = openWriteStream( destinationFileName );
	DC_BREAK_IF( !stream.valid() );

	s32 chunkCount = ( s32 )m_nodes.size();
	stream->write( &chunkCount, 4 );

	for( s32 i = 0; i < chunkCount; i++ ) {
		// Get mesh buffers
		const MeshNode::VertexBuffer& vertices = m_nodes[i].vertexBuffer();
		const MeshNode::IndexBuffer&  indices  = m_nodes[i].indexBuffer();

		// Write buffer size to stream
		s32 vertexCount = ( s32 )vertices.size();
		s32 indexCount  = ( s32 )indices.size();

		stream->write( &vertexCount, 4 );
		stream->write( &indexCount, 4 );

		// Write vertices to stream
		for( s32 j = 0; j < vertexCount; j++ ) {
			const Vertex& v = vertices[j];

			stream->write( &v.position, sizeof( v.position ) );
			stream->write( &v.normal, sizeof( v.normal ) );
			stream->write( &v.uv[0], sizeof( v.uv[0] ) );
			stream->write( &v.uv[1], sizeof( v.uv[1] ) );
		}

		// Write indices to stream
		stream->write( &indices[0], sizeof( u16 ) * indexCount );
	}

	return true;
}

// ** MeshImporter::VertexCompare::operator()
bool MeshImporter::VertexCompare::operator()( const Vertex& a, const Vertex& b ) const
{
	for( s32 i = 0; i < 3; i++ ) {
		if( a.position[i] != b.position[i] ) return a.position[i] < b.position[i];
	}
	for( s32 i = 0; i < 3; i++ ) {
		if( a.normal[i] != b.normal[i] ) return a.normal[i] < b.normal[i];
	}
	for( s32 j = 0; j < 2; j++ ) {
		for( s32 i = 0; i < 2; i++ ) {
			if( a.uv[j][i] != b.uv[j][i] ) return a.uv[j][i] < b.uv[j][i];
		}
	}

	return false;
}

#ifdef HAVE_FBX

// ---------------------------------------------------- MeshImporterFBX ---------------------------------------------------- //

// ** MeshImporterFBX::s_manager
FbxManager* MeshImporterFBX::s_manager = NULL;

// ** MeshImporterFBX::s_importer
FbxImporter* MeshImporterFBX::s_importer = NULL;

// ** MeshImporterFBX::MeshImporterFBX
MeshImporterFBX::MeshImporterFBX( void ) : m_scene( NULL )
{
	// Create the FBX manager instance
	if( !s_manager ) {
		s_manager = FbxManager::Create();
		DC_BREAK_IF( !s_manager );
	}

	// Create an importer
	if( !s_importer ) {
		s_importer = FbxImporter::Create( s_manager, "" );
		DC_BREAK_IF( !s_importer );
	}
}

MeshImporterFBX::~MeshImporterFBX( void )
{
	if( m_scene ) m_scene->Destroy();
}

// ** MeshImporterFBX::importNodes
bool MeshImporterFBX::importNodes( FileSystemWPtr fs, const Io::Path& sourceFileName )
{
	// Create the scene instance
	m_scene = FbxScene::Create( s_manager, "" );
	DC_BREAK_IF( !m_scene );

	// Initialize the importer by providing a filename.
	if( !s_importer->Initialize( sourceFileName.c_str() ) ) {
		DC_BREAK;
		return false;
	}

	// Import the scene
	if( !s_importer->Import( m_scene ) ) {
		DC_BREAK;
		return false;
	}

	// Import scene nodes
	importNode( m_scene->GetRootNode() );

	return true;
}

// ** MeshImporterFBX::importNode
void MeshImporterFBX::importNode( FbxNode* node )
{
	if( !node ) {
		return;
	}

	// Import data from node attributes
	importAttributes( node );

	// Import each child node
	for( s32 i = 0; i < node->GetChildCount(); i++ ) {
		importNode( node->GetChild( i ) );
	}
}

// ** MeshImporterFBX::importAttributes
void MeshImporterFBX::importAttributes( FbxNode* node )
{
    for( s32 i = 0, n = node->GetNodeAttributeCount(); i < n; i++) {
		// Get attribute by index
		FbxNodeAttribute* attribute = node->GetNodeAttributeByIndex( i );

		// Check attribute type
		if( attribute->GetAttributeType() != FbxNodeAttribute::eMesh ) {
			continue;
		}

		// Import mesh
        importMesh( node, static_cast<FbxMesh*>( attribute ) );
    }
}

// ** MeshImporterFBX::importMesh
void MeshImporterFBX::importMesh( FbxNode* node, FbxMesh* mesh )
{
	// Triangulate mesh node
	if( !mesh->IsTriangleMesh() ) {
		FbxGeometryConverter converter( s_manager );
		converter.Triangulate( node->GetNodeAttribute(), true );
		mesh = static_cast<FbxMesh*>( node->GetNodeAttribute() );
	}

	// Extract the number of vertices & faces
	s32			vertexCount = mesh->GetControlPointsCount();
    FbxVector4* vertices	= mesh->GetControlPoints();
    s32			faceCount   = mesh->GetPolygonCount();

	// Extract the texture coordinate names
    FbxStringList texCoordNames;
    mesh->GetUVSetNames( texCoordNames );
	
	while( texCoordNames.GetCount() > Vertex::MaxTexCoords ) {
		texCoordNames.RemoveLast();
	}

	// Vertex attributes
	Array<FbxVector2>	texCoords;
	FbxVector4			normal;
	FbxVector4			position;

	texCoords.resize( texCoordNames.GetCount() );

	// Declare mesh indexer
	MeshIndexer<Vertex, VertexCompare> meshIndexer;

    for( s32 i = 0; i < faceCount; i++ ) {
		// Get the number of vertices in face
        s32 faceSize = mesh->GetPolygonSize( i );

        for( s32 j = 0; j < faceSize; j++ ) {
            s32 vertexIndex = mesh->GetPolygonVertex( i, j );
			DC_BREAK_IF( vertexIndex >= vertexCount )

			// Get vertex position
            position = vertices[vertexIndex];

			// Get texture coordinates
			for( s32 k = 0; k < texCoordNames.GetCount(); k++ ) {
				bool pUnmapped = true;
				mesh->GetPolygonVertexUV( i, j, texCoordNames.GetStringAt( k ), texCoords[k], pUnmapped );
			}

			// Initialize vertex
            Vertex vertex;
            vertex.position = Vec3( position );
            vertex.normal	= Vec3( normal );

			for( s32 k = 0; k < texCoordNames.GetCount(); k++ ) {
				vertex.uv[k] = Vec2( texCoords[k] );
			}

            meshIndexer += vertex;
        }
    }

	// Push imported node to an array
	m_nodes.push_back( meshIndexer );
}

#endif	/*	HAVE_FBX	*/

} // namespace Importers

DC_END_COMPOSER