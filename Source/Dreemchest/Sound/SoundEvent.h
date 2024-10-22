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

#ifndef __DC_SoundEvent_H__
#define __DC_SoundEvent_H__

#include "Sound.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

    // ** struct SoundEventInfo
    struct SoundEventInfo {
        String          identifier;
        u8              probability;
        u8              type;
        Array<String>   sounds;
    };

    //! SoundEvent class represents a set of sounds, that are played
    //! when we raise an event from a SoundFx instance.
    class SoundEvent : public RefCounted {
    friend class SoundFx;
    public:

        //! Event types.
        enum EventType {
            Random,         //!< Plays a random sound when event is triggered.
            RandomNoRepeat, //!< Plays a random sound when event is triggered and avoids the last sound repeat.
            Playlist,       //!< The next sound in a list is played each time an event is triggered.
        };

    public:

        //! Returns the probability of triggering the sound event.
        u8                  probability( void ) const;
        //! Returns the event type.
        EventType           type( void ) const;

    private:

                            //! Sound event constructor.
                            /*!
                                \param info The sound event info.
                            */
                            SoundEvent( CString identifier );
                            ~SoundEvent( void );

        //! Returns a sound event identifier.
        CString                identifier( void ) const;
        //! Sets a sound event identifier.
        void                setIdentifier( CString value );
        //! Returns a pointer to a sound data for event playback.
        CString                requestIdentifier( void ) const;
        //! Returns a random sound index.
        u32                 generateSoundIndex( void ) const;
        //! Returns a serialized sound event data.
        SoundEventInfo      data( void ) const;
        //! Loads a serialized sound event data.
        void                setData( const SoundEventInfo& value );

    private:

        //! Sound event identifier.
        String              m_identifier;
        //! Sounds that can be played by this event.
        StringArray         m_sounds;
        //! Probability of triggering a sound event.
        u8                  m_probability;
        //! Last played sound index.
        mutable u32         m_lastPlayedIndex;
        //! Event type.
        EventType           m_type;
    };
    
} // namespace Sound

DC_END_DREEMCHEST

#endif        /*    __DC_SoundEvent_H__    */
