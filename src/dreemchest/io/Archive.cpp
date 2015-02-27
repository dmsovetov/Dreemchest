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

#include	"Archive.h"

#include    "FileStream.h"
#include    "PackedStream.h"
#include    "DiskFileSystem.h"

#include    "ZLibBufferCompressor.h"
#include    "FastLZBufferCompressor.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** Archive::Archive
Archive::Archive( const DiskFileSystem *diskFileSystem ) : m_diskFileSystem( diskFileSystem ), m_file( NULL )
{
    m_isCreating = false;
}

Archive::~Archive( void )
{
    DC_RELEASE( m_file );
}

// ** Archive::release
void Archive::release( void )
{
    delete this;
}

// ** Archive::createCompressor
IBufferCompressor* Archive::createCompressor( eCompressor compressor ) const
{
    switch( compressor ) {
    case CompressorFastLZ:  return DC_NEW FastLZBufferCompressor;
                            break;

    case CompressorZ:       return DC_NEW ZLibBufferCompressor;
                            break;

    default:                return NULL;
    }

    return NULL;
}

// ** Archive::packFile
bool Archive::packFile( const char *fileName, const char *compressedFileName )
{
    u8 chunk[CHUNK_SIZE + 1], compressed[CHUNK_SIZE * 2];

    sFileInfo         *file        = createFileInfo( compressedFileName, m_file->position(), 0, 0 );
    IBufferCompressor *compressor  = createCompressor( m_info.m_compressor );

    FILE *input = fopen( fileName, "rb" );
    if( !input ) {
        file->m_decompressedSize = 0;
        return false;
    }

    while( !feof( input ) ) {
        int read = fread( chunk, 1, CHUNK_SIZE, input );
        if( read == 0 ) {
            break;
        }

        int compressedSize = compressor->compressToBuffer( chunk, read, compressed, CHUNK_SIZE * 2 );

        m_file->write( &compressedSize, sizeof( compressedSize ) );
        m_file->write( compressed, compressedSize );
    }

    file->m_decompressedSize = ftell( input );
    fclose( input );
    delete compressor;

    return true;
}

// ** Archive::open
bool Archive::open( Stream* file )
{
    clearFiles();
    
    m_isCreating = false;
    m_file       = file;
    m_fileName   = file->isFileStream() ? file->isFileStream()->fileName() : "";

    m_file->read( &m_info, sizeof( sArchiveInfo ) );

    if( strncmp( m_info.m_token, "PACKAGE", 7 ) != 0 ) {
        return false;
    }

    readFiles();

    // !! WORKAROUND for in-memory files
    if( file->isMemoryStream() ) {
        return true;
    }

    DC_RELEASE( m_file );
    return true;
}

// ** Archive::create
void Archive::create( Stream* file, eCompressor compressor )
{
    clearFiles();

    m_isCreating        = true;
    m_file              = file;
    m_info.m_compressor = compressor;

    m_file->write( &m_info, sizeof( sArchiveInfo ) );
}

// ** Archive::close
void Archive::close( void )
{
    if( m_isCreating ) {
        writeFiles();
    }

    clearFiles();

    DC_RELEASE( m_file );
}

// ** Archive::fileName
CString Archive::fileName( void ) const
{
    return m_fileName.c_str();
}

// ** Archive::openFile
Stream* Archive::openFile( const char *fileName ) const
{
    const sFileInfo *fileInfo = findFileInfo( fileName );
    if( !fileInfo ) {
        return NULL;
    }

    Stream* file = m_file;

    // !! WORKAROUND for in-memory files
    if( m_fileName != "" ) {
        file = m_diskFileSystem->openFile( m_fileName.c_str() );
    }

    DC_BREAK_IF( file == NULL );
    if( !file ) {
        return NULL;
    }

    file->setPosition( fileInfo->m_offset );

    return DC_NEW PackedStream( file, createCompressor( m_info.m_compressor ), fileInfo->m_decompressedSize, fileInfo->m_offset );
}

// ** Archive::openFile
Stream* Archive::openFile( const char *fileName, const char *mode ) const
{
	if( mode[0] == 'r' ) {
		return openFile( fileName );
	}

    DC_BREAK_IF( true );
	return NULL;
}

// ** Archive::fileExists
bool Archive::fileExists( const char *fileName ) const
{
    return findFileInfo( fileName ) != NULL;
}

// ** Archive::extractFile
bool Archive::extractFile( const char *fileName, const char *outputFileName )
{
    AutoPtr<Stream> input = openFile( fileName );
    if( input == NULL ) {
        printf( "No such file %s in package\n", fileName );
        return false;
    }

    AutoPtr<Stream> output = m_diskFileSystem->openFile( outputFileName, "wb" );
    if( output == NULL ) {
        printf( "Failed to open file %s for writing\n", outputFileName );
        system( "ls" );
        return false;
    }

    const int kChunkSize = 16536;
    u8      chunk[kChunkSize];
    
    while( input->hasDataLeft() ) {
        int read = input->read( chunk, kChunkSize );
        output->write( chunk, read );
    }

    return true;
}

// ** Archive::clearFiles
void Archive::clearFiles( void )
{
    for( tFileInfoList::const_iterator i = m_files.begin(), end = m_files.end(); i != end; i++ ) {
        delete *i;
    }
    m_files.clear();
}

// ** Archive::writeFiles
void Archive::writeFiles( void )
{
    m_info.m_fileInfoOffset = m_file->position();
    m_info.m_totalFiles     = ( int )m_files.size();

    for( tFileInfoList::const_iterator i = m_files.begin(), end = m_files.end(); i != end; i++ ) {
        writeFileInfo( *i );
    }

    m_file->setPosition( 0 );
    m_file->write( &m_info, sizeof( sArchiveInfo ) );
}

// ** Archive::readFiles
void Archive::readFiles( void )
{
    m_file->setPosition( m_info.m_fileInfoOffset );

    for( int i = 0; i < m_info.m_totalFiles; i++ ) {
        m_files.push_back( readFileInfo() );
    }
}

// ** Archive::findOffsetEntry
const Archive::sFileInfo* Archive::findFileInfo( const char *fileName ) const
{
    for( tFileInfoList::const_iterator i = m_files.begin(), end = m_files.end(); i != end; i++ ) {
        if( strcmp( fileName, (*i)->m_name ) == 0 ) {
            return *i;
        }
    }

    return NULL;
}

// ** Archive::createFileInfo
Archive::sFileInfo* Archive::createFileInfo( const char *fileName, int offset, int compressedSize, int decompressedSize )
{
    sFileInfo *entry = DC_NEW sFileInfo( fileName, offset, decompressedSize );
    m_files.push_back( entry );
    return entry;
}

// ** Archive::readFileInfo
Archive::sFileInfo* Archive::readFileInfo( void ) const
{
    char buffer[256];

    m_file->readString( buffer, 256 );

    sFileInfo *file = DC_NEW sFileInfo( buffer, 0, 0 );

    m_file->read( &file->m_decompressedSize, sizeof( file->m_decompressedSize ) );
    m_file->read( &file->m_offset, sizeof( file->m_offset ) );

    return file;
}

// ** Archive::writeFileInfo
void Archive::writeFileInfo( const sFileInfo *file ) const
{
    m_file->writeString( file->m_name );
    m_file->write( &file->m_decompressedSize, sizeof( file->m_decompressedSize ) );
    m_file->write( &file->m_offset, sizeof( file->m_offset ) );
}

} // namespace io

DC_END_DREEMCHEST
