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

#ifndef __DC_WavSoundDecoder_H__
#define __DC_WavSoundDecoder_H__

#include "SoundDecoder.h"

namespace dreemchest {

namespace sound {

    // ** class WavSoundDecoder
    class WavSoundDecoder : public SoundDecoder {
    public:

                                WavSoundDecoder( void )
                                    : m_pcmSize( 0 ), m_pcmDataOffset( 0 ) {}
        virtual                 ~WavSoundDecoder( void );

        // ** SoundDecoder
        virtual bool            open( ISoundStream* stream );
        virtual long            read( u8 *buffer, int size );
        virtual void            seek( int pos );
        virtual int             size( void ) const;

    private:

        // ** struct sWaveFileHeader
        struct sWaveFileHeader {
            char        fileID[4];
            u32         fileSize;
            char        fileType[4];
        };

        // ** struct sWaveFileFormat
        struct sWaveFileFormat {
            u16        wFormatTag;         /* format type */
            u16        nChannels;          /* number of channels (i.e. mono, stereo...) */
            u32        nSamplesPerSec;     /* sample rate */
            u32        nAvgBytesPerSec;    /* for buffer estimation */
            u16        nBlockAlign;        /* block size of data */
            u16        wBitsPerSample;     /* number of bits per sample of mono data */
            u16        cbSize;             /* the count in bytes of the size of */
            /* extra information (after cbSize) */
        };

        u32                     m_pcmSize;
        long                    m_pcmDataOffset;
    };

} // namespace sound

} // namespace dreemchest

#endif    /*    !__DC_WavSoundDecoder_H__    */