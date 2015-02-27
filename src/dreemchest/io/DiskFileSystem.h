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

#ifndef		__DC_Io_DiskFileSystem_H__
#define		__DC_Io_DiskFileSystem_H__

#include	"FileSystem.h"

DC_BEGIN_DREEMCHEST

namespace io {

	// ** class DiskFileSystem
	class dcInterface DiskFileSystem : public FileSystem {
	public:

								DiskFileSystem( void );
		virtual					~DiskFileSystem( void );

		// ** FileSystem
		virtual Stream*		openFile( const char *fileName ) const;
		virtual Stream*		openFile( const char *fileName, const char *mode ) const;
		virtual bool			fileExists( const char *fileName ) const;

		// ** DiskFileSystem
		void					setBaseDir( const char *value );
		CString				baseDir( void ) const;
		void					addPath( const char *path, bool first = false );
		void					removePath( const char *path );
		Archive*				openPackage( const char *fileName, const char *path = NULL ) const;
		Archive*				loadPackage( const char *fileName );
		bool					unloadPackage( const char *fileName );
		Archive*				findPackage( const char *fileName );

        static bool				fileExistsAtPath( const char *fileName );

	protected:

        enum { MaxPathLength = 256 };

		ArchiveList             m_packages;
		StringArray             m_paths;
        String             m_baseDir;
	};

} // namespace io

DC_END_DREEMCHEST

#endif	/*	!__DC_Io_DiskFileSystem_H__	*/