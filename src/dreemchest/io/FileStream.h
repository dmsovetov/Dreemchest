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

#ifndef		__DC_Io_HDDFile_H__
#define		__DC_Io_HDDFile_H__

#include	"Stream.h"

DC_BEGIN_DREEMCHEST

namespace io {
        
	// ** class FileStream
	class FileStream : public Stream {

        DC_DECLARE_IS( FileStream, FileStream, this );

	public:

								FileStream( const char *fileName, const char *mode );
		virtual					~FileStream( void );

        // ** Stream
        virtual long            length( void ) const;
        virtual long            position( void ) const;
        virtual void            setPosition( long offset, eSeekOrigin origin = SeekSet );
        virtual bool            hasDataLeft( void ) const;
        virtual long            read( void *buffer, long size );
        virtual long            write( const void *buffer, long size );
		virtual long            writeWithFormat( const char *format, ... );

        // ** FileStream
        CString             fileName( void ) const;
        bool                    isValid( void ) const;

    private:

		FILE*                   m_file;
		u64                     m_length;
        String             m_fileName;

        static u32              s_openFileCount;
	};

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__File_H__	*/