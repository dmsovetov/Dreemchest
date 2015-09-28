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

// Include the engine header file.
#include <Dreemchest.h>

// Open a root engine namespace
DC_USE_DREEMCHEST

// Open a platform namespace to use shorter types.
using namespace Platform;

// Open a io namespace.
using namespace io;

struct A {
	float b;
};

struct Nested : public SerializableT<Nested> {
	String			m_a;
	int				m_b;

	IoBeginSerializer
		IoField( m_a )
		IoField( m_b )
	IoEndSerializer
};

struct Data : public SerializableT<Data> {
	bool			m_boolean;
	int				m_integer;
	float			m_float;
	String			m_string;
	Nested			m_nested;
	Array<double>	m_doubles;
	Array<Nested>	m_nestedItems;

	IoBeginSerializer
		IoField( m_string )
		IoField( m_boolean )
		IoField( m_integer )
		IoField( m_float )
		IoField( m_nested )
		IoField( m_doubles )
		IoField( m_nestedItems )
	IoEndSerializer
};

// Application delegate is used to handle an events raised by application instance.
class Files : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application ) {
        // Setup defaul loggers
        Platform::log::setStandardHandler();
        io::log::setStandardHandler();

		Data d1, d2, d3;
		d1.m_boolean = true;
		d1.m_integer = -23;
		d1.m_float   = 2.23f;
		d1.m_string = "str";
		d1.m_doubles.push_back( 1.0 );
		d1.m_doubles.push_back( 1.111 );
		d1.m_nested.m_a = "hello";
		d1.m_nested.m_b = 23;
		d1.m_nestedItems.push_back( d1.m_nested );
		d1.m_nestedItems.push_back( d1.m_nested );

		ByteBufferPtr stream = ByteBuffer::create();
		BinaryStorage binary( stream );

	#ifdef HAVE_JSON
		JsonStorage   json;

		d1.write( &binary );
		stream->setPosition( 0 );
		d2.read( &binary );

		d1.write( &json );
		printf( "%s\n", json.toString().c_str() );
		d3.read( &json );
	#endif
    }
};

// Now declare an application entry point with Files application delegate.
dcDeclareApplication( new Files )