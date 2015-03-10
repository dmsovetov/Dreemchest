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

// ** JsonStorage::JsonStorage
JsonStorage::JsonStorage( const StreamPtr& stream ) : m_stream( stream ), m_index( 0 )
{

}

// ** JsonStorage::pushObjectRead
void JsonStorage::pushObjectRead( CString key ) const
{

}

// ** JsonStorage::popObjectRead
void JsonStorage::popObjectRead( void ) const
{

}

// ** JsonStorage::pushObjectWrite
void JsonStorage::pushObjectWrite( CString key )
{
    if( Json::Value* parent = current() ) {
        writeTo( key ) = Json::objectValue;
        m_stack.push( &writeTo( key ) );
    } else {
        m_stack.push( new Json::Value );
    }
}

// ** JsonStorage::popObjectWrite
void JsonStorage::popObjectWrite( void )
{
    if( m_stack.size() == 1 ) {
        String formatted = Json::StyledWriter().write( *m_stack.top() );
        log::verbose( "JSON: %s\n", formatted.c_str() );
        m_stream->write( formatted.c_str(), formatted.length() );

        delete m_stack.top();
    }

    m_stack.pop();
}

// ** JsonStorage::writeNumber
void JsonStorage::writeNumber( CString key, double value )
{
    writeTo( key ) = value;
}

// ** JsonStorage::readNumber
void JsonStorage::readNumber( CString key, double& value ) const
{
    value = current()->get( key, 0.0 ).asDouble();
}

// ** JsonStorage::writeBoolean
void JsonStorage::writeBoolean( CString key, const bool& value )
{
    writeTo( key ) = value;
}

// ** JsonStorage::readBoolean
void JsonStorage::readBoolean( CString key, bool& value ) const
{
    value = current()->get( key, false ).asBool();
}

// ** JsonStorage::writeString
void JsonStorage::writeString( CString key, const String& value )
{
    writeTo( key ) = value;
}

// ** JsonStorage::readString
void JsonStorage::readString( CString key, String& value ) const
{
    value = current()->get( key, "" ).asCString();
}

// ** JsonStorage::pushArrayWrite
void JsonStorage::pushArrayWrite( CString key, u32 size )
{
    m_index = 0;
    
    writeTo( key ) = Json::arrayValue;
    m_stack.push( &writeTo( key ) );
}

// ** JsonStorage::popArrayWrite
void JsonStorage::popArrayWrite( void )
{
    m_stack.pop();
}


// ** JsonStorage::pushItemWrite
void JsonStorage::pushItemWrite( u32 index )
{
    m_index = index;
}


// ** JsonStorage::popItemWrite
void JsonStorage::popItemWrite( void )
{

}


// ** JsonStorage::pushArrayRead
u32 JsonStorage::pushArrayRead( CString key ) const
{
    return 0;
//    m_stack.push( &const_cast<JsonStorage*>( this )->current()[key] );
//    return current()[key].size();
}


// ** JsonStorage::popArrayRead
void JsonStorage::popArrayRead( void ) const
{

}

// ** JsonStorage::current
Json::Value* JsonStorage::current( void )
{
    return m_stack.empty() ? NULL : m_stack.top();
}

// ** JsonStorage::current
const Json::Value* JsonStorage::current( void ) const
{
    return m_stack.empty() ? NULL : m_stack.top();
}

// ** JsonStorage::writeTo
Json::Value& JsonStorage::writeTo( CString key )
{
    Json::Value& root = *current();
    return key ? root[key] : root[m_index];
}

} // namespace io

DC_END_DREEMCHEST