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

#ifndef __DC_SoundData_H__
#define __DC_SoundData_H__

#include "Sound.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

    // ** struct SoundDataInfo
    struct SoundDataInfo {
        String          identifier;
        String          uri;
        String          group;
        u32             type;
        u8              loading;
        f32             fadeTime;
        f32             volume;
        Range           volumeModifier;
        f32             pitch;
        Range           pitchModifier;
        bool            isLooped;
        bool            isRelative;
        f32             referenceDistance;
        f32             maximumDistance;
        u32             priority;
    };

    //! SoundData class represents a single sound that is shared
    //! across all hardware sources that are playing a sound.
    class SoundData : public RefCounted {
    friend class SoundFx;
    public:

        //! Supported sound loading flags
        enum LoadingFlags {
            Stream,            //!< The sound is streamed from an asset file.
            Decode,            //!< Decode the sound data and store the result in RAM.
            LoadToRam,         //!< The sound asset file is copied to the RAM without decoding.
        };

    public:

								//! Sound data constructor.
								/*!
									\param identifier Sound identifier.
									\param uri Sound asset unique identifier.
									\param group The sound group this sound belongs to. Pass NULL for orphan sounds.
								 */
								SoundData( SoundFxWPtr sfx, CString identifier, CString uri, SoundGroupWPtr group );
								~SoundData( void );

        //! Returns a sound identifier.
        CString					identifier( void ) const;
        //! Sets a sound identifier.
        void					setIdentifier( CString value );
        //! Returns a sound URI identifier.
        CString					uri( void ) const;
        //! Sets a sound URI identifier.
        void					setUri( CString value );
        //! Returns a pointer to a sound group (NULL for orphan sounds).
		SoundGroupWPtr			group( void ) const;
        //! Sets a pointer to a sound group.
        void					setGroup( SoundGroupWPtr value );
        //! Returns a sound type.
        u32						type( void ) const;
        //! Sets a sound type.
        void					setType( u32 value );
        //! Returns a sound loading flag.
        LoadingFlags			loading( void ) const;
        //! Sets a sound loading flag.
        void					setLoading( LoadingFlags value );
		//! Returns a sound container format.
		SoundContainerFormat	format( void ) const;
		//! Sets the sound container format.
		void					setFormat( SoundContainerFormat value );
        //! Returns a sound fade in/fade out time.
        f32						fadeTime( void ) const;
        //! Sets a sound fade in/fade out time.
        void					setFadeTime( f32 value );
        //! Returns base sound playback volume.
        f32						volume( void ) const;
        //! Sets a base sound playback volume.
        void					setVolume( f32 value );
        //! Returns base sound playback pitch.
        f32						pitch( void ) const;
        //! Sets a base sound playback pitch.
        void					setPitch( f32 value );
        //! Returns true if this sound is looped, otherwise false.
        bool					isLooped( void ) const;
        //! Sets a sound looping flag.
        void					setLooped( bool value );
        //! Returns true if this sound is positioned relatively to a listener.
        bool                    isRelative( void ) const;
        //! Sets a sound relative flag.
        void                    setRelative( bool value );
        //! Returns the reference distance value.
        /*!
            The distance under which the volume for the source
            would normally drop by half (before being influenced
            by rolloff factor or maximumDistance).   
        */
        f32                     referenceDistance( void ) const;
        //! Sets the reference distance value.
        void                    setReferenceDistance( f32 value );
        //! Returns maximum distance value.
        /*!
            Used with the Inverse Distance Model to set
            the distance where there will no longer be any
            attenuation of the source.
        */
        f32                     maximumDistance( void ) const;
        //! Sets the maximum distance value.
        void                    setMaximumDistance( f32 value );
        //! Sets the rolloff factor value.
        /*!
            The rolloff rate for the source default is 1.0.
        */
        f32                     rolloffFactor( void ) const;
        //! Sets the rollof factor value.
        void                    setRolloffFactor( f32 value );
        //! Returns a sound priority.
        u32						priority( void ) const;
        //! Sets a sound priority.
        void					setPriority( u32 value );
        //! Returns a serialized sound data.
        SoundDataInfo			data( void ) const;
        //! Loads a serialized sound data.
        void					setData( const SoundDataInfo& value );
        //! Returns a sound buffer with decoded PCM data.
        SoundBufferWPtr			pcm( void ) const;
        //! Sets a sound buffer with decoded PCM data.
        void					setPcm( SoundBufferPtr value );
        //! Calculates and returns a sound volume with a random modifier applied.
        f32						volumeForSound( void ) const;
        //! Calculates and returns a sound pitch with a random modifier applied.
        f32						pitchForSound( void ) const;

    private:

        //! Parent sound fx.
        SoundFxWPtr				m_soundFx;
        //! Sound identifier.
        String					m_identifier;
        //! Asset associated with this sound.
        String					m_uri;
        //! The group this sound belongs to.
        SoundGroupWPtr			m_group;
        //! Sound type.
        u32						m_type;
		//! The sound data container format.
		SoundContainerFormat	m_format;
        //! The flag indicating the loading behaviour for this sound.
        LoadingFlags			m_loading;
        //! Sound fade in/ fade out time in milliseconds.
        f32						m_fadeTime;
        //! Sound volume.
        f32						m_volume;
        //! Sound volume random multiplicative modifier.
        Range					m_volumeModifier;
        //! Sound pitch.
        f32						m_pitch;
        //! Sound pitch random multiplicative modifier.
        Range					m_pitchModifier;
        //! The flag indicating whether this sound is looped.
        bool					m_isLooped;
        //! The flag indicating whether this sound is positioned relatively to a listener.
        bool                    m_isRelative;
        //! Reference distance value (used by a distance model).
        f32                     m_referenceDistance;
        //! Maximum distance (used by a distance model).
        f32                     m_maximumDistance;
        //! Rolloff factor (used by a distance model).
        f32                     m_rolloffFactor;
        //! Sound playback priority.
        u32						m_priority;
        //! Decoded PCM data.
        SoundBufferPtr			m_pcm;
    };
    
} // namespace Sound
    
DC_END_DREEMCHEST

#endif        /*    __DC_SoundData_H__    */