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

#ifndef		__DC_Io_Storage_H__
#define		__DC_Io_Storage_H__

#include	"../Io.h"
#include    "../streams/Stream.h"

#ifdef HAVE_JSONCPP
	#include <json/json.h>
#endif

DC_BEGIN_DREEMCHEST

namespace io {

	//! Basic data storage interface
	class Storage : public RefCounted {
	public:

						DC_DECLARE_IS(BinaryStorage, BinaryStorage, NULL)
						DC_DECLARE_IS(KeyValueStorage, KeyValueStorage, NULL)

		virtual			~Storage( void ) {}

		//! Storage key.
		struct Key {
						//! Constructs Key instance with an object field name.
						Key( CString name )
							: name( name ), index( -1 ) {}

						//! Constructs Key instance with an array index.
						Key( s32 index )
							: name( NULL ), index( index ) {}

			CString		name;	//!< Object field.
			s32			index;	//!< Array index.
		};
	};

	//! Binary storage interface.
	class BinaryStorage : public Storage {
	public:

						DC_DECLARE_IS(BinaryStorage, BinaryStorage, this)

						//! Constructs BinaryStorage instance.
						BinaryStorage( const StreamPtr& stream );

		//! Writes the specified amount of bytes to a binary storage.
		virtual void	write( const void* ptr, u32 size );

		//! Reads the specified amount of bytes from a binary storage.
		virtual void	read( void* ptr, u32 size ) const;

		//! Writes value to a binary storage.
		template<typename T>
		void			write( const T& value );

		//! Reads value from a binary storage.
		template<typename T>
		void			read( T& value ) const;

	protected:

		//! Storage stream.
		StreamPtr		m_stream;
	};

	// ** BinaryStorage::write
	template<typename T>
	inline void BinaryStorage::write( const T& value )
	{
		write( &value, sizeof( T ) );
	}

	// ** BinaryStorage::read
	template<typename T>
	inline void BinaryStorage::read( T& value ) const
	{
		read( &value, sizeof( T ) );
	}

	//! Key value storage interface.
	class KeyValueStorage : public Storage {
	public:

									DC_DECLARE_IS(KeyValueStorage, KeyValueStorage, this)

									//! Constructs a KeyValueStorage.
									KeyValueStorage( Variant* value = NULL );

		//! Writes a value to a specified key of a storage.
		virtual void				write( const Key& key, const Variant& value );

		//! Reads a value at a specified key from a storage.
		virtual Variant				read( const Key& key ) const;

		//! Writes a new object to a specified key of a storage.
		virtual KeyValueStoragePtr	object( const Key& key );

		//! Reads an object from a specified key of a storage.
		virtual KeyValueStoragePtr	object( const Key& key ) const;

		//! Writes a new array to a specified key of a storage.
		virtual KeyValueStoragePtr	array( const Key& key );

		//! Reads an array from a specified key of a storage.
		virtual KeyValueStoragePtr	array( const Key& key ) const;

		//! Returns a total number of entries that exist inside this key-value storage.
		virtual u32					size( void ) const;

	private:

		//! Returns a property reference with a specified key.
		Variant&					get( const Key& key ) const;

	private:

		//! Root value.
		Variant*					m_root;
	};

#ifdef HAVE_JSONCPP

	//! Json key value storage.
	class JsonStorage : public KeyValueStorage {
	public:

									//! Constructs JsonStorage instance
									JsonStorage( Json::Value* root = NULL );
		virtual						~JsonStorage( void );

		//! Writes a value to a specified key of a storage.
		virtual void				write( const Key& key, const Variant& value );

		//! Reads a value at a specified key from a storage.
		virtual Variant				read( const Key& key ) const;

		//! Writes a new object to a specified key of a storage.
		virtual KeyValueStoragePtr	object( const Key& key );

		//! Reads an object from a specified key of a storage.
		virtual KeyValueStoragePtr	object( const Key& key ) const;

		//! Writes a new array to a specified key of a storage.
		virtual KeyValueStoragePtr	array( const Key& key );

		//! Reads an array from a specified key of a storage.
		virtual KeyValueStoragePtr	array( const Key& key ) const;

		//! Returns a total number of entries that exist inside this key-value storage.
		virtual u32					size( void ) const;

		//! Converts a storage to JSON string.
		String						toString( void ) const;

	private:

		//! Returns a value reference with a specified key.
		Json::Value&				get( const Key& key ) const;

	public:

		//! Flag indicating that we own a JSON value.
		bool						m_isOwned;

		//! Root JSON object.
		Json::Value*				m_root;
	};

#endif	// HAVE_JSONCPP

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Storage_H__	*/