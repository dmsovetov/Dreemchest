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
#include "MimeData.h"
#include "SignalDelegate.h"

namespace Ui {

#if DEV_CUSTOM_ASSET_MODEL

// ----------------------------------------------------------- QAsset ----------------------------------------------------------- //

// ** QAsset::kExtension
QString QAsset::kExtension = "asset";

// ** QAsset::QAssetInfo
QAsset::QAsset( const QFileInfo& fileInfo ) : m_fileInfo( fileInfo )
{
	m_uuid			= QUuid::createUuid();
	m_metaFileName  = absoluteFilePath() + "." + kExtension;
}

// ** QAsset::QAsset
QAsset::QAsset( const QString& path ) : m_fileInfo( path )
{
	m_uuid		   = QUuid::createUuid();
	m_metaFileName = absoluteFilePath() + "." + kExtension;
}

// ** QAsset::absoluteFilePath
QString QAsset::absoluteFilePath( void ) const
{
	return m_fileInfo.absoluteFilePath();
}

// ** QAsset::name
QString QAsset::name( void ) const
{
	return m_fileInfo.baseName();
}

// ** QAsset::fileName
QString QAsset::fileName( void ) const
{
	return m_fileInfo.fileName();
}

// ** QAsset::fileInfo
const QFileInfo& QAsset::fileInfo( void ) const
{
	return m_fileInfo;
}

// ** QAsset::uuid
const QUuid& QAsset::uuid( void ) const
{
	return m_uuid;
}

// ** QAsset::updateFileInfo
void QAsset::updateFileInfo( const QString& path )
{
	m_fileInfo = QFileInfo( path );
}

// ** QAsset::metaFileName
const QString& QAsset::metaFileName( void ) const
{
	return m_metaFileName;
}

// ** QAsset::readMetaFile
QString QAsset::readMetaFile( void ) const
{
	QFile file( metaFileName() );

	if( !file.open( QFile::ReadOnly | QFile::Text ) ) {
		return QString();
	}

	QTextStream in( &file );
	QString result = in.readAll();

	file.close();

	return result;
}

// ----------------------------------------------------------- QMetaFiles ----------------------------------------------------------- //

// ** QMetaFiles::QMetaFiles
QMetaFiles::QMetaFiles( QObject* parent ) : QObject( parent )
{
}

// ** QMetaFiles::addMetaFile
void QMetaFiles::addMetaFile( const QAsset& asset ) const
{
	// Get asset meta file name
	const QString& metaFileName = asset.metaFileName();

	// Check the existance of a meta file
	if( QFile( metaFileName ).exists() ) {
		return;
	}

	// No file found - create new
	QFile file( metaFileName );
	bool  result = file.open( QFile::WriteOnly );
	Q_ASSERT( result );
	file.close();
}

// ** QMetaFiles::removeMetaFile
void QMetaFiles::removeMetaFile( const QAsset& asset ) const
{
	const QString& metaFileName = asset.metaFileName();

	if( !QFile( metaFileName ).exists() ) {
		return;
	}

	bool result = QDir().remove( asset.metaFileName() );
	Q_ASSERT( result );
}

// ** QMetaFiles::renameMetaFile
void QMetaFiles::renameMetaFile( const QAsset& asset, const QString& oldName, const QString& newName ) const
{
	QString oldMetaFile = asset.fileInfo().dir().absoluteFilePath( oldName ) + "." + QAsset::kExtension;
	QString newMetaFile = asset.metaFileName();

	if( oldMetaFile == newMetaFile ) {
		return;
	}

	bool result = QDir().rename( oldMetaFile, newMetaFile );
	Q_ASSERT( result );
}

// ** QMetaFiles::moveMetaFile
void QMetaFiles::moveMetaFile( const QAsset& asset, const QString& oldPath, const QString& newPath ) const
{
	QString oldMetaFile = oldPath + "." + QAsset::kExtension;
	QString newMetaFile = asset.metaFileName();

	if( oldMetaFile == newMetaFile ) {
		return;
	}

	bool result = QDir().rename( oldMetaFile, newMetaFile );
	Q_ASSERT( result );
}

// ----------------------------------------------------------- QAssetsModel ----------------------------------------------------------- //

// ** QAssetsModel::QAssetsModel
QAssetsModel::QAssetsModel( QObject* parent ) : QGenericTreeModel( 1, parent ), m_isReadOnly( true )
{
	qRegisterMetaType<QAsset>( "QAsset" );
	qRegisterMetaType<QVector<int>>( "QVector<int>" );

	// Create the default icon provider
	m_iconProvider = new QFileIconProvider;

	// Create the file system watcher
	m_watcher = new QFileSystemWatcher( this );

	// Create meta files
	m_metaFiles = new QMetaFiles( this );
	connect( this, SIGNAL(assetAdded(const QAsset&)), m_metaFiles, SLOT(addMetaFile(const QAsset&)) );
	connect( this, SIGNAL(assetRemoved(const QAsset&)), m_metaFiles, SLOT(removeMetaFile(const QAsset&)) );
	connect( this, SIGNAL(assetRenamed(const QAsset&, const QString&, const QString&)), m_metaFiles, SLOT(renameMetaFile(const QAsset&, const QString&, const QString&)) );
	connect( this, SIGNAL(assetMoved(const QAsset&, const QString&, const QString&)), m_metaFiles, SLOT(moveMetaFile(const QAsset&, const QString&, const QString&)) );
	
	// Connect signals
	connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)) );
	connect( m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)) );
}

