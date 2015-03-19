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

#ifndef		__DC_Io_ByteBuffer_H__
#define		__DC_Io_ByteBuffer_H__

#include	"Stream.h"

DC_BEGIN_DREEMCHEST

namespace io {

	//! ByteBuffer class represents a stream located in RAM.
	class dcInterface ByteBuffer : public Stream {
    friend class FileSystem;

        DC_DECLARE_IS( ByteBuffer, ByteBuffer, this )

	public:

		virtual					~ByteBuffer( void );

        //! Returns a length of this stream.
        virtual u64             length( void ) const;

        //! Returns current memory offset in bytes.
        virtual u64             position( void ) const;

        //! Sets an offset inside this byte buffer.
        virtual void            setPosition( u64 offset, SeekOrigin origin = SeekSet );

        //! Reads data from stream.
        virtual u64             read( void* buffer, u64 size ) const;

        //! Writes data from stream.
        virtual u64             write( const void* buffer, u64 size );

        //! Fills this memory with a given value
        void                    fill( u8 value );

        //! Returns a total number of bytes available starting from current stream position.
        u64                     bytesAvailable( void ) const;

        //! Returns a data pointer.
        const u8*               buffer( void ) const;

        //! Returns a data pointer to current stream position.
        const u8*               current( void ) const;

        //! Trims a specified amount of bytes from the beginning of a stream.
        void                    trimFromLeft( u64 size );

        //! Trims a specified amount of bytes from the end of a stream.
        void                    trimFromRight( u64 size );

        //! Creates an empty byte buffer.
        static ByteBufferPtr    create( u64 size = 0 );

        //! Creates a byte buffer from data.
        static ByteBufferPtr    createWithData( const u8* pointer, u64 size );

    protected:

                                //! Constructs a memory stream from data.
                                ByteBuffer( const u8* pointer, u64 size );

	private:

        //! Current stream position.
        mutable u64             m_position;

        //! Stream buffer.
        Array<u8>               m_buffer;
	};

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_ByteBuffer_H__	*/