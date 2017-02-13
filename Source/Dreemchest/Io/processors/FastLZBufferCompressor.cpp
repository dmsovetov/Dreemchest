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

#include "FastLZBufferCompressor.h"

#ifdef HAVE_FASTLZ
    #include <fastlz.h>
#endif

DC_BEGIN_DREEMCHEST

namespace Io {

// ** FastLZBufferCompressor::compressToBuffer
s64 FastLZBufferCompressor::compressToBuffer( const u8 *in, u64 inSize, u8 *out, u64 maxSize )
{
#ifdef HAVE_FASTLZ
    return fastlz_compress( in, inSize, out );
#else
    return 0;
#endif
}

// ** FastLZBufferCompressor::decompressToBuffer
s64 FastLZBufferCompressor::decompressToBuffer( const u8 *in, u64 inSize, u8 *out, u64 maxSize )
{
#ifdef HAVE_FASTLZ
    return fastlz_decompress( in, inSize, out, maxSize );
#else
    return 0;
#endif
}
    
} // namespace Io

DC_END_DREEMCHEST
