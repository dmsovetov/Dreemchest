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

#ifndef		__DC_Io_Archive_H__
#define		__DC_Io_Archive_H__

#include    "FileSystem.h"
#include    "processors/IBufferCompressor.h"
#include    "Path.h"

DC_BEGIN_DREEMCHEST

namespace io {

    // ** eCompressor
    enum eCompressor {
        CompressorZ,
        CompressorFastLZ,
    };

    // ** class Archive
    class dcInterface Archive : public FileSystem {
    friend class PackedStream;

        // ** struct sFileInfo
        struct sFileInfo {
            const char		*m_name;
            s32				m_offset;
            s32             m_decompressedSize;

                            sFileInfo( CString name, s32 offset, s32 decompressedSize )
                                : m_offset( offset ), m_decompressedSize( decompressedSize ) {
                                m_name = strdup( name );
                            }
                            ~sFileInfo( void ) {
                                free( ( void* )m_name );
                            }
        };

        // ** struct sArchiveInfo
        struct sArchiveInfo {
            s8				m_token[8];
            s32             m_fileInfoOffset;
            s32             m_totalFiles;
            eCompressor     m_compressor;

                            sArchiveInfo( void )
                                : m_fileInfoOffset( 0 ), m_totalFiles( 0 ) { strcpy( m_token, "PACKAGE" ); }
        };

        typedef std::list<sFileInfo*> tFileInfoList;

    public:

                                Archive( const DiskFileSystem* diskFileSystem );
        virtual                 ~Archive( void );

        //! Opens a file for reading from this archive.
		virtual StreamPtr       openFile( const Path& fileName ) const;

        //! Opens a file from this archive, only reading is supported.
		virtual StreamPtr       openFile( const Path& fileName, StreamMode mode ) const;

        //! Returns true if the file with a given name exists inside an archive.
		virtual bool            fileExists( const Path& fileName ) const;

        // ** Archive
        void                    release( void );
        bool                    open( const StreamPtr& file );
        void                    create( const StreamPtr& file, eCompressor compressor = CompressorZ );
        void                    close( void );

        const Path&             fileName( void ) const;

        bool                    packFile( const Path& fileName, const Path& compressedFileName );
        bool                    extractFile( const Path& fileName, const Path& outputFileName );

    private:

        void                    clearFiles( void );
        void                    writeFiles( void );
        void                    readFiles( void );

        IBufferCompressor*      createCompressor( eCompressor compressor ) const;

        sFileInfo*              createFileInfo( const Path& fileName, s32 offset, s32 compressedSize = 0, s32 decompressedSize = 0 );
        const sFileInfo*        findFileInfo( const Path& fileName ) const;
        sFileInfo*              readFileInfo( void ) const;
        void                    writeFileInfo( const sFileInfo *file );

    private:

        static const int        CHUNK_SIZE = 16536;

        const DiskFileSystem*   m_diskFileSystem;
        tFileInfoList           m_files;
        sArchiveInfo            m_info;

        StreamPtr               m_file;
        bool                    m_isCreating;
        Path                    m_fileName;
    };

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Archive_H__	*/