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

#include "AssetTree.h"

#include "Menu.h"
#include "FileSystem.h"
#include "Document.h"
#include "../../Models/Qt/AssetsModelPrivate.h"
#include "../../Project/Project.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ------------------------------------------------ AssetTree ------------------------------------------------ //

// ** AssetTree::AssetTree
AssetTree::AssetTree( Project::ProjectWPtr project ) : PrivateInterface( new QAssetTree( project ) )
{
	m_private->setParent( this );
}

// ** AssetTree::selection
StringArray AssetTree::selection( void ) const
{
	return m_private->selection();
}

// ** AssetTree::expandSelectedItems
void AssetTree::expandSelectedItems( void )
{
	return m_private->expandSelectedItems();
}

// ** AssetTree::setModel
void AssetTree::setModel( AssetsModelWPtr value )
{
	m_private->setModel( value );
}

// ------------------------------------------------ QAssetTree ------------------------------------------------ //

// ** QAssetTree::QAssetTree
QAssetTree::QAssetTree( Project::ProjectWPtr project ) : m_project( project )
{
	setHeaderHidden( true );
	setDragEnabled( true );
	setDropIndicatorShown( true );
	setDragDropOverwriteMode( true );
	setSelectionMode( ExtendedSelection );
	setDragDropMode( InternalMove );
	setEditTriggers( EditTrigger::EditKeyPressed );
	viewport()->setAcceptDrops( true );

	connect( this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(itemDoubleClicked(const QModelIndex&) ) );
}

// ** QAssetTree::setParent
void QAssetTree::setParent( IAssetTreeWPtr value )
{
	m_parent = value;
}

// ** QAssetTree::setModel
void QAssetTree::setModel( AssetsModelWPtr value )
{
	m_model = value;
	QTreeView::setModel( m_model->privateInterface<QAssetsModel>() );
}

// ** QAssetTree::keyPressEvent
void QAssetTree::keyPressEvent( QKeyEvent *event )
{
	QAssetsModel* model = m_model->privateInterface<QAssetsModel>();

    switch( event->key() ) {
    case Qt::Key_Delete:	foreach( QModelIndex idx, selectedIndexes() ) {
								model->remove( idx );
							}
							break;
    }

    QTreeView::keyPressEvent( event );
}

// ** QAssetTree::contextMenuEvent
void QAssetTree::contextMenuEvent( QContextMenuEvent *e )
{
	IMenuPtr menu( new Menu( this ) );

    m_project->fillAssetMenu( menu, m_parent );
    menu->exec( e->globalPos().x(), e->globalPos().y() );
}

// ** QAssetTree::itemDoubleClicked
void QAssetTree::itemDoubleClicked( const QModelIndex& index )
{
	QAssetsModel* model = m_model->privateInterface<QAssetsModel>();

	if( model->isDir( index ) ) {
		return;
	}

	// Get an asset from index
	const QAsset& asset = model->asset( index );

	// Convert the asset info
	Asset assetInfo;
	assetInfo.absoluteFilePath = asset.absoluteFilePath().toStdString();
	assetInfo.ext			   = asset.fileInfo().completeSuffix().toStdString();
	assetInfo.name			   = asset.name().toStdString();
	assetInfo.metaInfo		   = io::Bson::object() << "uuid" << asset.uuid().toString().toStdString();

	// Open the asset editor
	m_project->edit( assetInfo );
}

// ** QAssetTree::selection
StringArray QAssetTree::selection( void ) const
{
	QAssetsModel* model = m_model->privateInterface<QAssetsModel>();

	StringArray result;

	foreach( QModelIndex idx, selectedIndexes() ) {
		result.push_back( model->asset( idx ).absoluteFilePath().toStdString() );
	}

	return result;
}

// ** QAssetTree::expandSelectedItems
void QAssetTree::expandSelectedItems( void )
{
	foreach( QModelIndex idx, selectedIndexes() ) {
		expand( idx );
	}
}

} // namespace Ui

DC_END_COMPOSER