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
#include "../../Project/Project.h"

namespace Ui {

// ------------------------------------------------ AssetTree ------------------------------------------------ //

// ** AssetTree::AssetTree
AssetTree::AssetTree( Project::ProjectWPtr project, QFileSystemModel* model ) : UserInterface( new QAssetTree( project, model ) )
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

// ------------------------------------------------ QAssetTree ------------------------------------------------ //

// ** QAssetTree::QAssetTree
QAssetTree::QAssetTree( Project::ProjectWPtr project, QFileSystemModel* model ) : m_project( project ), m_model( model )
{
	setModel( m_model );
	setRootIndex( m_model->index( m_model->rootPath() ) );
	setHeaderHidden( true );
	setDragEnabled( true );
	setDropIndicatorShown( true );
	setSelectionMode( ExtendedSelection );
	setDragDropMode( InternalMove );
	setEditTriggers( EditTrigger::EditKeyPressed );
	viewport()->setAcceptDrops( true );

	// Hide all columns except the first one
	for( s32 i = 1, n = m_model->columnCount(); i < n; i++ ) {
		hideColumn( i );
	}
}

// ** QAssetTree::setParent
void QAssetTree::setParent( IAssetTreeWPtr value )
{
	m_parent = value;
}

// ** QAssetTree::keyPressEvent
void QAssetTree::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() ) {
    case Qt::Key_Delete:	foreach( QModelIndex idx, selectedIndexes() ) {
								m_model->remove( idx );
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

// ** QAssetTree::selection
StringArray QAssetTree::selection( void ) const
{
	StringArray result;

	foreach( QModelIndex idx, selectedIndexes() ) {
		result.push_back( m_model->fileInfo( idx ).absoluteFilePath().toAscii().constData() );
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