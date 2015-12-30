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

#include "AssetsModel.h"

#include "../FileSystem.h"

DC_BEGIN_COMPOSER

// ----------------------------------------------------------- AssetsModelFilter ----------------------------------------------------------- //

#if DEV_CUSTOM_ASSET_MODEL

// ** AssetsModelFilter::AssetsModelFilter
AssetsModelFilter::AssetsModelFilter( AssetsModel* parentAssetsModel, QObject* parent )
	: QLocalFileSystemModel( parent ), m_parent( parentAssetsModel ), m_metaFileExtension( "asset" )
{
	connect( this, SIGNAL(fileAdded(const QFileInfo&)), this, SLOT(fileAdded(const QFileInfo&)) );
	connect( this, SIGNAL(fileRemoved(const QFileInfo&)), this, SLOT(fileRemoved(const QFileInfo&)) );
	connect( this, SIGNAL(fileChanged(const QFileInfo&)), this, SLOT(fileChanged(const QFileInfo&)) );
	connect( this, SIGNAL(fileRenamed(const QFileInfo&, const QString&, const QString&)), this, SLOT(fileRenamed(const QFileInfo&, const QString&, const QString&)) );
	connect( this, SIGNAL(fileMoved(const QFileInfo&, const QString&, const QString&)), this, SLOT(fileMoved(const QFileInfo&, const QString&, const QString&)) );
}

// ** AssetsModelFilter::fileAdded
void AssetsModelFilter::fileAdded( const QFileInfo& file )
{
	// Construct file info
	FileInfo fileInfo( new Ui::FileInfoPrivate( file ) );

	// Dispatch an event
	m_parent->notify<AssetsModel::Added>( fileInfo );
}

// ** AssetsModelFilter::fileRemoved
void AssetsModelFilter::fileRemoved( const QFileInfo& file )
{
	qDebug() << file.absoluteFilePath();

	// Construct file info
	FileInfo fileInfo( new Ui::FileInfoPrivate( file ) );

	// Dispatch an event
	m_parent->notify<AssetsModel::Removed>( fileInfo );

	// Remove meta file
	bool result = QDir().remove( metaFileName( fileInfo ) );
	Q_ASSERT( result );
}

// ** AssetsModelFilter::fileChanged
void AssetsModelFilter::fileChanged( const QFileInfo& file )
{
	// Construct file info
	FileInfo fileInfo( new Ui::FileInfoPrivate( file ) );

	// Dispatch an event
	m_parent->notify<AssetsModel::Removed>( fileInfo );
}

// ** AssetsModelFilter::fileRenamed
void AssetsModelFilter::fileRenamed( const QFileInfo& file, const QString& oldFileName, const QString& newFileName )
{
	renameMetaFile( oldFileName, newFileName );
}

// ** AssetsModelFilter::fileMoved
void AssetsModelFilter::fileMoved( const QFileInfo& file, const QString& oldFileName, const QString& newFileName )
{
	renameMetaFile( oldFileName, newFileName );
}

// ** AssetsModelFilter::renameMetaFile
bool AssetsModelFilter::renameMetaFile( const QString& oldFileName, const QString& newFileName )
{
	// Construct the old meta file name
	QString oldMetaFile = metaFileNameFromPath( oldFileName );

	// Meta file does not exist, probably unsupported asset type
	if( !QFile::exists( oldMetaFile ) ) {
		return true;
	}

	// Construct the new meta file name
	QString newMetaFile = metaFileNameFromPath( newFileName );

	// Nothing changed - skip
	if( oldMetaFile == newMetaFile ) {
		return true;
	}

	// Rename file
	bool result = QDir().rename( oldMetaFile, newMetaFile );
	Q_ASSERT( result );
	return result;
}

// ** AssetsModelFilter::uuid
String AssetsModelFilter::uuid( const FileInfo& assetFile ) const
{
	// Read the meta data
	Io::KeyValue data = metaData( assetFile );
	DC_BREAK_IF( !data.isObject() );

	if( !data.isObject() ) {
		return "";
	}

	return data.get( "uuid", "" ).asString();
}

