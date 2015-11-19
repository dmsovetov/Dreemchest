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

#include "Bson.h"

#include "streams/Stream.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** Bson::kEmptyArray
const Bson Bson::kEmptyArray( Bson::kArray );

// ** Bson::kEmptyObject
const Bson Bson::kEmptyObject( Bson::kObject );

// ** Bson::Bson
Bson::Bson( void ) : m_type( kNull )
{
}

// ** Bson::Bson
Bson::Bson( Type type ) : m_type( type )
{
	switch( type ) {
	case kString:	m_string = DC_NEW String;
					break;
	case kObject:	m_object = DC_NEW KeyValue;
					break;
	case kArray:	m_array  = DC_NEW ValueArray;
					break;
	default:		break;
	}
}

// ** Bson::Bson
Bson::Bson( bool value ) : m_type( kBoolean ), m_boolean( value )
{
}

// ** Bson::Bson
Bson::Bson( u8 value ) : m_type( kInt8 ), m_int8( value )
{
}

// ** Bson::Bson
Bson::Bson( s8 value ) : m_type( kInt8 ), m_int8( value )
{
}

// ** Bson::Bson
Bson::Bson( s16 value ) : m_type( kInt16 ), m_int32( value )
{
}

// ** Bson::Bson
Bson::Bson( u16 value ) : m_type( kInt16 ), m_int32( value )
{
}

// ** Bson::Bson
Bson::Bson( s32 value ) : m_type( kInt32 ), m_int32( value )
{
}

// ** Bson::Bson
Bson::Bson( f32 value ) : m_type( kFloat32 ), m_float32( value )
{
}

// ** Bson::Bson
Bson::Bson( f64 value ) : m_type( kFloat64 ), m_float64( value )
{
}

// ** Bson::Bson
Bson::Bson( const String& value ) : m_type( kString ), m_string( DC_NEW String( value ) )
{
}

// ** Bson::Bson
Bson::Bson( CString value ) : m_type( kString ), m_string( DC_NEW String( value ) )
{
}

// ** Bson::Bson
Bson::Bson( const Guid& value ) : m_type( kGuid ), m_guid( DC_NEW Guid( value ) )
{
}

// ** Bson::Bson
Bson::Bson( const Bson& other ) : m_type( other.m_type )
{
	setBson( other );
}

// ** Bson::~Bson
Bson::~Bson( void )
{
	clear();
}

// ** Bson::object
Bson Bson::object( void )
{
	return Bson( kObject );
}

// ** Bson::array
Bson Bson::array( void )
{
	return Bson( kArray );
}

// ** Bson::operator =
const Bson& Bson::operator = ( const Bson& other )
{
	setBson( other );
	return *this;
}

// ** Bson::operator bool
Bson::operator bool( void ) const
{
	return m_boolean;
}

// ** Bson::operator u8
Bson::operator u8( void ) const
{
	return m_int8;
}

// ** Bson::operator s8
Bson::operator s8( void ) const
{
	return m_int8;
}

// ** Bson::operator u16
Bson::operator u16( void ) const
{
	return m_int16;
}

// ** Bson::operator s16
Bson::operator s16( void ) const
{
	return m_int16;
}

// ** Bson::operator u32
Bson::operator u32( void ) const
{
	return m_int32;
}

// ** Bson::operator s32
Bson::operator s32( void ) const
{
	return m_int32;
}

// ** Bson::operator f32
Bson::operator f32( void ) const
{
	return m_float32;
}

// ** Bson::operator f64
Bson::operator f64( void ) const
{
	return m_float64;
}

// ** Bson::operator String
Bson::operator String( void ) const
{
	return *m_string;
}

// ** Bson::operator []
const Bson& Bson::operator []( s32 index ) const
{
	DC_BREAK_IF( m_type != kArray );
	return (*m_array)[index];
}

// ** Bson::operator []
Bson& Bson::operator []( s32 index )
{
	DC_BREAK_IF( m_type != kArray );
	return (*m_array)[index];
}

// ** Bson::operator []
const Bson& Bson::operator []( const String& key ) const
{
	DC_BREAK_IF( m_type != kObject );
	return (*m_object)[key];
}

// ** Bson::operator []
const Bson& Bson::operator []( CString key ) const
{
	DC_BREAK_IF( m_type != kObject );
	return (*m_object)[key];
}

