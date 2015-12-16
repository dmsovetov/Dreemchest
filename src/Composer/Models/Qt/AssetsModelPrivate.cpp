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

#include "AssetsModelPrivate.h"

#include "../../Widgets/Qt/MimeData.h"
#include "../../Widgets/Qt/FileSystem.h"

DC_BEGIN_COMPOSER

// ----------------------------------------------------------- QAssetsModel ----------------------------------------------------------- //

#if DEV_CUSTOM_ASSET_MODEL

// ** QAssetsModel::QAssetsModel
QAssetsModel::QAssetsModel( AssetsModel* parentAssetsModel, QObject* parent )
	: QLocalFileSystemModel( parent ), m_parent( parentAssetsModel ), m_metaFileExtension( "asset" )
{
	connect( this, SIGNAL(fileAdded(const QFileInfo&)), this, SLOT(fileAdded(const QFileInfo&)) );
	connect( this, SIGNAL(fileRemoved(const QFileInfo&)), this, SLOT(fileRemoved(const QFileInfo&)) );
	connect( this, SIGNAL(fileChanged(const QFileInfo&)), this, SLOT(fileChanged(const QFileInfo&)) );
	connect( this, SIGNAL(fileRenamed(const QFileInfo&, const QString&, const QString&)), this, SLOT(fileRenamed(const QFileInfo&, const QString&, const QString&)) );
	connect( this, SIGNAL(fileMoved(const QFileInfo&, const QString&, const QString&)), this, SLOT(fileMoved(const QFileInfo&, const QString&, const QString&)) );
}

// ** QAssetsModel::fileAdded
void QAssetsModel::fileAdded( const QFileInfo& file )
{
	// Construct file info
	FileInfoPtr fileInfo( new Ui::FileInfoPrivate( file ) );

	// Dispatch an event
	m_parent->notify<AssetsModel::Added>( fileInfo );
}

// ** QAssetsModel::fileRemoved
void QAssetsModel::fileRemoved( const QFileInfo& file )
{
	qDebug() << file.absoluteFilePath();

	// Construct file info
	FileInfoPtr fileInfo( new Ui::FileInfoPrivate( file ) );

	// Dispatch an event
	m_parent->notify<AssetsModel::Removed>( fileInfo );

	// Remove meta file
	bool result = QDir().remove( metaFileName( fileInfo ) );
	Q_ASSERT( result );
}

// ** QAssetsModel::fileChanged
void QAssetsModel::fileChanged( const QFileInfo& file )
{
	// Construct file info
	FileInfoPtr fileInfo( new Ui::FileInfoPrivate( file ) );

	// Dispatch an event
	m_parent->notify<AssetsModel::Removed>( fileInfo );
}

// ** QAssetsModel::fileRenamed
void QAssetsModel::fileRenamed( const QFileInfo& file, const QString& oldFileName, const QString& newFileName )
{
	renameMetaFile( oldFileName, newFileName );
}

// ** QAssetsModel::fileMoved
void QAssetsModel::fileMoved( const QFileInfo& file, const QString& oldFileName, const QString& newFileName )
{
	renameMetaFile( oldFileName, newFileName );
}

// ** QAssetsModel::renameMetaFile
bool QAssetsModel::renameMetaFile( const QString& oldFileName, const QString& newFileName )
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

// ** QAssetsModel::uuid
String QAssetsModel::uuid( const FileInfoWPtr& assetFile ) const
{
	// Read the meta data
	Io::KeyValue data = metaData( assetFile );
	DC_BREAK_IF( !data.isObject() );

	if( !data.isObject() ) {
		return "";
	}

	return data.get( "uuid", "" ).asString();
}

// ** QAssetsModel::assetFile
FileInfoPtr QAssetsModel::assetFile( const QModelIndex& index ) const
{
	return FileInfoPtr( new Ui::FileInfoPrivate( fileInfo( index ) ) );
}

// ** QAssetsModel::setMetaData
void QAssetsModel::setMetaData( const FileInfoWPtr& assetFile, const Io::KeyValue& data )
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

// ** QAssetsModel::metaData
Io::KeyValue QAssetsModel::metaData( const FileInfoWPtr& assetFile ) const
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

// ** QAssetsModel::metaFileName
QString QAssetsModel::metaFileName( const FileInfoWPtr& assetFile ) const
{
	return metaFileNameFromPath( QString::fromStdString( assetFile->absolutePath() ) );
	return QString::fromStdString( assetFile->absolutePath() ) + "." + m_metaFileExtension;
}

