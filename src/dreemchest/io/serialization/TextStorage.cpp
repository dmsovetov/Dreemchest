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

#include "TextStorage.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** TextStorage::write
void TextStorage::write( CString key, const bool& value )
{
    writeBoolean( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const u8& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const s8& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const u16& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const s16& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const u32& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const s32& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const u64& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const s64& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const f32& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const f64& value )
{
    writeNumber( key, value );
}

// ** TextStorage::write
void TextStorage::write( CString key, const String& value )
{
    writeString( key, value );
}

// ** TextStorage::read
void TextStorage::read( CString key, bool& value ) const
{
    readBoolean( key, value );
}

// ** TextStorage::read
void TextStorage::read( CString key, u8& value ) const
{
    double v;
    readNumber( key, v );
    value = ( u8 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, s8& value ) const
{
    double v;
    readNumber( key, v );
    value = ( s8 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, u16& value ) const
{
    double v;
    readNumber( key, v );
    value = ( u16 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, s16& value ) const
{
    double v;
    readNumber( key, v );
    value = ( s16 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, u32& value ) const
{
    double v;
    readNumber( key, v );
    value = ( u32 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, s32& value ) const
{
    double v;
    readNumber( key, v );
    value = ( s32 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, u64& value ) const
{
    double v;
    readNumber( key, v );
    value = ( u64 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, s64& value ) const
{
    double v;
    readNumber( key, v );
    value = ( s64 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, f32& value ) const
{
    double v;
    readNumber( key, v );
    value = ( f32 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, f64& value ) const
{
    double v;
    readNumber( key, v );
    value = ( f64 )v;
}

// ** TextStorage::read
void TextStorage::read( CString key, String& value ) const
{
    readString( key, value );
}

} // namespace io

DC_END_DREEMCHEST