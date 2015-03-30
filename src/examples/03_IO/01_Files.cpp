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

struct Field {
			Field( u32 size, u32 offset, String name )
				: size( size ), offset( offset ), name( name ) {}

	u32		size;
	u32		offset;
	String	name;
};

typedef Array<Field> Fields;

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

struct Nested : public SerializableTypeEx<Nested> {
	String			m_a;
	int				m_b;
};

struct Data : public SerializableTypeEx<Data> {
	bool			m_boolean;
	int				m_integer;
	float			m_float;
	String			m_string;
	SerializableTypeEx<Nested> m_xx;
	Nested			m_nested;
	Array<double>	m_doubles;
	Array<Nested>	m_nestedItems;
}; 

template<typename T>
void create( CString name, const T* value )
{
	printf( "%s is a POD type size=%d\n", name, sizeof( T ) );
}

template<typename T>
void create( CString name, const Array<T>* value )
{
	printf( "%s is an array\n", name );
}

template<typename T>
void create( CString name, const SerializableTypeEx<T>* value )
{
	printf( "%s is a serializable type\n", name );
}

// Application delegate is used to handle an events raised by application instance.
class Files : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application ) {
        // Setup defaul loggers
        platform::log::setStandardHandler();
        io::log::setStandardHandler();

		Data z;
		create( "m_boolean", &z.m_boolean );
		create( "m_integer", &z.m_integer );
		create( "m_float", &z.m_float );
		create( "m_string", &z.m_string );
		create( "m_xx", &z.m_xx );
		create( "m_nested", &z.m_nested );
		create( "m_doubles", &z.m_doubles );
		create( "m_nestedItems", &z.m_nestedItems );
    }
};

// Now declare an application entry point with Files application delegate.
dcDeclareApplication( new Files )