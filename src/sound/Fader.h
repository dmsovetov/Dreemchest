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

#ifndef __DC_SoundFader_H__
#define __DC_SoundFader_H__

#include    "Sound.h"

DC_BEGIN_DREEMCHEST

namespace sound {

    //! Fader callback type.
    typedef cClosure<void(class Fader*)> FaderCallback;

    // ** class Fader
    //! Fader object is used to fade sound source parameters.
    class Fader {
    public:

                            //! Constructs a new Fader object.
                            /*!
                             \param start Interpolation start value.
                             \param end Interpolation end value.
                             \param duration Interpolation duration in milliseconds.
                             \param callback Fader completion callback.
                             */
                            Fader( f32 start, f32 end, f32 duration, FaderCallback callback );

        //! Updates fader by a given amount of time.
        /*!
         \param dt Time in seconds passed since last fader update.
         \return New Fader value.
         */
        f32                 update( f32 dt );

        //! Returns a current Fader value.
        f32                 value( void ) const;

    private:

        //! Interpolation start value.
        f32                 m_start;

        //! Interpolation end value.
        f32                 m_end;

        //! Total interpolation duration.
        f32                 m_duration;

        //! Current interpolation time.
        f32                 m_time;

        //! Fader completion callback.
        FaderCallback       m_callback;
    };
    
} // namespace sound
    
DC_END_DREEMCHEST

#endif        /*    __DC_SoundFader_H__    */