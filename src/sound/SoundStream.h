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

#ifndef __DC__SoundStream_H__
#define __DC__SoundStream_H__

#include "Sound.h"

namespace dreemchest {

namespace sound {

    // ** class ISoundStream
    class ISoundStream {
    public:

        // ** SeekOrigin
        enum SeekOrigin {
            SeekSet,
            SeekCur,
            SeekEnd
        };

        virtual                     ~ISoundStream( void ) {}

        virtual u32                 length( void ) const                                        = 0;
        virtual void                release( void )                                             = 0;
        virtual u32                 read( void* buffer, u32 size )                              = 0;
        virtual void                setPosition( u32 offset, SeekOrigin origin = SeekSet )      = 0;
        virtual u32                 position( void ) const                                      = 0;
        virtual ISoundStream*       loadToRam( void ) const                                     = 0;
    };

    // ** class IStreamOpener
    class IStreamOpener {
    public:

        virtual                 ~IStreamOpener( void ) {}

        //! Opens a new sound stream.
        /*!
         \param uri Stream URI.
         \return ISoundStream object.
         */
        virtual ISoundStream*   open( const char* uri ) = 0;
    };

    // ** class StandardStreamOpener
    class StandardStreamOpener : public IStreamOpener {
    public:
        
        
        // ** IStreamOpener
        virtual ISoundStream*   open( const char* uri );
    };

    // ** class StandardSoundStream
    class StandardSoundStream : public ISoundStream {
    public:

        virtual                     ~StandardSoundStream( void );

        // ** ISoundStream
        virtual u32                 length( void ) const;
        virtual void                release( void );
        virtual u32                 read( void* buffer, u32 size );
        virtual void                setPosition( u32 offset, SeekOrigin origin = SeekSet );
        virtual u32                 position( void ) const;
        virtual ISoundStream*       loadToRam( void ) const;

        // ** StandardSoundStream
        static ISoundStream*        open( const char* uri );

    private:

                                    StandardSoundStream( FILE* file );

    private:

        FILE*                       m_file;
        u32                         m_length;
    };

} // namespace sound

} // namespace dreemchest

#endif /*   !defined( __DC__SoundStream_H__ ) */
