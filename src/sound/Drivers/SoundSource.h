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

namespace dreemchest {

namespace sound {

    // ** class SoundSource
    class SoundSource {
    public:

        // ** enum eSourceState
        enum eSourceState {
            Unknown,
            Playing,
            Stopped,
            Paused,
        };

    public:

                                SoundSource( void );
        virtual                 ~SoundSource( void );

        virtual void            update( void );
        virtual SoundBuffer*    buffer( void ) const;
        virtual void            setBuffer( SoundBuffer* value );
        virtual eSourceState    state( void ) const;
        virtual void            setState( eSourceState value );
        virtual float           volume( void ) const;
        virtual void            setVolume( float value );
        virtual float           pitch( void ) const;
        virtual void            setPitch( float value );
        virtual bool            isLooped( void ) const;
        virtual void            setLooped( bool value );

    protected:

        dcSoundBufferStrong     m_buffer;
        eSourceState            m_state;
        bool                    m_isLooped;
        float                   m_volume;
        float                    m_pitch;
    };

} // namespace sound

} // namespace dreemchest

#endif        /*    __DC_SoundSource_H__    */