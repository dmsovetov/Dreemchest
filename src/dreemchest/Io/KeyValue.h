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

#ifndef __DC_Io_KeyValue_H__
#define __DC_Io_KeyValue_H__

#include "serialization/Serializable.h"

DC_BEGIN_DREEMCHEST

namespace Io {

	//! Key-value storage type.
	class KeyValue : public Serializable {
	public:

		//! Key-value value types.
		enum Type {
			  kNull
			, kBoolean
			, kInt8
			, kInt16
			, kInt32
			, kInt64
			, kFloat32
			, kFloat64
			, kString
			, kGuid
			, kArray
			, kObject
		};

		typedef Map<String, KeyValue>	Properties;	//!< Key-value storage used to hold object properties.
		typedef Array<KeyValue>			ValueArray;	//!< Array of KeyValue values.

							//! Constructs KeyValue instance.
							KeyValue( void );

							//! Constructs KeyValue instance of specified type.
							KeyValue( Type type );

							//! Constructs KeyValue instance from value.
							KeyValue( bool value );

							//! Constructs KeyValue instance from value.
							KeyValue( u8 value );

							//! Constructs KeyValue instance from value.
							KeyValue( s8 value );

							//! Constructs KeyValue instance from value.
							KeyValue( u16 value );

							//! Constructs KeyValue instance from value.
							KeyValue( s16 value );

							//! Constructs KeyValue instance from value.
							KeyValue( u32 value );

							//! Constructs KeyValue instance from value.
							KeyValue( s32 value );
							
							//! Constructs KeyValue instance from value.
							KeyValue( f32 value );

							//! Constructs KeyValue instance from value.
							KeyValue( f64 value );

							//! Constructs KeyValue instance from value.
							KeyValue( const String& value );

							//! Constructs KeyValue instance from value.
							KeyValue( CString value );

							//! Constructs KeyValue instance from value.
							KeyValue( const Guid& value );

							//! Constructs KeyValue array instance from value.
							template<typename TValue>
							KeyValue( const Array<TValue>& value );

							//! Copies the KeyValue instance.
							KeyValue( const KeyValue& other );

		virtual				~KeyValue( void );

							//! Converts KeyValue value to a boolean.
		explicit			operator bool( void ) const;

							//! Converts KeyValue value to u8 value.
		explicit			operator u8( void ) const;

							//! Converts KeyValue value to s8 value.
		explicit			operator s8( void ) const;

							//! Converts KeyValue value to u16 value.
		explicit			operator u16( void ) const;

							//! Converts KeyValue value to s16 value.
		explicit			operator s16( void ) const;

							//! Converts KeyValue value to u32 value.
		explicit			operator u32( void ) const;

							//! Converts KeyValue value to s32 value.
		explicit			operator s32( void ) const;

							//! Converts KeyValue value to f32 value.
		explicit			operator f32( void ) const;

							//! Converts KeyValue value to f64 value.
		explicit			operator f64( void ) const;

							//! Converts KeyValue value to f64 value.
		explicit			operator String( void ) const;

		//! Copies the KeyValue instance.
		const KeyValue&		operator = ( const KeyValue& other );

		//! Cleans up the KeyValue instance.
		void				clear( void );

		//! Copies the KeyValue instance.
		void				setKeyValue( const KeyValue& value );

		//! Returns the array item with specified index.
		const KeyValue&		operator []( s32 index ) const;
		KeyValue&			operator []( s32 index );

		//! Returns the object property with specified key.
		const KeyValue&		operator []( CString key ) const;
		KeyValue&			operator []( CString key );

		//! Returns the object property with specified key.
		const KeyValue&		operator []( const String& key ) const;
		KeyValue&			operator []( const String& key );

		//! Appends the value to a KeyValue object.
		KeyValue&			operator << ( CString key );

		//! Appends the value to a KeyValue object.
		KeyValue&			operator << ( const String& key );

		//! Append the value to a KeyValue object.
		template<typename TValue>
		KeyValue&			operator << ( const TValue& value );

		//! Append the array of values to a KeyValue object.
		template<typename TValue>
		KeyValue&			operator << ( const Array<TValue>& values );

		//! Returns true if the KeyValue value is null.
		bool				isNull( void ) const;

		//! Returns true if the KeyValue value is an object.
		bool				isObject( void ) const;

		//! Returns true if the KeyValue value is array.
		bool				isArray( void ) const;

		//! Returns the KeyValue value type.
		Type				type( void ) const;

		//! Returns the KeyValue array size.
		s32					size( void ) const;