// ** AssetsModelFilter::assetFile
FileInfo AssetsModelFilter::assetFile( const QModelIndex& index ) const
{
	return FileInfo( new Ui::FileInfoPrivate( fileInfo( index ) ) );
}

// ** AssetsModelFilter::setMetaData
void AssetsModelFilter::setMetaData( const FileInfo& assetFile, const Io::KeyValue& data )
{
	// Write meta data to file
	QFile metaFile( metaFileName( assetFile ) );
	bool  result = metaFile.open( QFile::WriteOnly );
	Q_ASSERT( result );

	QTextStream stream( &metaFile );
	String json = Io::KeyValue::stringify( data );
	stream << json.c_str();

	metaFile.close();
}

// ** AssetsModelFilter::metaData
Io::KeyValue AssetsModelFilter::metaData( const FileInfo& assetFile ) const
{
	QFile metaFile( metaFileName( assetFile ) );

	if( !metaFile.open( QFile::ReadOnly ) ) {
		return Io::KeyValue::kNull;
	}

	QTextStream stream( &metaFile );
	QString json;
	stream >> json;

	metaFile.close();

	return Io::KeyValue::parse( json.toStdString() );
}

// ** AssetsModelFilter::metaFileName
QString AssetsModelFilter::metaFileName( const FileInfo& assetFile ) const
{
	return metaFileNameFromPath( QString::fromStdString( assetFile->absolutePath() ) );
	return QString::fromStdString( assetFile->absolutePath() ) + "." + m_metaFileExtension;
}

// ** AssetsModelFilter::metaFileNameFromPath
QString AssetsModelFilter::metaFileNameFromPath( const QString& fileName ) const
{
	return fileName + "." + m_metaFileExtension;
}

// ** AssetsModelFilter::shouldSkipFile
bool AssetsModelFilter::shouldSkipFile( const QFileInfo& file ) const
{
	return file.completeSuffix().endsWith( m_metaFileExtension );
}

// ** AssetsModelFilter::mimeTypes
QStringList AssetsModelFilter::mimeTypes( void ) const
{
	return QStringList() << Composer::kAssetMime.c_str();
}

#else

