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
#include "../streams/Stream.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** BinaryStorage::write
void BinaryStorage::write( CString key, const String& value )
{
    m_stream->writeString( value.c_str() );
}

// ** BinaryStorage::read
void BinaryStorage::read( CString key, String& value ) const
{
    m_stream->readString( value );
}

// ** BinaryStorage::pushWrite
void BinaryStorage::pushWrite( const StorageState& state )
{
    if( state == StorageState::Array ) {
        m_stream->write( &state.m_size, 4 );
    }
}

// ** BinaryStorage::pushRead
void BinaryStorage::pushRead( StorageState& state ) const
{
    if( state == StorageState::Array ) {
        m_stream->read( &state.m_size, 4 );
    }
}

// ** BinaryStorage::pop
StorageState BinaryStorage::pop( void ) const
{
    return StorageState();
}
    
} // namespace io

DC_END_DREEMCHEST