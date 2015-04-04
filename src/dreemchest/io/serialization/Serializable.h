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

#ifndef		__DC_Io_Serializable_H__
#define		__DC_Io_Serializable_H__

#include	"../Io.h"

#define IoBeginSerializer( Type )							\
	static io::SerializerList serializers( Type* value ) {	\
		io::SerializerList result;

#define IoBeginSerializerSuper( Type, Base )				\
	static io::SerializerList serializers( Type* value ) {	\
	io::SerializerList result = Base::serializers( value );

#define IoField( name )	\
	result.push_back( io::Serializer::create( #name, value->name ) );

#define IoEndSerializer	\
		return result;	\
	}

DC_BEGIN_DREEMCHEST

namespace io {

    //! Base class for all serializable data structs.
    class Serializable : public RefCounted {
    public:

						ClassEnableCloning( Serializable )
						ClassEnableTypeInfo( Serializable )

        //! Reads data from a storage.
        virtual void	read( const Storage& storage ) { DC_BREAK; }

        //! Writes data to a storage.
        virtual void	write( Storage& storage ) const	{ DC_BREAK; }
    };

	//! A template class for declaring serializable types.
	template<typename T>
	class SerializableType : public Serializable {
	public:

		//! A typedef of a template parameter.
	//	typedef T Type;

		//! Returns true if the specified type matches this type.
		virtual bool is( const TypeId& id ) const { return id == TypeInfo<T>::id(); }

		//! Clones this instance.
		virtual Serializable* clone( void ) const { return new T; }

		//! Returns a type id.
		virtual TypeId  typeId( void ) const  { return TypeInfo<T>::id(); }

		//! Returns a type index for this component.
		virtual TypeIdx	typeIndex( void ) const { return TypeIndex<T>::idx(); }

		//! Returns a type name.
		virtual CString typeName( void ) const { return TypeInfo<T>::name(); }

		//! Returns a type index.
		static  TypeIdx typeIdx( void ) { return TypeIndex<T>::idx(); }

		//! Returns a list of serializable fields.
		static SerializerList serializers( T* value ) { return SerializerList(); }

		//! Reads data from a storage.
		virtual void	read( const Storage& storage );

		//! Writes data to a storage.
		virtual void	write( Storage& storage ) const;

		//! Writes serializable type to a storage.
		template<typename Storage>
		void write( Storage* storage ) const
		{
			SerializerList fields = T::serializers( const_cast<T*>( static_cast<const T*>( this ) ) );

			for( SerializerList::iterator i = fields.begin(), end = fields.end(); i != end; ++i ) {
				i->get()->write( storage );
			}
		}

		//! Reads serializable type from a storage.
		template<typename Storage>
		void read( const Storage* storage )
		{
			SerializerList fields = T::serializers( static_cast<T*>( this ) );

			for( SerializerList::iterator i = fields.begin(), end = fields.end(); i != end; ++i ) {
				i->get()->read( storage );
			}
		}
	};

	// ** SerializableType::read
	template<typename T>
	inline void SerializableType<T>::read( const Storage& storage )
	{
		if( const BinaryStorage* binary = storage.isBinaryStorage() ) {
			read( binary );
		}
		else if( const KeyValueStorage* keyValue = storage.isKeyValueStorage() ) {
			read( keyValue );
		}
		else {
			DC_BREAK;
		}
	}

	// ** SerializableType::write
	template<typename T>
	inline void SerializableType<T>::write( Storage& storage ) const
	{
		if( BinaryStorage* binary = storage.isBinaryStorage() ) {
			write( binary );
		}
		else if( KeyValueStorage* keyValue = storage.isKeyValueStorage() ) {
			write( keyValue );
		}
		else {
			DC_BREAK;
		}
	}

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Serializable_H__	*/