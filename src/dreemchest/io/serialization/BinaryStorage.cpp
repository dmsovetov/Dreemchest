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

#include "BinaryStorage.h"
#include "Serializable.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** BinaryStorage::write
void BinaryStorage::write( CString key, const String& value )
{
    m_stream->writeString( value.c_str() );
}

// ** BinaryStorage::write
void BinaryStorage::write( CString key, const u8& value )
{
    m_stream->write( &value, 1 );
}

// ** BinaryStorage::write
void BinaryStorage::write( CString key, const u16& value )
{
    m_stream->write( &value, 2 );
}

// ** BinaryStorage::write
void BinaryStorage::write( CString key, const u32& value )
{
    m_stream->write( &value, 4 );
}

// ** BinaryStorage::write
void BinaryStorage::write( CString key, const s32& value )
{
    m_stream->write( &value, 4 );
}

// ** BinaryStorage::write
void BinaryStorage::write( CString key, const f32& value )
{
    m_stream->write( &value, 4 );
}

// ** BinaryStorage::write
void BinaryStorage::write( CString key, const f64& value )
{
    m_stream->write( &value, 8 );
}

// ** BinaryStorage::read
void BinaryStorage::read( CString key, String& value ) const
{
    m_stream->readString( value );
}

// ** BinaryStorage::read
void BinaryStorage::read( CString key, u8& value ) const
{
    m_stream->read( &value, 1 );
}

// ** BinaryStorage::read
void BinaryStorage::read( CString key, u16& value ) const
{
    m_stream->read( &value, 2 );
}

// ** BinaryStorage::read
void BinaryStorage::read( CString key, u32& value ) const
{
    m_stream->read( &value, 4 );
}

// ** BinaryStorage::read
void BinaryStorage::read( CString key, s32& value ) const
{
    m_stream->read( &value, 4 );
}

// ** BinaryStorage::read
void BinaryStorage::read( CString key, f32& value ) const
{
    m_stream->read( &value, 4 );
}

// ** BinaryStorage::read
void BinaryStorage::read( CString key, f64& value ) const
{
    m_stream->read( &value, 8 );
}

// ** BinaryStorage::write
void BinaryStorage::write( CString key, const Serializable& value )
{
    value.write( this );
}

// ** BinaryStorage::read
void BinaryStorage::read( CString key, Serializable& value ) const
{
    value.read( this );
}

// ** BinaryStorage::startWritingArray
void BinaryStorage::startWritingArray( CString key, u32 size )
{
    write( "array.size", size );
}

// ** BinaryStorage::endWritingArray
void BinaryStorage::endWritingArray( CString key )
{
    
}

// ** BinaryStorage::startReadingArray
u32 BinaryStorage::startReadingArray( CString key ) const
{
    u32 size;
    read( "array.size", size );
    return size;
}

// ** BinaryStorage::endReadingArray
void BinaryStorage::endReadingArray( CString key ) const
{
    
}

// ** BinaryStorage::startWritingItem
void BinaryStorage::startWritingItem( int index )
{

}

} // namespace io

DC_END_DREEMCHEST