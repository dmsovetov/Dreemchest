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

        //! Begins writing of an array.
        virtual void    pushArrayWrite( CString key, u32 size ) = 0;

        //! End writing of an array.
        virtual void    popArrayWrite( void ) = 0;

        //! Begins writing of an object.
        virtual void    pushObjectWrite( CString key ) = 0;

        //! Ends writing of an object.
        virtual void    popObjectWrite( void ) = 0;

        //! Begins writing of an array item.
        virtual void    pushItemWrite( u32 index ) = 0;

        //! Ends writing of an array item.
        virtual void    popItemWrite( void ) = 0;

        //! Begins reading of an array.
        virtual u32     pushArrayRead( CString key ) const = 0;

        //! Ends reading of an array.
        virtual void    popArrayRead( void ) const = 0;

        //! Begins reading of an object.
        virtual void    pushObjectRead( CString key ) const = 0;

        //! Ends reading of an object.
        virtual void    popObjectRead( void ) const = 0;
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

        //! Begins writing of an array.
        void            pushArrayWrite( CString key, u32 size );

        //! End writing of an array.
        void            popArrayWrite( void );

        //! Begins writing of an object.
        void            pushObjectWrite( CString key );

        //! Ends writing of an object.
        void            popObjectWrite( void );

        //! Begins writing of an array item.
        void            pushItemWrite( u32 index );

        //! Ends writing of an array item.
        void            popItemWrite( void );

        //! Begins reading of an array.
        u32             pushArrayRead( CString key ) const;

        //! Ends reading of an array.
        void            popArrayRead( void ) const;

        //! Begins reading of an object.
        void            pushObjectRead( CString key ) const;

        //! Ends reading of an object.
        void            popObjectRead( void ) const;

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
        pushArrayWrite( key, ( u32 )array.size() );

        for( int i = 0; i < array.size(); i++ ) {
            pushItemWrite( i );
            detail::createFieldSerializer( NULL, array[i] )->write( *this );
            popItemWrite();
        }

        popArrayWrite();
    }

    // ** Storage::read
    template<typename T>
    void Storage::read( CString key, Array<T>& array ) const
    {
        array.resize( pushArrayRead( key ) );

        for( int i = 0; i < array.size(); i++ ) {
            detail::createFieldSerializer( NULL, array[i] )->read( *this );
        }

        popArrayRead();
    }

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Storage_H__	*/