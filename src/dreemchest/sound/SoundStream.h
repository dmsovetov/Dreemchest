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

DC_BEGIN_DREEMCHEST

namespace Sound {

    // ** class ISoundStream
    //! Input stream interface used to read sound files.
    class ISoundStream {
    public:

        // ** SeekOrigin
        //! Seek file origin.
        enum SeekOrigin {
            SeekSet,    //!< Seek from the begining of a stream.
            SeekCur,    //!< Seek from the current position.
            SeekEnd     //!< Seek from the end of a stream.
        };

        virtual                     ~ISoundStream( void ) {}

        //! Returns a length of input stream in bytes.
        virtual u32                 length( void ) const                                        = 0;

        //! Releases this input stream.
        virtual void                release( void )                                             = 0;

        //! Reads data from input stream to a buffer.
        /*!
         \param buffer Destination buffer.
         \param size Number of bytes to read from input stream.
         \return Amount of bytes read.
         */
        virtual u32                 read( void* buffer, u32 size )                              = 0;

        //! Sets a stream read position.
        /*!
         \param offset Offset in bytes.
         \param origin Offset origin.
         */
        virtual void                setPosition( u32 offset, SeekOrigin origin = SeekSet )      = 0;

        //! Returns a current stream position.
        virtual u32                 position( void ) const                                      = 0;

        //! Reads all stream content to memory and returns a memory input stream.
        virtual ISoundStream*       loadToRam( void ) const                                     = 0;
    };

    // ** class IStreamOpener
    //! Interface used to open input streams.
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
    //! Standard file opener.
    class StandardStreamOpener : public IStreamOpener {
    public:
        
        
        // ** IStreamOpener
        virtual ISoundStream*   open( const char* uri );
    };

    // ** class StandardSoundStream
    //! Standard input stream.
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

} // namespace Sound

DC_END_DREEMCHEST

#endif /*   !defined( __DC__SoundStream_H__ ) */
