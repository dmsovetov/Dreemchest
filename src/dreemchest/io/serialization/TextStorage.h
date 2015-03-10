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

#ifndef __DC_Io_TextStorage_H__
#define __DC_Io_TextStorage_H__

#include "Storage.h"

DC_BEGIN_DREEMCHEST

namespace io {

    //! Textual data storage like json, xml or yaml.
    class TextStorage : public IStorage {
    public:

        IoStoreImplement( bool )
        IoStoreImplement( u8 )
        IoStoreImplement( s8 )
        IoStoreImplement( u16 )
        IoStoreImplement( s16 )
        IoStoreImplement( u32 )
        IoStoreImplement( s32 )
        IoStoreImplement( u64 )
        IoStoreImplement( s64 )
        IoStoreImplement( f32 )
        IoStoreImplement( f64 )
        IoStoreImplement( String )

    protected:

        //! Writes a number value.
        virtual void writeNumber( CString key, double value ) = 0;

        //! Reads a number value.
        virtual void readNumber( CString key, double& value ) const = 0;

        //! Writes a boolean value.
        virtual void writeBoolean( CString key, const bool& value ) = 0;

        //! Reads a boolean value.
        virtual void readBoolean( CString key, bool& value ) const = 0;

        //! Writes a string value.
        virtual void writeString( CString key, const String& value ) = 0;

        //! Reads a string value.
        virtual void readString( CString key, String& value ) const = 0;
    };

} // namespace io

DC_END_DREEMCHEST

#endif  /*  !defined(__DC_Io_TextStorage_H__)   */
