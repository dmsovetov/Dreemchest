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
    result.push_back( Field( #field, const_cast<void*>( reinterpret_cast<const void*>( &field ) ), generateFieldWriter( field ), generateFieldReader( field ) ) );

//! Macro definition for adding a serializable array.
#define IoArray( field )                            \
    result.push_back( Field( #field, const_cast<void*>( reinterpret_cast<const void*>( &field ) ), generateArrayWriter( field ), generateArrayReader( field ) ) );

//! Macro definition for serializer fields declaration
#define IoEndSerializer                             \
        return result;                              \
    }

//! Macro definition for overriding type size
#define IoTypeSize( T, size )   template<> struct TypeSize<T> { enum { value = size }; };

//! Macro definition for overriding field writer
#define IoFieldWriter( T, ... )                                                 \
    template<>                                                                  \
    inline io::FieldWriter io::generateFieldWriter( const T& value ) {          \
        struct _##T {                                                           \
            static void thunk( io::Storage& storage, const void* data ) {       \
                const T& value = *reinterpret_cast<const T*>( data );           \
                __VA_ARGS__;                                                    \
            }                                                                   \
        };                                                                      \
        return _##T::thunk;                                                     \
    }

//! Macro definition for overriding POD field writer
#define IoPODWriter( T )                                                        \
    template<>                                                                  \
    inline io::FieldWriter io::generateFieldWriter( const T& value ) {          \
        struct POD {                                                            \
            static void thunk( io::Storage& storage, const void* data ) {       \
                storage.write( data, io::TypeSize<T>::value );                  \
            }                                                                   \
        };                                                                      \
        return POD::thunk;                                                      \
    }

//! Macro definition for overriding field reader
#define IoFieldReader( T, ... )                                                 \
    template<>                                                                  \
    inline io::FieldReader io::generateFieldReader( const T& value ) {          \
        struct _##T {                                                           \
            static void thunk( const io::Storage& storage, void* data ) {       \
                T& value = *reinterpret_cast<T*>( data );                       \
                __VA_ARGS__;                                                    \
            }                                                                   \
        };                                                                      \
        return _##T::thunk;                                                     \
}

//! Macro definition for overriding POD field reader
#define IoPODReader( T )                                                        \
    template<>                                                                  \
    inline io::FieldReader io::generateFieldReader( const T& value ) {          \
        struct POD {                                                            \
            static void thunk( const io::Storage& storage, void* data ) {       \
                storage.read( data, io::TypeSize<T>::value );                   \
            }                                                                   \
        };                                                                      \
        return POD::thunk;                                                      \
    }

DC_BEGIN_DREEMCHEST

namespace io {

    //! A helper struct to define a type size for serialization.
    template<typename T>
    struct TypeSize {
        enum { value = 0 };
    };

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
    typedef void ( *FieldWriter )( class Storage& storage, const void* data );

    //! Function type for reading a field value.
    typedef void ( *FieldReader )( const class Storage& storage, void* data );

    //! Forward declaration for field readers/writers
    template<typename T> FieldReader generateFieldReader( const T& value );
    template<typename T> FieldWriter generateFieldWriter( const T& value );

    //! Storage interface
    class Storage {
    public:

                            Storage( const StreamPtr& stream )
                                : m_stream( stream ) {}

        operator bool() const { return m_stream != NULL; }

        void beginWritingArray( u32 size )
        {
            write( &size, 4 );
        }

        void endWritingArray( void )
        {

        }

        u32 beginReadingArray( void ) const
        {
            u32 size;
            read( &size, 4 );
            return size;
        }

        void endReadingArray( void ) const
        {
            
        }

        template<typename T>
        void writeArray( const Array<T>& array )
        {
            beginWritingArray( array.size() );

            for( int i = 0; i < array.size(); i++ ) {
                generateFieldWriter( array[i] )( *this, &array[i] );
            }

            endWritingArray();
        }

        template<typename T>
        void readArray( Array<T>& array ) const
        {
            array.resize( beginReadingArray() );

            for( int i = 0; i < array.size(); i++ ) {
                generateFieldReader( array[i] )( *this, &array[i] );
            }

            endReadingArray();
        }

        void write( const void* data, u64 size )
        {
            m_stream->write( data, size );
        }

        void read( void* data, u64 size ) const
        {
            m_stream->read( data, size );
        }

        void writeString( const String& str )
        {
            m_stream->writeString( str.c_str() );
        }

        void readString( String& str ) const
        {
            m_stream->readString( str );
        }

    private:

        StreamPtr           m_stream;
    };

    //! Base class for all serializable data structs.
    class Serializable {

        ClassEnableTypeInfo( Serializable )

    public:

        //! Reads data from stream.
        void                read( const Storage& storage );

        //! Writes data to stream.
        void                write( Storage& storage ) const;

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

    // ---------------------------- Field writers

    template<typename T>
    inline FieldWriter generateFieldWriter( const T& value ) {
        struct Struct {
            static void thunk( Storage& storage, const void* data ) {
                castTo<Serializable>( reinterpret_cast<const T*>( data ) )->write( storage );
            }
        };

        return Struct::thunk;
    }

    template<typename T>
    inline FieldWriter generateArrayWriter( const T& value ) {
        struct Arr {
            static void thunk( Storage& storage, const void* data ) {
                storage.writeArray( *reinterpret_cast<const T*>( data ) );
            }
        };

        return Arr::thunk;
    }

    // ---------------------------- Field readers

    template<typename T>
    inline FieldReader generateFieldReader( T& value ) {
        struct Struct {
            static void thunk( const Storage& storage, void* data ) {
                castTo<Serializable>( reinterpret_cast<T*>( data ) )->read( storage );
            }
        };

        return Struct::thunk;
    }

    template<typename T>
    inline FieldReader generateArrayReader( const T& value ) {
        struct Arr {
            static void thunk( const Storage& storage, void* data ) {
                storage.readArray( *reinterpret_cast<T*>( data ) );
            }
        };

        return Arr::thunk;
    }

} // namespace io

IoFieldWriter( String, storage.writeString( value ) )
IoFieldReader( String, storage.readString( value ) )

IoPODWriter( int )
IoPODReader( int )

IoPODWriter( unsigned int )
IoPODReader( unsigned int )

IoPODWriter( float )
IoPODReader( float )

IoPODWriter( double )
IoPODReader( double )

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Serializable_H__	*/