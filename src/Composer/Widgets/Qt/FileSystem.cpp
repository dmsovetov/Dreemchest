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

#include "FileSystem.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ------------------------------------------------------------ FileInfoPrivate ------------------------------------------------------------ //

// ** FileInfoPrivate::FileInfoPrivate
FileInfoPrivate::FileInfoPrivate( const QFileInfo& fileInfo ) : m_fileInfo( fileInfo )
{

}

// ** FileInfoPrivate::extension
String FileInfoPrivate::extension( void ) const
{
	return m_fileInfo.completeSuffix().toLower().toStdString();
}

// ** FileInfoPrivate::timestamp
u32 FileInfoPrivate::timestamp( void ) const
{
	return m_fileInfo.lastModified().toTime_t();
}

// ** FileInfoPrivate::fileName
String FileInfoPrivate::fileName( void ) const
{
	return m_fileInfo.baseName().toStdString();
}

// ** FileInfoPrivate::dir
String FileInfoPrivate::dir( void ) const
{
	return m_fileInfo.absolutePath().toStdString();
}

// ** FileInfoPrivate::absolutePath
String FileInfoPrivate::absolutePath( void ) const
{
	return m_fileInfo.absoluteFilePath().toStdString();
}

// ** FileInfoPrivate::isDir
bool FileInfoPrivate::isDir( void ) const
{
	return m_fileInfo.isDir();
}

// ----------------------------------------------------------- FileSystemPrivate ----------------------------------------------------------- //

// ** FileSystemPrivate::FileSystemPrivate
FileSystemPrivate::FileSystemPrivate( QWidget* parent ) : m_parent( parent )
{

}

// ** FileSystemPrivate::documentsLocation
String FileSystemPrivate::documentsLocation( void ) const
{
#ifdef DC_QT4_ENABLED
	return QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ).toStdString();
#elif DC_QT5_ENABLED
	return QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ).at( 0 ).toStdString();
#endif	/*	DC_QT4_ENABLED	*/
}

// ** FileSystemPrivate::selectExistingDirectory
String FileSystemPrivate::selectExistingDirectory( const String& title, const String& dir ) const
{
	QString base   = dir == "" ? documentsLocation().c_str() : dir.c_str();
	QString result = QFileDialog::getExistingDirectory( m_parent, title.c_str(), base );
	return result.toStdString();
}

// ** FileSystemPrivate::selectExistingFiles
StringArray FileSystemPrivate::selectExistingFiles( const String& title, const String& ext, const String& dir ) const
{
	QString		empty;
	QString		base     = dir == "" ? documentsLocation().c_str() : dir.c_str();
	QStringList absolute = QFileDialog::getOpenFileNames( m_parent, title.c_str(), base, ext.c_str(), &empty/*, QFileDialog::DontUseNativeDialog*/ );

	if( absolute.isEmpty() ) {
		return StringArray();
	}

	StringArray result;

	for( int i = 0; i < absolute.size(); i++ ) {
		result.push_back( absolute.at( i ).toStdString() );
	}

	return result;
}

// ** FileSystemPrivate::fileExists
bool FileSystemPrivate::fileExists( const String& path ) const
{
	bool result = QFile::exists( path.c_str() );
	return result;
}

// ** FileSystemPrivate::removeFile
bool FileSystemPrivate::removeFile( const String& path )
{
	if( !QFile::exists( path.c_str() ) ) {
		return true;
	}

	bool result = QFile::remove( path.c_str() );
	DC_BREAK_IF( !result );

	return result;
}

//! Extracts the file info from path.
FileInfoPtr FileSystemPrivate::extractFileInfo( const String& path ) const
{
	// Construct the file info from path
	QFileInfo info( path.c_str() );

	// Fill the resulting struct
	FileInfoPtr result = convertFileInfo( info );

	return result;
}

// ** FileSystemPrivate::convertFileInfo
FileInfoPtr FileSystemPrivate::convertFileInfo( const QFileInfo& fileInfo )
{
	FileInfoPtr result( new FileInfoPrivate( fileInfo ) );
	return result;
}

// ** FileSystemPrivate::copyFile
bool FileSystemPrivate::copyFile( const String& source, const String& dest, bool force )
{
	// Fail copy if the source file does not exist.
	if( !fileExists( source ) ) {
		return false;
	}

	// Create paths if the file creation is forces
	if( force ) {
		FileInfoPtr info = extractFileInfo( dest );
		createDirectory( info->dir() );		
	}

	// Remove the file if it exists
	removeFile( dest );

	bool result = QFile::copy( source.c_str(), dest.c_str() );
	DC_BREAK_IF( !result );

	return result;
}

// ** FileSystemPrivate::createDirectory
bool FileSystemPrivate::createDirectory( const String& path )
{
	if( QDir( path.c_str() ).exists() ) {
		return true;
	}

	bool result = QDir().mkpath( path.c_str() );
	DC_BREAK_IF( result == false );

	return result;
}

// ** FileSystemPrivate::createFile
bool FileSystemPrivate::createFile( const String& path )
{
	if( fileExists( path ) ) {
		return false;
	}

	bool result = QFile( path.c_str() ).open( QIODevice::WriteOnly );
	DC_BREAK_IF( !result );

	return result;
}

// ** FileSystemPrivate::generateFileName
String FileSystemPrivate::generateFileName( const String& path, const String& name, const String& ext ) const
{
	QDir dir( path.c_str() );

	for( int i = 0; i < 32; i++ ) {
		String id		 = i != 0 ? toString( i ) : "";
		String assetName = name + id;

		if( ext != "" ) {
			assetName += "." + ext;
		}

		if( !dir.exists( assetName.c_str() ) ) {
			return assetName;
		}
	}

	return "";
}

// ** FileSystemPrivate::browse
bool FileSystemPrivate::browse( const String& path )
{
	bool result = QDesktopServices::openUrl( QString( path.c_str() ) );
	DC_BREAK_IF( !result );
	return result;
}

} // namespace Ui

DC_END_COMPOSER