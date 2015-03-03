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
#define IoFieldWriter( T, code )                                        \
    template<>                                                          \
    inline io::FieldWriter io::fieldWriter( const T& data ) {           \
        struct Writer {                                                 \
            static void thunk( StreamPtr& stream, const void* data ) {  \
                const T& value = *reinterpret_cast<const T*>( data );   \
                code                                                    \
            }                                                           \
        };                                                              \
        return Writer::thunk;                                           \
    }

//! Macro definition for overriding field reader
#define IoFieldReader( T, code )                                        \
    template<>                                                          \
    inline io::FieldReader io::fieldReader( const T& data ) {           \
        struct Reader {                                                 \
            static void thunk( const StreamPtr& stream, void* data ) {  \
                T& value = *reinterpret_cast<T*>( data );               \
                code                                                    \
            }                                                           \
        };                                                              \
        return Reader::thunk;                                           \
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

    //! Default field writer function
    template<typename T>
    inline FieldWriter fieldWriter( const T& data )
    {
        if( !sizeOfType<T>() ) {
            return NULL;
        }

        struct Writer {
            static void thunk( StreamPtr& stream, const void* data ) {
                stream->write( data, sizeOfType<T>() );
            }
        };

        return Writer::thunk;
    }

    //! Default field reader function
    template<typename T>
    inline FieldReader fieldReader( const T& data )
    {
        if( !sizeOfType<T>() ) {
            return NULL;
        }
        
        struct Reader {
            static void thunk( const StreamPtr& stream, void* data ) {
                stream->read( data, sizeOfType<T>() );
            }
        };

        return Reader::thunk;
    }

    template<typename T>
    inline FieldWriter fieldWriter( const std::vector<T>& data )
    {
        struct Writer {
            static void thunk( StreamPtr& stream, const void* data ) {
                const std::vector<T>* value = reinterpret_cast<const std::vector<T>*>( data );
                for( int i = 0, n = ( int )value->size(); i < n; i++ ) {
                    if( FieldWriter writer = fieldWriter( value->at( i ) ) ) {
                        writer( stream, &value->at( i ) );
                    } else {
                        value->at( i ).write( stream );
                    }
                }
            }
        };

        return Writer::thunk;
    }

} // namespace io

IoFieldWriter( String, stream->writeString( value.c_str() ); )
IoFieldReader( String, stream->readString( value ); )

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Serializable_H__	*/