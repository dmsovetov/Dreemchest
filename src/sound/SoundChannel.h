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

namespace dreemchest {

namespace sound {

    // ** class SoundChannel
    class SoundChannel : public RefCounted {
    friend class SoundFx;
    public:

        bool                isPlaying( void ) const;
        bool                isStopped( void ) const;
        float               volume( void ) const;
        void                setVolume( float value );
        const SoundData*    sound( void ) const;

        void                pause( float fade = 0.0f );
        void                resume( float fade = 0.0f );
        void                stop( float fade = 0.0f );

    private:

                            SoundChannel( const SoundData* data, SoundSource* source );
        virtual             ~SoundChannel( void );

        bool                update( float dt );

        void                onFadeIn( Fader* fader );
        void                onFadeOut( Fader* fader );
        void                onStopped( Fader* fader );

    private:

        SoundSource*        m_source;
        Fader*              m_volumeFader;
        const SoundData*    m_sound;

        float               m_volume;
    };
    
} // namespace sound
    
} // namespace dreemchest

#endif        /*    __DC_SoundChannel_H__    */