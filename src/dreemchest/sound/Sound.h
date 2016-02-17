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

#ifndef __DC_Sound_H__
#define __DC_Sound_H__

#include "../Dreemchest.h"

#include <Io/Streams/ByteBuffer.h>
#include <Io/DiskFileSystem.h>

DC_BEGIN_DREEMCHEST

namespace Sound {

    NIMBLE_LOGGER_TAG( Sound )

    //! Supported sound sample formats.
    enum SoundSampleFormat {
        SoundSampleMono8,       //!< 8-bit mono.
        SoundSampleMono16,      //!< 16-bit mono.
        SoundSampleStereo8,     //!< 8-bit stereo.
        SoundSampleStereo16,    //!< 16-bit stereo.
    };

    //! Supported sound container formats.
    enum SoundContainerFormat {
        SoundFormatUnknown = 0, //!< Unknown file format.
        SoundFormatWav,         //!< Wave sound format.
        SoundFormatOgg,         //!< Ogg vorbis.
        SoundFormatMp3,         //!< MP3
    };

	dcDeclarePtrs( SoundFx )
	dcDeclarePtrs( SoundChannel )
	dcDeclarePtrs( SoundData )
	dcDeclarePtrs( SoundGroup )
	dcDeclarePtrs( SoundEvent )
	dcDeclarePtrs( Fader )

	dcDeclarePtrs( SoundEngine )
	dcDeclarePtrs( SoundBuffer )
	dcDeclarePtrs( SoundDecoder )
	dcDeclarePtrs( SoundSource )

	dcDeclarePtrs( IStreamOpener )
	dcDeclarePtrs( ISoundStream )

	//! Container type to store sound data by identifier.
    typedef Hash<SoundDataPtr> Sounds;

	//! Container type to store sound groups by name.
    typedef Hash<SoundGroupPtr> SoundGroups;

	//! Container type to store sound events by identifier.
    typedef Hash<SoundEventPtr> SoundEvents;

	//! Container type to store sound channel weak pointers.
	typedef Array<SoundChannelWPtr> SoundChannelsWeak;

	//! Container type to store sound channel strong pointers.
	typedef Array<SoundChannelPtr> SoundChannels;

} // namespace Sound

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
    #include "SoundFx.h"
    #include "SoundData.h"
    #include "SoundGroup.h"
    #include "SoundEvent.h"
    #include "SoundChannel.h"
	#include "SoundStream.h"
#endif

#endif    /*    !__DC_Sound_H__    */