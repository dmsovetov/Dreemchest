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

class Ar : public RefCounted {
public:

	virtual			~Ar( void ) {}
};

class BinaryArchive : public Ar {
public:

	template<typename T>
	void write( const T& value )
	{
		write( &value, sizeof( T ) );
	}

	template<typename T>
	void read( T& value ) const
	{
		read( &value, sizeof( T ) );
	}

	virtual void write( const void* ptr, u32 size )
	{
		printf( "Writing %d bytes\n", size );
	}

	virtual void read( void* ptr, u32 size ) const
	{
		printf( "Reading %d bytes\n", size );
	}
};

typedef StrongPtr<KeyValueArchive> KeyValueArchivePtr;

class KeyValueArchive : public Ar {
public:

	virtual void write( CString key, const Variant& value )
	{
		printf( "Writing %s as %s\n", key, value.typeName() );
	}

	virtual Variant read( CString key ) const
	{
		printf( "Reading %s\n", key );
		return Variant();
	}

	virtual KeyValueArchivePtr object( CString key )
	{
		return KeyValueArchivePtr( new KeyValueArchive );
	}

	virtual KeyValueArchivePtr object( CString key ) const
	{
		return KeyValueArchivePtr( new KeyValueArchive );
	}

	virtual KeyValueArchivePtr array( CString key )
	{
		return KeyValueArchivePtr( new KeyValueArchive );
	}

	virtual KeyValueArchivePtr array( CString key ) const
	{
		return KeyValueArchivePtr( new KeyValueArchive );
	}
};

#define _IoBeginSerializer								\
	static SerializerList serializers( Type* value ) {	\
		SerializerList result;

#define _IoField( name )	\
	result.push_back( Serializer::create( #name, value->name ) );

#define _IoEndSerializer	\
		return result;		\
	}

struct A {
	float b;
};

struct Nested : public SerializableType<Nested> {
	String			m_a;
	int				m_b;

	_IoBeginSerializer
		_IoField( m_a )
		_IoField( m_b )
	_IoEndSerializer

	template<typename T>
	void write( T* ar )
	{
		SerializerList fields = serializers( this );

		for( SerializerList::iterator i = fields.begin(), end = fields.end(); i != end; ++i ) {
			i->get()->write( ar );
		}
	}

	template<typename T>
	void read( const T* ar )
	{
		SerializerList fields = serializers( this );

		for( SerializerList::iterator i = fields.begin(), end = fields.end(); i != end; ++i ) {
			i->get()->read( ar );
		}
	}
};

struct Data : public SerializableType<Data> {
	bool			m_boolean;
	int				m_integer;
	float			m_float;
	String			m_string;
	Nested			m_nested;
	Array<double>	m_doubles;
	Array<Nested>	m_nestedItems;

	template<typename T>
	void write( T* ar )
	{
		SerializerList fields = serializers( this );

		for( SerializerList::iterator i = fields.begin(), end = fields.end(); i != end; ++i ) {
			i->get()->write( ar );
		}
	}

	template<typename T>
	void read( const T* ar )
	{
		SerializerList fields = serializers( this );

		for( SerializerList::iterator i = fields.begin(), end = fields.end(); i != end; ++i ) {
			i->get()->read( ar );
		}
	}

	_IoBeginSerializer
		_IoField( m_string )
		_IoField( m_boolean )
		_IoField( m_integer )
		_IoField( m_float )
		_IoField( m_nested )
		_IoField( m_doubles )
		_IoField( m_nestedItems )
	_IoEndSerializer
};

// Application delegate is used to handle an events raised by application instance.
class Files : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application ) {
        // Setup defaul loggers
        platform::log::setStandardHandler();
        io::log::setStandardHandler();

		Variant integer( 1 );
		Variant boolean( true );
		Variant number( 213.23f );
		Variant str( "hello" );

		printf( "%s\n", integer.toCString() );

		f64 a = number.as<f64>();
		String s = str.as<String>();
		s32 i = str.as<s32>();

		Data d;

		d.m_string = "hello";
		d.m_nestedItems.push_back( Nested() );
		d.m_nestedItems.push_back( Nested() );
		d.m_doubles.push_back( 1.0 );
		d.m_doubles.push_back( 2323.9 );
		d.m_doubles.push_back( 23.0321 );

	//	d.write( new BinaryArchive );
		d.write( new KeyValueArchive );

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