// ** AssetsModel::AssetsModel
AssetsModel::AssetsModel( QObject* parent ) : QFileSystemModel( parent ), m_isEditingModel( false ), m_metaFileExtension( "asset" )
{
	setReadOnly( false );

	connect( this, SIGNAL(directoryLoaded(const QString&)), this, SLOT(scanLoadedDirectory(const QString&)) );
	connect( this, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(assetsAdded(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(assetsRemoved(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(fileRenamed(const QString&, const QString&, const QString&)), this, SLOT(assetRenamed(const QString&, const QString&, const QString&)) );
}

// ** AssetsModel::dropMimeData
bool AssetsModel::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent )
{
	Q_ASSERT( action == Qt::MoveAction );

	// Move the file
	if( !QFileSystemModel::dropMimeData( data, action, row, column, parent ) ) {
		return false;
	}

	// Move meta file
	QString to = filePath( parent ) + QDir::separator();

	foreach( QUrl url, data->urls() ) {
		// Construct meta file names
		QString oldFileName = QDir::cleanPath( url.toLocalFile() );
		QString newFileName = QDir::cleanPath( to + QFileInfo( url.toLocalFile() ).fileName() );

		bool result = renameMetaFile( oldFileName, newFileName );
		Q_ASSERT( result );

		// Add this file to a moved set
		m_wasMoved << newFileName;
	}

	return true;
}

// ** AssetsModel::data
QVariant AssetsModel::data( const QModelIndex& index, int role ) const
{
	if( !index.isValid() ) {
		return QVariant();
	}

	switch( role ) {
	case Qt::DisplayRole:	return m_isEditingModel ? fileInfo( index ).fileName() : fileInfo( index ).baseName();
	case Qt::EditRole:		return fileInfo( index ).baseName();
	}

	return QFileSystemModel::data( index, role );
}

// ** AssetsModel::setData
bool AssetsModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
	if( index.isValid() && role == Qt::EditRole ) {
		m_isEditingModel = true;

		QFileInfo info		= fileInfo( index );
		QString	  suffix	= info.completeSuffix();
		QString	  fileName	= value.toString() + (suffix.isEmpty() ? "" : "." + suffix);
		bool	  result	= QFileSystemModel::setData( index, fileName, role );

		m_isEditingModel = false;

		return result;
	}

	return QFileSystemModel::setData( index, value, role );
}

// ** AssetsModel::scanLoadedDirectory
void AssetsModel::scanLoadedDirectory( const QString& path )
{
	// Remove this path from folders for scanning
	m_foldersToScan.remove( path );

	// Get the index by path
	QModelIndex parent = index( path );

	// Scan each child
	for( int i = 0; i < rowCount( parent ); i++ ) {
		QModelIndex child = index( i, 0, parent );

		// Only directories will be scanned
		if( !fileInfo( child ).isDir() ) {
			continue;
		}

		// Nothing to scan
		if( !canFetchMore( child ) ) {
			continue;
		}

		// Add this path to set
		m_foldersToScan << fileInfo( child ).absoluteFilePath();

		// Fetch data
		fetchMore( child );
	}

	// Emit the events
	if( m_foldersToScan.size() ) {
        emit folderScanned( m_foldersToScan.size() );
	} else {
        emit scanningCompleted();
	}
}

// ** AssetsModel::assetsAdded
void AssetsModel::assetsAdded( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		// Get index
		QModelIndex idx = index( i, 0, parent );

		// Get asset file info from index
		FileInfo file = assetFile( idx );

		// This file was moved - skip it
		if( wasMoved( file ) ) {
			continue;
		}

		// Emit the signal
        emit fileAdded( file );
	}
}

// ** AssetsModel::assetsRemoved
void AssetsModel::assetsRemoved( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		QModelIndex idx	 = index( i, 0, parent );

		// Construct file info
		FileInfo file = assetFile( idx );

		// Meta file does not exist - do nothing
		if( !QFile::exists( metaFileName( file ) ) ) {
			continue;
		}

		// Dispatch an event
        emit fileRemoved( file );

		// Remove meta file
		bool result = QDir().remove( metaFileName( file ) );
		Q_ASSERT( result );
	}
}

// ** AssetsModel::assetRenamed
void AssetsModel::assetRenamed( const QString& path, const QString& oldName, const QString& newName )
{
	QString oldFileName = QDir::cleanPath( path + "/" + oldName );
	QString newFileName = QDir::cleanPath( path + "/" + newName );

	renameMetaFile( oldFileName, newFileName );
}

// ** AssetsModel::wasMoved
bool AssetsModel::wasMoved( const FileInfo& assetFile ) const
{
	return m_wasMoved.remove( QString::fromStdString( assetFile.absolutePath() ) );
}

// ** AssetsModel::renameMetaFile
bool AssetsModel::renameMetaFile( const QString& oldFileName, const QString& newFileName )
{
	// Construct the old meta file name
	QString oldMetaFile = metaFileNameFromPath( oldFileName );

	// Meta file does not exist, probably unsupported asset type
	if( !QFile::exists( oldMetaFile ) ) {
		return true;
	}

	// Construct the new meta file name
	QString newMetaFile = metaFileNameFromPath( newFileName );

	// Nothing changed - skip
	if( oldMetaFile == newMetaFile ) {
		return true;
	}

	// Rename file
	bool result = QDir().rename( oldMetaFile, newMetaFile );
	Q_ASSERT( result );
	return result;
}

// ** AssetsModel::uuid
String AssetsModel::uuid( const FileInfo& assetFile ) const
{
	// Read the meta data
	Io::KeyValue data = metaData( assetFile );
	DC_BREAK_IF( !data.isObject() );

	if( !data.isObject() ) {
		return "";
	}

	return data.get( "uuid", "" ).asString();
}

