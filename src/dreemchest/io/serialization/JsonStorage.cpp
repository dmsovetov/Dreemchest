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

#ifndef HAVE_JSONCPP
    #error HAVE_JSONCPP should be defined when building with JsonStorage
#endif

#include "JsonStorage.h"
#include "Serializable.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** JsonStorage::write
void JsonStorage::write( CString key, const String& value )
{
    current()[key] = value;
}

// ** JsonStorage::write
void JsonStorage::write( CString key, const u8& value )
{
    current()[key] = value;
}

// ** JsonStorage::write
void JsonStorage::write( CString key, const u16& value )
{
    current()[key] = value;
}

// ** JsonStorage::write
void JsonStorage::write( CString key, const u32& value )
{
    current()[key] = value;
}

// ** JsonStorage::write
void JsonStorage::write( CString key, const s32& value )
{
    current()[key] = value;
}

// ** JsonStorage::write
void JsonStorage::write( CString key, const f32& value )
{
    current()[key] = value;
}

// ** JsonStorage::write
void JsonStorage::write( CString key, const f64& value )
{
    current()[key] = value;
}

// ** JsonStorage::read
void JsonStorage::read( CString key, String& value ) const
{
    value = current()[key].asCString();
}

// ** JsonStorage::read
void JsonStorage::read( CString key, u8& value ) const
{
    value = current()[key].asInt();
}

// ** JsonStorage::read
void JsonStorage::read( CString key, u16& value ) const
{
    value = current()[key].asInt();
}

// ** JsonStorage::read
void JsonStorage::read( CString key, u32& value ) const
{
    value = current()[key].asInt();
}

// ** JsonStorage::read
void JsonStorage::read( CString key, s32& value ) const
{
    value = current()[key].asInt();
}

// ** JsonStorage::read
void JsonStorage::read( CString key, f32& value ) const
{
    value = current()[key].asFloat();
}

// ** JsonStorage::read
void JsonStorage::read( CString key, f64& value ) const
{
    value = current()[key].asDouble();
}

// ** JsonStorage::read
void JsonStorage::write( CString key, const Serializable& value )
{
    value.write( this );
}

// ** JsonStorage::read
void JsonStorage::read( CString key, Serializable& value ) const
{
    value.read( this );
}

// ** JsonStorage::startWritingItem
void JsonStorage::startWritingItem( int index )
{

}

// ** JsonStorage::save
String JsonStorage::save( void ) const
{
    return Json::StyledWriter().write( m_json );
}

// ** JsonStorage::current
Json::Value& JsonStorage::current( void )
{
    return m_stack.empty() ? m_json : *m_stack.top();
}

// ** JsonStorage::current
const Json::Value& JsonStorage::current( void ) const
{
    return m_stack.empty() ? m_json : *m_stack.top();
}

// ** JsonStorage::startWritingArray
void JsonStorage::startWritingArray( CString key, u32 size )
{
    m_json[key] = Json::arrayValue;
    m_stack.push( &m_json[key] );
}

// ** JsonStorage::endWritingArray
void JsonStorage::endWritingArray( CString key )
{
    m_stack.pop();
}

// ** JsonStorage::startReadingArray
u32 JsonStorage::startReadingArray( CString key ) const
{
    u32 size;
    read( "array.size", size );
    return size;
}

// ** JsonStorage::endReadingArray
void JsonStorage::endReadingArray( CString key ) const
{

}

} // namespace io

DC_END_DREEMCHEST