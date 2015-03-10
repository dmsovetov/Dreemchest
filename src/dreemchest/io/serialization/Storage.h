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

DC_BEGIN_DREEMCHEST

namespace io {

    //! A storage interface.
    class Storage {
    public:

        virtual         ~Storage( void ) {}

        virtual void    write( CString key, const String& value ) = 0;

        virtual void    write( CString key, const u8& value ) = 0;

        virtual void    write( CString key, const u16& value ) = 0;

        virtual void    write( CString key, const u32& value ) = 0;

        virtual void    write( CString key, const s32& value ) = 0;

        virtual void    write( CString key, const f32& value ) = 0;

        virtual void    write( CString key, const f64& value ) = 0;

        virtual void    write( CString key, const Serializable& value ) = 0;

        virtual void    read( CString key, String& value ) const = 0;

        virtual void    read( CString key, u8& value ) const = 0;

        virtual void    read( CString key, u16& value ) const = 0;

        virtual void    read( CString key, u32& value ) const = 0;

        virtual void    read( CString key, s32& value ) const = 0;

        virtual void    read( CString key, f32& value ) const = 0;

        virtual void    read( CString key, f64& value ) const = 0;

        virtual void    read( CString key, Serializable& value ) const = 0;

        virtual void    startWritingItem( int index ) = 0;

        virtual void    startWritingArray( CString key, u32 size ) = 0;

        virtual void    endWritingArray( CString key ) = 0;

        virtual u32     startReadingArray( CString key ) const = 0;

        virtual void    endReadingArray( CString key ) const = 0;
/*
        template<typename T>
        void            write( CString key, const T& value );

        template<typename T>
        void            read( CString key, T& value ) const;
*/
        template<typename T>
        void            write( CString key, const Array<T>& array );
        
        template<typename T>
        void            read( CString key, Array<T>& array ) const;
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
        startWritingArray( key, array.size() );

        for( int i = 0; i < array.size(); i++ ) {
            startWritingItem( i );
            detail::createFieldSerializer( NULL, array[i] )->write( this );
        }

        endWritingArray( key );
    }

    // ** Storage::read
    template<typename T>
    void Storage::read( CString key, Array<T>& array ) const
    {
        array.resize( startReadingArray( key ) );

        for( int i = 0; i < array.size(); i++ ) {
            detail::createFieldSerializer( NULL, array[i] )->read( this );
        }

        endReadingArray( key );
    }

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Storage_H__	*/