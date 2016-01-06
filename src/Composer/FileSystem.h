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

#ifndef __DC_Composer_FileSystem_H__
#define __DC_Composer_FileSystem_H__

#include "Composer.h"

DC_BEGIN_COMPOSER

	//! Qt File info extension
	class FileInfo : public QFileInfo {
	public:

                                //! Constructs FileInfo instance from file path.
                                FileInfo( const String& path );

								//! Constructs FileInfo instance.
								FileInfo( const QFileInfo& fileInfo = QFileInfo() );

		//! Returns the file extension.
		String			        extension( void ) const;

		//! Returns the file timestamp.
		u32				        timestamp( void ) const;

		//! Returns the file name.
		String			        fileName( void ) const;

		//! Returns absolute directory path of a file.
		String			        dir( void ) const;

		//! Returns absolute file path.
		String			        absolutePath( void ) const;

		//! Returns relative file name to a specified path.
		String			        relativePath( const String& path ) const;
	};

	//! File system interface based on Qt file methods.
	class FileSystem : public QObject {
	public:

								//! Constructs FileSystem instance.
								FileSystem( QObject* parent );

		//! Returns the user's documents location.
		String			        documentsLocation( void ) const;

		//! Copies the file from one location to another.
		bool			        copyFile( const String& source, const String& dest, bool force = false );

		//! Returns true if the file exists.
		bool			        fileExists( const String& path ) const;

		//! Removes the existing file.
		bool			        removeFile( const String& path );

		//! Extracts the file info from path.
		FileInfo		        extractFileInfo( const String& path ) const;

		//! Selects an existing directory.
		String			        selectExistingDirectory( const String& title, const String& dir = "" ) const;

		//! Selects existing files.
		StringArray		        selectExistingFiles( const String& title, const String& ext = "*.*", const String& dir = "" ) const;

		//! Creates a new directory if it does not exists.
		bool			        createDirectory( const String& path );

		//! Creates an empty file at specified path.
		bool			        createFile( const String& path );

        //! Writes string to a text file.
        bool                    writeTextFile( const QString& path, const QString& data );

		//! Generates the file name that does not exist at specified path.
		String			        generateFileName( const String& path, const String& name, const String& ext ) const;

		//! Browses to a selected path.
		bool			        browse( const String& path );
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_FileSystem_H__	*/