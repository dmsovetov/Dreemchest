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

#ifndef __DC_Io_HDDFile_H__
#define __DC_Io_HDDFile_H__

#include "Stream.h"
#include "../Path.h"

DC_BEGIN_DREEMCHEST

namespace Io
{
    //! A FileStream class is used for work with physical files.
    class FileStream : public Stream
    {
    friend class DiskFileSystem;
    public:

        virtual                 ~FileStream( void );

        //! Disposes this file stream.
        virtual void            close( void ) NIMBLE_OVERRIDE;

        //! Returns a total lenght of this file.
        virtual s32             length( void ) const NIMBLE_OVERRIDE;

        //! Returns current file position.
        virtual s32             position( void ) const NIMBLE_OVERRIDE;

        //! Sets the position inside the file.
        virtual void            setPosition( s32 offset, SeekOrigin origin = SeekSet ) NIMBLE_OVERRIDE;

        //! Returns true if there are any data left.
        virtual bool            hasDataLeft( void ) const NIMBLE_OVERRIDE;

        //! Reads data from file.
        virtual s32             read( void* buffer, s32 size ) const NIMBLE_OVERRIDE;

        //! Writes data to file.
        virtual s32             write( const void* buffer, s32 size ) NIMBLE_OVERRIDE;

        //! Returns a file name of this file.
        const Path&             fileName( void ) const;

    private:

                                //! Constructs a file stream.
                                FileStream( void );

        //! Opens a file.
        bool                    open( const Path& fileName, StreamMode mode );

    private:

        //! File handle.
        FILE*                   m_file;

        //! Total file length in bytes.
        s32                     m_length;

        //! File name.
        Path                    m_fileName;

        //! Total opened files counter, used for debugging.
        static u32              s_openFileCount;
    };

} // namespace Io

DC_END_DREEMCHEST

#endif        /*    !__File_H__    */
