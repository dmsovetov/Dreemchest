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

// ** VariantTextStream::stringify
String VariantTextStream::stringify( const Variant& kv, bool formatted )
{
#ifdef HAVE_JSON
	Json::Value json = toJson( kv );
	return formatted ? json.toStyledString() : Json::FastWriter().write( json );
#else
	LogError( "variant", "failed to convert to string, built with no JSON support.\n" );
	return "";
#endif	/*	HAVE_JSON	*/
}

// ** VariantTextStream::parse
Variant VariantTextStream::parse( const String& text )
{
#ifdef HAVE_JSON
	Json::Value json;
	Json::Reader reader;
	
	if( !reader.parse( text, json ) ) {
		LogWarning( "keyValue", "failed to parse JSON string.\n" );
		return Variant();
	}

	return fromJson( json );
#else
	LogError( "variant", "failed to parse from string, built with no JSON support.\n" );
	return Variant();
#endif	/*	HAVE_JSON	*/
}

#ifdef HAVE_JSON

// ** VariantTextStream::toJson
Json::Value VariantTextStream::toJson( const Variant& value )
{
    const Type* type = value.type();

    if( !value.isValid() ) {
        return Json::Value::null;
    }
    else if( type->is<bool>() ) {
        return value.as<bool>();
    }
    else if( type->isArithmetic() ) {
        return value.as<double>();
    }
    else if( type->is<String>() ) {
        return value.as<String>();
    }
    else if( type->is<Guid>() ) {
	    return toJson( KvBuilder() << "typeID" << "guid" << "value" << value.as<Guid>().toString() );
    }
    else if( type->is<Vec2>() ) {
        return toJson( KvBuilder() <<  "x" << value.as<Vec2>().x <<  "y" << value.as<Vec2>().y );
    }
    else if( type->is<Vec3>() ) {
        return toJson( KvBuilder() <<  "x" << value.as<Vec3>().x <<  "y" << value.as<Vec3>().y <<  "z" << value.as<Vec3>().z );
    }
    else if( type->is<Vec4>() ) {
        return toJson( KvBuilder() <<  "x" << value.as<Vec4>().x <<  "y" << value.as<Vec4>().y <<  "z" << value.as<Vec4>().z <<  "w" << value.as<Vec4>().w );
    }
    else if( type->is<Quat>() ) {
         return toJson( KvBuilder() << "qx" << value.as<Quat>().x << "qy" << value.as<Quat>().y << "qz" << value.as<Quat>().z << "qw" << value.as<Quat>().w );
    }
    else if( type->is<Rgb>() ) {
        return toJson( KvBuilder() <<  "r" << value.as<Rgb>().r  <<  "g" << value.as<Rgb>().g  <<  "b" << value.as<Rgb>().b );
    }
    else if( type->is<Rgba>() ) {
        return toJson( KvBuilder() <<  "r" << value.as<Rgba>().r <<  "g" << value.as<Rgba>().g  <<  "b" << value.as<Rgba>().b  <<  "a" << value.as<Rgba>().a );
    }
    else if( type->is<KeyValue>() ) {
		Json::Value		  json( Json::objectValue );
        KeyValue          kv = value.as<KeyValue>();
		const KeyValue::Properties& properties = kv.properties();

		for( KeyValue::Properties::const_iterator i = properties.begin(), end = properties.end(); i != end; ++i ) {
			json[i->first] = toJson( i->second );
		}

		return json;    
    }
    else if( type->is<VariantArray>() ) {
    	Json::Value	json( Json::arrayValue );
        VariantArray ar = value.as<VariantArray>();
        const VariantArray::Container& items = ar;

		for( s32 i = 0, n = items.size(); i < n; i++ ) {
			json[i] = toJson( items[i] );
		}

		return json;
    }
    else {
        DC_NOT_IMPLEMENTED;
    }

	return Json::Value::null;
}