QAssetsModel::~QAssetsModel( void )
{
	delete m_iconProvider;
	delete m_watcher;
}

// ** QAssetsModel::rootPath
QString QAssetsModel::rootPath( void ) const
{
	return m_directory.absolutePath();
}

// ** QAssetsModel::setRootPath
void QAssetsModel::setRootPath( const QString& value )
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

// ** QAssetsModel::isReadOnly
bool QAssetsModel::isReadOnly( void ) const
{
	return m_isReadOnly;
}

// ** QAssetsModel::setReadOnly
void QAssetsModel::setReadOnly( bool value )
{
	m_isReadOnly = value;
}

// ** QAssetsModel::index
QModelIndex QAssetsModel::index( const QString& path ) const
{
	// Find item at path.
	Item* item = itemAtPath( path, root() );

	// Create index from item.
	QModelIndex index = indexFromItem( item );

	return index;
}

// ** QAssetsModel::remove
void QAssetsModel::remove( const QModelIndex& index )
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
	const QAsset& asset = item->data();

	// Remove asset at path
	if( !Recursive::remove( asset.fileInfo() ) ) {
		emit failedToRemove( asset );
	}
}

// ** QAssetsModel::asset
const QAsset& QAssetsModel::asset( const QModelIndex& index ) const
{
	return dataAt( index );
}

// ** QAssetsModel::isDir
bool QAssetsModel::isDir( const QModelIndex& index ) const
{
	return asset( index ).fileInfo().isDir();
}

// ** QAssetsModel::flags
Qt::ItemFlags QAssetsModel::flags( const QModelIndex& index ) const
{
	if( m_isReadOnly ) {
		return QGenericTreeModel::flags( index );
	}

	// Initialize default flags.
	Qt::ItemFlags defaultFlags = Qt::ItemIsEditable | QAbstractItemModel::flags( index );

	// Can drop to a root of the model
	if( !index.isValid() ) {
		return defaultFlags | Qt::ItemIsDropEnabled;
	}

	// Get the file info from
	const QFileInfo& fileInfo = dataAt( index ).fileInfo();

	// Only folders can accept drops.
	if( fileInfo.isDir() ) {
		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	}

	// Files can only be dragged
	return Qt::ItemIsDragEnabled | defaultFlags;
}

// ** QAssetsModel::supportsBackgroundLoading
bool QAssetsModel::supportsBackgroundLoading( void ) const
{
#if DEV_BACKGROUND_ASSET_LOADING
	return true;
#else
	return false;
#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/
}

#if DEV_BACKGROUND_ASSET_LOADING

