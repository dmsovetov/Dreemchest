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

#ifndef        __DC_Sound_H__
#define        __DC_Sound_H__

#include "../Dreemchest.h"

#ifndef FOO_INCLUDED
    #include <Foo/Foo.h>
#endif

DC_BEGIN_DREEMCHEST

    namespace sound {

        DECLARE_LOG( log )

        class SoundFx;
        class SoundChannel;
        class SoundData;
        class SoundGroup;
        class Fader;

        class SoundEngine;
        class SoundSource;
        class IStreamOpener;
        class ISoundStream;
        class SoundDecoder;
        class SoundBuffer;

        // ** enum SoundSampleFormat
        enum SoundSampleFormat {
            SoundSampleMono8,       //!< 8-bit mono.
            SoundSampleMono16,      //!< 16-bit mono.
            SoundSampleStereo8,     //!< 8-bit stereo.
            SoundSampleStereo16,    //!< 16-bit stereo.
        };

        // ** enum SoundFormat
        enum SoundFormat {
            SoundFormatUnknown = 0, //!< Unknown file format.
            SoundFormatWav,         //!< Wave sound format.
            SoundFormatOgg,         //!< Ogg vorbis.
            SoundFormatMp3,         //!< MP3
        };

        typedef std::vector<SoundSource*>   SoundSourceArray;
        typedef std::vector<SoundData*>     SoundDataArray;

    } // namespace sound

    typedef sound::SoundEngine*         dcSoundEngine;
    typedef sound::SoundSource*         dcSoundSource;
    typedef sound::SoundDecoder*        dcSoundDecoder;
    typedef sound::SoundFx*             dcSoundFx;
    typedef sound::SoundChannel*        dcSoundChannel;

    DC_DECLARE_PTRS( sound::SoundData,      SoundData )
    DC_DECLARE_PTRS( sound::SoundChannel,   SoundChannel )
    DC_DECLARE_PTRS( sound::SoundGroup,     SoundGroup )
    DC_DECLARE_PTRS( sound::SoundBuffer,    SoundBuffer );

    typedef std::vector<dcSoundChannelStrong>  SoundChannelArray;
    typedef std::vector<dcSoundChannelWeak>    SoundChannelWeakArray;

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
    #include "SoundFx.h"
    #include "SoundData.h"
    #include "SoundGroup.h"
    #include "SoundEvent.h"
    #include "SoundChannel.h"
#endif

#endif    /*    !__DC_Sound_H__    */