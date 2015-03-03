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

#ifndef		__DC_Io_Stream_H__
#define		__DC_Io_Stream_H__

#include	"../Io.h"

DC_BEGIN_DREEMCHEST

namespace io {

    //! Available seeking origins.
	enum SeekOrigin {
		SeekSet = 0,    //!< Seek from the begining of a stream.
		SeekCur,        //!< Seek from a current position inside a stream.
		SeekEnd         //!< Seek from the end of a stream.
	};

	//! Basic stream class.
    class dcInterface Stream : public RefCounted {

        DC_DECLARE_IS( FileStream, FileStream, NULL );
        DC_DECLARE_IS( ByteBuffer, ByteBuffer, NULL );

	public:

		virtual					~Stream( void );

        //! Disposes this stream.
        virtual void            dispose( void );

        //! Returns a length in bytes of this stream.
        virtual u64             length( void ) const;

        //! Returns a current position inside the stream.
        virtual u64             position( void ) const;

        //! Sets the position inside the stream.
        virtual void            setPosition( u64 offset, SeekOrigin origin = SeekSet );

        //! Returns true if there are any data left in stream.
        virtual bool            hasDataLeft( void ) const;

        //! Reads a string from stream.
        virtual u64             readString( String& str ) const;

        //! Writes a string to a stream.
        virtual u64             writeString( const char* str );

        //! Reads data from stream.
        /*!
         \param buffer A destination buffer to read the data.
         \param size Number of bytes to read from stream.
         \return Number of bytes read from stream.
         */
        virtual u64             read( void* buffer, u64 size ) const;

        //! Writes data to a stream.
        /*!
         \param buffer A source buffer to write data from.
         \param size Number of bytes to write to stream.
         \return Number of bytes written to stream.
         */
        virtual u64             write( const void* buffer, u64 size );

    protected:

                                //! Constructs a stream instance
                                Stream( void );
	};

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Stream_H__	*/