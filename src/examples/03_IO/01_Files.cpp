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

//! Override a field serializer for Vec2 data type.
IoBeginFieldSerializer( Vec2 )
    IoWriteField( storage->write( "x", m_pointer->x ); storage->write( "y", m_pointer->y ); )
    IoReadField ( storage->read ( "x", m_pointer->x ); storage->read ( "y", m_pointer->y ); )
IoEndFieldSerializer( Vec2 )

StreamPtr stream;

// Test structure to serialize data.
struct Item : public io::Serializable {
    int x;
    f32 z;
    f64 w;
    std::string msg;

    Vec2 v;

    IoBeginSerializer
        IoField( x )
        IoField( z )
        IoField( w )
        IoField( msg )
        IoField( v )
    IoEndSerializer

    void dump( void ) const {
        printf( "%d %f %f %s (%f %f)\n", x, z, w, msg.c_str(), v.x, v.y );
    }
};

struct ArrayOfPoints : public io::Serializable {
    std::string     label;
    Array<Item>     items;
    Array<u32>      idx;

    IoBeginSerializer
        IoField( label )
        IoArray( items )
        IoArray( idx )
    IoEndSerializer

    void dump( void ) const {
        printf( "%s\n", label.c_str() );
        for( int i = 0; i < items.size(); i++ ) {
            items[i].dump();
        }
        printf( "idx:" );
        for( int i = 0; i < idx.size(); i++ ) {
            printf( " %d", idx[i] );
        }
        printf( "\n" );
    }
};

// Application delegate is used to handle an events raised by application instance.
class Files : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application ) {
        // Setup defaul loggers
        platform::log::setStandardHandler();
        io::log::setStandardHandler();

        // Create a disk file system instance
        DiskFileSystem fs;

        {
            BinaryStorage storage = fs.openFile( "lol", BinaryWriteStream );
            if( !storage ) {
                return;
            }

            Item p1; p1.x = 1; p1.z = 0.01f; p1.w = 123.31112; p1.msg = "hello"; p1.v = Vec2( 11,-12);
            p1.write( &storage );

            printf( "Data written from p1\n" );
            p1.dump();

        //    JsonStorage json;
        //    p1.write( &json );
        //    printf( "result: %s\n", json.save().c_str() );
        }

        {
            BinaryStorage storage = fs.openFile( "lol", BinaryReadStream );
            if( !storage ) {
                return;
            }
            Item p2;
            p2.read( &storage );

            printf( "Data read to p2\n" );
            p2.dump();
        }

        {
            ArrayOfPoints arr;
            arr.label = "12";
            arr.idx.push_back( 1 );
            arr.idx.push_back( 2 );
            for( int i = 0; i < 5; i++ ) {
                Item p1;
                p1.x = i;
                p1.msg = "zxc";
                p1.v.x = 22;
                p1.v.y = 2222;
                arr.items.push_back( p1 );
            }

            BinaryStorage storage = fs.openFile( "points", BinaryWriteStream );
            if( !storage ) {
                return;
            }

            arr.write( &storage );
            printf( "Data written from arr\n" );
            arr.dump();

        //    JsonStorage json;
        //    arr.write( &json );
        //    printf( "result: %s\n", json.save().c_str() );
        }

        {
            BinaryStorage storage = fs.openFile( "points", BinaryReadStream );
            if( !storage ) {
                return;
            }

            ArrayOfPoints arr;
            arr.read( &storage );
            printf( "Data read to arr\n" );
            arr.dump();
        }
    }
};

// Now declare an application entry point with Files application delegate.
dcDeclareApplication( new Files )