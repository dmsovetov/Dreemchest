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

#ifndef __DC_SoundGroup_H__
#define __DC_SoundGroup_H__

#include "Sound.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

    // ** struct SoundGroupInfo
    struct SoundGroupInfo {
        String     identifier;
        u8         behaviour;
        u8         slotCount;
        u32        threshold;
        f32        fadeTime;
        f32           volume;
    };

    //! SoundGroup instance is a home for SoundChannel instances that are playing sounds.
    /*!
     Each sound playback resides inside the group. For example, short sound effects are put
     to a 'sfx' group and background music is put to 'bgm' group. Each group has a limit amount
     of sound playback slots and a behaviour, that is used to answer a question 'What should we do, when a sound group is full?'.
     */
    class SoundGroup : public RefCounted {
    friend class SoundFx;
    public:
        //! Behaviour of the sound group when it's full.
        enum Behaviour {
            Skip,                   //!< Don't play the sound.
            ReplaceLowest,          //!< Replace the sound with a lower priority then the played sound has.
            ReplaceLowestOldest,    //!< Replace the sound with a lower priority or the oldest sound with the same priority.
            ReplaceOldest           //!< Replace the oldest sound is a group.
        };

    public:

                                //! Sound group constructor.
                                /*!
                                    \param identifier The sound group identifier.
                                 */
                                SoundGroup( CString identifier );
                                ~SoundGroup( void );

        //! Returns true if the group has free playback slots.
        bool                    hasFreeSlots( void ) const;

        //! Returns a group identifier.
        CString                    identifier( void ) const;

        //! Sets a group identifier.
        void                    setIdentifier( CString value );

        //! Sets the count of playback slots (maximum sounds that can be playing simultaneously).
        void                    setSlotCount( u32 value );

        //! Returns the count of a playback slots (maximum sounds that can be playing simultaneously).
        u32                     slotCount( void ) const;

        //! Returns the count of a used playback slots.
        u32                     activeSlotCount( void ) const;

        //! Sets the behaviour to follow when the group is full and play is requested.
        void                    setBehaviour( Behaviour value );

        //! Returns the behaviour to follow when the group is full and play is requested.
        Behaviour               behaviour( void ) const;

        //! Sets the minimum sound priority to get the playback slot inside the group.
        void                    setThreshold( u32 value );

        //! Returns the minimum sound priority.
        u32                     threshold( void ) const;

        //! Returns a sound fade out time (in milliseconds) for the freed playback slots.
        f32                     fadeTime( void ) const;

        //! Sets a sound fade out time (in milliseconds) for the freed playback slots.
        void                    setFadeTime( f32 value );

        //! Returns a volume factor that is applied for all sounds inside this group.
        f32                        volume( void ) const;

        //! Sets a volume factor that is applied for all sounds inside this group.
        void                    setVolume( f32 value );

        //! Returns a serialized sound group data.
        SoundGroupInfo          data( void ) const;

        //! Loads a serialized sound group data.
        void                    setData( const SoundGroupInfo& value );

    private:

        //! Updates sound slots in a group.
        void                    update( void );

        //! Returns true if this group has a free slot for a given sound.
        /*!
         Searches for a free playback slot. Applies the defined behaviour if no more free slots available.
         \param data Sound data.
         \return True if a group has a free slot, otherwise false.
         */
        bool                    requestSlot( SoundDataWPtr data );

        //! Pushes a sound channel to a group.
        void                    addSound( const SoundChannelWPtr& channel );

        //! Returns an index of a lowest playback slot.
        s32                     findLowestSlot( u32 priority ) const;

        //! Returns an index of a lowest playback slot or an oldest with the given priority.
        s32                     findLowestOldest( u32 priority ) const;

    private:

        //! Sound group identifier.
        String                  m_identifier;

        //! Maximum sounds that can be playing simultaneously.
        u32                     m_slotCount;

        //! Behaviour to apply when the playback limit is reached.
        Behaviour               m_behaviour;

        //! Minimum sound priority to get the playback slot inside the group.
        u32                     m_threshold;

        //! Sound channel slots.
        SoundChannelsWeak        m_slots;
        
        //! Fade out time.
        f32                     m_fadeTime;

        //! Volume factor for all sounds in this group.
        f32                        m_volume;
    };

} // namespace Sound

DC_END_DREEMCHEST

#endif        /*    __DC_SoundGroup_H__    */
