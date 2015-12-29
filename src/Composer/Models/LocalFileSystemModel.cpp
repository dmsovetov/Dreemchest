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

#include "LocalFileSystemModel.h"

DC_BEGIN_COMPOSER

// ** LocalFileSystemModel::LocalFileSystemModel
LocalFileSystemModel::LocalFileSystemModel( QObject* parent ) : GenericTreeModel( 1, parent ), m_isReadOnly( true )
{
#if DEV_BACKGROUND_ASSET_LOADING
	//qRegisterMetaType<QAsset>( "QAsset" );
	qRegisterMetaType<QVector<int>>( "QVector<int>" );
#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/

	// Create the default icon provider
	m_iconProvider = new QFileIconProvider;

	// Create the file system watcher
	m_watcher = new QFileSystemWatcher( this );

	// Connect signals
	connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)) );
	connect( m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)) );
}

LocalFileSystemModel::~LocalFileSystemModel( void )
{
	delete m_iconProvider;
	delete m_watcher;
}

// ** LocalFileSystemModel::rootPath
QString LocalFileSystemModel::rootPath( void ) const
{
	return m_directory.absolutePath();
}

// ** LocalFileSystemModel::setRootPath
void LocalFileSystemModel::setRootPath( const QString& value )
{
	// Add path to watcher
	m_watcher->removePaths( m_watcher->directories() + m_watcher->files() );
	m_watcher->addPath( value );

	// Set the root directory info
	m_directory = value;

	// Clear the model
	clear();

	// Scan folder
#if DEV_BACKGROUND_ASSET_LOADING
	scanInBackground( value );
#else
	scan( value );
#endif	/*	DEV_BACKGROUND_ASSET_LOADING */
}

// ** LocalFileSystemModel::isReadOnly
bool LocalFileSystemModel::isReadOnly( void ) const
{
	return m_isReadOnly;
}

// ** LocalFileSystemModel::setReadOnly
void LocalFileSystemModel::setReadOnly( bool value )
{
	m_isReadOnly = value;
}

// ** LocalFileSystemModel::index
QModelIndex LocalFileSystemModel::index( const QString& path ) const
{
	// Find item at path.
	Item* item = itemAtPath( path, root() );

	// Create index from item.
	QModelIndex index = indexFromItem( item );

	return index;
}

// ** LocalFileSystemModel::remove
void LocalFileSystemModel::remove( const QModelIndex& index )
{
	// A local helper structure to recursively delete directories & files
	struct Recursive {
		// Performs the deletion operation.
		static bool remove( const QFileInfo& fileInfo )
		{
			// This is a file - just delete it
			if( !fileInfo.isDir() ) {
				return QFile::remove( fileInfo.absoluteFilePath() );
			}

			// First remove directory children
			QDir		  dir( fileInfo.absoluteFilePath() );
			QFileInfoList entries = dir.entryInfoList( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );

			foreach( const QFileInfo& entry, entries ) {
				if( !remove( entry ) ) {
					return false;
				}
			}

			// Remove directory
			bool result = dir.rmdir( fileInfo.absoluteFilePath() );
			//Q_ASSERT( result );

			return result;
		}
	};

	// Find item by index
	Item* item = itemAtIndex( index );
	Q_ASSERT( item != NULL );

	// Get asset info from item
	const QFileInfo& fileInfo = item->data();

	// Remove asset at path
	if( !Recursive::remove( fileInfo ) ) {
		emit failedToRemove( fileInfo );
	}
}

// ** LocalFileSystemModel::fileInfo
const QFileInfo& LocalFileSystemModel::fileInfo( const QModelIndex& index ) const
{
	return dataAt( index );
}

// ** LocalFileSystemModel::isDir
bool LocalFileSystemModel::isDir( const QModelIndex& index ) const
{
	return fileInfo( index ).isDir();
}

// ** LocalFileSystemModel::flags
Qt::ItemFlags LocalFileSystemModel::flags( const QModelIndex& index ) const
{
	if( m_isReadOnly ) {
		return GenericTreeModel::flags( index );
	}

	// Initialize default flags.
	Qt::ItemFlags defaultFlags = Qt::ItemIsEditable | QAbstractItemModel::flags( index );

	// Can drop to a root of the model
	if( !index.isValid() ) {
		return defaultFlags | Qt::ItemIsDropEnabled;
	}

	// Get the file info from
	const QFileInfo& fileInfo = dataAt( index );

	// Only folders can accept drops.
	if( fileInfo.isDir() ) {
		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	}

	// Files can only be dragged
	return Qt::ItemIsDragEnabled | defaultFlags;
}

