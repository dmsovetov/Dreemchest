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

#ifndef __DC_Io_PackedStream_H__
#define __DC_Io_PackedStream_H__

#include "Stream.h"

DC_BEGIN_DREEMCHEST

namespace Io {
        
    class IBufferCompressor;

	// ** class PackedStream
	class PackedStream : public Stream {

        DC_DECLARE_IS( PackedStream, PackedStream, this );

	public:

                                PackedStream( const StreamPtr& file, IBufferCompressor* compressor, s32 fileSize, s32 fileOffset );
		virtual					~PackedStream( void );

		//! Returns a decompressed file length.
        virtual s32             length( void ) const;

        //! Returns a current file position.
        virtual s32             position( void ) const;

        //! Sets a position inside the file.
        virtual void            setPosition( s32 offset, SeekOrigin origin = SeekSet );

        //! Reads data from file.
        virtual s32             read( void *buffer, s32 size );

        // ** PackedStream
        void                    reopen( void );
        void                    decompressChunk( void );
        s32                     readFile( u8* buffer, s32 length );
        s32                     readFromBuffer( u8* buffer, s32 size );

    private:

        IBufferCompressor*      m_compressor;
        StreamPtr               m_file;

        s32                     m_position;
        s32                     m_fileSize;
        s32                     m_fileOffset;

        s32                     m_bytesAvailable;
        s32                     m_bufferOffset;
        u8*						m_buffer;
	};

} // namespace Io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_PackedStream_H__	*/