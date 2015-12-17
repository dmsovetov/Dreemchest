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

#include "Loaders.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------ ImageLoaderRAW ------------------------------------------ //

// ** ImageLoaderRAW::loadFromStream
bool ImageLoaderRAW::loadFromStream( AssetBundleWPtr assets, AssetWPtr asset, const Io::StreamPtr& stream )
{
	// Call the base loadFromStream to init the asset pointer
	if( !GenericAssetLoader::loadFromStream( assets, asset, stream ) ) {
		return false;
	}

	u16 width, height;
	u8  channels;

	// Read image info
	stream->read( &width, 2 );
	stream->read( &height, 2 );
	stream->read( &channels, 1 );

	// Read image pixels
	ByteArray pixels;
	pixels.resize( width * height * channels );
	stream->read( &pixels[0], pixels.size() );

	// Setup image asset
	m_asset->setWidth( width );
	m_asset->setHeight( height );
	m_asset->setBytesPerPixel( channels );
	m_asset->setMipLevelCount( 1 );
	m_asset->setMipLevel( 0, pixels );

	return true;
}

// ------------------------------------------ MeshLoaderRAW ------------------------------------------ //

// ** MeshLoaderRAW::loadFromStream
bool MeshLoaderRAW::loadFromStream( AssetBundleWPtr assets, AssetWPtr asset, const Io::StreamPtr& stream )
{
	// Call the base loadFromStream to init the asset pointer
	if( !GenericAssetLoader::loadFromStream( assets, asset, stream ) ) {
		return false;
	}

	// Read the total number of mesh chunks
	u32 chunkCount;
	stream->read( &chunkCount, 4 );

	// Set the total number of mesh chunks
	m_asset->setChunkCount( chunkCount );

	for( u32 i = 0; i < chunkCount; i++ ) {
		// Read the total number of vertices & indices
		u32 vertexCount, indexCount;
		stream->read( &vertexCount, 4 );
		stream->read( &indexCount, 4 );

		// Read vertex buffer
		Mesh::VertexBuffer vertices;
		vertices.resize( vertexCount );

		for( u32 j = 0; j < vertexCount; j++ ) {
			Mesh::Vertex* v = &vertices[i];

			stream->read( &v->position.x, sizeof( v->position ) );
			stream->read( &v->normal.x, sizeof( v->normal ) );
			stream->read( &v->uv[0].x, sizeof( v->uv[0] ) );
			stream->read( &v->uv[1].x, sizeof( v->uv[1] ) );			
		}

		// Set chunk vertex buffer.
		m_asset->setVertexBuffer( i, vertices );

		// Read index buffer
		Mesh::IndexBuffer indices;
		indices.resize( indexCount );
		stream->read( &indices[0], indices.size() );

		// Set chunk index buffer
		m_asset->setIndexBuffer( i, indices );
	}

	// Update node bounds
	m_asset->updateBounds();

	return true;
}

// --------------------------------------- MaterialLoader --------------------------------------- //

#ifdef HAVE_JSON

// ** MaterialLoader::loadFromStream
bool MaterialLoader::loadFromStream( AssetBundleWPtr assets, AssetWPtr asset, const Io::StreamPtr& stream )
{
	// Call the base loadFromStream to init the asset pointer
	if( !GenericAssetLoader::loadFromStream( assets, asset, stream ) ) {
		return false;
	}

	String json;
	json.resize( stream->length() );
	stream->read( &json[0], stream->length() );

	Json::Reader reader;
	Json::Value root;

	if( !reader.parse( json, root ) ) {
		return false;
	}

	String      shader		   = root["shader"].asString();
	Json::Value diffuseColor   = root["colors"]["tint"] != Json::nullValue ? root["colors"]["tint"] : root["colors"]["diffuse"];
	Json::Value diffuseTexture = root["textures"]["diffuse"];
	Json::Value parameters = root["parameters"];

	if( shader.find( "cutout" ) != String::npos ) {
		m_asset->setRenderingMode( RenderCutout );
	}
	else if( shader.find( "transparent" ) != String::npos ) {
		m_asset->setRenderingMode( RenderTranslucent );
	}
	else if( shader.find( "additive" ) != String::npos ) {
		m_asset->setRenderingMode( RenderAdditive );
	}
	else {
		m_asset->setRenderingMode( RenderOpaque );
	}
	
	m_asset->setModel( Material::Phong );
	m_asset->setColor( Material::Diffuse, Rgba( diffuseColor[0].asFloat(), diffuseColor[1].asFloat(), diffuseColor[2].asFloat(), diffuseColor[3].asFloat() ) );
	m_asset->setTexture( Material::Diffuse, assets->find<Image>( diffuseTexture["asset"].asString() ) );

	return true;
}

#endif	/*	HAVE_JSON	*/

} // namespace Scene

DC_END_DREEMCHEST