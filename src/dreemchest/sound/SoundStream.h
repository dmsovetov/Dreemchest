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

    //! Input stream interface used to read sound files.
    class ISoundStream : public RefCounted {
    public:

        //! Seek file origin.
        enum SeekOrigin {
            SeekSet,    //!< Seek from the begining of a stream.
            SeekCur,    //!< Seek from the current position.
            SeekEnd     //!< Seek from the end of a stream.
        };

        virtual                     ~ISoundStream( void ) {}

        //! Returns a length of input stream in bytes.
        virtual u32                 length( void ) const                                        = 0;

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
        virtual ISoundStreamPtr		loadToRam( void ) const                                     = 0;
    };

    //! Interface used to open input streams.
    class IStreamOpener : public RefCounted {
    public:

        virtual                 ~IStreamOpener( void ) {}

        //! Opens a new sound stream.
        /*!
         \param uri Stream URI.
         \return ISoundStream object.
         */
        virtual ISoundStreamPtr	open( CString uri ) = 0;
    };

    //! Standard file opener.
    class StandardStreamOpener : public IStreamOpener {
    public:

        // ** IStreamOpener
        virtual ISoundStreamPtr	open( CString uri );
    };

    //! Standard input stream.
    class StandardSoundStream : public ISoundStream {
    public:

        // ** ISoundStream
        virtual u32                 length( void ) const;
        virtual u32                 read( void* buffer, u32 size );
        virtual void                setPosition( u32 offset, SeekOrigin origin = SeekSet );
        virtual u32                 position( void ) const;
        virtual ISoundStreamPtr		loadToRam( void ) const;

        // ** StandardSoundStream
        static ISoundStreamPtr		open( CString uri );

    private:

                                    StandardSoundStream( io::StreamPtr stream );

    private:

        io::StreamPtr				m_stream;	//!< Input stream.
    };

	//! Memory input stream.
	class MemorySoundStream : public ISoundStream {
    public:

									MemorySoundStream( io::ByteBufferPtr data );
        virtual                     ~MemorySoundStream( void );

        // ** ISoundStream
        virtual u32                 length( void ) const;
        virtual u32                 read( void* buffer, u32 size );
        virtual void                setPosition( u32 offset, SeekOrigin origin = SeekSet );
        virtual u32                 position( void ) const;
        virtual ISoundStreamPtr		loadToRam( void ) const;

    private:

        io::ByteBufferPtr			m_data;		//!< Data buffer.
	};

} // namespace Sound

DC_END_DREEMCHEST

#endif /*   !defined( __DC__SoundStream_H__ ) */
