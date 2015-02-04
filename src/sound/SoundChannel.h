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

#ifndef __DC_SoundChannel_H__
#define __DC_SoundChannel_H__

#include    "Sound.h"

DC_BEGIN_DREEMCHEST

namespace sound {

    // ** class SoundChannel
    //! A SoundChannel object represents a single sound playback.
    class SoundChannel : public RefCounted {
    friend class SoundFx;
    public:

        //! Returns true if sound is playing, otherwise false.
        bool                isPlaying( void ) const;

        //! Returns true if sound is stopped, otherwise false.
        bool                isStopped( void ) const;

        //! Returns a sound channel volume.
        f32                 volume( void ) const;

        //! Sets a sound channel volume.
        void                setVolume( f32 value );

        //! Returns a pointer to an attached sound data.
        const SoundData*    sound( void ) const;

        //! Pauses a sound playback with a given fade in time.
        /*!
         \param fade Sound fade in time in milliseconds.
         */
        void                pause( f32 fade = 0.0f );

        //! Resumes a paused sound playback with a given fade out time.
        /*!
         \param fade Sound fade out time in milliseconds.
         */
        void                resume( f32 fade = 0.0f );

        //! Stops a sound playback with a given fade in time.
        /*!
         \param fade Sound fade in time in milliseconds.
         */
        void                stop( f32 fade = 0.0f );

    private:

                            //! Constructs a new SoundChannel instance.
                            /*!
                             \param data Attached sound data instance.
                             \param source Hardware sound source.
                             */
                            SoundChannel( const SoundData* data, SoundSource* source );
        virtual             ~SoundChannel( void );

        //! Does a sound channel update (sound streaming, fading, stopping).
        /*!
         \param dt Time in seconds passed since last sound channel update.
         */
        bool                update( f32 dt );

        //! Fade in callback.
        void                onFadeIn( Fader* fader );
        //! Fade out callback.
        void                onFadeOut( Fader* fader );
        //! Fade in callback that is called after stopping the sound playback.
        void                onStopped( Fader* fader );

    private:

        //! Hardware sound source used for sound playback.
        SoundSource*        m_source;

        //! Fade instance to fade in/out sound volume.
        Fader*              m_volumeFader;

        //! Attached sound data, PCM sound samples are come from here.
        const SoundData*    m_sound;

        //! Sound channel volume.
        f32                 m_volume;
    };
    
} // namespace sound
    
DC_END_DREEMCHEST

#endif        /*    __DC_SoundChannel_H__    */