		//! Returns the boolean value of this KeyValue instance.
		bool				asBool( void ) const;

		//! Returns the byte value of this KeyValue instance.
		s8					asByte( void ) const;

		//! Returns the unsigned byte value of this KeyValue instance.
		u8					asUByte( void ) const;

		//! Returns the short value of this KeyValue instance.
		s16					asShort( void ) const;

		//! Returns the unsigned short value of this KeyValue instance.
		u16					asUShort( void ) const;

		//! Returns the int value of this KeyValue instance.
		s32					asInt( void ) const;

		//! Returns the unsigned int value of this KeyValue instance.
		u32					asUInt( void ) const;

		//! Returns the long value of this KeyValue instance.
		s64					asLong( void ) const;

		//! Returns the unsigned long value of this KeyValue instance.
		u64					asULong( void ) const;

		//! Returns the float value of this KeyValue instance.
		f32					asFloat( void ) const;

		//! Returns the double value of this KeyValue instance.
		f64					asDouble( void ) const;

		//! Returns the GUID value of this KeyValue instance.
		const Guid&			asGuid( void ) const;

		//! Returns the string value of this KeyValue instance.
		const String&		asString( void ) const;

		//! Converts KeyValue to a JSON string.
		static String		stringify( const KeyValue& kv, bool formatted = false );

		//! Parses KeyValue from a JSON string.
		static KeyValue		parse( const String& text );

	#ifdef HAVE_JSON
		//! Converts KeyValue to JSON object.
		static Json::Value	toJson( const KeyValue& kv );

		//! Constructs KeyValue from JSON object.
		static KeyValue		fromJson( const Json::Value& json );
	#endif	/*	HAVE_JSON*/

		//! Returns object properties.
		const Properties&	properties( void ) const;
		Properties&			properties( void );

		//! Returns array items.
		const ValueArray&	items( void ) const;

		//! Returns the KeyValue object property with specified name.
		const KeyValue&		get( const String& key, const KeyValue& defaultValue = kNull ) const;

		//! Writes KeyValue to a binary stream.
		s32					write( StreamPtr stream ) const;

		//! Reads KeyValue from a binary stream.
		s32					read( StreamPtr stream );

		//! Reads data from a storage.
		virtual void		read( const Storage* storage );

		//! Writes data to a storage.
		virtual void		write( Storage* storage ) const;

		//! Constructs and returns the KeyValue object.
		static KeyValue		object( void );

		//! Constructs and returns the KeyValue array.
		static KeyValue		array( void );

		//! Static empty KeyValue array.
		static const KeyValue	kEmptyArray;

		//! Static empty KeyValue object.
		static const KeyValue	kEmptyObject;

	private:

		//! Writes the KeyValue value to a binary stream.
		void				writeValue( StreamPtr stream ) const;

		//! Reads KeyValue from a binary stream.
		void				readValue( StreamPtr stream );

	private:

		union {
			bool		m_boolean;
			u8			m_int8;
			u16			m_int16;
			u32			m_int32;
			u64			m_int64;
			f32			m_float32;
			f64			m_float64;
			Guid*		m_guid;
			String*		m_string;
			Properties*	m_object;
			ValueArray*	m_array;
		};

		Type	m_type;		//!< KeyValue value type.
		String	m_key;		//!< Active KeyValue key.
	};

	// ** KeyValue::KeyValue
	template<typename TValue>
	KeyValue::KeyValue( const Array<TValue>& value )
	{
		KeyValue items = array();

		for( u32 i = 0, n = ( u32 )value.size(); i < n; i++ ) {
			items << value[i];
		}

		setKeyValue( items );
	}

	// ** KeyValue::operator <<
	template<typename TValue>
	KeyValue& KeyValue::operator << ( const TValue& value )
	{
		DC_BREAK_IF( m_type != kObject && m_type != kArray );

		if( m_type == kObject ) {
			DC_BREAK_IF( m_key.empty() );
			(*this)[m_key] = value;
		}
		else if( m_type == kArray ) {
			m_array->push_back( value );
		}

		m_key = "";	

		return *this;
	}

	// ** KeyValue::operator <<
	template<typename TValue>
	KeyValue& KeyValue::operator << ( const Array<TValue>& value )
	{
		DC_BREAK_IF( m_type != kObject && m_type != kArray );

		KeyValue items = array();

		for( u32 i = 0, n = ( u32 )value.size(); i < n; i++ ) {
			items << value[i];
		}

		return *this << items;
	}

} // namespace Io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_KeyValue_H__	*/