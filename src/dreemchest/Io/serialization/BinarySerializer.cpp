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

#include "Storage.h"
#include "BinarySerializer.h"
#include "Serializable.h"
#include "../KeyValue.h"
#include "../streams/ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace Io {

// ** SerializableTypes::s_typeById
SerializableTypes::TypeById SerializableTypes::s_typeById;

// ** SerializableTypes::s_typeByName
SerializableTypes::TypeByName SerializableTypes::s_typeByName;

// ** BinarySerializer::read
BinarySerializer::Result BinarySerializer::read( ByteBufferPtr& bytes, Serializable** data )
{
	BinaryStorage storage( bytes );
	return read( &storage, data );
}

// ** BinarySerializer::read
BinarySerializer::Result BinarySerializer::read( const BinaryStorage* storage, Serializable** data )
{
	if( storage->bytesAvailable() < Header::size() ) {
		return NotEnoughData;
	}

	// ** Save current stream position
	s32 initial = storage->position();

	// ** Read header
	Header header;
	storage->read( &header.m_type, sizeof( header.m_type ) );
	storage->read( &header.m_size, sizeof( header.m_size ) );

	// ** Check the data size
	if( storage->bytesAvailable() < header.m_size ) {
		storage->setPosition( initial );
		return NotEnoughData;
	}

	// ** Create instance of serializable type.
	Serializable* result = SerializableTypes::create( header.m_type );

	if( result == NULL ) {
		storage->setPosition( header.m_size, SeekCur );
		return Unknown;
	}

	// ** Instantiate and read the data
	result->read( storage );
	*data = result;

	return Success;
}

// ** BinarySerializer::read
Serializables BinarySerializer::read( ByteBufferPtr& bytes )
{
	Serializables serializables;

	while( bytes->hasDataLeft() ) {
		Serializable* item   = NULL;
		Result		  result = read( bytes, &item );

		if( result == NotEnoughData ) {
			break;
		}

		if( result == Malformed ) {
			bytes->setPosition( 0, SeekEnd );
			break;
		}

		if( item ) {
			serializables.push_back( item );
		}
	}

	return serializables;
}

// ** BinarySerializer::write
s32 BinarySerializer::write( ByteBufferPtr& bytes, Serializable* data )
{
	BinaryStorage storage( bytes );
	return write( &storage, data );
}

// ** BinarySerializer::write
s32 BinarySerializer::write( BinaryStorage* storage, Serializable* data )
{
	DC_BREAK_IF( !SerializableTypes::isRegistered( data->typeId() ) );

	s32	   position = storage->position();
	Header header( data->typeId(), 0 );

	storage->write( &header.m_type, sizeof( header.m_type ) );
	storage->write( &header.m_size, sizeof( header.m_size ) );

	s32 start = storage->position();
	data->write( storage );
	header.m_size = storage->position() - start;

	// ** Write size
	storage->setPosition( position + sizeof( TypeId ) );
	storage->write( &header.m_size, sizeof( header.m_size ) );

	storage->setPosition( start + header.m_size );

	return storage->position() - position;
}

// ** BinarySerializer::writeToByteBuffer
ByteBufferPtr BinarySerializer::write( const Serializable& data )
{
	ByteBufferPtr stream = ByteBuffer::create();
	BinaryStorage storage( stream );
	data.write( &storage );

	return stream;
}

// ** BinarySerializer::writeToByteBuffer
ByteBufferPtr BinarySerializer::write( const KeyValue& data )
{
	ByteBufferPtr stream = ByteBuffer::create();
	BinaryVariantStream( stream ).write( Variant::fromValue( data ) );

	return stream;
}

} // namespace Io

DC_END_DREEMCHEST