// ** LocalFileSystemModel::supportsBackgroundLoading
bool LocalFileSystemModel::supportsBackgroundLoading( void ) const
{
#if DEV_BACKGROUND_ASSET_LOADING
	return true;
#else
	return false;
#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/
}

#if DEV_BACKGROUND_ASSET_LOADING

// ** LocalFileSystemModel::scanInBackground
void LocalFileSystemModel::scanInBackground( const QString& path, TreeItem* parent )
{
	// Emit signal about loading start
	if( m_scanWatchers.empty() ) {
		emit loadingStarted();
	}

	// Construct future object
	QFuture<void> future = QtConcurrent::run( &LocalFileSystemModel::workerScan, this, path );

	// Construct future watcher
	QFutureWatcher<void>* watcher = new QFutureWatcher<void>;
	connect( watcher, SIGNAL(finished()), this, SLOT(scanFinished()) );
	watcher->setFuture( future );

	m_scanWatchers.push_back( watcher );
}

// ** LocalFileSystemModel::workerScan
void LocalFileSystemModel::workerScan( LocalFileSystemModel* model, const QString& path )
{
	model->scan( path );
}

// ** LocalFileSystemModel::scanFinished
void LocalFileSystemModel::scanFinished( void )
{
	// Type cast watcher
	QFutureWatcher<void>* watcher = static_cast<QFutureWatcher<void>*>( qobject_cast<QFutureWatcherBase*>( sender() ) );

	// Remove watcher from list
	int index = m_scanWatchers.indexOf( watcher );
	Q_ASSERT( index != -1 );
	m_scanWatchers.removeAt( index );
	delete watcher;

	// If no more watchers left - we are done
	if( m_scanWatchers.empty() ) {
		emit loadingFinished();
	}
}

#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/

// ** LocalFileSystemModel::scan
void LocalFileSystemModel::scan( const QString& path, TreeItem* parent )
{
	// List all files & directories at specified path
	QFileInfoList items = listDirectory( path );

	// Create tree item for each entry
	foreach( const QFileInfo& fileInfo, items ) {
		// Add new asset
		TreeItem* item = createItem( fileInfo );
		addItem( item, parent );

		// Continue recursive scan of folders
		if( fileInfo.isDir() ) {
			scan( fileInfo.absoluteFilePath(), item );
		}
	}
}

// ** LocalFileSystemModel::listDirectory
QFileInfoList LocalFileSystemModel::listDirectory( const QString& path ) const
{
	QFileInfoList list = QDir( path ).entryInfoList( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );
	QFileInfoList result;

	foreach( const QFileInfo& fileInfo, list ) {
		if( shouldSkipFile( fileInfo ) ) {
			continue;
		}

		result << fileInfo;
	}

	return result;
}

// ** LocalFileSystemModel::shouldSkipFile
bool LocalFileSystemModel::shouldSkipFile( const QFileInfo& fileInfo ) const
{
	return false;
}

// ** LocalFileSystemModel::watch
void LocalFileSystemModel::watch( const Item* item )
{
	m_watcher->addPath( item->data().absoluteFilePath() );
}

// ** LocalFileSystemModel::unwatch
void LocalFileSystemModel::unwatch( const Item* item )
{
	m_watcher->removePath( item->data().absoluteFilePath() );
}

// ** LocalFileSystemModel::addAsset
void LocalFileSystemModel::addItem( TreeItem* item, TreeItem* parent )
{
	Item* asset = static_cast<Item*>( item );

	// Add item to model
	GenericTreeModel::addItem( item, parent );

	// Add path to watcher
	watch( asset );

	// Emit signal
	emit fileAdded( asset->data() );
}

// ** LocalFileSystemModel::removeAsset
void LocalFileSystemModel::removeItem( TreeItem* item )
{
	Item* asset = static_cast<Item*>( item );

	// Remove from watcher
	unwatch( asset );

	// Emit signal
	emit fileRemoved( asset->data() );

	// Remove item
	GenericTreeModel::removeItem( item );
}

// ** LocalFileSystemModel::moveItem
bool LocalFileSystemModel::moveItem( Item* sourceParent, Item* destinationParent, Item* item, int destinationRow ) const
{
	const QFileInfo& asset = item->data();

	// Get source & destination paths
	QString source = constructAbsoluteFileName( item, sourceParent );
	QString destination = constructAbsoluteFileName( item, destinationParent );

	// Try moving the file
	if( !move( item, source, destination ) ) {
		emit failedToMove( asset, destination );
		return false;
	}

	// Emit signal
	emit fileMoved( asset, source, destination );

	return true;
}

// ** LocalFileSystemModel::constructAbsoluteFileName
QString LocalFileSystemModel::constructAbsoluteFileName( const Item* item, const Item* parent ) const
{
	// Get the parent absolute path
	QString directory = parent ? parent->data().absoluteFilePath() : m_directory.absoluteFilePath();

	// Construct the resulting path
	QString fileName = directory + QDir::separator() + item->data().fileName();

	return QDir::cleanPath( fileName );
}