// ** VariantTextStream::fromJson
Variant VariantTextStream::fromJson( const Json::Value& json )
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

        static bool guid( const Json::Value& json, Guid& result )
        {
            if( json.get( "typeID", "" ).asString() == "guid" ) {
                result = Guid( json.get( "value", "" ).asString() );
                return true;
            }

            return false;
        }
    };

    Vec2 vec2;
    Vec3 vec3;
    Vec4 vec4;
    Quat quat;
    Rgb  rgb;
    Rgba rgba;
    Guid guid;

	switch( json.type() ) {
	case Json::nullValue:		return Variant();
    case Json::intValue:		return Variant::fromValue<s32>( json.asInt() );
	case Json::uintValue:		return Variant::fromValue<u32>( json.asUInt() );
	case Json::realValue:		return Variant::fromValue<f64>( json.asDouble() );
	case Json::stringValue:		return Variant::fromValue<String>( json.asString() );
	case Json::booleanValue:	return Variant::fromValue<bool>( json.asBool() );
	case Json::arrayValue:		{
									VariantArray ar;

									for( s32 i = 0, n = json.size(); i < n; i++ ) {
										ar << fromJson( json[i] );
									}

									return Variant::fromValue<VariantArray>( ar );
								}
								break;
	case Json::objectValue:		{
                                    if( Parser::quat( json, quat ) ) return Variant::fromValue<Quat>( quat );
                                    if( Parser::rgba( json, rgba ) ) return Variant::fromValue<Rgba>( rgba );
                                    if( Parser::rgb( json, rgb ) )   return Variant::fromValue<Rgb>( rgb );
                                    if( Parser::vec4( json, vec4 ) ) return Variant::fromValue<Vec4>( vec4 );
                                    if( Parser::vec3( json, vec3 ) ) return Variant::fromValue<Vec3>( vec3 );
                                    if( Parser::vec2( json, vec2 ) ) return Variant::fromValue<Vec2>( vec2 );
                                    if( Parser::guid( json, guid ) ) return Variant::fromValue<Guid>( guid );

                                    DC_BREAK_IF( !json["typeID"].isNull() );
									KeyValue kv;

									for( Json::Value::const_iterator i = json.begin(), end = json.end(); i != end; i++ ) {
										String key = i.key().asString();
										kv.setValueAtKey( key, fromJson( *i ) );
									}

									return Variant::fromValue<KeyValue>( kv );
								}
								break;
	default:					DC_BREAK;
	}

	return Variant();
}

#endif	/*	HAVE_JSON*/

// ** BinaryVariantStream::BinaryVariantStream
BinaryVariantStream::BinaryVariantStream( StreamPtr stream ) : m_stream( stream )
{
    DC_BREAK_IF( !m_stream.valid(), "invalid stream" );
}

// ** BinaryVariantStream::write
s32 BinaryVariantStream::write( const Variant& value )
{
	DC_BREAK_IF( !m_stream.valid() );

	// KeyValue layout type placeholder.
	u32 type = 0;

	// Save the starting position
	s32 start = m_stream->position();

	// Write the layout type.
	m_stream->write( &type, 4 );

	// Write the value.
	writeValue( value );

	return m_stream->position() - start;
}

// ** BinaryVariantStream::read
s32 BinaryVariantStream::read( Variant& value )
{
	// Save the starting position
	s32 start = m_stream->position();

	// KeyValue layout type placeholder.
	u32 type = 0;

	// Read the layout type.
	m_stream->read( &type, 4 );

	// Read actual data.
	value = readValue();

	return m_stream->position() - start;
}

// ** BinaryVariantStream::readValue
Variant BinaryVariantStream::readValue( void )
{
	// Read value type.
    u8 type;
	m_stream->read( &type, 1 );

	switch( type ) {
	case kNull:		break;
	case kBoolean:	{
                        bool value;
                        m_stream->read( &value, 1 );
                        return Variant::fromValue( value );
                    }
                    break;
	case kInt8:		{
                        s8 value;
                        m_stream->read( &value, 1 );
                        return Variant::fromValue( value );
                    }
                    break;
	case kInt16:	{
                        s16 value;
                        m_stream->read( &value, 2 );
                        return Variant::fromValue( value );
                    }
                    break;
	case kInt32:	{
                        s32 value;
                        m_stream->read( &value, 4 );
                        return Variant::fromValue( value );
                    }
                    break;
	case kInt64:	{
                        s64 value;
                        m_stream->read( &value, 8 );
                        return Variant::fromValue( value );
                    }
                    break;
	case kFloat32:	{
                        f32 value;
                        m_stream->read( &value, 4 );
                        return Variant::fromValue( value );
                    }
                    break;
	case kFloat64:	{
                        f64 value;
                        m_stream->read( &value, 8 );
                        return Variant::fromValue( value );
                    }
                    break;
	case kGuid:		{
						u8 bytes[Guid::Size];
						m_stream->read( bytes, Guid::Size );
						return Variant::fromValue<Guid>( bytes );
					}
					break;

	case kObject:	{
						KeyValue object;

						while( m_stream->hasDataLeft() ) {
							String key;

							// Read the property key
							m_stream->readString( key );

							if( key.empty() ) {
								break;
							}

                            object.setValueAtKey( key, readValue() );
						}

                        return Variant::fromValue<KeyValue>( object );
					}
					break;

	case kArray:	{
						VariantArray array;

						u16 count;
						m_stream->read( &count, 2 );

						for( u16 i = 0; i < count; i++ ) {
                            array << readValue();
						}

                        return Variant::fromValue<VariantArray>( array );
					}
					break;

	case kString:	{
						String value;
						m_stream->readString( value );
                        return Variant::fromValue<String>( value );
					}
					break;

	default:		DC_NOT_IMPLEMENTED;
	}

    return Variant();
}

