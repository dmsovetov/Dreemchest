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

#include "Archive.h"

#include "streams/FileStream.h"
#include "streams/PackedStream.h"
#include "DiskFileSystem.h"

#ifdef ZLIB_FOUND
    #include "processors/ZLibBufferCompressor.h"
#endif

#ifdef FASTLZ_FOUND
    #include "processors/FastLZBufferCompressor.h"
#endif

DC_BEGIN_DREEMCHEST

namespace Io {

// ** Archive::Archive
Archive::Archive( const DiskFileSystem *diskFileSystem ) : m_diskFileSystem( diskFileSystem ), m_file( NULL )
{
    m_isCreating = false;
}

Archive::~Archive( void )
{

}

// ** Archive::createCompressor
IBufferCompressor* Archive::createCompressor( eCompressor compressor ) const
{
    switch( compressor ) {
    case CompressorFastLZ:  
                            #ifdef FASTLZ_FOUND
                                return DC_NEW FastLZBufferCompressor;
                            #else
                                LogWarning( "archive", "%s", "unsupported FastLZ compressor requested\n" );
                            #endif
                            break;

    case CompressorZ:      
                            #ifdef ZLIB_FOUND
                                return DC_NEW ZLibBufferCompressor;
                            #else
                                LogWarning( "archive", "%s", "unsupported ZLib compressor requested\n" );
                            #endif
                            break;

    default:                return NULL;
    }

    return NULL;
}

// ** Archive::packFile
bool Archive::packFile( const Path& fileName, const Path& compressedFileName )
{
    u8 chunk[CHUNK_SIZE + 1], compressed[CHUNK_SIZE * 2];

    sFileInfo         *file        = createFileInfo( compressedFileName, m_file->position(), 0, 0 );
    IBufferCompressor *compressor  = createCompressor( m_info.m_compressor );

    FILE *input = fopen( fileName.c_str(), "rb" );
    if( !input ) {
        file->m_decompressedSize = 0;
        return false;
    }

    while( !feof( input ) ) {
        s32 read = fread( chunk, 1, CHUNK_SIZE, input );
        if( read == 0 ) {
            break;
        }

        s32 compressedSize = compressor->compressToBuffer( chunk, read, compressed, CHUNK_SIZE * 2 );

        m_file->write( &compressedSize, sizeof( compressedSize ) );
        m_file->write( compressed, compressedSize );
    }

    file->m_decompressedSize = ftell( input );
    fclose( input );
    delete compressor;

    return true;
}

// ** Archive::open
bool Archive::open( const StreamPtr& file )
{
#if 0
    clearFiles();

    m_file = StreamPtr();
    
    m_isCreating = false;
    m_file       = file;
    m_fileName   = file->isFileStream() ? file->isFileStream()->fileName() : "";

    m_file->read( &m_info, sizeof( sArchiveInfo ) );

    if( strncmp( m_info.m_token, "PACKAGE", 7 ) != 0 ) {
        return false;
    }

    readFiles();

    // !! WORKAROUND for in-memory files
    if( file->isByteBuffer() ) {
        return true;
    }

    m_file = StreamPtr();
#else
    NIMBLE_NOT_IMPLEMENTED
#endif
    return true;
}

// ** Archive::create
void Archive::create( const StreamPtr& file, eCompressor compressor )
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

    m_file = StreamPtr();
}

// ** Archive::fileName
const Path& Archive::fileName( void ) const
{
    return m_fileName;
}

// ** Archive::openFile
StreamPtr Archive::openFile( const Path& fileName ) const
{
    const sFileInfo *fileInfo = findFileInfo( fileName );
    if( !fileInfo ) {
        return NULL;
    }

    StreamPtr file = m_file;

    // !! WORKAROUND for in-memory files
    if( m_fileName != "" ) {
        file = m_diskFileSystem->openFile( m_fileName );
    }

    NIMBLE_BREAK_IF( file == NULL );
    if( file == NULL ) {
        return StreamPtr();
    }

    file->setPosition( fileInfo->m_offset );

    return DC_NEW PackedStream( file, createCompressor( m_info.m_compressor ), fileInfo->m_decompressedSize, fileInfo->m_offset );
}

// ** Archive::openFile
StreamPtr Archive::openFile( const Path& fileName, StreamMode mode ) const
{
    if( mode == BinaryReadStream ) {
        return openFile( fileName );
    }

    NIMBLE_BREAK_IF( true );
    return StreamPtr();
}

// ** Archive::fileExists
bool Archive::fileExists( const Path& fileName ) const
{
    return findFileInfo( fileName ) != NULL;
}

// ** Archive::extractFile
bool Archive::extractFile( const Path& fileName, const Path& outputFileName )
{
    StreamPtr input = openFile( fileName );
    if( input == NULL ) {
        LogError( "archive", "extraction, no such file %s in package\n", fileName.c_str() );
        return false;
    }

    StreamPtr output = m_diskFileSystem->openFile( outputFileName, BinaryWriteStream );
    if( output == NULL ) {
        LogError( "archive", "extraction, failed to open file %s for writing\n", outputFileName.c_str()  );
        return false;
    }

    const s32   kChunkSize = 16536;
    UPtr<u8> chunk = DC_NEW u8[kChunkSize];
    
    while( input->hasDataLeft() ) {
        s32 read = input->read( chunk.get(), kChunkSize );
        output->write( chunk.get(), read );
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
const Archive::sFileInfo* Archive::findFileInfo( const Path& fileName ) const
{
    for( tFileInfoList::const_iterator i = m_files.begin(), end = m_files.end(); i != end; i++ ) {
        if( fileName == (*i)->m_name ) {
            return *i;
        }
    }

    return NULL;
}

// ** Archive::createFileInfo
Archive::sFileInfo* Archive::createFileInfo( const Path& fileName, s32 offset, s32 compressedSize, s32 decompressedSize )
{
    sFileInfo *entry = DC_NEW sFileInfo( fileName.c_str(), offset, decompressedSize );
    m_files.push_back( entry );
    return entry;
}

// ** Archive::readFileInfo
Archive::sFileInfo* Archive::readFileInfo( void ) const
{
    String name;

    m_file->readString( name );

    sFileInfo *file = DC_NEW sFileInfo( name.c_str(), 0, 0 );

    m_file->read( &file->m_decompressedSize, sizeof( file->m_decompressedSize ) );
    m_file->read( &file->m_offset, sizeof( file->m_offset ) );

    return file;
}

// ** Archive::writeFileInfo
void Archive::writeFileInfo( const sFileInfo *file )
{
    m_file->writeString( file->m_name );
    m_file->write( &file->m_decompressedSize, sizeof( file->m_decompressedSize ) );
    m_file->write( &file->m_offset, sizeof( file->m_offset ) );
}

} // namespace Io

DC_END_DREEMCHEST