// ** Bson::operator []
Bson& Bson::operator []( const String& key )
{
	DC_BREAK_IF( m_type != kObject );
	return (*m_object)[key];
}

// ** Bson::operator []
Bson& Bson::operator []( CString key )
{
	DC_BREAK_IF( m_type != kObject );
	return (*m_object)[key];
}

// ** Bson::operator <<
Bson& Bson::operator << ( CString value )
{
	if( m_type == kObject ) {
		if( m_key.empty() ) {
			m_key = value;
			return *this;
		} else {
			(*this)[m_key] = value;
		}
	}
	else if( m_type == kArray ) {
		m_array->push_back( value );
	}

	m_key = "";	

	return *this;
}

// ** Bson::operator <<
Bson& Bson::operator << ( const String& value )
{
	return this->operator << ( value.c_str() );
}

// ** Bson::isNull
bool Bson::isNull( void ) const
{
	return m_type == kNull;
}

// ** Bson::type
Bson::Type Bson::type( void ) const
{
	return m_type;
}

// ** Bson::properties
const Bson::KeyValue& Bson::properties( void ) const
{
	DC_BREAK_IF( m_type != kObject );
	return *m_object;
}

// ** Bson::properties
Bson::KeyValue& Bson::properties( void )
{
	DC_BREAK_IF( m_type != kObject );
	return *m_object;
}

// ** Bson::items
const Bson::ValueArray& Bson::items( void ) const
{
	DC_BREAK_IF( m_type != kArray );
	return *m_array;
}

// ** Bson::get
const Bson& Bson::get( const String& key, const Bson& defaultValue ) const
{
	if( m_type != kObject ) {
		return defaultValue;
	}

	KeyValue::const_iterator i = m_object->find( key );
	return i != m_object->end() ? i->second : defaultValue;
}

// ** Bson::asBool
bool Bson::asBool( void ) const
{
	return m_boolean;
}

// ** Bson::asByte
u8 Bson::asByte( void ) const
{
	return m_int8;
}

// ** Bson::asShort
u16 Bson::asShort( void ) const
{
	return m_int16;
}

// ** Bson::asInt
u32 Bson::asInt( void ) const
{
	return m_int32;
}

// ** Bson::asFloat
f32 Bson::asFloat( void ) const
{
	return m_float32;
}

// ** Bson::asDouble
f64 Bson::asDouble( void ) const
{
	return m_float64;
}

// ** Bson::asLong
u64 Bson::asLong( void ) const
{
	return m_int64;
}

// ** Bson::asGuid
const Guid& Bson::asGuid( void ) const
{
	return *m_guid;
}

// ** Bson::asString
const String& Bson::asString( void ) const
{
	return *m_string;
}

// ** Bson::setBson
void Bson::setBson( const Bson& value )
{
	m_type = value.m_type;

	switch( m_type ) {
	case kNull:		break;
	case kBoolean:	m_boolean = value.m_boolean;
					break;
	case kInt8:		m_int8 = value.m_int8;
					break;
	case kInt16:	m_int16 = value.m_int16;
					break;
	case kInt32:	m_int32 = value.m_int32;
					break;
	case kFloat32:	m_float32 = value.m_float32;
					break;
	case kFloat64:	m_float64 = value.m_float64;
					break;
	case kString:	m_string = DC_NEW String( *value.m_string );
					break;
	case kGuid:		m_guid	 = DC_NEW Guid( *value.m_guid );
					break;
	case kArray:	m_array  = DC_NEW ValueArray( *value.m_array );
					break;
	case kObject:	m_object = DC_NEW KeyValue( *value.m_object );
					break;
	default:		DC_BREAK;
	}
}

// ** Bson::clear
void Bson::clear( void )
{
	switch( m_type ) {
	case kNull:
	case kBoolean:
	case kInt8:
	case kInt16:
	case kInt32:
	case kInt64:
	case kFloat32:
	case kFloat64:
	case kString:	break;
	case kGuid:		delete m_guid;
					m_guid = NULL;
					break;
	case kArray:		delete m_array;
					m_array = NULL;
					break;
	case kObject:	delete m_object;
					break;
	default:		DC_BREAK;
	}

	m_type = kNull;
}

