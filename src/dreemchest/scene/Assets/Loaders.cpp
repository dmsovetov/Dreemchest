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
bool RawImageLoader::loadFromStream( Renderer::HalPtr hal, const io::StreamPtr& stream ) const
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
bool RawMeshLoader::loadFromStream( Renderer::HalPtr hal, const io::StreamPtr& stream ) const
{
	u32 vertexCount, indexCount;

	stream->read( &vertexCount, 4 );
	stream->read( &indexCount, 4 );

	Vertex* vertices = DC_NEW Vertex[vertexCount];
	u16*	indices  = DC_NEW u16[indexCount];

	for( u32 i = 0; i < vertexCount; i++ ) {
		Vertex* v = vertices + i;

		stream->read( v->position, sizeof( v->position ) );
		stream->read( v->normal, sizeof( v->normal ) );
		stream->read( v->tex0, sizeof( v->tex0 ) );
		stream->read( v->tex1, sizeof( v->tex1 ) );
	}

	stream->read( indices, sizeof( u16 ) * indexCount );

	return true;
}

} // namespace Scene

DC_END_DREEMCHEST