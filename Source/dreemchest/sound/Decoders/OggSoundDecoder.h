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

#ifndef __DC_OggSoundDecoder_H__
#define __DC_OggSoundDecoder_H__

#include "SoundDecoder.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

DC_BEGIN_DREEMCHEST

namespace Sound {

    // ** class cOggSoundDecoder
    class OggSoundDecoder : public SoundDecoder {
    public:

                                OggSoundDecoder( void )
                                    : m_vorbisFile( NULL ) {}
        virtual                 ~OggSoundDecoder( void );

        // ** SoundDecoder
        virtual bool            open( ISoundStreamPtr stream );
        virtual void            close( void );
        virtual u32             read( u8 *buffer, u32 size );
        virtual void            seek( u32 pos );
        virtual u32             size( void ) const;

    private:

        static size_t           readOgg( void *ptr, size_t size, size_t nmemb, void *source );
        static s32              seekOgg( void *source, ogg_int64_t offset, s32 origin );
        static long             tellOgg( void *source );
        static s32              closeOgg( void *source );

    private:

        OggVorbis_File*         m_vorbisFile;
        vorbis_comment*         m_vorbisComment;
        vorbis_info*            m_vorbisInfo;
    };

} // namespace Sound

DC_END_DREEMCHEST

#endif    /*    !__DC_OggSoundDecoder_H__    */