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

#ifndef __DC_SoundBuffer_H__
#define __DC_SoundBuffer_H__

#include    "../Sound.h"

DC_BEGIN_DREEMCHEST

namespace sound {

    // ** class SoundBuffer
    //! Hardware sound buffer that holds decoded PCM samples.
    class SoundBuffer : public RefCounted {
    public:

                        //! Constructs a new SoundBuffer instance.
                        /*!
                         \param decoder Sound decoder instance used for streaming sound data from input stream and decoding them to raw PCM.
                         \param chunks Number of raw PCM buffers (used for streamed sound playback).
                         */
                        SoundBuffer( SoundDecoder* decoder, u32 chunks );
        virtual         ~SoundBuffer( void );

        //! Attaches this sound buffer to hardware sound source.
        virtual void    attachToSource( SoundSource* target );

        //! Detaches this sound buffer from hardware sound source.
        virtual void    detachFromSource( SoundSource* target );

        //! Updates a streamed buffer. Reads a new PCM data samles and uploads them to hardware sound source.
        virtual bool    updateStream( SoundSource* target, bool isLooped );

        //! Returns a pointer to used sound decoder.
        SoundDecoder*   decoder( void ) const;

        //! Returns an amount of PCM chunks. (This property is greater than 1 for streamed sound buffers).
        u32             chunks( void ) const;

        //! Returns the total PCM data size.
        u64             size( void ) const;

    protected:

        //! Attached sound decoder.
        SoundDecoder*   m_decoder;

        //! Total PCM data size.
        u64             m_size;

        //! Number of PCM chunks.
        u32             m_chunks;
    };

} // namespace sound
    
DC_END_DREEMCHEST

#endif  /*    __DC_SoundBuffer_H__    */