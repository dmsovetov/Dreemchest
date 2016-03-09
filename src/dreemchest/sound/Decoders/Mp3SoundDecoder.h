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

#ifndef __DC_Mp3SoundDecoder_H__
#define __DC_Mp3SoundDecoder_H__

#include "SoundDecoder.h"
#include <mad/mad.h>

#define MP3_BUFFER_SIZE    4096

DC_BEGIN_DREEMCHEST

namespace sound {

    // ** class Mp3SoundDecoder
    class Mp3SoundDecoder : public SoundDecoder {
    private:

        u8                      buffer[MP3_BUFFER_SIZE];
        u8                      outBuf[MP3_BUFFER_SIZE];
        u32                     bufferPos;
        mad_synth               mp3Synth;
        mad_stream              mp3Stream;
        mad_frame               mp3Frame;
        u32                     bitRate;
        u32                     numChannels;
        u16                     *resumeData;
        mad_fixed_t             *resumeLeft, *resumeRight;
        u32                     resumeSize;

    private:

        u32                     ReadFrame( void );
        void                    ResumeFrame( u8 *buffer );

    public:

                                Mp3SoundDecoder( u32 blockSize ) : SoundDecoder( blockSize ) {}
        virtual                 ~Mp3SoundDecoder( void );

        // ** SoundDecoder
        virtual void            close( void );
        virtual u32             read( u8 *buffer, u32 size );
        virtual void            seek( u32 pos );
    };

} // namespace sound

DC_END_DREEMCHEST

#endif    /*    !__DC_Mp3SoundDecoder_H__    */