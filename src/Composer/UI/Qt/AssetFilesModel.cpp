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

#include "AssetFilesModel.h"
#include "MimeData.h"

namespace Ui {

// ** AssetFilesModel::AssetFilesModel
AssetFilesModel::AssetFilesModel( QObject* parent, const String& path ) : m_isEditingModel( false )
{
	setRootPath( path.c_str() );
	setReadOnly( false );

	connect( this, SIGNAL(layoutChanged()), this, SLOT(layoutChanged()) );
	connect( this, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(assetsAdded(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(assetsAboutToBeRemoved(const QModelIndex&, int, int)) );
	connect( this, SIGNAL(fileRenamed(const QString&, const QString&, const QString&)), this, SLOT(assetRenamed(const QString&, const QString&, const QString&)) );
}

// ** AssetFilesModel::mimeData
QMimeData* AssetFilesModel::mimeData( const QModelIndexList& indexes ) const
{
	QComposerMime* mime = new QComposerMime( "Dreemchest/Asset" );
	IMimeDataPtr   data = new MimeData( mime );

	mime->setMime( data );

	return mime;
}

// ** AssetFilesModel::data
QVariant AssetFilesModel::data( const QModelIndex& index, int role ) const
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

// ** AssetFilesModel::setData
bool AssetFilesModel::setData( const QModelIndex& index, const QVariant& value, int role )
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

// ** AssetFilesModel::layoutChanged
void AssetFilesModel::layoutChanged( void )
{
	printf( "Layout changed\n" );
}

// ** AssetFilesModel::assetsAdded
void AssetFilesModel::assetsAdded( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		QModelIndex idx	 = index( i, 0, parent );
		QFileInfo	info = fileInfo( idx );
		QString		path = info.absoluteFilePath();

		if( m_skipAddRemove.remove( path ) ) {
			continue;
		}

		printf( "Added: %s\n", info.absoluteFilePath().toAscii().constData(), idx.internalPointer() );
	}
}

// ** AssetFilesModel::assetsAboutToBeRemoved
void AssetFilesModel::assetsAboutToBeRemoved( const QModelIndex& parent, int start, int end )
{
	for( s32 i = start; i <= end; i++ ) {
		QModelIndex idx	 = index( i, 0, parent );
		QFileInfo	info = fileInfo( idx );
		QString		path = info.absoluteFilePath();

		if( m_skipAddRemove.remove( path ) ) {
			continue;
		}

		printf( "Removed: %s\n", info.absoluteFilePath().toAscii().constData(), idx.internalPointer() );
	}
}

// ** AssetFilesModel::assetRenamed
void AssetFilesModel::assetRenamed( const QString& path, const QString& oldName, const QString& newName )
{
	m_skipAddRemove.insert( path + "/" + newName );
	m_skipAddRemove.insert( path + "/" + oldName );

	String oldn = (path + "/" + oldName).toAscii().constData();
	String newn = (path + "/" + newName).toAscii().constData();
	printf( "Renamed: %s -> %s\n", oldn.c_str(), newn.c_str() );
}

bool AssetFilesModel::beginMoveRows(const QModelIndex & sourceParent, int sourceFirst, int sourceLast, const QModelIndex & destinationParent, int destinationChild)
{
	DC_BREAK;
	return false;
}

} // namespace Ui