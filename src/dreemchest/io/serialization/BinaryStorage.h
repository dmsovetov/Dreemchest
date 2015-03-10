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

        //! Pushes a write state.
        virtual void            pushWrite( const StorageState& state );

        //! Pushes a read state.
        virtual void            pushRead( StorageState& state ) const;

        //! Pops a state.
        virtual StorageState    pop( void ) const;

    private:
        
        //! Binary stream.
        StreamPtr       m_stream;
    };

} // namespace io

DC_END_DREEMCHEST

#endif  /*  !defined(__DC_Io_BinaryStorage_H__) */
