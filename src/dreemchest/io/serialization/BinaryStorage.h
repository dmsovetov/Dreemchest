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

#ifndef __DC_Io_BinaryStorage_H__
#define __DC_Io_BinaryStorage_H__

#include "Storage.h"

DC_BEGIN_DREEMCHEST

namespace io {

    //! A binary storage interface to use in serialization
    class BinaryStorage : public IStorage {
    public:

                        // Constructs BinaryStorage instance.
                        BinaryStorage( const StreamPtr& stream )
                            : m_stream( stream ) {}

        IoStoreBinary( bool, 1 )
        IoStoreBinary( u8,   1 )
        IoStoreBinary( s8,   1 )
        IoStoreBinary( u16,  2 )
        IoStoreBinary( s16,  2 )
        IoStoreBinary( u32,  4 )
        IoStoreBinary( s32,  4 )
        IoStoreBinary( f32,  4 )
        IoStoreBinary( u64,  8 )
        IoStoreBinary( s64,  8 )
        IoStoreBinary( f64,  8 )
        IoStoreImplement( String )

    private:

        //! Does nothing
        virtual void    commit( void ) {}

        //! Begins writing of an array.
        virtual void    pushArrayWrite( CString key, u32 size );

        //! End writing of an array.
        virtual void    popArrayWrite( void );

        //! Begins writing of an object.
        virtual void    pushObjectWrite( CString key );

        //! Ends writing of an object.
        virtual void    popObjectWrite( void );

        //! Begins writing of an array item.
        virtual void    pushItemWrite( u32 index );

        //! Ends writing of an array item.
        virtual void    popItemWrite( void );

        //! Begins reading of an array.
        virtual u32     pushArrayRead( CString key ) const;

        //! Ends reading of an array.
        virtual void    popArrayRead( void ) const;

        //! Begins reading of an object.
        virtual void    pushObjectRead( CString key ) const;

        //! Ends reading of an object.
        virtual void    popObjectRead( void ) const;

    private:
        
        //! Binary stream.
        StreamPtr       m_stream;
    };

} // namespace io

DC_END_DREEMCHEST

#endif  /*  !defined(__DC_Io_BinaryStorage_H__) */
