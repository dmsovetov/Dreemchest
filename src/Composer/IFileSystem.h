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

#ifndef __DC_Composer_IFileSystem_H__
#define __DC_Composer_IFileSystem_H__

#include "Composer.h"

DC_BEGIN_COMPOSER

	//! File info interface.
	class FileInfo : public RefCounted {
	public:

		virtual					~FileInfo( void ) {}

		//! Returns the file extension.
		virtual String			extension( void ) const = 0;

		//! Returns the file timestamp.
		virtual u32				timestamp( void ) const = 0;

		//! Returns the file name.
		virtual String			fileName( void ) const = 0;

		//! Returns absolute directory path of a file.
		virtual String			dir( void ) const = 0;

		//! Returns absolute file path.
		virtual String			absolutePath( void ) const = 0;

		//! Returns relative file name to a specified path.
		virtual String			relativePath( const String& path ) const = 0;

		//! Returns true if this is a directory.
		virtual bool			isDir( void ) const = 0;
	};

	//! File system interface.
	class FileSystem : public RefCounted {
	public:

		virtual					~FileSystem( void ) {}

		//! Returns the user's documents location.
		virtual String			documentsLocation( void ) const = 0;
		
		//! Copies the file from one location to another.
		virtual bool			copyFile( const String& source, const String& dest, bool force = false ) = 0;

		//! Removes the existing file.
		virtual bool			removeFile( const String& path ) = 0;

		//! Returns true if the file exists.
		virtual bool			fileExists( const String& path ) const = 0;

		//! Extracts the file info from path.
		virtual FileInfoPtr		extractFileInfo( const String& path ) const = 0;

		//! Selects an existing directory.
		virtual String			selectExistingDirectory( const String& title, const String& dir = "" ) const = 0;

		//! Selects existing files.
		virtual StringArray		selectExistingFiles( const String& title, const String& ext = "*.*", const String& dir = "" ) const = 0;

		//! Creates a new directory if it does not exists.
		virtual bool			createDirectory( const String& path ) = 0;

		//! Creates an empty file at specified path.
		virtual bool			createFile( const String& path ) = 0;

		//! Generates the file name that does not exist at specified path.
		virtual String			generateFileName( const String& path, const String& name, const String& ext ) const = 0;

		//! Browses to a selected path.
		virtual bool			browse( const String& path ) = 0;
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_IFileSystem_H__	*/