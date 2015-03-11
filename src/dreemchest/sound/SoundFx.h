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

#ifndef __DC_SoundFx_H__
#define __DC_SoundFx_H__

#include    "Sound.h"

DC_BEGIN_DREEMCHEST

namespace sound {

    typedef Hash<dcSoundDataStrong>    Sounds;
    typedef Hash<dcSoundGroupStrong>   SoundGroups;
    typedef Hash<class SoundEvent*>    SoundEvents;

    // ** struct SoundFxInfo
    struct SoundFxInfo {
        Array<struct SoundDataInfo>  sounds;   //! Array of sounds.
        Array<struct SoundGroupInfo> groups;   //! Array of sound groups.
        Array<struct SoundEventInfo> events;   //! Array of sound events.
    };

    // ** class SoundFx
    class SoundFx {
    public:

        // ** enum eSoundHal
        //! Supported sound fx HALs
        enum SoundHal {
            None,        //!< No HAL, the sound playback is disabled.
            Default,     //!< Use a platform default HAL.
        };

    public:

                                //! Constructs a new SoundFx object.
                                /*!
                                 \param hal Used hardware abstraction layer.
                                 \param streamOpener A stream opener interface used to load sounds.
                                 */
                                SoundFx( SoundHal hal = Default, IStreamOpener* streamOpener = NULL );
        virtual                 ~SoundFx( void );

        //! Starts a playback of a single sound.
        /*!
            The playback cursor is rewind to the start of the sound data source.
            If no sound with a given identifier is found, the call is ignored.

            \param identifier Sound identifier to play.
            \param loop Initial looping state for the sound.
            \param fade Volume fade in time (milliseconds).
            \return SoundChannel of a started playback. If no playback started, NULL is returned.
        */
        dcSoundChannelStrong    playWithParameters( const char *identifier, bool loop, f32 fade );

        //! Starts a playback of a single sound.
        /*!
            The playback cursor is rewind to the start of the sound data source.
            If no sound with a given identifier is found, the call is ignored.
         
            \param identifier Sound identifier to play.
            \return SoundChannel of a started playback. If no playback started, NULL is returned.
        */
        dcSoundChannelStrong    play( const char *identifier );

        //! Triggers an event.
        /*!
            If no event with a given identifier is found, the call is ignored.
         
            \param identifier Event identifier to play.
            \return SoundChannel of a started playback. If no playback started, NULL is returned.
        */
        dcSoundChannelStrong    event( const char *identifier );

        //! Returns a group by a given name (NULL if no such group found).
        SoundGroup*             findGroupByName( const char *identifier );
        //! Removes a sound group by a given name.
        void                    removeGroupByName( const char *identifier );
        //! Renames a sound group.
        bool                    renameGroup( const char *identifier, const char *newName );

        //! Returns a sound data by a given name (NULL if no such sound found).
        SoundData*              findSoundByName( const char *identifier );
        //! Removes a sound data by a given name.
        void                    removeSoundByName( const char *identifier );
        //! Renames a sound data.
        bool                    renameSound( const char *identifier, const char *newName );

        //! Returns an event by a given name (NULL if no such sound found).
        SoundEvent*             findEventByName( const char *identifier );

        //! Creates a new sound group.
        /*!
            This method creates a new sound group with a given identifier and parameters.
            If the group with a same name is found, the call is ignored.

            \param identifier Sound group identifier.
            \param info Sound group initial parameters.
            \return created SoundGroup instance, otherwise NULL.
        */
        SoundGroup*             createGroup( const char *identifier );

        //! Creates a new sound.
        /*!
            This method creates a new sound with a given identifier, asset and parameters.
            If the sound with a same name is found, the call is ignored.

            \param identifier Sound identifier.
            \param asset Asset associated with this sound.
            \param info Sound initial parameters.
            \return created SoundData instance, otherwise NULL.
        */
        SoundData*              createSound( const char *identifier, const char* uri, const SoundGroup* group = NULL );

        //! Creates a new sound event.
        /*!
            This method creates a new event with a given identifier and parameters.
            If the event with a same name is found, the call is ignored.
         
            \param identifier Sound event identifier.
            \return Created SoundEvent instance, otherwise NULL.
        */
        SoundEvent*             createEvent( const char *identifier );

        //! Loads a sound fx from a serialized data
        /*!
            \param value Serialized data.
        */
        void                    setData( const SoundFxInfo& value );
        //! Returns a serialized sound fx data.
        SoundFxInfo             data( void ) const;
        //! Loads a sound fx data from file.
        /*!
            \param identifier File identifier.
        */
        bool                    load( const char *identifier );
        //! Resets all sound fx data and stops all sound playbacks.
        void                    reset( void );
        //! Updates all sound groups and channels, should be called each frame.
        void                    update( f32 dt );

        //! Returns a master sound volume.
        f32                     volume( void ) const;
        //! Sets a master sound volume.
        void                    setVolume( f32 value );
        //! Returns a master sound pitch.
        f32                     pitch( void ) const;
        //! Sets a master sound pitch.
        void                    setPitch( f32 value );

        //! Returns a reference to a sound group container.
        const SoundGroups&      groups( void ) const;
        //! Returns a reference to a sound container.
        const Sounds&           sounds( void ) const;

    private:

        //! Creates a sound playback source for a given sound data.
        /*!
            Creates a HAL sound source for a sound playback.
            If no HAL is created the NULL pointer is returned.
         
            \param data Sound data instance.
            \return Created SoundSource instance, otherwise NULL.
        */
        SoundSource*            createSource( SoundData* data ) const;

        //! Creates a sound buffer for a given sound data.
        /*!
            Creates a HAL sound buffer for a sound playback.
            If no HAL is created the NULL pointer is returned.
         
            \param data Source data instance.
            \return Creates SoundBuffer instance, otherwise NULL.
        */
        SoundBuffer*            createBuffer( SoundData *data ) const;

        //! Creates a decoder for a given sound source.
        SoundDecoder*           createDecoder( const SoundData* data ) const;

        //! Removes all stopped sound channels from an update queue.
        void                    cleanupChannels( void );

    private:

        //! Sound engine HAL.
        SoundEngine*            m_hal;

        //! Stream opener interface.
        IStreamOpener*          m_streamOpener;

        //! Array of sound channels the are now playing.
        SoundChannelArray       m_channels;

        //! Sounds registered inside this SoundFx.
        /*! Contains pairs of (strhash, sound data pointer). */
        Sounds                  m_sounds;

        //! Sound groups registered inside this SoundFx.
        /*! Contains pairs of (strhash, sound group pointer). */
        SoundGroups            m_groups;

        //! Sound event registered inside this SoundFx.
        /*! Contains pairs of (strhash, sound event pointer). */
        SoundEvents             m_events;
    };
    
} // namespace sound
    
DC_END_DREEMCHEST

#endif    /*    !__DC_SoundFx_H__    */