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

#include "FileStream.h"
#include <errno.h>

DC_BEGIN_DREEMCHEST

namespace Io {

// ** FileStream::s_openFileCount
u32 FileStream::s_openFileCount = 0;

// ** FileStream::FileStream
FileStream::FileStream( void ) : m_file( NULL ), m_length( 0 )
{

}

FileStream::~FileStream( void )
{
    dispose();
}

// ** FileStream::dispose
void FileStream::dispose( void )
{
    if( m_file == NULL ) {
        return;
    }

    fclose( m_file );
    s_openFileCount--;

    m_file = NULL;
}

// ** FileStream::open
bool FileStream::open( const Path& fileName, StreamMode mode )
{
    CString fmode = "rb";

    switch( mode ) {
    case BinaryReadStream:  fmode = "rb"; break;
    case BinaryWriteStream: fmode = "wb"; break;
    }

    // ** Open file
    m_fileName = fileName;
    m_file     = fopen( fileName.c_str(), fmode );

    if( !m_file ) {
        return false;
    }

    // ** Increase file counter
    s_openFileCount++;

    // ** Cache the file length
    setPosition( 0, SeekEnd );
    m_length = position();
    setPosition( 0, SeekSet );

    return true;
}

// ** FileStream::fileName
const Path& FileStream::fileName( void ) const
{
	return m_fileName;
}

// ** FileStream::read
s32 FileStream::read( void* buffer, s32 size ) const
{
    DC_ABORT_IF( m_file == NULL, "invalid file handle" );
    DC_ABORT_IF( buffer == NULL, "invalid destination buffer" );
    DC_ABORT_IF( size <= 0, "the size should be positive" );

    s32 bytesRead = fread( buffer, 1, size, m_file );

    if( bytesRead != size && ferror( m_file ) ) {
        LogError( "stream", "failed to read %d bytes from file, %d\n", size, errno );
    }
    
    return bytesRead;
}

// ** FileStream::write
s32 FileStream::write( const void* buffer, s32 size )
{
    DC_ABORT_IF( m_file == NULL, "invalid file handle" );
    DC_ABORT_IF( buffer == NULL, "invalid destination buffer" );
    DC_ABORT_IF( size <= 0, "the size should be positive" );

    s32 bytesWritten = fwrite( buffer, 1, size, m_file );
    
    if( bytesWritten != size && ferror( m_file ) ) {
        LogError( "stream", "failed to write %d bytes to file, %d\n", size, errno );
    }
    
    return bytesWritten;
}

// ** FileStream::setPosition
void FileStream::setPosition( s32 offset, SeekOrigin origin )
{
    DC_ABORT_IF( m_file == NULL, "invalid file handle" );

    s32 result = 0;

    switch( origin ) {
    case SeekSet: result = fseek( m_file, offset, SEEK_SET ); break;
    case SeekCur: result = fseek( m_file, offset, SEEK_CUR ); break;
    case SeekEnd: result = fseek( m_file, offset, SEEK_END ); break;
    }

    DC_BREAK_IF( result != 0, "fseek returned an unexpected value" );
}

// ** FileStream::position
s32 FileStream::position( void ) const
{
    DC_ABORT_IF( m_file == NULL, "invalid file handle" );
    return ftell( m_file );
}

// ** FileStream::hasDataLeft
bool FileStream::hasDataLeft( void ) const
{
	return feof( m_file ) == 0;
}

// ** FileStream::length
s32 FileStream::length( void ) const
{
	return m_length;
}

} // namespace Io

DC_END_DREEMCHEST
