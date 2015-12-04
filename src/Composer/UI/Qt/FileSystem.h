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

#ifndef __DC_Composer_Qt_FileSystem_H__
#define __DC_Composer_Qt_FileSystem_H__

#include "../IFileSystem.h"

namespace Ui {

	//! File system interface Qt implementation.
	class FileSystem : public IFileSystem {
	public:

								//! Constructs FileSystem instance.
								FileSystem( QWidget* parent );

		//! Returns the user's documents location.
		virtual String			documentsLocation( void ) const;

		//! Copies the file from one location to another.
		virtual bool			copyFile( const String& source, const String& dest, bool force = false );

		//! Returns true if the file exists.
		virtual bool			fileExists( const String& path ) const;

		//! Removes the existing file.
		virtual bool			removeFile( const String& path );

		//! Extracts the file info from path.
		virtual FileInfo		extractFileInfo( const String& path ) const;

		//! Selects an existing directory.
		virtual String			selectExistingDirectory( const String& title, const String& dir = "" ) const;

		//! Selects existing files.
		virtual StringArray		selectExistingFiles( const String& title, const String& ext = "*.*", const String& dir = "" ) const;

		//! Creates a new directory if it does not exists.
		virtual bool			createDirectory( const String& path );

		//! Creates an empty file at specified path.
		virtual bool			createFile( const String& path );

		//! Generates the file name that does not exist at specified path.
		virtual String			generateFileName( const String& path, const String& name, const String& ext ) const;

		//! Browses to a selected path.
		virtual bool			browse( const String& path );

		//! Converts the QFileInfo to Ui::FileInfo.
		static FileInfo			convertFileInfo( const QFileInfo& fileInfo );

	private:

		QWidget*				m_parent;	//!< The parent QWidget to be used.
	};

} // namespace Ui

#endif	/*	!__DC_Composer_Qt_FileSystem_H__	*/