// ** LocalFileSystemModel::constructRelativeFileName
QString LocalFileSystemModel::constructRelativeFileName( const Item* item, const Item* parent ) const
{
	return m_directory.dir().relativeFilePath( constructAbsoluteFileName( item, parent ) );
}

// ** LocalFileSystemModel::data
QVariant LocalFileSystemModel::data( const QModelIndex& index, int role ) const
{
	Item* item = itemAtIndex( index );

	switch( role ) {
	case Qt::DisplayRole:
	case Qt::EditRole:			return item->data().baseName();
	case Qt::DecorationRole:	return m_iconProvider->icon( item->data() );
	}

	return QVariant();
}

// ** LocalFileSystemModel::setData
bool LocalFileSystemModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
	// Skip all roles except the editing one
	if( role != Qt::EditRole ) {
		return GenericTreeModel::setData( index, value, role );
	}

	// Get the item at index
	Item* item = itemAtIndex( index );

	// Get the file info at index
	const QFileInfo& file = item->data();

	// Skip if the file name was not changed
	if( value.toString() == file.baseName() ) {
		return false;
	}

	// Rename the file
	QString fileName	= file.isDir() ? value.toString() : (value.toString() + "." + file.completeSuffix());
	QString oldFileName = QDir::cleanPath( file.absoluteFilePath() );
	QString newFileName = QDir::cleanPath( file.absolutePath() + QDir::separator() + fileName );

	qDebug() << oldFileName << "->" << newFileName;

	if( !move( item, oldFileName, newFileName ) ) {
		emit failedToRename( file, oldFileName );
		return false;
	}

	// Emit signal
	emit fileRenamed( file, oldFileName, newFileName );

	return true;
}

// ** LocalFileSystemModel::move
bool LocalFileSystemModel::move( Item* item, const QString& source, const QString& destination ) const
{
	// Rename file
	if( !QDir().rename( source, destination ) ) {
		return false;
	}

	// Update watcher
	m_watcher->removePath( source );
	m_watcher->addPath( destination );

	// Update asset info
	//item->data().updateFileInfo( destination );

	return true;
}

// ** LocalFileSystemModel::itemAtPath
LocalFileSystemModel::Item* LocalFileSystemModel::itemAtPath( const QString& fileName, const TreeItem* root ) const
{
	root = root ? root : this->root();

	for( int i = 0, n = root->childCount(); i < n; i++ ) {
		// Type-cast the item
		Item* item = static_cast<Item*>( root->child( i ) );

		// Get asset data from item
		const QFileInfo& file = item->data();

		// Compare file names
		if( file.absoluteFilePath() == fileName ) {
			return item;
		}

		// Check children
		if( Item* nested = itemAtPath( fileName, item ) ) {
			return nested;
		}
	}

	return NULL;
}

// ** LocalFileSystemModel::fileChanged
void LocalFileSystemModel::fileChanged( const QString& fileName )
{
	// Get the item from path
	TreeItem* item = itemAtPath( fileName, root() );

	// The item is already deleted
	if( !item ) {
		return;
	}

	// Check that file still exists
	bool stillExists = QFile( fileName ).exists();

	if( !stillExists ) {
		removeItem( item );
	} else {
		emit fileChanged( static_cast<Item*>( item )->data() );
	}
}

// ** LocalFileSystemModel::directoryChanged
void LocalFileSystemModel::directoryChanged( const QString& path )
{
	qDebug() << "Changed" << path;

	// Get the item from path
	bool isRoot = (path == m_directory.fileName());
	TreeItem* item = itemAtPath( path, root() );

	// Check that folder still exists
	bool stillExists = QFile( path ).exists();

	if( !stillExists && item ) {
		removeItem( item );
		return;
	}

	// List directory
	QFileInfoList items = listDirectory( path );

	// Lookup added items
	foreach( const QFileInfo& fileInfo, items ) {
		// Find item by path
		Item* child = itemAtPath( fileInfo.absoluteFilePath(), item );

		// This asset is already added
		if( child ) {
			continue;
		}

		qDebug() << fileInfo.absoluteFilePath();
		
		// New asset added
		Item* newItem = createItem( fileInfo );
		addItem( newItem, item );

		if( fileInfo.isDir() ) {
		#if DEV_BACKGROUND_ASSET_LOADING
			scanInBackground( fileInfo.absoluteFilePath(), newItem );
		#else
			scan( fileInfo.absoluteFilePath(), newItem );
		#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/
		}
	}
}

DC_END_COMPOSER