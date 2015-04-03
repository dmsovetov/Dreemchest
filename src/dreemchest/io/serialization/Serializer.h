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

#ifndef __Io_Serializer_H__
#define __Io_Serializer_H__

#include "../Io.h"

DC_BEGIN_DREEMCHEST

namespace io {

	//! Base class for a field serializers.
	class Serializer : RefCounted {
								//! Constructs field serializer.
								Serializer( CString name )
									: m_name( name ) {}

		virtual					~Serializer( void ) {}

		//! Writes field to a binary storage.
		virtual void			write( BinaryArchive* ar ) = 0;

		//! Reads field from a binary storage.
		virtual void			read( const BinaryArchive* ar ) = 0;

		//! Writes field to a binary storage.
		virtual void			write( KeyValueArchive* ar ) = 0;

		//! Reads field from a binary storage.
		virtual void			read( const KeyValueArchive* ar ) = 0;

		//! Creates a field serializer with a specified type.
		template<typename T>
		static SerializerPtr	create( CString name, const T& value );

	protected:

		//! Field name.
		CString					m_name;
	};

	//! Generic type serializer class.
	template<typename T>
	class TypeSerializer : public Serializer {
	public:
				
								//! Constructs a TypeSerializer instance.
								TypeSerializer( CString name, T& value )
									: Serializer( name ), m_value( value ) {}
		virtual					~TypeSerializer( void ) {}

	protected:

		//! Reference to a serializable value.
		T&						m_value;
	};

	//! A POD field serializer.
	template<typename T>
	class PodSerializer : public TypeSerializer<T> {
	public:

								//! Constructs POD serializer instance.
								PodSerializer( CString name, T& value )
									: TypeSerializer( name, value ) {}

		//! Writes value to a binary storage.
		virtual void			write( BinaryArchive* ar );

		//! Reads value from a binary storage.
		virtual void			read( const BinaryArchive* ar );

		//! Writes value to a key-value storage.
		virtual void			write( KeyValueArchive* ar );

		//! Reads value from a key-value storage.
		virtual void			read( const KeyValueArchive* ar );
	};

	//! Object serializer type
	template<typename T>
	class ObjectSerializer : public TypeSerializer<T> {
	public:

								//! Constructs object serializer instance.
								ObjectSerializer( CString name, T& value )
									: TypeSerializer( name, value ) {}

		//! Writes object to a binary storage.
		virtual void			write( BinaryArchive* ar );

		//! Reads object from a binary storage.
		virtual void			read( const BinaryArchive* ar );

		//! Writes object to a key-value storage.
		virtual void			write( KeyValueArchive* ar );

		//! Reads object from a key-value storage.
		virtual void			read( const KeyValueArchive* ar );
	};

	//! String serializer type.
	class StringSerializer : public TypeSerializer<String> {
	public:

								//! Constructs string serializer instance.
								StringSerializer( CString name, String& value )
									: TypeSerializer( name, value ) {}

		//! Writes string to a binary storage.
		virtual void			write( BinaryArchive* ar );

		//! Reads string from binary storage.
		virtual void			read( const BinaryArchive* ar );

		//! Writes string to a key-value storage.
		virtual void			write( KeyValueArchive* ar );

		//! Reads string from a key-value storage.
		virtual void			read( const KeyValueArchive* ar );
	};

	//! Collection serializer type.
	template<typename T, typename E>
	struct CollectionSerializer : public TypeSerializer<T>
	{
								//! Constructs CollectionSerializer type.
								CollectionSerializer( CString name, T& value )
									: TypeSerializer( name, value ) {}

		//! Writes collection to a binary storage.
		virtual void			write( BinaryArchive* ar );

		//! Reads collection from a binary storage.
		virtual void			read( const BinaryArchive* ar );

		//! Writes collection to a key-value storage.
		virtual void			write( KeyValueArchive* ar );

		//! Reads collection from a key-value storage.
		virtual void			read( const KeyValueArchive* ar );
	};

	//! Value serialization type.
	template <class T, class Enable = void>
	struct SerializerType
	{
		typedef PodSerializer<T> Type;
	};

	//! Object serializer type.
	template <class T>
	struct SerializerType< T, typename std::enable_if<std::is_base_of<SerializableType<T>, T>::value>::type >
	{
		typedef ObjectSerializer<T> Type;
	};

	//! Collection serializer type.
	template <class T>
	struct SerializerType< Array<T> >
	{
		typedef CollectionSerializer< Array<T>, T > Type;
	};

	//! String serializer type.
	template <>
	struct SerializerType< String >
	{
		typedef StringSerializer Type;
	};

	// ** Serializer::create
	template<typename T>
	StrongPtr<Serializer> Serializer::create( CString name, const T& value )
	{
		return StrongPtr<Serializer>( new SerializerType<T>::Type( name, const_cast<T&>( value ) ) );
	}

} // namespace io

DC_END_DREEMCHEST

#endif	/*	!__Io_Serializer_H__	*/
