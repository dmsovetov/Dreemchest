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

#ifndef __DC_SoundSource_H__
#define __DC_SoundSource_H__

#include    "../Sound.h"

DC_BEGIN_DREEMCHEST

namespace sound {

    // ** class SoundSource
    //! SoundSource represents a hardware sound source.
    class SoundSource {
    public:

        // ** enum SourceState
        //! Sound source state.
        enum SourceState {
            Unknown,    //!< The sound source state is unknown.
            Playing,    //!< The sound source is now playing.
            Stopped,    //!< The sound source is stopped and can be freed.
            Paused,     //!< The sound source is paused.
        };

    public:

                                SoundSource( void );
        virtual                 ~SoundSource( void );

        //! Updates a sound source state and streams new PCM data.
        virtual void            update( void );

        //! Returns a pointer to attached hardware sound buffer.
        virtual SoundBuffer*    buffer( void ) const;

        //! Sets a hardware sound buffer.
        virtual void            setBuffer( SoundBuffer* value );

        //! Returns a sound source current state.
        virtual SourceState     state( void ) const;

        //! Sets a sound source state.
        virtual void            setState( SourceState value );

        //! Returns a sound source volme.
        virtual f32             volume( void ) const;

        //! Sets a sound source volume.
        virtual void            setVolume( f32 value );

        //! Returns sound source pitch.
        virtual f32             pitch( void ) const;

        //! Sets a sound source pitch.
        virtual void            setPitch( f32 value );

        //! Returns true if sound source is looped, otherwise false.
        virtual bool            isLooped( void ) const;

        //! Sets the looping of a sound source.
        virtual void            setLooped( bool value );

    protected:

        //! Strong pointer to a hardware sound buffer.
        dcSoundBufferStrong     m_buffer;

        //! Sound source state.
        SourceState             m_state;

        //! Looping flag.
        bool                    m_isLooped;

        //! Sound source volume.
        f32                     m_volume;

        //! Sound source pitch.
        f32                     m_pitch;
    };

} // namespace sound

DC_END_DREEMCHEST

#endif        /*    __DC_SoundSource_H__    */