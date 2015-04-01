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

// Include a Platform module header.
#include <platform/Platform.h>

// Include a IO module header.
//
// This module contains all file system staff.
#include <io/Io.h>

// Open a root engine namespace
DC_USE_DREEMCHEST

// Open a platform namespace to use shorter types.
using namespace platform;

// Open a io namespace.
using namespace io;

class BinaryArchive {
public:

	virtual void write( const void* data, u32 size )
	{
		printf( "Writing %d bytes\n", size );
	}

	virtual void read( void* data, u32 size )
	{
		printf( "Reading %d bytes\n", size );
	}
};

class KeyValueArchive {
public:

	virtual void write( CString key )
	{
		printf( "Writing %s\n", key );
	}

	virtual void read( CString key )
	{
		printf( "Reading %s\n", key );
	}
};

struct Serializer {
	virtual void write( BinaryArchive* ar, const u8* data ) = 0;
	virtual void read( const BinaryArchive* ar, u8* data ) = 0;

	virtual void write( KeyValueArchive* ar, CString name, const u8* data ) = 0;
	virtual void read( const KeyValueArchive* ar, CString name, u8* data ) = 0;

	template<typename T>
	static Serializer& get( const T& value );

	template<typename T>
	const T& as( const void* data )
	{
		return *reinterpret_cast<const T*>( reinterpret_cast<u8*>( data ) + m_offset );
	}

	template<typename T>
	T& as( void* data )
	{
		return *reinterpret_cast<T*>( reinterpret_cast<u8*>( data ) + m_offset );
	}
};

template<typename T>
struct PodSerializer : public Serializer
{
	virtual void write( BinaryArchive* ar, const u8* data )
	{
		ar->write( data, sizeof( T ) );
	}

	virtual void read( const BinaryArchive* ar, u8* data )
	{
		ar->read( data, sizeof( T ) );
	}

	virtual void write( KeyValueArchive* ar, CString name, const u8* data )
	{
		ar->write( name );
	}

	virtual void read( const KeyValueArchive* ar, CString name, u8* data )
	{
		ar->read( name );
	}
};

template<typename T>
struct ObjectSerializer : public Serializer
{
	virtual void write( BinaryArchive* ar, const u8* data )
	{
		T::write( ar, as<T>() );
	}

	virtual void read( const BinaryArchive* ar, u8* data )
	{
		T::read( ar, as<T>() );
	}

	virtual void write( KeyValueArchive* ar, const u8* data )
	{
		T::write( ar, as<T>() );
	}

	virtual void read( const KeyValueArchive* ar, u8* data )
	{
		T::read( ar, as<T>() );
	}
};

template<typename T>
struct CollectionSerializer : public Serializer
{
	virtual void write( BinaryArchive* ar, const u8* data )
	{
		printf( "Collection with %d elements\n", as<T>( data ).size() );
	}

	virtual void read( const BinaryArchive* ar, u8* data )
	{

	}

	virtual void write( KeyValueArchive* ar, const u8* data )
	{

	}

	virtual void read( const KeyValueArchive* ar, u8* data )
	{

	}
};

//! Value serialization type.
template <class T, class Enable = void>
struct SerializerType
{
	typedef PodSerializer Type;
};

//! Object serializer type.
template <class T>
struct SerializerType< T, typename std::enable_if<std::is_base_of<SerializableType<T>, T>::value>::type >
{
	typedef ObjectSerializer Type;
};

//! Collection serializer type.
template <class T>
struct SerializerType< Array<T> >
{
	typedef CollectionSerializer Type;
};

template<typename T>
Serializer& Serializer::get( const T& value )
{
	static SerializerType<T>::Type serializer;
	return serializer;
}

