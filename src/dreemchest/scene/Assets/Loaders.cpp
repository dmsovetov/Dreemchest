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

// ------------------------------------------ RawImageLoader ------------------------------------------ //

// ** RawImageLoader::loadFromStream
bool RawImageLoader::loadFromStream( AssetBundleWPtr assets, Renderer::HalPtr hal, const io::StreamPtr& stream ) const
{
	u16 width, height;
	u8  channels;

	stream->read( &width, 2 );
	stream->read( &height, 2 );
	stream->read( &channels, 1 );

	u8* pixels = DC_NEW u8[width * height * channels];
	stream->read( pixels, width * height * channels );

	Renderer::Texture2DPtr texture = hal->createTexture2D( width, height, channels == 3 ? Renderer::PixelRgb8 : Renderer::PixelRgba8 );
	texture->setData( 0, pixels );
	delete[]pixels;

	AssetTexture* data = DC_NEW AssetTexture;
	data->texture = texture;
	m_image->setData( data );

	return true;
}

// ------------------------------------------ RawMeshLoader ------------------------------------------ //

// ** RawMeshLoader::loadFromStream
bool RawMeshLoader::loadFromStream( AssetBundleWPtr assets, Renderer::HalPtr hal, const io::StreamPtr& stream ) const
{
	u32 chunkCount, vertexCount, indexCount;
	
	AssetMesh* data = DC_NEW AssetMesh;
	Bounds bounds;

	stream->read( &chunkCount, 4 );

	for( u32 i = 0; i < chunkCount; i++ ) {
		stream->read( &vertexCount, 4 );
		stream->read( &indexCount, 4 );

		Renderer::VertexDeclarationPtr vertexFormat = hal->createVertexDeclaration( "P3:N:T0:T1" );
		Renderer::VertexBufferPtr	   vertexBuffer = hal->createVertexBuffer( vertexFormat, vertexCount );
		Renderer::IndexBufferPtr	   indexBuffer  = hal->createIndexBuffer( indexCount );

		Vertex* vertices = reinterpret_cast<Vertex*>( vertexBuffer->lock() );

		for( u32 i = 0; i < vertexCount; i++ ) {
			Vertex* v = vertices + i;

			stream->read( v->position, sizeof( v->position ) );
			stream->read( v->normal, sizeof( v->normal ) );
			stream->read( v->tex0, sizeof( v->tex0 ) );
			stream->read( v->tex1, sizeof( v->tex1 ) );

			for( u32 j = 0; j < 3; j++ ) {
				v->position[j] *= 0.01f;
			}

			bounds += v->position;
		}

		vertexBuffer->unlock();

		u16* indices = indexBuffer->lock();
		stream->read( indices, sizeof( u16 ) * indexCount );
		indexBuffer->unlock();

		data->vertexBuffers.push_back( vertexBuffer );
		data->indexBuffers.push_back( indexBuffer );
	}

	m_mesh->setData( data );
	m_mesh->setBounds( bounds );

	return true;
}

// --------------------------------------- JsonMaterialLoader --------------------------------------- //

#ifdef HAVE_JSON

// ** JsonMaterialLoader::loadFromStream
bool JsonMaterialLoader::loadFromStream( AssetBundleWPtr assets, Renderer::HalPtr hal, const io::StreamPtr& stream ) const
{
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
		m_material->setRenderingMode( RenderCutout );
	}
	else if( shader.find( "transparent" ) != String::npos ) {
		m_material->setRenderingMode( RenderTranslucent );
	}
	else if( shader.find( "additive" ) != String::npos ) {
		m_material->setRenderingMode( RenderAdditive );
	}
	else {
		m_material->setRenderingMode( RenderOpaque );
	}
	
	m_material->setModel( Material::Phong );
	m_material->setColor( Material::Diffuse, Rgba( diffuseColor[0].asFloat(), diffuseColor[1].asFloat(), diffuseColor[2].asFloat(), diffuseColor[3].asFloat() ) );
	m_material->setTexture( Material::Diffuse, assets->find<Image>( diffuseTexture["asset"].asString() ) );

	return true;
}

#endif	/*	HAVE_JSON	*/

} // namespace Scene

DC_END_DREEMCHEST