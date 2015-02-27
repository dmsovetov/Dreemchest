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
#include    "IBufferCompressor.h"

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
            int				m_offset;
            int             m_decompressedSize;

                            sFileInfo( const char *name, int offset, int decompressedSize )
                                : m_offset( offset ), m_decompressedSize( decompressedSize ) {
                                m_name = strdup( name );
                            }
                            ~sFileInfo( void ) {
                                free( ( void* )m_name );
                            }
        };

        // ** struct sArchiveInfo
        struct sArchiveInfo {
            char            m_token[8];
            int             m_fileInfoOffset;
            int             m_totalFiles;
            eCompressor     m_compressor;

                            sArchiveInfo( void )
                                : m_fileInfoOffset( 0 ), m_totalFiles( 0 ) { strcpy( m_token, "PACKAGE" ); }
        };

        typedef std::list<sFileInfo*> tFileInfoList;

    public:

                                Archive( const DiskFileSystem *diskFileSystem );
        virtual                 ~Archive( void );

        // ** FileSystem
		virtual Stream*        openFile( const char *fileName ) const;
		virtual Stream*        openFile( const char *fileName, const char *mode ) const;
		virtual bool            fileExists( const char *fileName ) const;

        // ** Archive
        void                    release( void );
        bool                    open( Stream* file );
        void                    create( Stream* file, eCompressor compressor = CompressorZ );
        void                    close( void );

        CString             fileName( void ) const;

        bool                    packFile( const char *fileName, const char *compressedFileName );
        bool                    extractFile( const char *fileName, const char *outputFileName );

    private:

        void                    clearFiles( void );
        void                    writeFiles( void );
        void                    readFiles( void );

        IBufferCompressor*      createCompressor( eCompressor compressor ) const;

        sFileInfo*              createFileInfo( const char *fileName, int offset, int compressedSize = 0, int decompressedSize = 0 );
        const sFileInfo*        findFileInfo( const char *fileName ) const;
        sFileInfo*              readFileInfo( void ) const;
        void                    writeFileInfo( const sFileInfo *file ) const;

    private:

        static const int        CHUNK_SIZE = 16536;

        const DiskFileSystem*   m_diskFileSystem;
        tFileInfoList           m_files;
        sArchiveInfo            m_info;

        Stream*                m_file;
        bool                    m_isCreating;
        String             m_fileName;
    };

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Archive_H__	*/