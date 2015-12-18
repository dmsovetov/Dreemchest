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

#include "SceneTree.h"

#include "Menu.h"
#include "../../Models/Qt/SceneModelPrivate.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ------------------------------------------------ SceneTree ------------------------------------------------ //

// ** SceneTree::SceneTree
SceneTree::SceneTree( void ) : PrivateInterface( new QSceneTree( this ) )
{

}

// ** SceneTree::setModel
void SceneTree::setModel( SceneModelWPtr value )
{
	m_private->setModel( value );
}

// ------------------------------------------------ QSceneTree ------------------------------------------------ //

// ** QSceneTree::QSceneTree
QSceneTree::QSceneTree( SceneTree* parent ) : m_parent( parent )
{
	setHeaderHidden( true );
	setEditTriggers( EditTrigger::EditKeyPressed );
	setSelectionMode( ExtendedSelection );

	setDragEnabled( true );
	setDropIndicatorShown( true );
	setDragDropOverwriteMode( true );
	setDragDropMode( InternalMove );
	viewport()->setAcceptDrops( true );

	connect( this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(itemDoubleClicked(const QModelIndex&) ) );
}

// ** QSceneTree::setModel
void QSceneTree::setModel( SceneModelWPtr value )
{
	m_model = value;
	QTreeView::setModel( m_model.valid() ? m_model->privateInterface<QSceneModel>() : NULL );
}

// ** QSceneTree::keyPressEvent
void QSceneTree::keyPressEvent( QKeyEvent *event )
{
	QSceneModel* model = m_model->privateInterface<QSceneModel>();

    switch( event->key() ) {
    case Qt::Key_Delete:	foreach( QModelIndex idx, selectedIndexes() ) {
								model->remove( idx );
							}
							break;
    }

    QTreeView::keyPressEvent( event );
}

// ** QSceneTree::contextMenuEvent
void QSceneTree::contextMenuEvent( QContextMenuEvent *e )
{
	IMenuPtr menu( new Menu( this ) );

//    m_project->fillAssetMenu( menu, m_parent );
    menu->exec( e->globalPos().x(), e->globalPos().y() );
}

// ** QSceneTree::itemDoubleClicked
void QSceneTree::itemDoubleClicked( const QModelIndex& index )
{
	QSceneModel* model = m_model->privateInterface<QSceneModel>();

	// Get the scene object by index.
	Scene::SceneObjectWPtr sceneObject = model->dataAt( index );

	// Emit the event.
	m_parent->notify<ISceneTree::DoubleClicked>( sceneObject );
}

} // namespace Ui

DC_END_COMPOSER