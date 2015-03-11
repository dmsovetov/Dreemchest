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

#include	"FileSystem.h"
#include	"streams/ByteBuffer.h"

DC_BEGIN_DREEMCHEST

namespace io {

IMPLEMENT_LOGGER( log )

// ** FileSystem::FileSystem
FileSystem::FileSystem( void )
{
}

FileSystem::~FileSystem( void )
{
}

// ** FileSystem::createByteBuffer
ByteBufferPtr FileSystem::createByteBuffer( u8 *pointer, u32 size ) const
{
	return DC_NEW ByteBuffer( pointer, size );
}

// ** FileSystem::openFile
StreamPtr FileSystem::openFile( const Path& path ) const
{
    return NULL;
}

// ** FileSystem::openFile
StreamPtr FileSystem::openFile( const Path& path, StreamMode mode ) const
{
    return NULL;
}

// ** FileSystem::fileExists
bool FileSystem::fileExists( const Path& path ) const
{
    return false;
}

} // namespace io

DC_END_DREEMCHEST