// ** QAssetsModel::scanInBackground
void QAssetsModel::scanInBackground( const QString& path, TreeItem* parent )
{
	// Emit signal about loading start
	if( m_scanWatchers.empty() ) {
		emit loadingStarted();
	}

	// Construct future object
	QFuture<void> future = QtConcurrent::run( &QAssetsModel::workerScan, this, path );

	// Construct future watcher
	QFutureWatcher<void>* watcher = new QFutureWatcher<void>;
	connect( watcher, SIGNAL(finished()), this, SLOT(scanFinished()) );
	watcher->setFuture( future );

	m_scanWatchers.push_back( watcher );
}

// ** QAssetsModel::workerScan
void QAssetsModel::workerScan( QAssetsModel* model, const QString& path )
{
	model->scan( path );
}

// ** QAssetsModel::scanFinished
void QAssetsModel::scanFinished( void )
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

// ** QAssetsModel::scan
void QAssetsModel::scan( const QString& path, TreeItem* parent )
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

// ** QAssetsModel::listDirectory
QFileInfoList QAssetsModel::listDirectory( const QString& path ) const
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

// ** QAssetsModel::shouldSkipFile
bool QAssetsModel::shouldSkipFile( const QFileInfo& fileInfo ) const
{
	return fileInfo.completeSuffix().endsWith( QAsset::kExtension );
}

// ** QAssetsModel::watch
void QAssetsModel::watch( const Item* item )
{
	m_watcher->addPath( item->data().absoluteFilePath() );
}

// ** QAssetsModel::unwatch
void QAssetsModel::unwatch( const Item* item )
{
	m_watcher->removePath( item->data().absoluteFilePath() );
}

// ** QAssetsModel::addAsset
void QAssetsModel::addItem( TreeItem* item, TreeItem* parent )
{
	Item* asset = static_cast<Item*>( item );

	// Add item to model
	QGenericTreeModel::addItem( item, parent );

	// Add path to watcher
	watch( asset );

	// Emit signal
	emit assetAdded( asset->data() );
}

// ** QAssetsModel::removeAsset
void QAssetsModel::removeItem( TreeItem* item )
{
	Item* asset = static_cast<Item*>( item );

	// Remove from watcher
	unwatch( asset );

	// Emit signal
	emit assetRemoved( asset->data() );

	// Remove item
	QGenericTreeModel::removeItem( item );
}

// ** QAssetsModel::moveItem
bool QAssetsModel::moveItem( Item* sourceParent, Item* destinationParent, Item* item, int destinationRow ) const
{
	const QAsset& asset = item->data();

	// Get source & destination paths
	QString source = constructAbsoluteFileName( item, sourceParent );
	QString destination = constructAbsoluteFileName( item, destinationParent );

	// Try moving the file
	if( !move( item, source, destination ) ) {
		emit failedToMove( asset, destination );
		return false;
	}

	// Emit signal
	emit assetMoved( asset, source, destination );

	return true;
}

// ** QAssetsModel::constructAbsoluteFileName
QString QAssetsModel::constructAbsoluteFileName( const Item* item, const Item* parent ) const
{
	// Get the parent absolute path
	QString directory = parent ? parent->data().absoluteFilePath() : m_directory.absoluteFilePath();

	// Construct the resulting path
	QString fileName = directory + QDir::separator() + item->data().fileName();

	return QDir::cleanPath( fileName );
}

// ** QAssetsModel::constructRelativeFileName
QString QAssetsModel::constructRelativeFileName( const Item* item, const Item* parent ) const
{
	return m_directory.dir().relativeFilePath( constructAbsoluteFileName( item, parent ) );
}

// ** QAssetsModel::data
QVariant QAssetsModel::data( const QModelIndex& index, int role ) const
{
	Item* item = itemAtIndex( index );

	switch( role ) {
	case Qt::DisplayRole:
	case Qt::EditRole:			return item->data().fileInfo().baseName();
	case Qt::DecorationRole:	return m_iconProvider->icon( item->data().fileInfo() );
	}

	return QVariant();
}

