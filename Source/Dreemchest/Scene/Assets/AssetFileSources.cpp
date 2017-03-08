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

#include "AssetFileSources.h"

#include "Image.h"
#include "Mesh.h"
#include "Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------ ImageLoaderRaw ------------------------------------------ //

// ** ImageFormatRaw::constructFromStream
bool ImageFormatRaw::constructFromStream( Io::StreamPtr stream, Assets::Assets& assets, Image& asset )
{
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
    asset.setWidth( width );
    asset.setHeight( height );
    asset.setBytesPerPixel( channels );
    asset.setMipLevelCount( 1 );
    asset.setMipLevel( 0, pixels );

    return true;
}

// ------------------------------------------ MeshFormatRaw ------------------------------------------ //

// ** MeshFormatRaw::constructFromStream
bool MeshFormatRaw::constructFromStream( Io::StreamPtr stream, Assets::Assets& assets, Mesh& asset )
{
    // Read the total number of mesh chunks
    u32 chunkCount;
    stream->read( &chunkCount, 4 );

    // Set the total number of mesh chunks
    asset.setChunkCount( chunkCount );
    NIMBLE_BREAK_IF( chunkCount != 1 );

    Mesh::VertexBuffer vertices;
    Mesh::IndexBuffer indices;

    for( u32 i = 0; i < chunkCount; i++ ) {
        // Read chunk texture name
        String texture;
        stream->readString( texture );

        // Read the total number of vertices & indices
        u32 vertexCount, indexCount;
        stream->read( &vertexCount, 4 );
        stream->read( &indexCount, 4 );

        // Read vertex buffer
        vertices.resize( vertices.size() + vertexCount );

        for( u32 j = vertices.size() - vertexCount; j < vertexCount; j++ ) {
            Mesh::Vertex* v = &vertices[j];

            stream->read( &v->position.x, sizeof( v->position ) );
            stream->read( &v->normal.x, sizeof( v->normal ) );
            stream->read( &v->uv[0].x, sizeof( v->uv[0] ) );
            stream->read( &v->uv[1].x, sizeof( v->uv[1] ) );
        }

        // Read index buffer
        indices.resize( indices.size() + indexCount );

        for( u32 j = indices.size() - indexCount; j < indexCount; j++ ) {
            u16 idx;
            stream->read( &idx, sizeof(u16) );
            indices[j] = idx;
        }

        // Set chunk texture
        asset.setTexture( i, texture );
    }

    // Set mesh vertex buffer.
    asset.setVertexBuffer( vertices );

    // Set mesh index buffer
    asset.setIndexBuffer( indices );

    // Update node bounds
    asset.updateBounds();

    return true;
}

// --------------------------------------- MaterialSourceKeyValue --------------------------------------- //

// ** MaterialSourceKeyValue::constructFromStream
bool MaterialSourceKeyValue::constructFromStream( Io::StreamPtr stream, Assets::Assets& assets, Material& asset )
{
#ifdef JSONCPP_FOUND

#if 0
    String json;
    json.resize( stream->length() );

    if( json.empty() ) {
        return true;
    }

    stream->read( &json[0], stream->length() );

    Json::Reader reader;
    Json::Value root;

    if( !reader.parse( json, root ) ) {
        return false;
    }

    String      shader           = root["shader"].asString();
    Json::Value diffuseColor   = root["colors"]["tint"] != Json::nullValue ? root["colors"]["tint"] : root["colors"]["diffuse"];
    Json::Value diffuseTexture = root["textures"]["diffuse"];
    Json::Value parameters = root["parameters"];

    if( shader.find( "cutout" ) != String::npos ) {
        asset.setRenderingMode( RenderingMode::Cutout );
    }
    else if( shader.find( "transparent" ) != String::npos ) {
        asset.setRenderingMode( RenderingMode::Translucent );
    }
    else if( shader.find( "additive" ) != String::npos ) {
        asset.setRenderingMode( RenderingMode::Additive );
    }
    else {
        asset.setRenderingMode( RenderingMode::Opaque );
    }
    
    asset.setLightingModel( LightingModel::Phong );
    asset.setColor( Material::Diffuse, Rgba( diffuseColor[0].asFloat(), diffuseColor[1].asFloat(), diffuseColor[2].asFloat(), diffuseColor[3].asFloat() ) );
    asset.setTexture( Material::Diffuse, assets.find<Image>( diffuseTexture["asset"].asString() ) );

    return true;
#else
	return 0;
#endif
#else
    return false;
#endif    /*    #ifdef JSONCPP_FOUND    */
}

} // namespace Scene

DC_END_DREEMCHEST
