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

namespace Scene {

// ** Image::Image
Image::Image( void ) : m_width( 0 ), m_height( 0 ), m_bytesPerPixel( 0 ), m_channels( 0 )
{
	setMipLevelCount( 1 );
	setBytesPerPixel( 4 );
}

// ** Image::width
s32 Image::width( void ) const
{
	return m_width;
}

// ** Image::setWidth
void Image::setWidth( s32 value )
{
	m_width = value;
}

// ** Image::height
s32 Image::height( void ) const
{
	return m_height;
}

// ** Image::setHeight
void Image::setHeight( s32 value )
{
	m_height = value;
}

// ** Image::bytesPerPixel
s32 Image::bytesPerPixel( void ) const
{
	return m_bytesPerPixel;
}

// ** Image::setBytesPerPixel
void Image::setBytesPerPixel( s32 value )
{
	m_bytesPerPixel = value;
}

// ** Image::mipLevelCount
s32 Image::mipLevelCount( void ) const
{
	return ( s32 )m_mips.size();
}

// ** Image::setMipLevelCount
void Image::setMipLevelCount( s32 value )
{
	m_mips.resize( value );
}

// ** Image::mipLevel
const ByteArray& Image::mipLevel( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= mipLevelCount() );

	const ByteArray& pixels = m_mips[index];
	DC_BREAK_IF( pixels.empty() );

	return pixels;
}

// ** Image::setMipLevel
void Image::setMipLevel( s32 index, const ByteArray& value )
{
	DC_BREAK_IF( index < 0 || index >= mipLevelCount() );
	m_mips[index] = value;
}

// ** Image::mipLevelWidth
s32 Image::mipLevelWidth( s32 index ) const
{
	s32 result = m_width;

	while( index-- ) {
		result = result >> 1;
	}

	return result;
}

// ** Image::mipLevelHeight
s32 Image::mipLevelHeight( s32 index ) const
{
	s32 result = m_width;

	while( index-- ) {
		result = result >> 1;
	}

	return result;
}

} // namespace Scene

DC_END_DREEMCHEST