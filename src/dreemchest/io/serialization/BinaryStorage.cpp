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

// ** BinaryStorage::pushItemWrite
void BinaryStorage::pushItemWrite( u32 index )
{

}

// ** BinaryStorage::pushObjectRead
void BinaryStorage::pushObjectRead( CString key ) const
{

}

// ** BinaryStorage::popObjectRead
void BinaryStorage::popObjectRead( void ) const
{

}

// ** BinaryStorage::pushObjectWrite
void BinaryStorage::pushObjectWrite( CString key )
{

}

// ** BinaryStorage::popObjectWrite
void BinaryStorage::popObjectWrite( void )
{

}


// ** BinaryStorage::popItemWrite
void BinaryStorage::popItemWrite( void )
{

}

// ** BinaryStorage::pushArrayWrite
void BinaryStorage::pushArrayWrite( CString key, u32 size )
{
    m_stream->write( &size, 4 );
}

// ** BinaryStorage::popArrayWrite
void BinaryStorage::popArrayWrite( void )
{

}

// ** BinaryStorage::pushArrayRead
u32 BinaryStorage::pushArrayRead( CString key ) const
{
    u32 size;
    m_stream->read( &size, 4 );
    return size;
}

// ** BinaryStorage::popArrayRead
void BinaryStorage::popArrayRead( void ) const
{

}
    
} // namespace io

DC_END_DREEMCHEST