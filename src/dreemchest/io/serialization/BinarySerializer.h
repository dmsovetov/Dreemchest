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

#ifndef __Io_BinarySerializer_H__
#define __Io_BinarySerializer_H__

#include "../Io.h"

DC_BEGIN_DREEMCHEST

namespace io {

	//! BinarySerializer class writes serializable types to binary storage.
	class BinarySerializer {
	public:

		//! Binary serializer result.
		enum Result {
			Success,		//!< Binary data successfuly read from storage.
			NotEnoughData,	//!< Not enough data to read serializable.
			Malformed,		//!< Malformed input data.
			Unknown,		//!< Unknown serializable type.
		};

		//! Read a serializable from storage.
		static Result				read( ByteBufferPtr& bytes, Serializable** data );

		//! Reads a list of serializables from storage.
		static Serializables		read( ByteBufferPtr& bytes );

		//! Reads a serializable of a specified type from a byte buffer.
		template<typename T>
		static T					read( const Array<u8>& bytes );

		//! Writes a serializable to storage.
		static s32					write( ByteBufferPtr& bytes, Serializable* data );

		//! Writes a serializable to a byte buffer.
		static ByteBufferPtr		write( const Serializable& data );

		//! Registers a new serializable type.
		template<typename T>
		static void					registerType( void );

	private:

		//! Serializable data header.
		struct Header {
			TypeId		m_type;	//!< Serialized data type.
			u16			m_size;	//!< Serialized data size.

						//! Constructs Header instance.
						Header( TypeId type = 0, u16 size = 0 )
							: m_type( type ), m_size( size ) {}

			static s32	size( void ) { return sizeof( TypeId ) + sizeof( u16 ); }
		};

		//! A container type to store all registered data types.
		typedef Map< TypeId, AutoPtr<Serializable> >	SerializableTypes;

		//! Registered serializable types.
		static SerializableTypes	s_types;
	};

	// ** BinarySerializer::registerType
	template<typename T>
	inline void BinarySerializer::registerType( void )
	{
		s_types[TypeInfo<T>::id()] = new T;
	}

	// ** BinarySerializer::read
	template<typename T>
	inline T BinarySerializer::read( const Array<u8>& bytes )
	{
		T data;

		if( bytes.empty() ) {
			return data;
		}

		BinaryStorage storage( ByteBuffer::createFromData( &bytes[0], bytes.size() ) );
		data.read( &storage );
		return data;
	}

} // namespace io

DC_END_DREEMCHEST

#endif	/*	!__Io_BinarySerializer_H__	*/
