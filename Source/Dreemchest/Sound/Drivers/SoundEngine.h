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

#ifndef __DC_SoundEngine_H__
#define __DC_SoundEngine_H__

#include "../Sound.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

    //! Hardware sound engine, used for creating hardware sound sources and buffers.
    class SoundEngine : public RefCounted {
    public:

                                SoundEngine( void );
        virtual                 ~SoundEngine( void );

		//! Sets the master volume.
		virtual void			setVolume( f32 value ) = 0;

		//! Sets the master pitch.
		virtual void			setPitch( f32 value ) = 0;

        //! Sets the listener position.
        virtual void            setPosition( const Vec3& value ) = 0;

        //! Sets the distance attenuation model.
        virtual void            setDistanceModel( DistanceModel value ) = 0;

        //! Creates a new hardware sound source.
        virtual SoundSourcePtr	createSource( void );

        //! Creates a new hardware sound buffer.
        /*!
         \param decoder Sound decoder used to read PCM sound samples from input stream.
         \param chunks
         */
        virtual SoundBufferPtr	createBuffer( SoundDecoderPtr decoder, u32 chunks );

        //! Creates a sound decoder with a given input stream and file format.
        SoundDecoderPtr         createSoundDecoder( ISoundStreamPtr stream, SoundContainerFormat format = SoundFormatUnknown );

    private:

        //! Creates a sound decoder instance for a given format.
        SoundDecoderPtr			createSoundDecoder( SoundContainerFormat format ) const;

        //! Creates a sound decoder with a given input stream and file format.
        SoundDecoderPtr			createSoundDecoderWithFormat( ISoundStreamPtr stream, SoundContainerFormat format );
    };
    
} // namespace Sound
    
DC_END_DREEMCHEST

#endif        /*    __DC_SoundEngine_H__    */