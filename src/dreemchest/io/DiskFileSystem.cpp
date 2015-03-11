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

#include	"DiskFileSystem.h"
#include	"streams/FileStream.h"
#include    "Archive.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** DiskFileSystem::DiskFileSystem
DiskFileSystem::DiskFileSystem( void )
{
}

DiskFileSystem::~DiskFileSystem( void )
{

}

// ** DiskFileSystem::openFile
StreamPtr DiskFileSystem::openFile( const Path& fileName, StreamMode mode ) const
{
    FileStreamPtr file = DC_NEW FileStream;

    if( !file->open( fileName, mode ) ) {
        return StreamPtr();
    }

	return file;
}

// ** DiskFileSystem::openFile
StreamPtr DiskFileSystem::openFile( const Path& fileName ) const
{
/*
#ifdef DC_PLATFORM_FLASH
    inline_as3(
               "import com.adobe.flascc.CModule                                 \n"
               ""
               "CModule.rootSprite.addFile( CModule.readString( %0, %1 ) );     \n"
               : : "r"( fileName ), "r"( strlen( fileName ) )
    );
#endif

	char path[MaxPathLength + 1];

	for( int i = 0; i < ( int )m_paths.size(); i++ ) {
    #if defined( DC_PLATFORM_OSX ) || defined( DC_PLATFORM_IOS )
        if( fileName[0] == '/' ) {
            _snprintf( path, MaxPathLength, "%s", fileName );
        } else {
            if( m_paths[i][0] != '/' ) {
                _snprintf( path, MaxPathLength, "%s%s%s", m_baseDir.c_str(), m_paths[i].c_str(), fileName );
            } else {
                _snprintf( path, MaxPathLength, "%s%s", m_paths[i].c_str(), fileName );
            }
        }
    #else
        _snprintf( path, MaxPathLength, "%s%s%s", m_baseDir.c_str(), m_paths[i].c_str(), fileName );
    #endif
		
		if( fileExistsAtPath( path ) ) {
			return openFile( path, "rb" );
		}
	}

	for( ArchiveList::const_iterator i = m_packages.begin(), end = m_packages.end(); i != end; i++ ) {
		if( (*i)->fileExists( fileName ) ) {
			return (*i)->openFile( fileName );
		}
	}
*/
	return NULL;
}

// ** DiskFileSystem::fileExists
bool DiskFileSystem::fileExists( const Path& fileName ) const
{
/*
	char path[MaxPathLength + 1];

	for( int i = 0; i < ( int )m_paths.size(); i++ ) {
		_snprintf( path, MaxPathLength, "%s%s%s", m_baseDir.c_str(), m_paths[i].c_str(), fileName );
		if( fileExistsAtPath( path ) ) {
			return true;
		}
	}

#ifdef DC_ZIP
	for( tPackageList::const_iterator i = packages.begin(), end = packages.end(); i != end; i++ ) {
		if( (*i)->fileExists( fileName ) ) {
			return true;
		}
	}
#endif
*/
	return false;
}

// ** DiskFileSystem::fileExistsAtPath
bool DiskFileSystem::fileExistsAtPath( const Path& fileName )
{
    FILE *file = fopen( fileName.c_str(), "rb" );

    if( !file ) {
        return false;
    }
    
    fclose( file );
    
    return true;
}

// ** DiskFileSystem::openPackage
ArchivePtr DiskFileSystem::openPackage( const Path& fileName ) const
{
	StreamPtr file = openFile( fileName );
	if( file == NULL ) {
		return ArchivePtr();
	}

	// ** Open package
	ArchivePtr package = DC_NEW Archive( this );
    if( !package->open( file ) ) {
        return ArchivePtr();
    }
    
	return package;
}

// ** DiskFileSystem::loadPackage
ArchivePtr DiskFileSystem::loadPackage( const Path& fileName )
{
    // ** First search for previously loaded package
    ArchivePtr alreadyLoaded = findPackage( fileName );
    if( alreadyLoaded != NULL ) {
        return alreadyLoaded;
    }

    // ** Open a package
    ArchivePtr archive = openPackage( fileName );
    if( archive != NULL ) {
        m_archives[fileName] = archive;
        return archive;
    }

	return NULL;
}

// ** DiskFileSystem::unloadPackage
bool DiskFileSystem::unloadPackage( const Path& fileName )
{
    Archives::iterator i = m_archives.find( fileName );

    if( i == m_archives.end() ) {
        return false;
    }

    m_archives.erase( i );
    return true;
}

// ** DiskFileSystem::findPackage
ArchivePtr DiskFileSystem::findPackage( const Path& fileName )
{
    Archives::iterator i = m_archives.find( fileName );
    return i != m_archives.end() ? i->second : NULL;
}

// ** DiskFileSystem::addPath
void DiskFileSystem::addPath( const Path& path )
{
    m_paths.insert( path );
}

// ** DiskFileSystem::removePath
void DiskFileSystem::removePath( const Path& path )
{
    PathSet::iterator i = m_paths.find( path );

    if( i != m_paths.end() ) {
        m_paths.erase( i );
    }
}

// ** DiskFileSystem::setBaseDir
void DiskFileSystem::setBaseDir( const Path& value )
{
    m_baseDir = value;
}

// ** DiskFileSystem::baseDir
const Path& DiskFileSystem::baseDir(void ) const
{
	return m_baseDir;
}

} // namespace io

DC_END_DREEMCHEST
