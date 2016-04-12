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