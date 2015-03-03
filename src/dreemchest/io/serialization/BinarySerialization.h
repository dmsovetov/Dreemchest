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

#ifndef		__DC_Io_BinarySerialization_H__
#define		__DC_Io_BinarySerialization_H__

#include	"../Io.h"

DC_BEGIN_DREEMCHEST

namespace io {
    
    // ** writeField
    template<typename T>
    inline void writeField( Stream* stream, const T *data, int count = 1 )
    {
		for( int i = 0; i < count; i++ ) {
			data[i].write( stream );
		}
    }

    // ** writeVector
    template<typename T>
    inline void writeVector( Stream* stream, const T& data )
    {
        int size = ( int )data.size();

        writeField( stream, &size );
		if( size == 0 ) {
			return;
		}

		writeField( stream, &data[0], size );
    }

    // ** writeMap
    template<typename T>
    inline void writeMap( Stream* stream, const T& data )
    {
        int size = ( int )data.size();

        writeField( stream, &size );
        if( size == 0 ) {
            return;
        }

        for( typename T::const_iterator i = data.begin(), end = data.end(); i != end; ++i ) {
            writeField( stream, &i->first );
            writeField( stream, &i->second );
        }
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const String *data, int count )
    {
		for( int i = 0; i < count; i++ ) {
			writeVector( stream, data[i] );
		}
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const bool *data, int count )
    {
        stream->write( data, count * sizeof( bool ) );
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const int *data, int count )
    {
        stream->write( data, count * sizeof( int ) );
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const float *data, int count )
    {
        stream->write( data, count * sizeof( float ) );
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const double *data, int count )
    {
        stream->write( data, count * sizeof( double ) );
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const unsigned short *data, int count )
    {
        stream->write( data, count * sizeof( unsigned short ) );
    }

	// ** writeField
	template<>
	inline void writeField( Stream* stream, const unsigned int *data, int count )
	{
		stream->write( data, count * sizeof( unsigned int ) );
	}

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const unsigned char *data, int count )
    {
        stream->write( data, count * sizeof( unsigned char ) );
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const char *data, int count )
    {
        stream->write( data, count * sizeof( char ) );
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const unsigned long *data, int count )
    {
        stream->write( data, count * sizeof( unsigned long ) );
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const Vec2 *data, int count )
    {
		for( int i = 0; i < count; i++ ) {
			stream->write( &data[i].x, sizeof( float ) );
			stream->write( &data[i].y, sizeof( float ) );
		}
    }
/*
    // ** writeField
    template<>
    inline void writeField( Stream* stream, const Vec3 *data, int count )
    {
		for( int i = 0; i < count; i++ ) {
			stream->write( &data[i].x, sizeof( float ) );
			stream->write( &data[i].y, sizeof( float ) );
			stream->write( &data[i].z, sizeof( float ) );
		}
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const Vec4 *data, int count )
    {
		for( int i = 0; i < count; i++ ) {
			stream->write( &data[i].x, sizeof( float ) );
			stream->write( &data[i].y, sizeof( float ) );
			stream->write( &data[i].z, sizeof( float ) );
			stream->write( &data[i].w, sizeof( float ) );
		}
    }

    // ** writeField
    template<>
    inline void writeField( Stream* stream, const cValue *data, int count )
    {
        for( int i = 0; i < count; i++ ) {
            data[i].writeToStream( stream );
        }
    }
*/
    // ** readField
    template<typename T>
    inline void readField( Stream* stream, T *data, int count = 1 )
    {
		for( int i = 0; i < count; i++ ) {
			data[i].read( stream );
		}
    }

    // ** readVector
    template<typename T>
    inline void readVector( Stream* stream, T& data )
    {
        int size = 0;

        readField( stream, &size );
		if( size == 0 ) {
			return;
		}

        data.resize( size );
		readField( stream, &data[0], size );
    }

    // ** readMap
    template<typename T>
    inline void readMap( Stream* stream, T& data )
    {
        int size = 0;

        readField( stream, &size );
		if( size == 0 ) {
			return;
		}

        typename T::key_type key;
        typename T::mapped_type value;

        for( int i = 0; i < size; i++ ) {
            readField( stream, &key );
            readField( stream, &value );

            data[key] = value;
        }
    }

    // ** readField
    template<>
    inline void readField( Stream* stream, String *data, int count )
    {
		for( int i = 0; i < count; i++ ) {
			readVector( stream, data[i] );
		}
    }

    // ** readField
    template<>
    inline void readField( Stream* stream, bool *data, int count )
    {
        stream->read( data, count * sizeof( bool ) );
    }

    // ** readField
    template<>
    inline void readField( Stream* stream, int *data, int count )
    {
        stream->read( data, count * sizeof( int ) );
    }

    // ** readField
    template<>
    inline void readField( Stream* stream, float *data, int count )
    {
        stream->read( data, count * sizeof( float ) );
    }

    // ** readField
    template<>
    inline void readField( Stream* stream, double *data, int count )
    {
        stream->read( data, count * sizeof( double ) );
    }

    // ** readField
    template<>
    inline void readField( Stream* stream, unsigned short *data, int count )
    {
        stream->read( data, count * sizeof( unsigned short ) );
    }

	// ** readField
	template<>
	inline void readField( Stream* stream, unsigned int *data, int count )
	{
		stream->read( data, count * sizeof( unsigned int ) );
	}

    // ** readField
    template<>
    inline void readField( Stream* stream, unsigned char *data, int count )
    {
        stream->read( data, count * sizeof( unsigned char ) );
    }

    // ** readField
    template<>
    inline void readField( Stream* stream, unsigned long *data, int count )
    {
        stream->read( data, count * sizeof( unsigned long ) );
    }
/*
    // ** readField
    template<>
    inline void readField( Stream* stream, vec2 *data, int count )
    {
		for( int i = 0; i < count; i++ ) {
			stream->read( &data[i].x, sizeof( float ) );
			stream->read( &data[i].y, sizeof( float ) );
		}
    }

    // ** readField
    template<>
    inline void readField( Stream* stream, cVector3 *data, int count )
    {
		for( int i = 0; i < count; i++ ) {
			stream->read( &data[i].x, sizeof( float ) );
			stream->read( &data[i].y, sizeof( float ) );
			stream->read( &data[i].z, sizeof( float ) );
		}
    }

    // ** readField
    template<>
    inline void readField( Stream* stream, vec4 *data, int count )
    {
		for( int i = 0; i < count; i++ ) {
			stream->read( &data[i].x, sizeof( float ) );
			stream->read( &data[i].y, sizeof( float ) );
			stream->read( &data[i].z, sizeof( float ) );
			stream->read( &data[i].w, sizeof( float ) );
		}
    }
*/
    // ** readField
    template<>
    inline void readField( Stream* stream, char *data, int count )
    {
        stream->read( data, count * sizeof( char ) );
    }
/*
    // ** readField
    template<>
    inline void readField( Stream* stream, cValue *data, int count )
    {
        for( int i = 0; i < count; i++ ) {
            data[i].readFromStream( stream );
        }
    }
*/
} // namespace io
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Io_BinarySerialization_H__	*/