// ** QAssetsModel::metaFileNameFromPath
QString QAssetsModel::metaFileNameFromPath( const QString& fileName ) const
{
	return fileName + "." + m_metaFileExtension;
}

// ** QAssetsModel::shouldSkipFile
bool QAssetsModel::shouldSkipFile( const QFileInfo& file ) const
{
	return file.completeSuffix().endsWith( m_metaFileExtension );
}

// ** QAssetsModel::mimeTypes
QStringList QAssetsModel::mimeTypes( void ) const
{
	return QStringList() << Composer::kAssetMime.c_str();
}

#else

// ** QAssetFileSystemModel::QAssetFileSystemModel
QAssetFileSystemModel::QAssetFileSystemModel( AssetsModel* parentAssetsModel, QObject* parent ) : QFileSystemModel( parent ), m_parent( parentAssetsModel ), m_isEditingModel( false ), m_metaFileExtension( "asset" )
{
	setReadOnly( false );

	connect( this, SIGNAL(directoryLoaded(const QString&)), this, SLOT(scanLoadedDirectory(const QString&)) );
	connect( this, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(assetsAdded(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(assetsRemoved(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(fileRenamed(const QString&, const QString&, const QString&)), this, SLOT(assetRenamed(const QString&, const QString&, const QString&)) );
}

// ** QAssetFileSystemModel::mimeData
//QStringList QAssetFileSystemModel::mimeTypes( void ) const 
//{
//	return QStringList() << QString::fromStdString( Composer::kAssetMime ) << QFileSystemModel::mimeTypes() ;
//}

// ** QAssetFileSystemModel::dropMimeData
bool QAssetFileSystemModel::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent )
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

// ** QAssetFileSystemModel::data
QVariant QAssetFileSystemModel::data( const QModelIndex& index, int role ) const
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

// ** QAssetFileSystemModel::setData
bool QAssetFileSystemModel::setData( const QModelIndex& index, const QVariant& value, int role )
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

// ** QAssetFileSystemModel::scanLoadedDirectory
void QAssetFileSystemModel::scanLoadedDirectory( const QString& path )
{
	QModelIndex parent = index( path );

	for( int i = 0; i < rowCount( parent ); i++ ) {
		QModelIndex child = index( i, 0, parent );

		if( !fileInfo( child ).isDir() ) {
			continue;
		}

		if( canFetchMore( child ) ) {
			fetchMore( child );
		}
	}
}

// ** QAssetFileSystemModel::assetsAdded
void QAssetFileSystemModel::assetsAdded( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		// Get index
		QModelIndex idx = index( i, 0, parent );

		// Get asset file info from index
		FileInfoPtr file = assetFile( idx );

		// This file was moved - skip it
		if( wasMoved( file ) ) {
			continue;
		}

		// Dispatch an event
		m_parent->notify<AssetsModel::Added>( file );
	}
}

// ** QAssetFileSystemModel::assetsRemoved
void QAssetFileSystemModel::assetsRemoved( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		QModelIndex idx	 = index( i, 0, parent );

		// Construct file info
		FileInfoPtr fileInfo = assetFile( idx );

		// Meta file does not exist - do nothing
		if( !QFile::exists( metaFileName( fileInfo ) ) ) {
			continue;
		}

		// Dispatch an event
		m_parent->notify<AssetsModel::Removed>( fileInfo );

		// Remove meta file
		bool result = QDir().remove( metaFileName( fileInfo ) );
		Q_ASSERT( result );
	}
}

// ** QAssetFileSystemModel::assetRenamed
void QAssetFileSystemModel::assetRenamed( const QString& path, const QString& oldName, const QString& newName )
{
	QString oldFileName = QDir::cleanPath( path + "/" + oldName );
	QString newFileName = QDir::cleanPath( path + "/" + newName );

	renameMetaFile( oldFileName, newFileName );
}

// ** QAssetFileSystemModel::wasMoved
bool QAssetFileSystemModel::wasMoved( const FileInfoWPtr& assetFile ) const
{
	DC_BREAK_IF( !assetFile.valid() );
	return m_wasMoved.remove( QString::fromStdString( assetFile->absolutePath() ) );
}

// ** QAssetFileSystemModel::renameMetaFile
bool QAssetFileSystemModel::renameMetaFile( const QString& oldFileName, const QString& newFileName )
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

// ** QAssetFileSystemModel::uuid
String QAssetFileSystemModel::uuid( const FileInfoWPtr& assetFile ) const
{
	// Read the meta data
	Io::KeyValue data = metaData( assetFile );
	DC_BREAK_IF( !data.isObject() );

	if( !data.isObject() ) {
		return "";
	}

	return data.get( "uuid", "" ).asString();
}

// ** QAssetFileSystemModel::assetFile
FileInfoPtr QAssetFileSystemModel::assetFile( const QModelIndex& index ) const
{
	return FileInfoPtr( new Ui::FileInfoPrivate( fileInfo( index ) ) );
}

// ** QAssetFileSystemModel::setMetaData
void QAssetFileSystemModel::setMetaData( const FileInfoWPtr& assetFile, const Io::KeyValue& data )
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

// ** QAssetFileSystemModel::metaData
Io::KeyValue QAssetFileSystemModel::metaData( const FileInfoWPtr& assetFile ) const
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

// ** QAssetFileSystemModel::metaFileName
QString QAssetFileSystemModel::metaFileName( const FileInfoWPtr& assetFile ) const
{
	return metaFileNameFromPath( QString::fromStdString( assetFile->absolutePath() ) );
	return QString::fromStdString( assetFile->absolutePath() ) + "." + m_metaFileExtension;
}

// ** QAssetFileSystemModel::metaFileNameFromPath
QString QAssetFileSystemModel::metaFileNameFromPath( const QString& fileName ) const
{
	return fileName + "." + m_metaFileExtension;
}

// ----------------------------------------------------------- QAssetsModel ----------------------------------------------------------- //

// ** QAssetsModel::QAssetsModel
QAssetsModel::QAssetsModel( AssetsModel* parentAssetsModel, QObject* parent ) : QSortFilterProxyModel( parent )
{
	m_model = new QAssetFileSystemModel( parentAssetsModel, this );
	setSourceModel( m_model.get() );
}

// ** QAssetsModel::filterAcceptsRow
bool QAssetsModel::filterAcceptsRow( int row, const QModelIndex& parent ) const
{
	QFileInfo file = m_model->fileInfo( m_model->index( row, 0, parent ) );

	if( file.suffix().toLower() == "asset" ) {
		return false;
	}

	return true;
}

// ** QAssetsModel::model
QAssetFileSystemModel* QAssetsModel::model( void ) const
{
	return m_model.get();
}

// ** QAssetsModel::root
QModelIndex QAssetsModel::root( void ) const
{
	return mapFromSource( m_model->index( m_model->rootPath() ) );
}

// ** QAssetsModel::assetFile
FileInfoPtr QAssetsModel::assetFile( const QModelIndex& index ) const
{
	return m_model->assetFile( mapToSource( index ) );
}

// ** QAssetsModel::remove
void QAssetsModel::remove( const QModelIndex& index )
{
	m_model->remove( mapToSource( index ) );
}

#endif	/*	DEV_CUSTOM_ASSET_MODEL	*/

// ** createAssetsModel
AssetsModelPtr createAssetsModel( void )
{
	return new AssetsModelPrivate;
}

// ----------------------------------------------------------- AssetsModelPrivate ----------------------------------------------------------- //

// ** AssetsModelPrivate::AssetsModelPrivate
AssetsModelPrivate::AssetsModelPrivate( QObject* parent ) : PrivateInterface( new QAssetsModel( this, parent ) )
{

}

// ** AssetsModelPrivate::setRootPath
void AssetsModelPrivate::setRootPath( const String& value )
{
	m_private->model()->blockSignals( true );
	m_private->model()->setRootPath( value.c_str() );
	m_private->model()->blockSignals( false );
}

// ** AssetsModelPrivate::setReadOnly
void AssetsModelPrivate::setReadOnly( bool value )
{
	m_private->model()->setReadOnly( value );
}

// ** AssetsModelPrivate::uuid
String AssetsModelPrivate::uuid( const FileInfoWPtr& assetFile ) const
{
	return m_private->model()->uuid( assetFile );
}

// ** AssetsModelPrivate::setMetaData
void AssetsModelPrivate::setMetaData( const FileInfoWPtr& assetFile, const Io::KeyValue& data )
{
	m_private->model()->setMetaData( assetFile, data );
}

// ** AssetsModelPrivate::metaData
Io::KeyValue AssetsModelPrivate::metaData( const FileInfoWPtr& assetFile ) const
{
	return m_private->model()->metaData( assetFile );
}

// ** AssetsModelPrivate::metaData
Io::KeyValue AssetsModelPrivate::metaData( const String& assetFileName ) const
{
	return metaData( new Ui::FileInfoPrivate( QFileInfo( assetFileName.c_str() ) ) );
}

DC_END_COMPOSER