// ** Bson::write
s32 Bson::write( StreamPtr stream ) const
{
	DC_BREAK_IF( !stream.valid() );

	// Bson layout type placeholder.
	u32 type  = 0;

	// Save the starting position
	s32 start = stream->position();

	// Write the layout type.
	stream->write( &type, 4 );

	// Write the value.
	writeValue( stream );

	return stream->position() - start;
}

// ** Bson::writeValue
void Bson::writeValue( StreamPtr stream ) const
{
	// Write value type.
	u8 type = m_type;
	stream->write( &type, 1 );

	// Now write the value.
	switch( type ) {
	case kNull:		break;
	case kBoolean:	stream->write( &m_boolean, 1 );					break;
	case kInt8:		stream->write( &m_int8, 1 );					break;
	case kInt16:	stream->write( &m_int16, 2 );					break;
	case kInt32:	stream->write( &m_int32, 4 );					break;
	case kInt64:	stream->write( &m_int64, 8 );					break;
	case kFloat32:	stream->write( &m_float32, 4 );					break;
	case kFloat64:	stream->write( &m_float64, 8 );					break;
	case kString:	stream->writeString( m_string->c_str() );		break;
	case kGuid:		stream->write( m_guid->bytes(), Guid::Size );	break;

	case kObject:	{
						for( KeyValue::const_iterator i = m_object->begin(), end = m_object->end(); i != end; ++i ) {
							stream->writeString( i->first.c_str() );
							i->second.writeValue( stream );
						}
						stream->writeString( "" );
					}
					break;

	case kArray:	{
						DC_BREAK_IF( m_array->size() >= USHRT_MAX );

						u16 count = m_array->size();
						stream->write( &count, 2 );

						for( u16 i = 0; i < count; i++ ) {
							m_array->at( i ).writeValue( stream );
						}
					}
					break;

	default:		DC_BREAK;
	}
}

// ** Bson::read
s32 Bson::read( StreamPtr stream )
{
	DC_BREAK_IF( !stream.valid() );

	// Save the starting position
	s32 start = stream->position();

	// Bson layout type placeholder.
	u32 type = 0;

	// Read the layout type.
	stream->read( &type, 4 );

	// Read actual data.
	readValue( stream );

	return stream->position() - start;
}

// ** Bson::readValue
void Bson::readValue( StreamPtr stream )
{
	// Clean old data.
	clear();

	// Read value type.
	stream->read( &m_type, 1 );

	switch( m_type ) {
	case kNull:		break;
	case kBoolean:	stream->read( &m_boolean, 1 );		break;
	case kInt8:		stream->read( &m_int8, 1 );			break;
	case kInt16:	stream->read( &m_int16, 2 );		break;
	case kInt32:	stream->read( &m_int32, 4 );		break;
	case kInt64:	stream->read( &m_int64, 8 );		break;
	case kFloat32:	stream->read( &m_float32, 4 );		break;
	case kFloat64:	stream->read( &m_float64, 8 );		break;
	case kGuid:		{
						u8 bytes[Guid::Size];
						stream->read( bytes, Guid::Size );
						m_guid = DC_NEW Guid( bytes );
					}
					break;

	case kObject:	{
						m_object = DC_NEW KeyValue;

						while( stream->hasDataLeft() ) {
							String key;

							// Read the property key
							stream->readString( key );

							if( key.empty() ) {
								break;
							}

							(*m_object)[key] = Bson();
							(*m_object)[key].readValue( stream );
						}
					}
					break;

	case kArray:	{
						m_array = DC_NEW ValueArray;

						u16 count;
						stream->read( &count, 2 );
						m_array->resize( count );

						for( u16 i = 0; i < count; i++ ) {
							(*m_array)[i].readValue( stream );
						}
					}
					break;

	case kString:	{
						m_string = DC_NEW String;
						stream->readString( *m_string );
					}
					break;

	default:		DC_BREAK;
	}
}

// ** Bson::read
void Bson::read( const Storage* storage )
{
	read( storage->isBinaryStorage()->stream() );
}

// ** Bson::write
void Bson::write( Storage* storage ) const
{
	write( storage->isBinaryStorage()->stream() );
}

} // namespace io

DC_END_DREEMCHEST