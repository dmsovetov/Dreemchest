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

#ifndef		__DC_Io_JSONSerialization_H__
#define		__DC_Io_JSONSerialization_H__

#include	"../Io.h"

DC_BEGIN_DREEMCHEST

namespace io {

    // ** writeField
    template<typename T>
    inline void writeField( JSON& stream, const T *data, const JSON::sKey& key )
    {
        JSON target = stream[key];
        data->write( target );
    }

    // ** writeVector
    template<typename T>
    inline void writeVector( JSON& stream, const T& data, const JSON::sKey& key )
    {
        JSON array( JSON::Array );
        
        for( int i = 0, n = ( int )data.size(); i < n; i++ ) {
            writeField( array, &data[i], i );
        }

        stream[key] = array;
    }

    // ** writeMap
    template<typename T>
    inline void writeMap( JSON& stream, const T& data, const JSON::sKey& key )
    {
        JSON map( JSON::Object );

        for( typename T::const_iterator i = data.begin(), end = data.end(); i != end; ++i ) {
            writeField( map, &i->second, i->first );
        }

        stream[key] = map;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const String *data, const JSON::sKey& key )
    {
        stream[key] = data->c_str();
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const bool *data, const JSON::sKey& key )
    {
        stream[key] = *data;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const int *data, const JSON::sKey& key )
    {
        stream[key] = ( float )*data;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const float *data, const JSON::sKey& key )
    {
        stream[key] = ( float )*data;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const double *data, const JSON::sKey& key )
    {
        stream[key] = ( float )*data;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const unsigned short *data, const JSON::sKey& key )
    {
        stream[key] = ( float )*data;
    }

	// ** writeField
	template<>
	inline void writeField( JSON& stream, const unsigned int *data, const JSON::sKey& key )
	{
		stream[key] = ( float )*data;
	}

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const unsigned char *data, const JSON::sKey& key )
    {
        stream[key] = ( float )*data;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const char *data, const JSON::sKey& key )
    {
        stream[key] = ( float )*data;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const unsigned long *data, const JSON::sKey& key )
    {
        stream[key] = ( float )*data;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const vec2 *data, const JSON::sKey& key )
    {
        JSON v( JSON::Array );

        v[0] = data->x;
        v[1] = data->y;

        stream[key] = v;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const cVector3 *data, const JSON::sKey& key )
    {
        JSON v( JSON::Array );

        v[0] = data->x;
        v[1] = data->y;
        v[2] = data->z;

        stream[key] = v;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const vec4 *data, const JSON::sKey& key )
    {
        JSON v( JSON::Array );

        v[0] = data->x;
        v[1] = data->y;
        v[2] = data->z;
        v[3] = data->w;

        stream[key] = v;
    }

    // ** writeField
    template<>
    inline void writeField( JSON& stream, const cValue *data, const JSON::sKey& key )
    {
        data->writeToJson( stream, key );
    }

    // ** readField
    template<typename T>
    inline void readField( const JSON& stream, T *data, const JSON::sKey& key )
    {
        data->read( stream[key] );
    }

    // ** readVector
    template<typename T>
    inline void readVector( const JSON& stream, T& data, const JSON::sKey& key )
    {
        JSON array = stream[key];

        data.resize( array.GetSize() );

        for( int i = 0, n = ( int )data.size(); i < n; i++ ) {
            readField( array, &data[i], i );
        }
    }

    // ** readMap
    template<typename T>
    inline void readMap( const JSON& stream, T& data, const JSON::sKey& key )
    {
        JSON       map  = stream[key];
        StringList keys = map.GetKeys();
        typename T::mapped_type value;

        for( StringList::const_iterator i = keys.begin(), end = keys.end(); i != end; ++i ) {
            readField( map, &value, *i );
            data[*i] = value;
        }
    }

    // ** readField
    template<>
    inline void readField( const JSON& stream, String *data, const JSON::sKey& key )
    {
        *data = stream[key].AsString();
    }

    // ** readField
    template<>
    inline void readField( const JSON& stream, bool *data, const JSON::sKey& key )
    {
        *data = stream[key].AsBool();
    }

    // ** readField
    template<>
    inline void readField( const JSON& stream, int *data, const JSON::sKey& key )
    {
        *data = stream[key].AsNumber();
    }

    // ** readField
    template<>
    inline void readField( const JSON& stream, float *data, const JSON::sKey& key )
    {
        *data = stream[key].AsNumber();
    }

    // ** readField
    template<>
    inline void readField( const JSON& stream, double *data, const JSON::sKey& key )
    {
        *data = stream[key].AsNumber();
    }

    // ** readField
    template<>
    inline void readField( const JSON& stream, unsigned short *data, const JSON::sKey& key )
    {
        *data = stream[key].AsNumber();
    }

	// ** readField
	template<>
	inline void readField( const JSON& stream, unsigned int *data, const JSON::sKey& key )
	{
		*data = stream[key].AsNumber();
	}

    // ** readField
    template<>
    inline void readField( const JSON& stream, unsigned char *data, const JSON::sKey& key )
    {
        *data = stream[key].AsNumber();
    }

    // ** readField
    template<>
    inline void readField( const JSON& stream, unsigned long *data, const JSON::sKey& key )
    {
        *data = stream[key].AsNumber();
    }

    // ** readField
    template<>
    inline void readField( const JSON& stream, vec2 *data, const JSON::sKey& key )
    {
        JSON v = stream[key];

        data->x = v[0].AsNumber();
        data->y = v[1].AsNumber();
    }
    
    // ** readField
    template<>
    inline void readField( const JSON& stream, cVector3 *data, const JSON::sKey& key )
    {
        JSON v = stream[key];

        data->x = v[0].AsNumber();
        data->y = v[1].AsNumber();
        data->z = v[2].AsNumber();
    }
    
    // ** readField
    template<>
    inline void readField( const JSON& stream, vec4 *data, const JSON::sKey& key )
    {
        JSON v = stream[key];

        data->x = v[0].AsNumber();
        data->y = v[1].AsNumber();
        data->z = v[2].AsNumber();
        data->w = v[3].AsNumber();
    }
    
    // ** readField
    template<>
    inline void readField( const JSON& stream, char *data, const JSON::sKey& key )
    {
        DC_NOT_IMPLEMENTED;
    }
    
    // ** readField
    template<>
    inline void readField( const JSON& stream, cValue *data, const JSON::sKey& key )
    {
        data->readFromJson( stream, key );
    }
    
} // namespace io
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Io_BinarySerialization_H__	*/