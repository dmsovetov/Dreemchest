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

#ifndef __DC_Io_FileSystem_H__
#define __DC_Io_FileSystem_H__

#include "Io.h"

DC_BEGIN_DREEMCHEST

namespace Io {

    //! FileSystem class is a base for all other file systems, like disk file system or archive.
    class dcInterface FileSystem : public RefCounted {
    public:

        virtual                    ~FileSystem( void );

        //! Creates a byte buffer stream from an in-memory data.
        /*!
         \param pointer A source memory buffer that will be copied to a created byte buffer.
         \param size Byte buffer size.
         \return ByteBuffer instance.
         */
        ByteBufferPtr           createByteBuffer( u8* pointer, u32 size ) const;

        //! Opens a file with a given file name for reading.
        /*!
         \param path File path.
         \return Instance of stream if file exists, otherwise false.
         */
        virtual StreamPtr       openFile( const Path& path ) const;

        //! Opens a file with a given file name an mode.
        /*!
         \param path File path.
         \param mode File open mode.
         \return Instance of stream if file exists, otherwise false.
         */
        virtual StreamPtr       openFile( const Path& path, StreamMode mode ) const;

        //! Returns true if file at a given path exists.
        /*!
         \param path File path to check.
         \return True if file exists at path.
         */
        virtual bool            fileExists( const Path& path ) const;

    protected:

                                //! Constructs file system instance.
                                FileSystem( void );
    };

} // namespace Io

DC_END_DREEMCHEST

#endif        /*    !__DC_Io_FileSystem_H__    */
