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

#include	"ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** ByteBuffer::ByteBuffer
ByteBuffer::ByteBuffer( u8 *buffer, int size ) : m_buffer( buffer ), m_size( size )
{
}

// ** ByteBuffer::ByteBuffer
ByteBuffer::ByteBuffer( int size ) : m_size( size )
{
	m_buffer = DC_NEW u8[size];
}

// ** ByteBuffer::~ByteBuffer
ByteBuffer::~ByteBuffer( void )
{
	if( m_buffer ) {
		delete[]m_buffer;
		m_buffer = NULL;
	}
}

// ** ByteBuffer::release
void ByteBuffer::release( void )
{
	delete this;
}

// ** ByteBuffer::data
const u8* ByteBuffer::data( void ) const
{
	return m_buffer;
}

// ** ByteBuffer::data
u8* ByteBuffer::data( void )
{
	return m_buffer;
}

// ** ByteBuffer::size
int ByteBuffer::size( void ) const
{
	return m_size;
}

} // namespace io

DC_END_DREEMCHEST