// ** QAssetsModel::setData
bool QAssetsModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
	// Skip all roles except the editing one
	if( role != Qt::EditRole ) {
		return QGenericTreeModel::setData( index, value, role );
	}

	// Get the item at index
	Item* item = itemAtIndex( index );

	// Get the file info at index
	const QAsset&	 asset	  = item->data();
	const QFileInfo& fileInfo = asset.fileInfo();

	// Skip if the file name was not changed
	if( value.toString() == fileInfo.baseName() ) {
		return false;
	}

	// Rename the file
	QString oldName		= fileInfo.fileName();
	QString newName		= fileInfo.isDir() ? value.toString() : (value.toString() + "." + fileInfo.completeSuffix());
	QString newFileName = fileInfo.absolutePath() + QDir::separator() + newName;

	if( !move( item, fileInfo.absoluteFilePath(), newFileName ) ) {
		emit failedToRename( asset, newFileName );
		return false;
	}

	// Emit signal
	emit assetRenamed( asset, oldName, newName );

	return true;
}

// ** QAssetsModel::move
bool QAssetsModel::move( Item* item, const QString& source, const QString& destination ) const
{
	// Rename file
	if( !QDir().rename( source, destination ) ) {
		return false;
	}

	// Update watcher
	m_watcher->removePath( source );
	m_watcher->addPath( destination );

	// Update asset info
	item->data().updateFileInfo( destination );

	return true;
}

// ** QAssetsModel::itemAtPath
QAssetsModel::Item* QAssetsModel::itemAtPath( const QString& fileName, const TreeItem* root ) const
{
	root = root ? root : this->root();

	for( int i = 0, n = root->childCount(); i < n; i++ ) {
		// Type-cast the item
		Item* item = static_cast<Item*>( root->child( i ) );

		// Get asset data from item
		const QAsset& asset = item->data();

		// Compare file names
		if( asset.absoluteFilePath() == fileName ) {
			return item;
		}

		// Check children
		if( Item* nested = itemAtPath( fileName, item ) ) {
			return nested;
		}
	}

	return NULL;
}

// ** QAssetsModel::fileChanged
void QAssetsModel::fileChanged( const QString& fileName )
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
		emit assetChanged( static_cast<Item*>( item )->data() );
	}
}

