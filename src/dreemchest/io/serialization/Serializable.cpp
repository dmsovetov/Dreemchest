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

#include    "Storage.h"
#include	"Serializable.h"
#include	"../streams/ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** Serializable::read
void Serializable::read( const Storage& storage, CString key )
{
    detail::FieldSerializers serializers = fieldSerializers();
    StorageState state;

    storage.pushRead( state );

    for( detail::FieldSerializers::iterator i = serializers.begin(), end = serializers.end(); i != end; ++i ) {
        i->get()->read( storage );
    }

    storage.pop();
}

// ** Serializable::write
void Serializable::write( Storage& storage, CString key ) const
{
    detail::FieldSerializers serializers = fieldSerializers();

    storage.pushWrite( StorageState( key ) );

    for( detail::FieldSerializers::iterator i = serializers.begin(), end = serializers.end(); i != end; ++i ) {
        i->get()->write( storage );
    }

    storage.pop();
}

// ** Serializable::writeToByteBuffer
ByteBufferPtr Serializable::writeToByteBuffer( void ) const
{
	ByteBufferPtr buffer = ByteBuffer::create();
	Storage		  storage( StorageBinary, buffer );
	write( storage );

	return buffer;
}

// ** Serializable::readFromByteBuffer
void Serializable::readFromByteBuffer( const ByteBufferPtr& buffer )
{
	read( Storage( StorageBinary, buffer ) );
}

// ** Serializable::readFromBytes
void Serializable::readFromBytes( const Array<u8>& bytes )
{
	if( bytes.empty() ) {
		return;
	}

	readFromByteBuffer( ByteBuffer::createFromData( &bytes[0], bytes.size() ) );
}

// ** Serializable::fieldSerializers
detail::FieldSerializers Serializable::fieldSerializers( void ) const
{
    log::warn( "Serializable::fieldSerializers : not implemented in a subclass\n" );
    return detail::FieldSerializers();
}

} // namespace io

DC_END_DREEMCHEST