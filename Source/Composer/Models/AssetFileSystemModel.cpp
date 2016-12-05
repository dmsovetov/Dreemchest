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

#include "AssetFileSystemModel.h"

#include "../FileSystem.h"

DC_BEGIN_COMPOSER

// ** AssetFileSystemModel::AssetFileSystemModel
AssetFileSystemModel::AssetFileSystemModel( QObject* parent ) : QFileSystemModel( parent ), m_isEditingModel( false ), m_metaFileExtension( "asset" )
{
	setReadOnly( false );

    // Create file system watcher
    m_watcher = new QFileSystemWatcher( this );
    connect( m_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(assetChanged(const QString&)) );

	connect( this, SIGNAL(directoryLoaded(const QString&)), this, SLOT(scanLoadedDirectory(const QString&)) );
	connect( this, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(assetsAdded(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(assetsRemoved(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(fileRenamed(const QString&, const QString&, const QString&)), this, SLOT(assetRenamed(const QString&, const QString&, const QString&)) );
}

// ** AssetFileSystemModel::dropMimeData
bool AssetFileSystemModel::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent )
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

        // Remove old path from watcher
        m_watcher->removePath( oldFileName );

		// Add this file to a moved set
		m_wasMoved << newFileName;
	}

	return true;
}

// ** AssetFileSystemModel::data
QVariant AssetFileSystemModel::data( const QModelIndex& index, int role ) const
{
	if( !index.isValid() ) {
		return QVariant();
	}

	switch( role ) {
	case Qt::DisplayRole:	    return m_isEditingModel ? fileInfo( index ).fileName() : fileInfo( index ).baseName();
	case Qt::EditRole:		    return fileInfo( index ).baseName();
#if DEV_DISABLE_SYSTEM_ICON_PROVIDER
    case Qt::DecorationRole:    return QPixmap();
#endif  /*  DEV_DISABLE_SYSTEM_ICON_PROVIDER    */
	}

	return QFileSystemModel::data( index, role );
}

// ** AssetFileSystemModel::setData
bool AssetFileSystemModel::setData( const QModelIndex& index, const QVariant& value, int role )
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

// ** AssetFileSystemModel::scanLoadedDirectory
void AssetFileSystemModel::scanLoadedDirectory( const QString& path )
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
        Q_EMIT folderScanned( m_foldersToScan.size() );
	} else {
        Q_EMIT scanningCompleted();
	}
}

// ** AssetFileSystemModel::assetsAdded
void AssetFileSystemModel::assetsAdded( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		// Get index
		QModelIndex idx = index( i, 0, parent );

		// Get asset file info from index
		FileInfo file = assetFile( idx );

        // Remove this file from watcher
        m_watcher->addPath( file.absoluteFilePath() );

		// This file was moved - skip it
		if( wasMoved( file ) ) {
			continue;
		}

		// Emit the signal
        Q_EMIT fileAdded( file );
	}
}

// ** AssetFileSystemModel::assetsRemoved
void AssetFileSystemModel::assetsRemoved( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		QModelIndex idx	 = index( i, 0, parent );

		// Construct file info
		FileInfo file = assetFile( idx );

        // Remove this file from watcher
        m_watcher->removePath( file.absoluteFilePath() );

		// Meta file does not exist - do nothing
		if( !QFile::exists( metaFileName( file ) ) ) {
			continue;
		}

		// Dispatch an event
        Q_EMIT fileRemoved( QString::fromStdString( uuid( file ) ), file );

		// Remove meta file
		bool result = QDir().remove( metaFileName( file ) );
		Q_ASSERT( result );
	}
}

// ** AssetFileSystemModel::assetRenamed
void AssetFileSystemModel::assetRenamed( const QString& path, const QString& oldName, const QString& newName )
{
	QString oldFileName = QDir::cleanPath( path + "/" + oldName );
	QString newFileName = QDir::cleanPath( path + "/" + newName );

	renameMetaFile( oldFileName, newFileName );
}

