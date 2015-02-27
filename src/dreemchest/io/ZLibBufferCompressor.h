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

#ifndef		__DC_Io_ZLibBufferCompressor_H__
#define		__DC_Io_ZLibBufferCompressor_H__

#include	"IBufferCompressor.h"

#ifdef HAVE_ZLIB
	#include <zlib/zlib.h>
#endif

DC_BEGIN_DREEMCHEST

namespace io {

    // ** class ZLibBufferCompressor
    class ZLibBufferCompressor : public IBufferCompressor {
    public:

        // ** IBufferCompressor
        virtual int     compressToBuffer( const u8 *in, int size, u8 *out, int maxSize );
        virtual int     decompressToBuffer( const u8 *in, int size, u8 *out, int maxSize );

        // ** ZLibBufferCompressor
        bool            beginStreamCompression( void );
        void            endStreamCompression( void );
        bool            beginStreamDecompression( void );
        void            endStreamDecompression( void );

    private:

	#ifdef HAVE_ZLIB
        z_stream        m_stream;
	#endif
    };

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_ZLibBufferCompressor_H__	*/
