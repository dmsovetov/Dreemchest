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

#include "ZLibBufferCompressor.h"

#define CHUNK 1024

DC_BEGIN_DREEMCHEST

namespace Io {

// ** ZLibBufferCompressor::compressToBuffer
s32 ZLibBufferCompressor::compressToBuffer( const u8 *in, s32 size, u8 *out, s32 maxSize )
{
    s64 result = 0;

    if( !beginStreamCompression() ) {
        return -1;
    }

    m_stream.next_in   = ( Bytef* )in;
    m_stream.avail_in  = size;

    m_stream.next_out  = ( Bytef* )out;
    m_stream.avail_out = maxSize;

    if( deflate( &m_stream, Z_FINISH ) == Z_STREAM_END ) {
        result = m_stream.total_out;
    }

    endStreamCompression();

    return result;
}

// ** ZLibBufferCompressor::decompressToBuffer
s32 ZLibBufferCompressor::decompressToBuffer( const u8 *in, s32 size, u8 *out, s32 maxSize )
{
    s64 result = -1;

    if( !beginStreamDecompression() ) {
        return -1;
    }

    m_stream.next_in   = ( Bytef* )in;
    m_stream.avail_in  = size;

    m_stream.next_out  = ( Bytef* )out;
    m_stream.avail_out = maxSize;

    if( inflate( &m_stream, Z_FIXED ) == Z_STREAM_END ) {
        result = m_stream.total_out;
    }

    endStreamDecompression();

    return result;
}

// ** ZLibBufferCompressor::beginStreamCompression
bool ZLibBufferCompressor::beginStreamCompression( void )
{
    m_stream.zalloc = Z_NULL;
    m_stream.zfree  = Z_NULL;
    m_stream.opaque = Z_NULL;

    if( deflateInit( &m_stream, Z_DEFAULT_COMPRESSION ) != Z_OK ) {
        return false;
    }

    return true;
}

// ** ZLibBufferCompressor::endStreamCompression
void ZLibBufferCompressor::endStreamCompression( void )
{
    deflateEnd( &m_stream );
}

// ** ZLibBufferCompressor::beginStreamDecompression
bool ZLibBufferCompressor::beginStreamDecompression( void )
{
    m_stream.zalloc        = Z_NULL;
    m_stream.zfree        = Z_NULL;
    m_stream.opaque        = Z_NULL;
    m_stream.next_in    = Z_NULL;
    m_stream.avail_in    = 0;
    
    if( inflateInit( &m_stream ) != Z_OK ) {
        return false;
    }

    return true;
}

// ** ZLibBufferCompressor::endStreamDecompression
void ZLibBufferCompressor::endStreamDecompression( void )
{
    inflateEnd( &m_stream );
}
    
} // namespace Io
    
DC_END_DREEMCHEST
