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

// Include the engine header file.
#include <Dreemchest.h>

// Open a root engine namespace
DC_USE_DREEMCHEST

// Open a platform namespace to use shorter types.
using namespace Platform;

// Open a sound namespace.
using namespace Sound;

// Application delegate is used to handle an events raised by application instance.
class SoundPlayback : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application ) {
		Sound::log::setStandardHandler();

        // Create the SoundFx instance
		SoundFxPtr sfx = SoundFx::create();

		SoundDataPtr s1 = sfx->createSound( "air_raid", "sounds/air_raid.wav" );
		s1->setLoading( SoundData::LoadToRam );
		s1->setFormat( SoundFormatWav );
		s1->setLooped( false );

		SoundDataPtr s2 = sfx->createSound( "air_raid_m", "sounds/air_raid.ogg" );
		s2->setLoading( SoundData::Stream );
		s2->setFormat( SoundFormatOgg );
		s2->setLooped( true );

		SoundChannelPtr ch = sfx->play( "air_raid" );

		while( true ) {
			sfx->update( 0.1f );

			if( ch.valid() && !ch->isPlaying() ) {
				ch = SoundChannelPtr();
				sfx->play( "air_raid_m" );
			}

			Threads::Thread::sleep( 10 );
		}
    }

};

// Now declare an application entry point with Sounds application delegate.
dcDeclareApplication( new SoundPlayback )