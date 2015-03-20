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

#ifndef		__DC_Io_Storage_H__
#define		__DC_Io_Storage_H__

#include	"../Io.h"
#include    "../streams/Stream.h"

#define IoStoreAbstract( T )                                \
    virtual void write( CString key, const T& value ) = 0;  \
    virtual void read ( CString key, T& value ) const = 0;

#define IoStoreImplement( T )                               \
    virtual void write( CString key, const T& value );      \
    virtual void read ( CString key, T& value ) const;

#define IoStoreBinary( T, size )                                                            \
    virtual void write( CString key, const T& value ) { m_stream->write( &value, size ); }  \
    virtual void read ( CString key, T& value ) const { m_stream->read ( &value, size ); }

#define IoStoreInterface( T )                               \
    virtual void write( CString key, const T& value ) {     \
        m_impl->write( key, value );                        \
    }                                                       \
    virtual void read( CString key, T& value ) const {      \
        m_impl->read( key, value );                         \
    }

DC_BEGIN_DREEMCHEST

namespace io {

    //! Storage state info.
    struct StorageState {
        enum State { Unknown, Property, Object, Array, ArrayItem };

                //! Constructs a storage state instance
                StorageState( void )
                    : m_key( NULL ), m_size( 0 ), m_state( Unknown ) {}

                //! Constructs an Object storage state.
                StorageState( CString key )
                    : m_key( key ), m_size( 0 ), m_state( Object ) {}

                //! Constructs an Array storage state instance.
                StorageState( CString key, u32 size )
                    : m_key( key ), m_size( size ), m_state( Array ) {}

                //! Constructs an ArrayItem storage state instance.
                StorageState( u32 index )
                    : m_key( NULL ), m_index( index ), m_size( 0 ), m_state( ArrayItem ) {}

        bool    operator == ( State state ) const { return m_state == state; }

        CString m_key;
        u32     m_index;
        u32     m_size;
        State   m_state;
    };

    //! A data storage interface.
    class IStorage {
    public:

        virtual         ~IStorage( void ) {}

        IoStoreAbstract( bool )
        IoStoreAbstract( u8 )
        IoStoreAbstract( s8 )
        IoStoreAbstract( u16 )
        IoStoreAbstract( s16 )
        IoStoreAbstract( u32 )
        IoStoreAbstract( s32 )
        IoStoreAbstract( u64 )
        IoStoreAbstract( s64 )
        IoStoreAbstract( f32 )
        IoStoreAbstract( f64 )
        IoStoreAbstract( String )

        //! Pushes a write state.
        virtual void            pushWrite( const StorageState& state ) = 0;

        //! Pushes a read state.
        virtual void            pushRead( StorageState& state ) const = 0;

        //! Pops a state.
        virtual StorageState    pop( void ) const = 0;
    };

    //! A data storage into which the data is serialized.
    class Storage {
    friend class Serializable;
    public:

                        //! Constructs a Storage instance.
                        Storage( StorageType type, const StreamPtr& stream );
        virtual         ~Storage( void );

                        //! Returns true if this storage is valid.
                        operator bool( void ) const;

        IoStoreInterface( bool )
        IoStoreInterface( u8 )
        IoStoreInterface( s8 )
        IoStoreInterface( u16 )
        IoStoreInterface( s16 )
        IoStoreInterface( u32 )
        IoStoreInterface( s32 )
        IoStoreInterface( u64 )
        IoStoreInterface( s64 )
        IoStoreInterface( f32 )
        IoStoreInterface( f64 )
        IoStoreInterface( String )

        IoStoreImplement( Serializable )

        template<typename T>
        void            write( CString key, const Array<T>& array );

        template<typename T>
        void            read( CString key, Array<T>& array ) const;

        //! Pushes a write state.
        virtual void            pushWrite( const StorageState& state );

        //! Pushes a read state.
        virtual void            pushRead( StorageState& state ) const;

        //! Pops a state.
        virtual StorageState    pop( void ) const;

    private:

        IStorage*       m_impl;
    };

    namespace detail {

        template<typename T>
        FieldSerializerPtr createFieldSerializer( CString name, const T& field );

        template<typename T>
        FieldSerializerPtr createFieldSerializer( CString name, const Array<T>& field );

    } // namespace detail

    // ** Storage::write
    template<typename T>
    void Storage::write( CString key, const Array<T>& array )
    {
        pushWrite( StorageState( key, ( u32 )array.size() ) );

        for( u32 i = 0; i < ( u32 )array.size(); i++ ) {
            pushWrite( StorageState( i ) );
            detail::createFieldSerializer( NULL, array[i] )->write( *this );
        }

        pop();
    }

    // ** Storage::read
    template<typename T>
    void Storage::read( CString key, Array<T>& array ) const
    {
        StorageState state( key, 0 );

        pushRead( state );

        array.resize( state.m_size );
        for( u32 i = 0; i < ( u32 )array.size(); i++ ) {
            StorageState state( i );
            pushRead( state );
            detail::createFieldSerializer( NULL, array[i] )->read( *this );
        }

        pop();
    }

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Storage_H__	*/