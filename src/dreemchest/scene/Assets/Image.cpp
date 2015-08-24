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

#include "Image.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ** Image::Image
Image::Image( AssetBundle* bundle, const String& name, ImageFormat format, u16 width, u16 height )
	: Asset( bundle, Asset::Image, name ), m_width( width ), m_height( height ), m_format( format )
{
}

// ** Image::width
u16 Image::width( void ) const
{
	return m_width;
}

// ** Image::height
u16 Image::height( void ) const
{
	return m_height;
}

// ** Image::loadFromStream
bool Image::loadFromStream( const io::StreamPtr& stream )
{
	stream->read( &m_width, 2 );
	stream->read( &m_height, 2 );
	stream->read( &m_channels, 1 );

	m_pixels = DC_NEW u8[m_width * m_height * m_channels];
	stream->read( m_pixels.get(), m_width * m_height * m_channels );

	m_texture = renderer::TexturePtr();

	return true;
}

// ** Image::unload
void Image::unload( void )
{
	m_texture = renderer::TexturePtr();
	m_pixels = NULL;
}

// ** Image::requestTexture
renderer::TexturePtr Image::requestTexture( renderer::Hal* hal )
{
	if( m_texture != renderer::TexturePtr() ) {
		return m_texture;
	}

	if( !m_pixels.get() ) {
		return renderer::TexturePtr();
	}

	renderer::Texture2D* texture = hal->createTexture2D( m_width, m_height, m_channels == 4 ? renderer::PixelRgba8 : renderer::PixelRgb8 );
	texture->setData( 0, m_pixels.get() );
	m_texture = texture;
	m_pixels = NULL;

	return m_texture;
}

} // namespace scene

DC_END_DREEMCHEST