// ** AssetFileSystemModel::assetChanged
void AssetFileSystemModel::assetChanged( const QString& path )
{
    FileInfo file( path );
    Q_EMIT fileChanged( QString::fromStdString( uuid( file ) ), file ); 
}

// ** AssetFileSystemModel::wasMoved
bool AssetFileSystemModel::wasMoved( const FileInfo& assetFile ) const
{
	return m_wasMoved.remove( QString::fromStdString( assetFile.absolutePath() ) );
}

// ** AssetFileSystemModel::renameMetaFile
bool AssetFileSystemModel::renameMetaFile( const QString& oldFileName, const QString& newFileName )
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

// ** AssetFileSystemModel::uuid
String AssetFileSystemModel::uuid( const FileInfo& assetFile ) const
{
	// Read the meta data
	Archive data = metaData( assetFile );

    NIMBLE_BREAK_IF( !data.type()->is<KeyValue>() );
    return data.as<KeyValue>().get<String>( "uuid" );
}

// ** AssetFileSystemModel::hasMetaData
bool AssetFileSystemModel::hasMetaData( const FileInfo& assetFile ) const
{
	return QFile::exists( metaFileName( assetFile ) );
}

// ** AssetFileSystemModel::assetFile
FileInfo AssetFileSystemModel::assetFile( const QModelIndex& index ) const
{
	NIMBLE_BREAK_IF( !index.isValid() );
	return FileInfo( fileInfo( index ) );
}

// ** AssetFileSystemModel::setMetaData
void AssetFileSystemModel::setMetaData( const FileInfo& assetFile, const Archive& data )
{
	// Write meta data to file
	QFile metaFile( metaFileName( assetFile ) );
	bool  result = metaFile.open( QFile::WriteOnly );
	Q_ASSERT( result );

    QTextStream stream( &metaFile );
    String json = Io::VariantTextStream::stringify( data );
    stream << json.c_str();

	metaFile.close();
}

// ** AssetFileSystemModel::metaData
Archive AssetFileSystemModel::metaData( const FileInfo& assetFile ) const
{
	QFile metaFile( metaFileName( assetFile ) );

	if( !metaFile.open( QFile::ReadOnly ) ) {
        return Archive();
	}

	QTextStream stream( &metaFile );
	QString json;
	stream >> json;

	metaFile.close();

    return Io::VariantTextStream::parse( json.toStdString() );
}

// ** AssetFileSystemModel::metaFileName
QString AssetFileSystemModel::metaFileName( const FileInfo& assetFile ) const
{
	return metaFileNameFromPath( QString::fromStdString( assetFile.absolutePath() ) );
}

// ** AssetFileSystemModel::metaFileNameFromPath
QString AssetFileSystemModel::metaFileNameFromPath( const QString& fileName ) const
{
	return fileName + "." + m_metaFileExtension;
}

// ** AssetFileSystemModel::rootPath
String AssetFileSystemModel::rootPath( void ) const
{
	return QFileSystemModel::rootPath().toStdString();
}

// ** AssetFileSystemModel::setRootPath
void AssetFileSystemModel::setRootPath( const String& value )
{
	blockSignals( true );
	QFileSystemModel::setRootPath( value.c_str() );
	blockSignals( false );
}

// ** AssetFileSystemModel::metaData
Archive AssetFileSystemModel::metaData( const String& assetFileName ) const
{
	return metaData( FileInfo( assetFileName ) );
}

// ----------------------------------------------------------- FilteredAssetsModel ----------------------------------------------------------- //

// ** FilteredAssetsModel::FilteredAssetsModel
FilteredAssetsModel::FilteredAssetsModel( AssetFileSystemModelQPtr assetFileSystem, QObject* parent ) : QSortFilterProxyModel( parent )
{
	m_model = assetFileSystem;
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
AssetFileSystemModelQPtr FilteredAssetsModel::model( void ) const
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

DC_END_COMPOSER
