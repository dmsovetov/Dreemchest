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

#include "Path.h"

DC_BEGIN_DREEMCHEST

namespace Io {

// ** Path::Path
Path::Path( void )
{

}

// ** Path::Path
Path::Path( CString str )
{
    m_items = split( str );
    build();
}

// ** Path::Path
Path::Path( const String& str )
{
    m_items = split( str.c_str() );
    build();
}

// ** Path::Path
Path::Path( const Path& other )
{
    m_items = other.m_items;
    build();
}

// ** Path::operator =
const Path& Path::operator = ( CString str )
{
    m_items = split( str );
    build();

    return *this;
}

// ** Path::operator ==
bool Path::operator == ( const Path& other ) const
{
    return m_str == other.m_str;
}

// ** Path::operator !=
bool Path::operator != ( CString str ) const
{
    return m_str != str;
}

// ** Path::operator <
bool Path::operator < ( const Path& other ) const
{
    return m_str < other.m_str;
}

// ** Path::operator =
const Path& Path::operator = ( const String& str )
{
    m_items = split( str.c_str() );
    build();
    
    return *this;
}

// ** Path::operator +
Path Path::operator + ( const String& str ) const
{
    Path result = *this;
    result += str;
    return result;
}

// ** Path::operator +
Path Path::operator + ( CString str ) const
{
    Path result = *this;
    result += str;
    return result;
}

// ** Path::operator +=
const Path& Path::operator += ( CString str )
{
    m_items << split( str );
    build();

    return *this;
}

// ** Path::operator +=
const Path& Path::operator += ( const Path& other )
{
    m_items << other.m_items;
    build();

    return *this;
}

// ** Path::isEmpty
bool Path::isEmpty( void ) const
{
    return m_items.isEmpty();
}

// ** Path::folder
Path Path::folder( void ) const
{
    Path result;
    result.m_items = m_items;
    result.m_items.pop();
    result.build();

    return result;
}

// ** Path::lastmi
CString Path::last( void ) const
{
    return m_items.count() ? m_items.last().c_str() : "";
}

// **  Path::c_str
CString Path::c_str( void ) const
{
    return m_str.c_str();
}

// ** Path::build
void Path::build( void )
{
    m_str = "";

    const StringList::Container& strings = m_items.container();

    for( StringList::Container::const_iterator i = strings.begin(), end = strings.end(); i != end; ++i ) {
        if( m_str != "" ) {
            m_str += '/';
        }
        
        m_str += *i;
    }
}

// ** Path::split
StringList Path::split( CString str )
{
    StringList result;
    
    char* path = _strdup( str );

    // ** Replace \\ in path
    for( s32 i = 0; i < ( s32 )strlen( path ); i++ ) {
        if( path[i] == '\\' ) {
            path[i] = '/';
        }
    }

    // ** Tokenize path
    CString item = strtok( path, "/" );

    while( item ) {
        result << item;
        item = strtok( NULL, "/" );
    }

    // ** Free temporary string
    free( path );

    return result;
}

} // namespace Io
    
DC_END_DREEMCHEST