// ** AssetsModel::hasMetaData
bool AssetsModel::hasMetaData( const FileInfo& assetFile ) const
{
	return QFile::exists( metaFileName( assetFile ) );
}

// ** AssetsModel::assetFile
FileInfo AssetsModel::assetFile( const QModelIndex& index ) const
{
	DC_BREAK_IF( !index.isValid() );
	return FileInfo( fileInfo( index ) );
}

// ** AssetsModel::setMetaData
void AssetsModel::setMetaData( const FileInfo& assetFile, const Io::KeyValue& data )
{
	// Write meta data to file
	QFile metaFile( metaFileName( assetFile ) );
	bool  result = metaFile.open( QFile::WriteOnly );
	Q_ASSERT( result );

	QTextStream stream( &metaFile );
	String json = Io::KeyValue::stringify( data );
	stream << json.c_str();

	metaFile.close();
}

// ** AssetsModel::metaData
Io::KeyValue AssetsModel::metaData( const FileInfo& assetFile ) const
{
	QFile metaFile( metaFileName( assetFile ) );

	if( !metaFile.open( QFile::ReadOnly ) ) {
		return Io::KeyValue::kNull;
	}

	QTextStream stream( &metaFile );
	QString json;
	stream >> json;

	metaFile.close();

	return Io::KeyValue::parse( json.toStdString() );
}

// ** AssetsModel::metaFileName
QString AssetsModel::metaFileName( const FileInfo& assetFile ) const
{
	return metaFileNameFromPath( QString::fromStdString( assetFile.absolutePath() ) );
}

// ** AssetsModel::metaFileNameFromPath
QString AssetsModel::metaFileNameFromPath( const QString& fileName ) const
{
	return fileName + "." + m_metaFileExtension;
}

// ** AssetsModel::rootPath
String AssetsModel::rootPath( void ) const
{
	return QFileSystemModel::rootPath().toStdString();
}

// ** AssetsModel::setRootPath
void AssetsModel::setRootPath( const String& value )
{
	blockSignals( true );
	QFileSystemModel::setRootPath( value.c_str() );
	blockSignals( false );
}

// ** AssetsModel::metaData
Io::KeyValue AssetsModel::metaData( const String& assetFileName ) const
{
	return metaData( FileInfo( assetFileName ) );
}

// ----------------------------------------------------------- FilteredAssetsModel ----------------------------------------------------------- //

// ** FilteredAssetsModel::FilteredAssetsModel
FilteredAssetsModel::FilteredAssetsModel( AssetsModelQPtr assetsModel, QObject* parent ) : QSortFilterProxyModel( parent )
{
	m_model = assetsModel;
	setSourceModel( m_model );
}

// ** FilteredAssetsModel::filterAcceptsRow
bool FilteredAssetsModel::filterAcceptsRow( int row, const QModelIndex& parent ) const
{
	QFileInfo file = m_model->fileInfo( m_model->index( row, 0, parent ) );

	if( file.suffix().toLower() == "asset" ) {
		return false;
	}

	return true;
}

// ** FilteredAssetsModel::model
AssetsModelQPtr FilteredAssetsModel::model( void ) const
{
	return m_model;
}

// ** FilteredAssetsModel::root
QModelIndex FilteredAssetsModel::root( void ) const
{
	return mapFromSource( m_model->index( QString::fromStdString( m_model->rootPath() ) ) );
}

// ** FilteredAssetsModel::assetFile
FileInfo FilteredAssetsModel::assetFile( const QModelIndex& index ) const
{
	return m_model->assetFile( mapToSource( index ) );
}

// ** FilteredAssetsModel::remove
void FilteredAssetsModel::remove( const QModelIndex& index )
{
	m_model->remove( mapToSource( index ) );
}

#endif	/*	DEV_CUSTOM_ASSET_MODEL	*/

DC_END_COMPOSER