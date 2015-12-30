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

// ------------------------------------------------------------ FileInfo ------------------------------------------------------------ //

// ** FileInfo::FileInfo
FileInfo::FileInfo( const String& path ) : QFileInfo( QString::fromStdString( path ) )
{

}

// ** FileInfo::FileInfo
FileInfo::FileInfo( const QFileInfo& fileInfo ) : QFileInfo( fileInfo )
{

}

// ** FileInfo::extension
String FileInfo::extension( void ) const
{
	return completeSuffix().toLower().toStdString();
}

// ** FileInfo::timestamp
u32 FileInfo::timestamp( void ) const
{
	return lastModified().toTime_t();
}

// ** FileInfo::fileName
String FileInfo::fileName( void ) const
{
	return baseName().toStdString();
}

// ** FileInfo::dir
String FileInfo::dir( void ) const
{
	return QFileInfo::absolutePath().toStdString();
}

// ** FileInfo::absolutePath
String FileInfo::absolutePath( void ) const
{
	return absoluteFilePath().toStdString();
}

// ** FileInfo::relativePath
String FileInfo::relativePath( const String& path ) const
{
	QDir root( QString::fromStdString( path ) );
	return root.relativeFilePath( absoluteFilePath() ).toStdString();
}

// ----------------------------------------------------------- FileSystem ----------------------------------------------------------- //

// ** FileSystem::FileSystem
FileSystem::FileSystem( QObject* parent ) : QObject( parent )
{

}

// ** FileSystem::documentsLocation
String FileSystem::documentsLocation( void ) const
{
#ifdef DC_QT4_ENABLED
	return QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ).toStdString();
#elif DC_QT5_ENABLED
	return QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ).at( 0 ).toStdString();
#endif	/*	DC_QT4_ENABLED	*/
}

// ** FileSystem::selectExistingDirectory
String FileSystem::selectExistingDirectory( const String& title, const String& dir ) const
{
	QString base   = dir == "" ? documentsLocation().c_str() : dir.c_str();
	QString result = QFileDialog::getExistingDirectory( qobject_cast<QWidget*>( parent() ), title.c_str(), base );
	return result.toStdString();
}

// ** FileSystem::selectExistingFiles
StringArray FileSystem::selectExistingFiles( const String& title, const String& ext, const String& dir ) const
{
	QString		empty;
	QString		base     = dir == "" ? documentsLocation().c_str() : dir.c_str();
	QStringList absolute = QFileDialog::getOpenFileNames( qobject_cast<QWidget*>( parent() ), title.c_str(), base, ext.c_str(), &empty );

	if( absolute.isEmpty() ) {
		return StringArray();
	}

	StringArray result;

	for( int i = 0; i < absolute.size(); i++ ) {
		result.push_back( absolute.at( i ).toStdString() );
	}

	return result;
}

// ** FileSystem::fileExists
bool FileSystem::fileExists( const String& path ) const
{
	bool result = QFile::exists( path.c_str() );
	return result;
}

// ** FileSystem::removeFile
bool FileSystem::removeFile( const String& path )
{
	if( !QFile::exists( path.c_str() ) ) {
		return true;
	}

	bool result = QFile::remove( path.c_str() );
	DC_BREAK_IF( !result );

	return result;
}

// ** FileSystem::extractFileInfo
FileInfo FileSystem::extractFileInfo( const String& path ) const
{
	// Construct the file info from path
	QFileInfo info( path.c_str() );

	// Fill the resulting struct
	FileInfo result( info );

	return result;
}

// ** FileSystem::copyFile
bool FileSystem::copyFile( const String& source, const String& dest, bool force )
{
	// Fail copy if the source file does not exist.
	if( !fileExists( source ) ) {
		return false;
	}

	// Create paths if the file creation is forces
	if( force ) {
		FileInfo info = extractFileInfo( dest );
		createDirectory( info.dir() );		
	}

	// Remove the file if it exists
	removeFile( dest );

	bool result = QFile::copy( source.c_str(), dest.c_str() );
	DC_BREAK_IF( !result );

	return result;
}

// ** FileSystem::createDirectory
bool FileSystem::createDirectory( const String& path )
{
	if( QDir( path.c_str() ).exists() ) {
		return true;
	}

	bool result = QDir().mkpath( path.c_str() );
	DC_BREAK_IF( result == false );

	return result;
}

// ** FileSystem::createFile
bool FileSystem::createFile( const String& path )
{
	if( fileExists( path ) ) {
		return false;
	}

	bool result = QFile( path.c_str() ).open( QIODevice::WriteOnly );
	DC_BREAK_IF( !result );

	return result;
}

// ** FileSystem::generateFileName
String FileSystem::generateFileName( const String& path, const String& name, const String& ext ) const
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

// ** FileSystem::browse
bool FileSystem::browse( const String& path )
{
	bool result = QDesktopServices::openUrl( QString( path.c_str() ) );
	DC_BREAK_IF( !result );
	return result;
}

DC_END_COMPOSER