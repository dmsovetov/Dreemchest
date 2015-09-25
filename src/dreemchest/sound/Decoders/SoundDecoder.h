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

#ifndef __DC_SoundDecoder_H__
#define __DC_SoundDecoder_H__

#include    "../Sound.h"

DC_BEGIN_DREEMCHEST

namespace sound {

    // ** class SoundDecoder
    //! SoundDecoder class is used for reading PCM samples from input stream.
    class SoundDecoder {
    public:

                                    SoundDecoder( void );
        virtual                     ~SoundDecoder( void );

        //! Initializes sound decoder with a given input stream.
        virtual bool                open( ISoundStream* stream );

        //! Closes sound decoder and releases an input stream.
        virtual void                close( void );

        //! Reads PCM samles from an input stream.
        /*!
         \param buffer Destination buffer.
         \param size The amount of bytes to read.
         \return Amount of bytes read.
         */
        virtual u32                 read( u8* buffer, u32 size );

        //! Seeks for the PCM data position.
        virtual void                seek( u32 pos );

        //! Returns a sample rate.
        virtual u32                 rate( void ) const;

        //! Returns a sound sample format.
        virtual SoundSampleFormat   format( void ) const;

        //! Returns a total size of PCM data.
        virtual u32                 size( void ) const;

    protected:

        //! Attached sound stream.
        ISoundStream*               m_stream;

        //! Sample rate.
        u32                         m_rate;

        //! Sound sample format.
        SoundSampleFormat           m_format;
    };

} // namespace sound

DC_END_DREEMCHEST

#endif    /*    !__DC_SoundDecoder_H__    */