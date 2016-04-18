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

#ifndef __DC_Io_Serializable_H__
#define __DC_Io_Serializable_H__

#include "Serializer.h"

#if DEV_DEPRECATED_SERIALIZATION

#define IoRegisterType( Type ) Io::SerializableTypes::registerType<Type>();

#define IoOverrideSerializable( Type )                              \
    virtual Serializable* clone( void ) const { return new Type; }  \
    ClassEnableTypeInfo( Type )

#define IoOverrideSerializableSuper( Type, Super )                  \
    virtual Serializable* clone( void ) const { return new Type; }  \
    ClassEnableTypeInfoSuper( Type, Super )

#define IoBeginSerializer							        \
	virtual Io::SerializerList serializers( void ) const {  \
		Io::SerializerList result;

#define IoBeginSerializerSuper( Base )				        \
	virtual Io::SerializerList serializers( void ) const {  \
	Io::SerializerList result = Base::serializers();

#define IoField( name )	\
    result.push_back( Io::Serializer::create( #name, this->name ) );

#define IoEndSerializer	\
		return result;	\
	}

#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

DC_BEGIN_DREEMCHEST

namespace Io {

#if DEV_DEPRECATED_SERIALIZATION

    //! Base interface class for all serializable data structs.
    class ISerializable : public RefCounted {
    public:

        virtual         ~ISerializable( void ) {}

        //! Reads data from a storage.
        virtual void	read( const Storage& storage ) { DC_NOT_IMPLEMENTED; }

        //! Writes data to a storage.
        virtual void	write( Storage& storage ) const	{ DC_NOT_IMPLEMENTED; }
    };

    //! Base class for all types that has a serializers defined by macroses.
    class Serializable : public ISerializable {
    public:

                        IoOverrideSerializable( Serializable )

                        IoBeginSerializer
                        IoEndSerializer

		//! Reads data from a storage.
		virtual void	read( const Storage* storage );

		//! Writes data to a storage.
		virtual void	write( Storage* storage ) const;
    };

	//! A template class for declaring serializable types and overriding the type info in a base class.
	template<typename T>
	class SerializableT : public Serializable {
	public:

                        IoOverrideSerializable( T )
	};

	//! Serializable types registry.
	class SerializableTypes {
	public:

		//! Returns true if a type with specified id is registered.
		static bool					isRegistered( const TypeId& id );

		//! Creates a serializable type instance by id.
		static Serializable*		create( const TypeId& id );

		//! Creates a serializable type instance by name.
		static Serializable*		create( CString name );

		//! Registers a new serializable type.
		template<typename T>
		static void					registerType( void );

	private:

		//! A container type to store all registered data types by type id.
		typedef Map< TypeId, AutoPtr<Serializable> >	TypeById;

		//! A container type to store all registered data types by type name.
		typedef Map< String, AutoPtr<Serializable> >	TypeByName;

		//! Registered serializable types.
		static TypeById		s_typeById;

		//! Registered serializable types.
		static TypeByName	s_typeByName;
	};

	// ** SerializableTypes::isRegistered
	inline bool SerializableTypes::isRegistered( const TypeId& id )
	{
		return s_typeById.find( id ) != s_typeById.end();
	}

	// ** SerializableTypes::create
	inline Serializable* SerializableTypes::create( const TypeId& id )
	{
		TypeById::iterator i = s_typeById.find( id );

		if( i == s_typeById.end() ) {
			return NULL;
		}

		return i->second->clone();
	}

	// ** SerializableTypes::create
	inline Serializable* SerializableTypes::create( CString name )
	{
		TypeByName::iterator i = s_typeByName.find( name );

		if( i == s_typeByName.end() ) {
			return NULL;
		}

		return i->second->clone();
	}

	// ** SerializableTypes::registerType
	template<typename T>
	inline void SerializableTypes::registerType( void )
	{
		s_typeById  [TypeInfo<T>::id()]   = new T;
		s_typeByName[TypeInfo<T>::name()] = new T;
	}

	// ** Serializable::read
	inline void Serializable::read( const Storage* storage )
	{
		SerializerList fields = serializers();

        const BinaryStorage*   binary   = storage->isBinaryStorage();

    #if DC_DEPRECATED_FEATURE
		const KeyValueStorage* keyValue = storage->isKeyValueStorage();
    #endif  /*  DC_DEPRECATED_FEATURE   */

		for( SerializerList::iterator i = fields.begin(), end = fields.end(); i != end; ++i ) {
        #if DC_DEPRECATED_FEATURE
            binary ? (*i)->read( binary ) : (*i)->read( keyValue );
        #else
            (*i)->read( binary );
        #endif  /*  DC_DEPRECATED_FEATURE   */
		}
	}

	// ** Serializable::write
	inline void Serializable::write( Storage* storage ) const
	{
		SerializerList fields = serializers();

        BinaryStorage*   binary   = storage->isBinaryStorage();

    #if DC_DEPRECATED_FEATURE
		KeyValueStorage* keyValue = storage->isKeyValueStorage();
    #endif  /*  DC_DEPRECATED_FEATURE   */

		for( SerializerList::iterator i = fields.begin(), end = fields.end(); i != end; ++i ) {
        #if DC_DEPRECATED_FEATURE
            binary ? (*i)->write( binary ) : (*i)->write( keyValue );
        #else
            (*i)->write( binary );
        #endif  /*  DC_DEPRECATED_FEATURE   */
		}
	}

#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

} // namespace Io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Serializable_H__	*/