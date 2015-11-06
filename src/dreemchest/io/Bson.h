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

#ifndef __DC_Io_Bson_H__
#define __DC_Io_Bson_H__

#include "serialization/Serializable.h"

DC_BEGIN_DREEMCHEST

namespace io {

	//! BSON is a binary key-value storage type.
	class Bson : public Serializable {
	public:

		//! BSON value types.
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

		typedef Map<String, Bson>	KeyValue;	//!< Key-value storage used to hold object properties.
		typedef Array<Bson>			ValueArray;	//!< Array of BSON values.

							//! Constructs Bson instance.
							Bson( void );

							//! Constructs Bson instance of specified type.
							Bson( Type type );

							//! Constructs Bson instance from value.
							Bson( bool value );

							//! Constructs Bson instance from value.
							Bson( u8 value );

							//! Constructs Bson instance from value.
							Bson( s8 value );

							//! Constructs Bson instance from value.
							Bson( u16 value );

							//! Constructs Bson instance from value.
							Bson( s16 value );

							//! Constructs Bson instance from value.
							Bson( u32 value );

							//! Constructs Bson instance from value.
							Bson( s32 value );
							
							//! Constructs Bson instance from value.
							Bson( f32 value );

							//! Constructs Bson instance from value.
							Bson( f64 value );

							//! Constructs Bson instance from value.
							Bson( const String& value );

							//! Constructs Bson instance from value.
							Bson( CString value );

							//! Constructs Bson instance from value.
							Bson( const Guid& value );

							//! Constructs Bson array instance from value.
							template<typename TValue>
							Bson( const Array<TValue>& value );

							//! Copies the Bson instance.
							Bson( const Bson& other );

		virtual				~Bson( void );

							//! Converts BSON value to a boolean.
		explicit			operator bool( void ) const;

							//! Converts BSON value to u8 value.
		explicit			operator u8( void ) const;

							//! Converts BSON value to s8 value.
		explicit			operator s8( void ) const;

							//! Converts BSON value to u16 value.
		explicit			operator u16( void ) const;

							//! Converts BSON value to s16 value.
		explicit			operator s16( void ) const;

							//! Converts BSON value to u32 value.
		explicit			operator u32( void ) const;

							//! Converts BSON value to s32 value.
		explicit			operator s32( void ) const;

							//! Converts BSON value to f32 value.
		explicit			operator f32( void ) const;

							//! Converts BSON value to f64 value.
		explicit			operator f64( void ) const;

							//! Converts BSON value to f64 value.
		explicit			operator String( void ) const;

		//! Copies the Bson instance.
		const Bson&			operator = ( const Bson& other );

		//! Cleans up the Bson instance.
		void				clear( void );

		//! Copies the Bson instance.
		void				setBson( const Bson& value );

		//! Returns the array item with specified index.
		const Bson&			operator []( s32 index ) const;
		Bson&				operator []( s32 index );

		//! Returns the object property with specified key.
		const Bson&			operator []( CString key ) const;
		Bson&				operator []( CString key );

		//! Returns the object property with specified key.
		const Bson&			operator []( const String& key ) const;
		Bson&				operator []( const String& key );

		//! Appends the value to a Bson object.
		Bson&				operator << ( CString key );

		//! Appends the value to a Bson object.
		Bson&				operator << ( const String& key );

		//! Append the value to a Bson object.
		template<typename TValue>
		Bson&				operator << ( const TValue& value );

		//! Append the array of values to a Bson object.
		template<typename TValue>
		Bson&				operator << ( const Array<TValue>& values );

		//! Returns true if the Bson value is null.
		bool				isNull( void ) const;

		//! Returns the Bson value type.
		Type				type( void ) const;

		//! Returns the boolean value of this Bson instance.
		bool				asBool( void ) const;

		//! Returns the byte value of this Bson instance.
		u8					asByte( void ) const;

		//! Returns the short value of this Bson instance.
		u16					asShort( void ) const;

		//! Returns the int value of this Bson instance.
		u32					asInt( void ) const;

		//! Returns the long value of this Bson instance.
		u64					asLong( void ) const;

		//! Returns the float value of this Bson instance.
		f32					asFloat( void ) const;

		//! Returns the double value of this Bson instance.
		f64					asDouble( void ) const;

		//! Returns the GUID value of this Bson instance.
		const Guid&			asGuid( void ) const;

		//! Returns the string value of this Bson instance.
		const String&		asString( void ) const;

		//! Returns object properties.
		const KeyValue&		properties( void ) const;

		//! Returns array items.
		const ValueArray&	items( void ) const;

		//! Returns the Bson object property with specified name.
		const Bson&			get( const String& key, const Bson& defaultValue = kNull ) const;

		//! Writes BSON to a binary stream.
		s32					write( StreamPtr stream ) const;

		//! Reads BSON from a binary stream.
		s32					read( StreamPtr stream );

		//! Reads data from a storage.
		virtual void		read( const Storage* storage );

		//! Writes data to a storage.
		virtual void		write( Storage* storage ) const;

		//! Constructs and returns the Bson object.
		static Bson			object( void );

		//! Constructs and returns the Bson array.
		static Bson			array( void );

		//! Static empty BSON array.
		static const Bson	kEmptyArray;

		//! Static empty BSON object.
		static const Bson	kEmptyObject;

	private:

		//! Writes the BSON value to a binary stream.
		void				writeValue( StreamPtr stream ) const;

		//! Reads BSON from a binary stream.
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
			KeyValue*	m_object;
			ValueArray*	m_array;
		};

		Type	m_type;		//!< Bson value type.
		String	m_key;		//!< Active bson key.
	};

	// ** Bson::Bson
	template<typename TValue>
	Bson::Bson( const Array<TValue>& value )
	{
		Bson items = array();

		for( u32 i = 0, n = ( u32 )value.size(); i < n; i++ ) {
			items << value[i];
		}

		setBson( items );
	}

	// ** Bson::operator <<
	template<typename TValue>
	Bson& Bson::operator << ( const TValue& value )
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

	// ** Bson::operator <<
	template<typename TValue>
	Bson& Bson::operator << ( const Array<TValue>& value )
	{
		DC_BREAK_IF( m_type != kObject && m_type != kArray );

		Bson items = array();

		for( u32 i = 0, n = ( u32 )value.size(); i < n; i++ ) {
			items << value[i];
		}

		return *this << items;
	}

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Bson_H__	*/