// ** BinaryVariantStream::writeValue
void BinaryVariantStream::writeValue( const Variant& value )
{
	// Write value type.
	u8 type = valueType( value );
	m_stream->write( &type, 1 );

	// Now write the value.
	switch( type ) {
	case kNull:		break;
	case kBoolean:	{
                        bool v = value.as<bool>();
                        m_stream->write( &v, 1 );				
                    }
                    break;
	case kInt8:		{
                        s8 v = value.as<s8>();
                        m_stream->write( &v, 1 );				
                    }
                    break;
	case kInt16:	{
                        s16 v = value.as<s16>();
                        m_stream->write( &v, 2 );				
                    }
                    break;
	case kInt32:	{
                        s32 v = value.as<s32>();
                        m_stream->write( &v, 4 );				
                    }
                    break;
	case kInt64:	{
                        s64 v = value.as<s64>();
                        m_stream->write( &v, 8 );				
                    }
                    break;
	case kFloat32:	{
                        f32 v = value.as<f32>();
                        m_stream->write( &v, 4 );				
                    }
                    break;
	case kFloat64:	{
                        f64 v = value.as<f64>();
                        m_stream->write( &v, 8 );				
                    }
                    break;
	case kString:	{
                        String v = value.as<String>();
                        m_stream->writeString( v.c_str() );				
                    }
                    break;
	case kGuid:		{
                        m_stream->write( value.as<Guid>().bytes(), Guid::Size );				
                    }
                    break;

	case kObject:	{
                        KeyValue::Properties properties = value.as<KeyValue>().properties();
						for( KeyValue::Properties::const_iterator i = properties.begin(), end = properties.end(); i != end; ++i ) {
							m_stream->writeString( i->first.c_str() );
							writeValue( i->second );
						}
						m_stream->writeString( "" );
					}
					break;

	case kArray:	{
                        VariantArray::Container array = value.as<VariantArray>();
						DC_BREAK_IF( array.size() >= USHRT_MAX );

						u16 count = array.size();
						m_stream->write( &count, 2 );

						for( u16 i = 0; i < count; i++ ) {
							writeValue( array[i] );
						}
					}
					break;

	default:		DC_BREAK;
	}
}

#define RETURN_TYPEID_IF( typeA, typeB, id ) if( type->is<typeA>() || type->is<typeB>() ) return id

// ** BinaryVariantStream::valueType
BinaryVariantStream::Type BinaryVariantStream::valueType( const Variant& value ) const
{
    const ::DC_DREEMCHEST_NS Type* type = value.type();
    if( !type ) {
        return kNull;
    }

    RETURN_TYPEID_IF( u8, s8, kInt8 );
    RETURN_TYPEID_IF( u16, s16, kInt16 );
    RETURN_TYPEID_IF( u32, s32, kInt32 );
    RETURN_TYPEID_IF( u64, s64, kInt64 );
	RETURN_TYPEID_IF( f32, f32, kFloat32 );
	RETURN_TYPEID_IF( f64, f64, kFloat32 );
    RETURN_TYPEID_IF( bool, bool, kBoolean );
    RETURN_TYPEID_IF( String, String, kString );
    RETURN_TYPEID_IF( VariantArray, VariantArray, kArray );
    RETURN_TYPEID_IF( KeyValue, KeyValue, kObject );
    RETURN_TYPEID_IF( Guid, Guid, kGuid );

    DC_NOT_IMPLEMENTED;
    return kNull;
}

} // namespace Io

DC_END_DREEMCHEST