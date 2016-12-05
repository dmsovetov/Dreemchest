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

#ifndef __DC_Io_ByteBuffer_H__
#define __DC_Io_ByteBuffer_H__

#include "Stream.h"

DC_BEGIN_DREEMCHEST

namespace Io {

	//! ByteBuffer class represents a stream located in RAM.
	class dcInterface ByteBuffer : public Stream {
    friend class FileSystem;

        DC_DECLARE_IS( ByteBuffer, ByteBuffer, this )

	public:

		virtual					~ByteBuffer( void );

        //! Returns a length of this stream.
        virtual s32             length( void ) const NIMBLE_OVERRIDE;

        //! Returns current memory offset in bytes.
        virtual s32             position( void ) const NIMBLE_OVERRIDE;

        //! Sets an offset inside this byte buffer.
        virtual void            setPosition( s32 offset, SeekOrigin origin = SeekSet ) NIMBLE_OVERRIDE;

        //! Reads data from stream.
        virtual s32             read( void* buffer, s32 size ) const NIMBLE_OVERRIDE;

        //! Writes data from stream.
        virtual s32             write( const void* buffer, s32 size ) NIMBLE_OVERRIDE;

        //! Fills this memory with a given value
        void                    fill( u8 value );

        //! Returns a total number of bytes available starting from current stream position.
        s32                     bytesAvailable( void ) const;

		//! Returns an array of bytes.
		const Array<u8>&		array( void ) const;

        //! Returns a data pointer.
        const u8*               buffer( void ) const;

        //! Returns a data pointer to current stream position.
        const u8*               current( void ) const;

		//! Constructs the copy of a byte array.
		ByteBufferPtr			copy( void ) const;

        //! Trims a specified amount of bytes from the beginning of a stream.
        void                    trimFromLeft( s32 size );

        //! Trims a specified amount of bytes from the end of a stream.
        void                    trimFromRight( s32 size );

        //! Creates an empty byte buffer.
        static ByteBufferPtr    create( s32 size = 0 );

        //! Creates a byte buffer from data.
        static ByteBufferPtr    createFromData( const u8* pointer, s32 size );

		//! Creates a byte buffer from an array of bytes.
		static ByteBufferPtr	createFromArray( const Array<u8>& data );

    protected:

                                //! Constructs a memory stream from data.
                                ByteBuffer( const u8* pointer, s32 size );

	private:

        //! Current stream position.
        mutable s32             m_position;

        //! Stream buffer.
        Array<u8>               m_buffer;
	};

} // namespace Io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_ByteBuffer_H__	*/
