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
#include	"FileStream.h"
#include    "Archive.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** DiskFileSystem::DiskFileSystem
DiskFileSystem::DiskFileSystem( void )
{
	addPath( "", true );
	m_packages.clear();
}

DiskFileSystem::~DiskFileSystem( void )
{
	for( ArchiveList::iterator i = m_packages.begin(), end = m_packages.end(); i != end; i++ ) {
		(*i)->release();
	}
}

// ** DiskFileSystem::openFile
Stream* DiskFileSystem::openFile( const char *fileName, const char *mode ) const
{
	char		path[MaxPathLength + 1];

	if( strncmp( fileName, m_baseDir.c_str(), m_baseDir.length() ) != 0 ) {
    #ifdef DC_PLATFORM_IOS
        if( fileName[0] != '/' ) {
            _snprintf( path, MaxPathLength, "%s%s", m_baseDir.c_str(), fileName );
        } else {
            _snprintf( path, MaxPathLength, "%s", fileName );
        }
    #else
        _snprintf( path, MaxPathLength, "%s%s", m_baseDir.c_str(), fileName );
    #endif
	} else {
		_snprintf( path, MaxPathLength, "%s", fileName );
	}
    
#ifdef DC_PLATFORM_IOS
    if( mode[0] == 'w' ) {
    //    if( path[0] != '/' ) {
        if( fileName[0] != '/' ) {
            _snprintf( path, MaxPathLength, "%s/%s", m_ctx->iNative->GetDocumentsPath(), fileName );
        }
    }
#endif

    FileStream* file = DC_NEW FileStream( path, mode );
    if( !file->isValid() ) {
        delete file;
        return NULL;
    }

	return file;
}

// ** DiskFileSystem::openFile
Stream* DiskFileSystem::openFile( const char *fileName ) const
{
/*
#ifndef embeddedAssetsNotFound
	if( strcmp( fileName, "AssetsEmbedded" ) == 0 ) {
		return DC_NEW embeddedAssets;
	}
#endif

#ifndef embeddedDefaultNotFound
	if( strcmp( fileName, "em/embedded.font" ) == 0 ) {
		return DC_NEW embeddedDefault;
	}
#endif
  */
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

	return NULL;
}

// ** DiskFileSystem::fileExists
bool DiskFileSystem::fileExists( const char *fileName ) const
{
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

	return false;
}

// ** DiskFileSystem::fileExistsAtPath
bool DiskFileSystem::fileExistsAtPath( const char *fileName )
{
    FILE *file = fopen( fileName, "rb" );

    if( !file ) {
        return false;
    }
    
    fclose( file );
    
    return true;
}

// ** DiskFileSystem::openPackage
Archive* DiskFileSystem::openPackage( const char *fileName, const char *path ) const
{
	Stream* file = openFile( fileName );
	if( !file ) {
		return NULL;
	}

	// ** Open package
	Archive* package = DC_NEW Archive( this );
    if( !package->open( file ) ) {
        delete package;
        return NULL;
    }
    
	return package;
}

// ** DiskFileSystem::loadPackage
Archive* DiskFileSystem::loadPackage( const char *fileName )
{
	if( Archive* a = findPackage( fileName ) ) {
		return a;
	}

	if( Archive* a = openPackage( fileName ) ) {
		m_packages.push_back( a );
		return a;
	}

	return NULL;
}

// ** DiskFileSystem::unloadPackage
bool DiskFileSystem::unloadPackage( const char *fileName )
{
	for( ArchiveList::iterator i = m_packages.begin(), end = m_packages.end(); i != end; i++ ) {
		Archive* a = *i;

		if( strcmp( a->fileName(), fileName ) == 0 ) {
			a->release();
			m_packages.erase( i );
			return true;
		}
	}

	return false;
}

// ** DiskFileSystem::findPackage
Archive* DiskFileSystem::findPackage( const char *fileName )
{
	for( ArchiveList::iterator i = m_packages.begin(), end = m_packages.end(); i != end; i++ ) {
		Archive* a = *i;

		if( strcmp( a->fileName(), fileName ) == 0 ) {
			return a;
		}
	}

	return NULL;
}

// ** DiskFileSystem::addPath
void DiskFileSystem::addPath( const char *path, bool first )
{
	if( first ) {
		m_paths.insert( m_paths.begin(), path );
	} else {
		m_paths.push_back( path );
	}
}

// ** DiskFileSystem::removePath
void DiskFileSystem::removePath( const char *path )
{
	for( StringArray::iterator i = m_paths.begin(), end = m_paths.end(); i != end; i++ ) {
        if( path != *i ) {
            continue;
        }
        
        m_paths.erase( i );
        break;
	}
}

// ** DiskFileSystem::setBaseDir
void DiskFileSystem::setBaseDir( const char *value )
{
	DC_BREAK_IF( value == NULL );
    
    m_baseDir = value;
	if( m_baseDir.empty() ) {
		return;
	}

    if( m_baseDir[m_baseDir.length() - 1] != '/' ) {
        m_baseDir += '/';
    }
}

// ** DiskFileSystem::baseDir
CString DiskFileSystem::baseDir(void ) const {
	return m_baseDir.c_str();
}

} // namespace io

DC_END_DREEMCHEST