// ** QAssetsModel::directoryChanged
void QAssetsModel::directoryChanged( const QString& path )
{
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

// ------------------------------------------------------ QAssetsModelDispatcher ----------------------------------------------------- //

// ** QAssetsModelDispatcher::QAssetsModelDispatcher
QAssetsModelDispatcher::QAssetsModelDispatcher( QAssetsModel* model, IAssetsModelDelegateWPtr delegate ) : m_delegate( delegate )
{
	connect( model, SIGNAL(assetAdded(const QAsset&)), this, SLOT(dispatchAssetAdded(const QAsset&)) );
	connect( model, SIGNAL(assetRemoved(const QAsset&)), this, SLOT(dispatchAssetRemoved(const QAsset&)) );
	connect( model, SIGNAL(assetChanged(const QAsset&)), this, SLOT(dispatchAssetChanged(const QAsset&)) );
}

// ** QAssetsModelDispatcher::delegate
IAssetsModelDelegateWPtr QAssetsModelDispatcher::delegate( void ) const
{
	return m_delegate;
}

// ** QAssetsModelDispatcher::delegate
void QAssetsModelDispatcher::dispatchAssetAdded( const QAsset& asset )
{
	if( m_delegate.valid() ) {
		m_delegate->handleAssetAdded( extractAsset( asset ) );
	}
}

// ** QAssetsModelDispatcher::delegate
void QAssetsModelDispatcher::dispatchAssetRemoved( const QAsset& asset )
{
	if( m_delegate.valid() ) {
		m_delegate->handleAssetRemoved( extractAsset( asset ) );
	}
}

// ** QAssetsModelDispatcher::delegate
void QAssetsModelDispatcher::dispatchAssetChanged( const QAsset& asset )
{
	if( m_delegate.valid() ) {
		m_delegate->handleAssetChanged( extractAsset( asset ) );
	}
}

// ** QAssetsModelDispatcher::extractAsset
Asset QAssetsModelDispatcher::extractAsset( const QAsset& asset ) const
{
	// Clean the UUID
	QString uuid = asset.uuid().toString().replace( "{", "" ).replace( "}", "" ).replace( "-", "" );

	// Setup asset
	Asset result;
	result.absoluteFilePath = asset.absoluteFilePath().toStdString();
	result.ext				= asset.fileInfo().completeSuffix().toStdString();
	result.metaInfo			= io::Bson::object() << "uuid" << uuid.toStdString();

	return result;
}

// ----------------------------------------------------------- AssetsModel ----------------------------------------------------------- //

// ** AssetsModel::AssetsModel
AssetsModel::AssetsModel( QObject* parent ) : UserInterface( new QAssetsModel( parent ) )
{

}

// ** AssetsModel::setDelegate
void AssetsModel::setDelegate( IAssetsModelDelegateWPtr value )
{
	m_dispatcher = new QAssetsModelDispatcher( m_private.get(), value );
}

// ** AssetsModel::delegate
IAssetsModelDelegateWPtr AssetsModel::delegate( void ) const
{
	return m_dispatcher.get() ? m_dispatcher->delegate() : IAssetsModelDelegateWPtr();
}

#else

// ** AssetsModel::AssetsModel
AssetsModel::AssetsModel( QObject* parent, const String& path ) : m_isEditingModel( false )
{
	setRootPath( path.c_str() );
	setReadOnly( false );

	connect( this, SIGNAL(layoutChanged()), this, SLOT(layoutChanged()) );
	connect( this, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(assetsAdded(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(assetsAboutToBeRemoved(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(fileRenamed(const QString&, const QString&, const QString&)), this, SLOT(assetRenamed(const QString&, const QString&, const QString&)) );
}

// ** AssetsModel::mimeData
QMimeData* AssetsModel::mimeData( const QModelIndexList& indexes ) const
{
	QComposerMime* mime = new QComposerMime( "Dreemchest/Asset" );
	IMimeDataPtr   data = new MimeData( mime );

	mime->setMime( data );

	return mime;
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

// ** AssetsModel::layoutChanged
void AssetsModel::layoutChanged( void )
{
	printf( "Layout changed\n" );
}

// ** AssetsModel::assetsAdded
void AssetsModel::assetsAdded( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		QModelIndex idx	 = index( i, 0, parent );
		QFileInfo	info = fileInfo( idx );
		QString		path = info.absoluteFilePath();

		if( m_skipAddRemove.remove( path ) ) {
			continue;
		}

		printf( "Added: %s\n", info.absoluteFilePath().toStdString().c_str(), idx.internalPointer() );
	}
}

// ** AssetsModel::assetsAboutToBeRemoved
void AssetsModel::assetsAboutToBeRemoved( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		QModelIndex idx	 = index( i, 0, parent );
		QFileInfo	info = fileInfo( idx );
		QString		path = info.absoluteFilePath();

		if( m_skipAddRemove.remove( path ) ) {
			continue;
		}

		printf( "Removed: %s\n", info.absoluteFilePath().toStdString().c_str(), idx.internalPointer() );
	}
}

// ** AssetsModel::assetRenamed
void AssetsModel::assetRenamed( const QString& path, const QString& oldName, const QString& newName )
{
	m_skipAddRemove.insert( path + "/" + newName );
	m_skipAddRemove.insert( path + "/" + oldName );

	String oldn = (path + "/" + oldName).toStdString();
	String newn = (path + "/" + newName).toStdString();
	printf( "Renamed: %s -> %s\n", oldn.c_str(), newn.c_str() );
}

bool AssetsModel::beginMoveRows(const QModelIndex & sourceParent, int sourceFirst, int sourceLast, const QModelIndex & destinationParent, int destinationChild)
{
	DC_BREAK;
	return false;
}

#endif	/*	DEV_CUSTOM_ASSET_MODEL	*/

} // namespace Ui