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

#ifndef __DC_Io_DiskFileSystem_H__
#define __DC_Io_DiskFileSystem_H__

#include "FileSystem.h"
#include "Path.h"

DC_BEGIN_DREEMCHEST

namespace Io {

	//! DiskFileSystem class is an interface to work with physical file system.
	class dcInterface DiskFileSystem : public FileSystem {
	public:

                                //! Constructs DiskFileSystem instance.
                                DiskFileSystem( void );

		virtual					~DiskFileSystem( void );

		//! Opens a file at path for reading.
		virtual StreamPtr       openFile( const Path& path ) const DC_DECL_OVERRIDE;

        //! Opens a file at path.
		virtual StreamPtr       openFile( const Path& path, StreamMode mode ) const DC_DECL_OVERRIDE;

        //! Checks if file exists at path.
		virtual bool			fileExists( const Path& path ) const DC_DECL_OVERRIDE;

		//! Sets a base directory.
        /*!
         Path to a base directory is prepended to each file path that is tried to be opened by file system.
         */
		//void					setBaseDir( const Path& path );

        //! Returns a base directory path.
		//const Path&             baseDir( void ) const;

        //! Registers a file search path.
        /*!
         \param path Directory path.
         */
		//void					addPath( const Path& path );

        //! Removes a file search path.
		//void					removePath( const Path& path );

        //! Opens an archive file.
        /*!
         \param fileName Archive file name to be opened.
         \return Archive instance if source file found and can be opened for reading.
         */
		ArchivePtr				openPackage( const Path& fileName ) const;

        //! Adds a new archive file.
        /*!
         Loads an archive from file and adds it to a list of file archives.
         Each time the DiskFileSystem::openFile method is called and there is no
         file found a search in this list will be preformed.

         \param fileName Archive file name.
         \return Archive instance if source file found and can be opened for reading.
         */
		ArchivePtr				loadPackage( const Path& fileName );

        //! Removes a previously loaded file archive.
		bool					unloadPackage( const Path& fileName );

        //! Searches for a loaded file archive by it's file name.
		ArchivePtr				findPackage( const Path& fileName );

        //! Returns true if file exists at path.
        static bool				fileExistsAtPath( const Path& fileName );

		//! Reads the text file content.
		static String			readTextFile( const Path& fileName );

		//! Opens the file for reading.
		static StreamPtr		open( const Path& fileName, StreamMode mode = BinaryReadStream );

	protected:

        //! List of loaded file archives.
		Archives                m_archives;

        //! Set of file search paths.
		PathSet                 m_paths;

        //! Base file dir.
        Path                    m_baseDir;
	};

} // namespace Io

DC_END_DREEMCHEST

#endif	/*	!__DC_Io_DiskFileSystem_H__	*/