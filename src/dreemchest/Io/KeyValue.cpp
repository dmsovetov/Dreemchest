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

#include "KeyValue.h"

#include "streams/Stream.h"

DC_BEGIN_DREEMCHEST

namespace Io {

// ** KeyValue::kEmptyArray
const KeyValue KeyValue::kEmptyArray( KeyValue::kArray );

// ** KeyValue::kEmptyObject
const KeyValue KeyValue::kEmptyObject( KeyValue::kObject );

// ** KeyValue::KeyValue
KeyValue::KeyValue( void ) : m_type( kNull )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( Type type ) : m_type( type )
{
	switch( type ) {
	case kString:	m_string = DC_NEW String;
					break;
	case kObject:	m_object = DC_NEW Properties;
					break;
	case kArray:	m_array  = DC_NEW ValueArray;
					break;
	default:		break;
	}
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( bool value ) : m_type( kBoolean ), m_boolean( value )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( u8 value ) : m_type( kInt8 ), m_int8( value )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( s8 value ) : m_type( kInt8 ), m_int8( value )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( s16 value ) : m_type( kInt16 ), m_int32( value )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( u16 value ) : m_type( kInt16 ), m_int32( value )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( s32 value ) : m_type( kInt32 ), m_int32( value )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( u32 value ) : m_type( kInt32 ), m_int32( value )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( f32 value ) : m_type( kFloat32 ), m_float32( value )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( f64 value ) : m_type( kFloat64 ), m_float64( value )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( const String& value ) : m_type( kString ), m_string( DC_NEW String( value ) )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( CString value ) : m_type( kString ), m_string( DC_NEW String( value ) )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( const Guid& value ) : m_type( kGuid ), m_guid( DC_NEW Guid( value ) )
{
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( const Vec2& value ) : m_type( kVec2 )
{
    m_vector[0] = value.x;
    m_vector[1] = value.y;
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( const Vec3& value ) : m_type( kVec3 )
{
    m_vector[0] = value.x;
    m_vector[1] = value.y;
    m_vector[2] = value.z;
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( const Vec4& value ) : m_type( kVec4 )
{
    m_vector[0] = value.x;
    m_vector[1] = value.y;
    m_vector[2] = value.z;
    m_vector[3] = value.w;
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( const Quat& value ) : m_type( kQuat )
{
    m_vector[0] = value.x;
    m_vector[1] = value.y;
    m_vector[2] = value.z;
    m_vector[3] = value.w;
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( const Rgb& value ) : m_type( kRgb )
{
    m_vector[0] = value.r;
    m_vector[1] = value.g;
    m_vector[2] = value.b;
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( const Rgba& value ) : m_type( kRgba )
{
    m_vector[0] = value.r;
    m_vector[1] = value.g;
    m_vector[2] = value.b;
    m_vector[3] = value.a;
}

// ** KeyValue::KeyValue
KeyValue::KeyValue( const KeyValue& other ) : m_type( other.m_type )
{
	setKeyValue( other );
}

// ** KeyValue::~KeyValue
KeyValue::~KeyValue( void )
{
	clear();
}

// ** KeyValue::object
KeyValue KeyValue::object( void )
{
	return KeyValue( kObject );
}

// ** KeyValue::array
KeyValue KeyValue::array( void )
{
	return KeyValue( kArray );
}

// ** KeyValue::operator =
const KeyValue& KeyValue::operator = ( const KeyValue& other )
{
	setKeyValue( other );
	return *this;
}

// ** KeyValue::operator bool
KeyValue::operator bool( void ) const
{
	return m_boolean;
}

// ** KeyValue::operator u8
KeyValue::operator u8( void ) const
{
	return m_int8;
}

// ** KeyValue::operator s8
KeyValue::operator s8( void ) const
{
	return m_int8;
}

// ** KeyValue::operator u16
KeyValue::operator u16( void ) const
{
	return m_int16;
}

// ** KeyValue::operator s16
KeyValue::operator s16( void ) const
{
	return m_int16;
}

// ** KeyValue::operator u32
KeyValue::operator u32( void ) const
{
	return m_int32;
}

// ** KeyValue::operator s32
KeyValue::operator s32( void ) const
{
	return m_int32;
}

// ** KeyValue::operator f32
KeyValue::operator f32( void ) const
{
	return m_float32;
}

// ** KeyValue::operator f64
KeyValue::operator f64( void ) const
{
	return m_float64;
}

// ** KeyValue::operator String
KeyValue::operator String( void ) const
{
	return *m_string;
}

// ** KeyValue::operator []
const KeyValue& KeyValue::operator []( s32 index ) const
{
	DC_BREAK_IF( m_type != kArray );
	return (*m_array)[index];
}

// ** KeyValue::operator []
KeyValue& KeyValue::operator []( s32 index )
{
	DC_BREAK_IF( m_type != kArray );
	return (*m_array)[index];
}

// ** KeyValue::operator []
const KeyValue& KeyValue::operator []( const String& key ) const
{
	DC_BREAK_IF( m_type != kObject );
	return (*m_object)[key];
}

// ** KeyValue::operator []
const KeyValue& KeyValue::operator []( CString key ) const
{
	DC_BREAK_IF( m_type != kObject );
	return (*m_object)[key];
}

// ** KeyValue::operator []
KeyValue& KeyValue::operator []( const String& key )
{
	DC_BREAK_IF( m_type != kObject );
	return (*m_object)[key];
}

// ** KeyValue::operator []
KeyValue& KeyValue::operator []( CString key )
{
	DC_BREAK_IF( m_type != kObject );
	return (*m_object)[key];
}

// ** KeyValue::operator <<
KeyValue& KeyValue::operator << ( CString value )
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

// ** KeyValue::operator <<
KeyValue& KeyValue::operator << ( const String& value )
{
	return this->operator << ( value.c_str() );
}

// ** KeyValue::isNull
bool KeyValue::isNull( void ) const
{
	return m_type == kNull;
}

// ** KeyValue::isObject
bool KeyValue::isObject( void ) const
{
	return m_type == kObject;
}

// ** KeyValue::isArray
bool KeyValue::isArray( void ) const
{
	return m_type == kArray;
}

// ** KeyValue::type
KeyValue::Type KeyValue::type( void ) const
{
	return m_type;
}

// ** KeyValue::properties
const KeyValue::Properties& KeyValue::properties( void ) const
{
	DC_BREAK_IF( m_type != kObject );
	return *m_object;
}

// ** KeyValue::properties
KeyValue::Properties& KeyValue::properties( void )
{
	DC_BREAK_IF( m_type != kObject );
	return *m_object;
}

// ** KeyValue::items
const KeyValue::ValueArray& KeyValue::items( void ) const
{
	DC_BREAK_IF( m_type != kArray );
	return *m_array;
}

// ** KeyValue::size
s32 KeyValue::size( void ) const
{
	return m_type == kArray ? m_array->size() : 0;
}


// ** KeyValue::get
const KeyValue& KeyValue::get( const String& key, const KeyValue& defaultValue ) const
{
	if( m_type != kObject ) {
		return defaultValue;
	}

	Properties::const_iterator i = m_object->find( key );
	return i != m_object->end() ? i->second : defaultValue;
}

// ** KeyValue::asBool
bool KeyValue::asBool( void ) const
{
	return m_boolean;
}

// ** KeyValue::asByte
s8 KeyValue::asByte( void ) const
{
	return m_int8;
}

// ** KeyValue::asUByte
u8 KeyValue::asUByte( void ) const
{
	return m_int8;
}

// ** KeyValue::asShort
s16 KeyValue::asShort( void ) const
{
	return m_int16;
}

// ** KeyValue::asUShort
u16 KeyValue::asUShort( void ) const
{
	return m_int16;
}

// ** KeyValue::asInt
s32 KeyValue::asInt( void ) const
{
	switch( m_type ) {
	case kInt8:		return static_cast<s32>( m_int8 );
	case kInt16:	return static_cast<s32>( m_int16 );
	case kInt32:	return m_int32;
	case kInt64:	return static_cast<s32>( m_int64 );
	case kFloat32:	return static_cast<u32>( m_float32 );
	case kFloat64:	return static_cast<u32>( m_float64 );
	}

	return 0;
}

// ** KeyValue::asUInt
u32 KeyValue::asUInt( void ) const
{
	return asInt();
}

// ** KeyValue::asFloat
f32 KeyValue::asFloat( void ) const
{
	switch( m_type ) {
	case kInt8:		return static_cast<f32>( m_int8 );
	case kInt16:	return static_cast<f32>( m_int16 );
	case kInt32:	return static_cast<f32>( static_cast<s32>( m_int32 ) );
	case kInt64:	return static_cast<f32>( m_int64 );
	case kFloat32:	return m_float32;
	case kFloat64:	return static_cast<f32>( m_float64 );
	}

	return 0.0f;
}

// ** KeyValue::asDouble
f64 KeyValue::asDouble( void ) const
{
	switch( m_type ) {
	case kInt8:		return static_cast<f64>( m_int8 );
	case kInt16:	return static_cast<f64>( m_int16 );
	case kInt32:	return static_cast<f64>( m_int32 );
	case kInt64:	return static_cast<f64>( m_int64 );
	case kFloat32:	return static_cast<f64>( m_float32 );
	case kFloat64:	return m_float64;
	}

	return 0.0;
}

// ** KeyValue::asVec2
Vec2 KeyValue::asVec2( void ) const
{
	switch( m_type ) {
	case kVec2:		
	case kVec3:
	case kVec4:
	case kQuat:
	case kRgb:
	case kRgba:	    return Vec2( m_vector );
	}

	return Vec2( 0.0f, 0.0f );
}

// ** KeyValue::asVec3
Vec3 KeyValue::asVec3( void ) const
{
	switch( m_type ) {
	case kVec2:		
	case kVec3:
	case kVec4:
	case kQuat:
	case kRgb:
	case kRgba:	    return Vec3( m_vector );
	}

	return Vec3();
}

// ** KeyValue::asVec4
Vec4 KeyValue::asVec4( void ) const
{
	switch( m_type ) {
	case kVec2:		
	case kVec3:
	case kVec4:
	case kQuat:
	case kRgb:
	case kRgba:	    return Vec4( m_vector );
	}

	return Vec4();
}

// ** KeyValue::asQuat
Quat KeyValue::asQuat( void ) const
{
	switch( m_type ) {
	case kVec2:		
	case kVec3:
	case kVec4:
	case kQuat:
	case kRgb:
	case kRgba:	    return Quat( m_vector[0], m_vector[1], m_vector[2], m_vector[3] );
	}

	return Quat();
}

// ** KeyValue::asRgb
Rgb KeyValue::asRgb( void ) const
{
	switch( m_type ) {
	case kVec2:		
	case kVec3:
	case kVec4:
	case kQuat:
	case kRgb:
	case kRgba:	    return Rgb( m_vector );
	}

	return Rgb();
}

// ** KeyValue::asRgba
Rgba KeyValue::asRgba( void ) const
{
	switch( m_type ) {
	case kVec2:		
	case kVec3:
	case kVec4:
	case kQuat:
	case kRgb:
	case kRgba:	    return Rgba( m_vector );
	}

	return Rgba();
}

// ** KeyValue::asLong
s64 KeyValue::asLong( void ) const
{
	return m_int64;
}

// ** KeyValue::asULong
u64 KeyValue::asULong( void ) const
{
	return m_int64;
}

// ** KeyValue::asGuid
const Guid& KeyValue::asGuid( void ) const
{
	return *m_guid;
}

// ** KeyValue::asString
const String& KeyValue::asString( void ) const
{
	return *m_string;
}

// ** KeyValue::setKeyValue
void KeyValue::setKeyValue( const KeyValue& value )
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
	case kObject:	m_object = DC_NEW Properties( *value.m_object );
					break;
    case kVec2:
    case kVec3:
    case kVec4:
    case kQuat:
    case kRgb:
    case kRgba:     memcpy( m_vector, value.m_vector, sizeof( m_vector ) );
                    break;
	default:		DC_BREAK;
	}
}

// ** KeyValue::clear
void KeyValue::clear( void )
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
    case kVec2:
    case kVec3:
    case kVec4:
    case kQuat:
    case kRgb:
    case kRgba:
	case kString:	break;
	case kGuid:		delete m_guid;
					m_guid = NULL;
					break;
	case kArray:	delete m_array;
					m_array = NULL;
					break;
	case kObject:	delete m_object;
					break;
	default:		DC_BREAK;
	}

	m_type = kNull;
}

// ** KeyValue::write
s32 KeyValue::write( StreamPtr stream ) const
{
	DC_BREAK_IF( !stream.valid() );

	// KeyValue layout type placeholder.
	u32 type  = 0;

	// Save the starting position
	s32 start = stream->position();

	// Write the layout type.
	stream->write( &type, 4 );

	// Write the value.
	writeValue( stream );

	return stream->position() - start;
}

// ** KeyValue::writeValue
void KeyValue::writeValue( StreamPtr stream ) const
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
						for( Properties::const_iterator i = m_object->begin(), end = m_object->end(); i != end; ++i ) {
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

// ** KeyValue::read
s32 KeyValue::read( StreamPtr stream )
{
	DC_BREAK_IF( !stream.valid() );

	// Save the starting position
	s32 start = stream->position();

	// KeyValue layout type placeholder.
	u32 type = 0;

	// Read the layout type.
	stream->read( &type, 4 );

	// Read actual data.
	readValue( stream );

	return stream->position() - start;
}

// ** KeyValue::readValue
void KeyValue::readValue( StreamPtr stream )
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
						m_object = DC_NEW Properties;

						while( stream->hasDataLeft() ) {
							String key;

							// Read the property key
							stream->readString( key );

							if( key.empty() ) {
								break;
							}

							(*m_object)[key] = KeyValue();
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

// ** KeyValue::read
void KeyValue::read( const Storage* storage )
{
	read( storage->isBinaryStorage()->stream() );
}

// ** KeyValue::write
void KeyValue::write( Storage* storage ) const
{
	write( storage->isBinaryStorage()->stream() );
}

// ** KeyValue::stringify
String KeyValue::stringify( const KeyValue& kv, bool formatted )
{
#ifdef HAVE_JSON
	Json::Value json = toJson( kv );
	return formatted ? json.toStyledString() : Json::FastWriter().write( json );
#else
	LogError( "KeyValue::stringify : failed to convert to string, built with no JSON support.\n" );
	return "";
#endif	/*	HAVE_JSON	*/
}

// ** KeyValue::parse
KeyValue KeyValue::parse( const String& text )
{
#ifdef HAVE_JSON
	Json::Value json;
	Json::Reader reader;
	
	if( !reader.parse( text, json ) ) {
		LogWarning( "KeyValue::parse : failed to parse JSON string.\n" );
		return kNull;
	}

	return fromJson( json );
#else
	LogError( "KeyValue::parse : failed to parse from string, built with no JSON support.\n" );
	return kNull;
#endif	/*	HAVE_JSON	*/
}

#ifdef HAVE_JSON

// ** KeyValue::toJson
Json::Value KeyValue::toJson( const KeyValue& kv )
{
	switch( kv.type() ) {
	case kNull:		return Json::Value::null;
	case kBoolean:	return kv.asBool();
	case kInt8:		
	case kInt16:
	case kInt32:
	case kInt64:	return kv.asInt();
	case kFloat32:
	case kFloat64:	return kv.asDouble();
	case kString:	return kv.asString();
	case kGuid:		return toJson( object() << "typeID" << "guid" << "value" << kv.asGuid().toString() );
    case kVec2:     return toJson( object() <<  "x" << kv.asVec2().x <<  "y" << kv.asVec2().y );
    case kVec3:     return toJson( object() <<  "x" << kv.asVec3().x <<  "y" << kv.asVec3().y <<  "z" << kv.asVec3().z );
    case kVec4:     return toJson( object() <<  "x" << kv.asVec4().x <<  "y" << kv.asVec4().y <<  "z" << kv.asVec4().z <<  "w" << kv.asVec4().w );
    case kQuat:     return toJson( object() << "qx" << kv.asQuat().x << "qy" << kv.asQuat().y << "qz" << kv.asQuat().z << "qw" << kv.asQuat().w );
    case kRgb:      return toJson( object() <<  "r" << kv.asRgb().r  <<  "g" << kv.asRgb().g  <<  "b" << kv.asRgb().b );
    case kRgba:     return toJson( object() <<  "r" << kv.asRgba().r <<  "g" << kv.asRgba().g  <<  "b" << kv.asRgba().b  <<  "a" << kv.asRgba().a );
	case kArray:	{
						Json::Value	json( Json::arrayValue );

						for( s32 i = 0, n = kv.size(); i < n; i++ ) {
							json[i] = toJson( kv[i] );
						}

						return json;
					}
					break;
	case kObject:	{
						Json::Value		  json( Json::objectValue );
						const Properties& properties = kv.properties();

						for( Properties::const_iterator i = properties.begin(), end = properties.end(); i != end; ++i ) {
							json[i->first] = toJson( i->second );
						}

						return json;
					}
					break;
	default:		DC_BREAK;
	}

	return Json::Value::null;
}

// ** KeyValue::fromJson
KeyValue KeyValue::fromJson( const Json::Value& json )
{
    // Internal parsing helper
    struct Parser {
        static bool vec2( const Json::Value& json, Vec2& result )
        {
            Json::Value x = json.get( "x", Json::Value::null );
            Json::Value y = json.get( "y", Json::Value::null );

            if( x.isNull() || y.isNull() ) {
                return false;
            }

            result.x = x.asFloat();
            result.y = y.asFloat();
            return true;
        }

        static bool vec3( const Json::Value& json, Vec3& result )
        {
            Json::Value x = json.get( "x", Json::Value::null );
            Json::Value y = json.get( "y", Json::Value::null );
            Json::Value z = json.get( "z", Json::Value::null );

            if( x.isNull() || y.isNull() || z.isNull() ) {
                return false;
            }

            result.x = x.asFloat();
            result.y = y.asFloat();
            result.z = z.asFloat();
            return true;
        }

        static bool vec4( const Json::Value& json, Vec4& result )
        {
            Json::Value x = json.get( "x", Json::Value::null );
            Json::Value y = json.get( "y", Json::Value::null );
            Json::Value z = json.get( "z", Json::Value::null );
            Json::Value w = json.get( "w", Json::Value::null );

            if( x.isNull() || y.isNull() || z.isNull() || w.isNull() ) {
                return false;
            }

            result.x = x.asFloat();
            result.y = y.asFloat();
            result.z = z.asFloat();
            result.w = w.asFloat();
            return true;
        }

        static bool quat( const Json::Value& json, Quat& result )
        {
            Json::Value x = json.get( "qx", Json::Value::null );
            Json::Value y = json.get( "qy", Json::Value::null );
            Json::Value z = json.get( "qz", Json::Value::null );
            Json::Value w = json.get( "qw", Json::Value::null );

            if( x.isNull() || y.isNull() || z.isNull() || w.isNull() ) {
                return false;
            }

            result.x = x.asFloat();
            result.y = y.asFloat();
            result.z = z.asFloat();
            result.w = w.asFloat();
            return true;
        }

        static bool rgb( const Json::Value& json, Rgb& result )
        {
            Json::Value r = json.get( "r", Json::Value::null );
            Json::Value g = json.get( "g", Json::Value::null );
            Json::Value b = json.get( "b", Json::Value::null );

            if( r.isNull() || g.isNull() || b.isNull() ) {
                return false;
            }

            result.r = r.asFloat();
            result.g = g.asFloat();
            result.b = b.asFloat();
            return true;
        }

        static bool rgba( const Json::Value& json, Rgba& result )
        {
            Json::Value r = json.get( "r", Json::Value::null );
            Json::Value g = json.get( "g", Json::Value::null );
            Json::Value b = json.get( "b", Json::Value::null );
            Json::Value a = json.get( "a", Json::Value::null );

            if( r.isNull() || g.isNull() || b.isNull() || a.isNull() ) {
                return false;
            }

            result.r = r.asFloat();
            result.g = g.asFloat();
            result.b = b.asFloat();
            result.a = a.asFloat();
            return true;
        }
    };

    Vec2 vec2;
    Vec3 vec3;
    Vec4 vec4;
    Quat quat;
    Rgb  rgb;
    Rgba rgba;

	switch( json.type() ) {
	case Json::nullValue:		return kNull;
	case Json::intValue:		return json.asInt();
	case Json::uintValue:		return json.asUInt();
	case Json::realValue:		return json.asDouble();
	case Json::stringValue:		return json.asString();
	case Json::booleanValue:	return json.asBool();
	case Json::arrayValue:		{
									KeyValue kv( kArray );

									for( s32 i = 0, n = json.size(); i < n; i++ ) {
										kv << fromJson( json[i] );
									}

									return kv;
								}
								break;
	case Json::objectValue:		{
                                    if( Parser::quat( json, quat ) ) return quat;
                                    if( Parser::rgba( json, rgba ) ) return rgba;
                                    if( Parser::rgb( json, rgb ) ) return rgb;
                                    if( Parser::vec4( json, vec4 ) ) return vec4;
                                    if( Parser::vec3( json, vec3 ) ) return vec3;
                                    if( Parser::vec2( json, vec2 ) ) return vec2;

                                    DC_BREAK_IF( !json["typeID"].isNull() );
									KeyValue kv( kObject );

									for( Json::Value::const_iterator i = json.begin(), end = json.end(); i != end; i++ ) {
										String key = i.key().asString();
										kv[key] = fromJson( *i );
									}

									return kv;
								}
								break;
	default:					DC_BREAK;
	}

	return kNull;
}

#endif	/*	HAVE_JSON*/

} // namespace Io

DC_END_DREEMCHEST