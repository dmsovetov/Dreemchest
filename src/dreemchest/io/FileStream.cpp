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

#include	"FileStream.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** FileStream::s_openFileCount
u32 FileStream::s_openFileCount = 0;

// ** FileStream::FileStream
FileStream::FileStream( const char *fileName, const char *mode )
{
	m_length    = 0;
	m_fileName  = fileName;
    m_file      = fopen( fileName, mode );

    if( !m_file ) {
        return;
    }

    s_openFileCount++;

 	fseek( m_file, 0, SEEK_END );
	m_length = ftell( m_file );
	fseek( m_file, 0, SEEK_SET );
}

FileStream::~FileStream( void )
{
	if( m_file ) {
        fclose( m_file );
        s_openFileCount--;
    }
}

// ** FileStream::fileName
CString FileStream::fileName( void ) const
{
	return m_fileName.c_str();
}

// ** FileStream::read
long FileStream::read( void *buffer, long size )
{
    DC_BREAK_IF( buffer == NULL );
    DC_BREAK_IF( size <= 0 );
    
	if( m_file ) {
		int bytesRead = fread( buffer, 1, size, m_file );
        return bytesRead;
	}

	return 0;
}

// ** FileStream::write
long FileStream::write( const void *buffer, long size )
{
    DC_BREAK_IF( buffer == NULL );
    DC_BREAK_IF( size <= 0 );
    
	if( m_file ) {
		int bytesWritten = fwrite( buffer, 1, size, m_file );
        DC_BREAK_IF( bytesWritten != size );
        return bytesWritten;
	}

	return 0;
}

// ** FileStream::writeWithFormat
long FileStream::writeWithFormat( const char *format, ... )
{
/*    DC_BREAK_IF( format == NULL );
    
    va_list		ap;
    char		formated[MAX_FORMAT_BUFFER_SIZE];

    va_start( ap, format );
    vsnprintf( formated, MAX_FORMAT_BUFFER_SIZE, format, ap );
    va_end( ap );

	return fprintf( m_file, formated );*/
	return 0;
}

// ** FileStream::setPosition
void FileStream::setPosition( long offset, eSeekOrigin origin )
{
    if( !m_file ) {
        return;
    }

    int result = 0;

    switch( origin ) {
    case SeekSet: result = fseek( m_file, offset, SEEK_SET ); break;
    case SeekCur: result = fseek( m_file, offset, SEEK_CUR ); break;
    case SeekEnd: result = fseek( m_file, offset, SEEK_END ); break;
    }
    
    DC_BREAK_IF( result != 0 );
}

// ** FileStream::position
long FileStream::position( void ) const
{
	if( m_file ) {
		return ftell( m_file );
	}

	return 0;
}

// ** FileStream::hasDataLeft
bool FileStream::hasDataLeft( void ) const
{
	return feof( m_file ) == 0;
}

// ** FileStream::isValid
bool FileStream::isValid( void ) const
{
	return (m_file != NULL);
}

// ** FileStream::length
long FileStream::length( void ) const
{
	return m_length;
}

} // namespace io

DC_END_DREEMCHEST