/*
typedef void (*ShowProc)();

template<typename T>
ShowProc getShow( const T& value )
{
	return baz<T>::show;
}
*/
/*
struct ITypeSerializer {
	virtual void	write( const void* ptr, StreamPtr& stream ) = 0;
	virtual void	read( void* ptr, const StreamPtr& stream ) = 0;
};

template<typename T>
struct TypeSerializer : public ITypeSerializer {
	static T& get( void )
	{
		static T serializer;
		return serializer;
	}

	String	name;
};

template<typename T>
struct PodSerializer : public TypeSerializer {
	virtual void write( const void* ptr, StreamPtr& stream )
	{
		stream->write( ptr, sizeof( T ) );
	}

	virtual void read( void* ptr, const StreamPtr& stream )
	{
		stream->read( ptr, sizeof( T ) );
	}
};

template<typename T>
struct ComplexSerializer : public TypeSerializer {
				ComplexSerializer( u32 offset, String name )
					: Field( offset, name ) {}

	virtual void write( const void* ptr, StreamPtr& stream )
	{
		reinterpret_cast<const T*>( ptr )->write( stream );
	}

	virtual void read( void* ptr, const StreamPtr& stream )
	{
		reinterpret_cast<T*>( ptr )->read( stream );
	}
};

template<typename Collection, typename Type>
struct CollectionSerializer : public TypeSerializer {
				CollectionSerializer( u32 offset, String name )
					: TypeSerializer( offset, name ) {}

	virtual void write( const void* ptr, StreamPtr& stream )
	{
		const Collection<T>& collection = *reinterpret_cast< const Collection<T> >( ptr + offset );

		for( T::const_iterator i = collection->begin(), end = collection->end(); i != end; ++i ) {
	
		}
	}

	virtual void read( void* ptr, const StreamPtr& stream )
	{
		reinterpret_cast<T*>( ptr + offset )->read( stream );
	}
};

typedef Array<ITypeSerializer> Fields;

struct ISerializable {
	virtual void	write( StreamPtr& stream ) = 0;
	virtual void	read( const StreamPtr& stream ) = 0;
};

template<typename T>
struct SerializableTypeEx {
	typedef T Type;

	virtual void	write( StreamPtr& stream )
	{

	}

	virtual void	read( const StreamPtr& stream )
	{

	}
};

// ¬ключение перегруженных вариантов foo2 при помощи дополнительного неиспользуемого параметра
template<class T>
TypeSerializer* foo2(const char* name, T& t, typename EnableIf< !IsBaseOf<SerializableTypeEx<T>, T>::Value, T >::Type* = 0)
{
	printf( "%s is a POD type\n", name );
    return NULL;
}

template<class T>
TypeSerializer* foo2(const char* name, T& t, typename EnableIf< IsBaseOf<SerializableTypeEx<T>, T>::Value, T >::Type* = 0)
{
	printf( "%s is a serializable type\n", name );
    return NULL;
}

template<class T>
TypeSerializer* foo2(const char* name, Array<T>& t)
{
	printf( "%s is an array\n", name );
	return NULL;
}
*/
struct A {
	float b;
};

struct Nested : public SerializableType<Nested> {
	String			m_a;
	int				m_b;
};

struct Data : public SerializableType<Data> {
	bool			m_boolean;
	int				m_integer;
	float			m_float;
	String			m_string;
	Nested			m_nested;
	Array<double>	m_doubles;
	Array<Nested>	m_nestedItems;

	static void write( BinaryArchive* ar, const Data& value )
	{
		Serializer::get( value.m_integer ).write( ar, "m_integer", &m_integer );
	}
};

/*
template<typename T>
void getTypeSerializer( T& value )
{
	TypeSerializer<T>::Type::show();
}*/


// Application delegate is used to handle an events raised by application instance.
class Files : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application ) {
        // Setup defaul loggers
        platform::log::setStandardHandler();
        io::log::setStandardHandler();

		Data d;

		Data::write( new BinaryArchive, d );

/*
		Data z;

		foo2( "m_boolean", z.m_boolean );
		foo2( "m_integer", z.m_integer );
		foo2( "m_float", z.m_float );
		foo2( "m_a", z.m_a );
		foo2( "m_string", z.m_string );
		foo2( "m_xx", z.m_xx );
		foo2( "m_nested", z.m_nested );
		foo2( "m_doubles", z.m_doubles );
		foo2( "m_nestedItems", z.m_nestedItems );
*/
    }
};

// Now declare an application entry point with Files application delegate.
dcDeclareApplication( new Files )