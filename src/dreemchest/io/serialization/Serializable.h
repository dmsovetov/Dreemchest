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
#include    "../streams/Stream.h"

//! Macro definition for serializer fields declaration
#define IoBeginSerializer                           \
    virtual Array<Field> fields( void ) const {     \
        Array<Field> result;

//! Macro definition for adding a serializable field.
#define IoField( field )                            \
    result.push_back( Field( #field, const_cast<void*>( reinterpret_cast<const void*>( &field ) ), fieldWriter( field ), fieldReader( field ) ) );

//! Macro definition for serializer fields declaration
#define IoEndSerializer                             \
        return result;                              \
    }

//! Macro definition for overriding type size
#define IoTypeSize( T, size )   template<> inline u64 sizeOfType<T>( void ) { return size; }

//! Macro definition for overriding field writer
#define IoFieldWriter( T, ... )                                             \
    template<>                                                              \
    inline void io::writeField<T>( StreamPtr& stream, const void* data ) {  \
        const T& value = *reinterpret_cast<const T*>( data );               \
        __VA_ARGS__                                                         \
    }

//! Macro definition for overriding field reader
#define IoFieldReader( T, ... )                                             \
    template<>                                                              \
    inline void io::readField<T>( const StreamPtr& stream, void* data ) {   \
        T& value = *reinterpret_cast<T*>( data );                           \
        __VA_ARGS__                                                         \
    }

DC_BEGIN_DREEMCHEST

namespace io {

    //! A helper function to define a type size for serialization.
    template<typename T>
    u64 sizeOfType( void ) { return 0; }

    //! Override type size for long & integers.
    IoTypeSize( s8,  1 );
    IoTypeSize( u8,  1 );
    IoTypeSize( s16, 2 );
    IoTypeSize( u16, 2 );
    IoTypeSize( s32, 4 );
    IoTypeSize( u32, 4 );
    IoTypeSize( f32, 4 );
    IoTypeSize( u64, 8 );
    IoTypeSize( f64, 8 );

    //! Function type for writing a field value.
    typedef void ( *FieldWriter )( StreamPtr& stream, const void* data );

    //! Function type for reading a field value.
    typedef void ( *FieldReader )( const StreamPtr& stream, void* data );

    //! Base class for all serializable data structs.
    class Serializable {
    public:

        //! Reads data from stream.
        void                read( const StreamPtr& stream );

        //! Writes data to stream.
        void                write( StreamPtr& stream ) const;

    protected:

        //! A data struct for field.
        struct Field {
                            //! Constructs Field instance
                            Field( CString name, void* pointer, FieldWriter writer, FieldReader reader )
                                : m_name( name ), m_pointer( pointer ), m_writer( writer ), m_reader( reader ) {}

            CString         m_name;     //!< Field name.
            void*           m_pointer;  //!< Field data pointer.
            FieldWriter     m_writer;   //!< Field writer.
            FieldReader     m_reader;   //!< Field reader.
        };

        //! Returns an array of serializable fields.
        virtual Array<Field>    fields( void ) const;
    };

    // ------------------------ Field serialization

    template<typename T>
    inline void writeField( StreamPtr& stream, const void* data ) {
        stream->write( data, sizeOfType<T>() );
    }

    template<typename T>
    inline FieldWriter fieldWriter( const T& value ) {
        return &writeField<T>;
    }

    template<typename T>
    inline void readField( const StreamPtr& stream, void* data ) {
        stream->read( data, sizeOfType<T>() );
    }

    template<typename T>
    inline FieldReader fieldReader( const T& value ) {
        return &readField<T>;
    }

    // ------------------------ Array serialization

    template<typename T>
    inline void writeArray( StreamPtr& stream, const void* data ) {
        const Array<T>& array = *reinterpret_cast< const Array<T>* >( data );

        u32 size = array.size();
        stream->write( &size, 4 );

        for( int i = 0; i < size; i++ ) {
            if( sizeOfType<T>() ) {
                writeField<T>( stream, &array[i] );
            } else {
                array[i].write( stream );
            }
        }
    }

    template<typename T>
    inline FieldWriter fieldWriter( const Array<T>& data )
    {
        return &writeArray<T>;
    }

    template<typename T>
    inline void readArray( const StreamPtr& stream, void* data ) {
        Array<T>& array = *reinterpret_cast< Array<T>* >( data );

        u32 size;
        stream->read( &size, 4 );

        array.resize( size );
        for( int i = 0; i < size; i++ ) {
            if( sizeOfType<T>() ) {
                readField<T>( stream, &array[i] );
            } else {
                array[i].read( stream );
            }
        }
    }

    template<typename T>
    inline FieldReader fieldReader( const Array<T>& data )
    {
        return &readArray<T>;
    }

} // namespace io

//! Declare reader/writer for String
IoFieldWriter( String, stream->writeString( value.c_str() ); )
IoFieldReader( String, stream->readString( value ); )

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Serializable_H__	*/