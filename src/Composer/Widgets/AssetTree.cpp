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

#include "Document.h"
#include "Inspector.h"
#include "Menu.h"

#include "../Models/AssetModels.h"
#include "../Models/PropertyModel.h"
#include "../Models/AssetFileSystemModel.h"
#include "../Project/Project.h"
#include "../FileSystem.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ------------------------------------------------ AssetSelector ------------------------------------------------ //

// ** AssetSelector::AssetSelector
AssetSelector::AssetSelector( u32 mask, QWidget* parent ) : QWidget( parent ), m_mask( mask )
{
    DC_NOT_IMPLEMENTED
//	qRegisterMetaType<Scene::AssetWPtr>( "Scene::AssetWPtr" );

	m_line = new QLineEdit( this );
	m_line->installEventFilter( this );
	m_line->setReadOnly( true );

	QHBoxLayout* layout = new QHBoxLayout( this );
	layout->addWidget( m_line );
	layout->addWidget( new QToolButton );
	layout->setMargin( 0 );
	layout->setSpacing( 1 );
}

// ** AssetSelector::eventFilter
bool AssetSelector::eventFilter( QObject* target, QEvent* e )
{
    switch( e->type() ) {
    case QEvent::DragEnter: {
								QDragEnterEvent* de = static_cast<QDragEnterEvent*>( e );

								// MIME data does not contain an accepted format.
								if( !de->mimeData()->hasFormat( Composer::kAssetMime.c_str() ) ) {
									return true;
								}

								// Only single assets are accepted
								if( de->mimeData()->urls().size() > 1 ) {
									return true;
								}

                            #if 0
								// Decode asset
								Scene::AssetPtr asset = qComposer->assetFromMime( de->mimeData() );

								// Check asset type
								if( !asset.valid() || (asset->type() & m_mask) == 0 ) {
									return true;
								}
                            #else
                                DC_NOT_IMPLEMENTED
                            #endif

								// Accept this action
								de->acceptProposedAction();
							}
							return true;

    case QEvent::Drop:		{
								QDropEvent* de = static_cast<QDropEvent*>( e );

                            #if 0
								// Decode asset
								Scene::AssetWPtr asset = qComposer->assetFromMime( de->mimeData() );
								
								// Set the value
								setValue( asset );
                            #else
                                DC_NOT_IMPLEMENTED
                            #endif

								// Emit the signal
								emit valueChanged();
							}
							return true;
    }

    return false;
}

// ** AssetSelector::assetChanged
Scene::Asset AssetSelector::value( void ) const
{
	return m_asset;
}

// ** AssetSelector::setValue
void AssetSelector::setValue( const Scene::Asset& value )
{
#if 0
	// Save the asset pointer
	m_asset = value;

	// Update the text field
	m_line->setText( m_asset.valid() ? QString::fromStdString( m_asset->name() ) : "" );
#else
    DC_NOT_IMPLEMENTED
#endif
}

// ------------------------------------------------ AssetTree ------------------------------------------------ //

// ** AssetTree::AssetTree
AssetTree::AssetTree( ProjectQPtr project, QWidget* parent ) : QTreeView( parent ), m_project( project ), m_selectionChanged( false )
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
	connect( selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)) );
}

// ** AssetTree::model
AssetFileSystemModelQPtr AssetTree::model( void ) const
{
    return m_proxy->model();
}

// ** AssetTree::setModel
void AssetTree::setModel( AssetFileSystemModelQPtr value )
{
	m_proxy = new FilteredAssetsModel( value, this );
	QTreeView::setModel( m_proxy );

    // Set the root index
	setRootIndex( m_proxy->root() );

    // Hide columns
	for( s32 i = 1; i < m_proxy->columnCount(); i++ ) {
		setColumnHidden( i, true );
	}
}

// ** AssetTree::keyPressEvent
void AssetTree::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() ) {
    case Qt::Key_Delete:	foreach( QModelIndex idx, selectedIndexes() ) {
								m_proxy->remove( idx );
							}
							break;
    }

    QTreeView::keyPressEvent( event );
}

// ** AssetTree::contextMenuEvent
void AssetTree::contextMenuEvent( QContextMenuEvent *e )
{
	MenuQPtr menu = new Menu( this );

    m_project->fillAssetMenu( menu, this );
    menu->exec( e->globalPos() );
}

// ** AssetTree::itemDoubleClicked
void AssetTree::itemDoubleClicked( const QModelIndex& index )
{
	// Get the file info by index
	FileInfo file = m_proxy->assetFile( index );

	if( file.isDir() ) {
		return;
	}

	// Read the corresponding meta data
	Io::KeyValue data = model()->metaData( file );

	if( !data.isObject() ) {
		return;
	}

	// Open the asset editor
	m_project->edit( data["uuid"].asString(), file );
}

// ** AssetTree::mousePressEvent
void AssetTree::mousePressEvent( QMouseEvent* e )
{
	// Reset the flag
	m_selectionChanged = false;

	// Run the base class method
	QTreeView::mousePressEvent( e );
}

// ** AssetTree::mouseReleaseEvent
void AssetTree::mouseReleaseEvent( QMouseEvent* e )
{
	QTreeView::mouseReleaseEvent( e );

	// The selection was not changed - skip
	if( !m_selectionChanged ) {
		return;
	}

	// Bind to an object inspector
	bindToInspector( selectedIndexes() );

	// Reset the flag
	m_selectionChanged = false;
}

// ** AssetTree::selectionChanged
void AssetTree::selectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
{
	m_selectionChanged = true;
}

// ** AssetTree::bindToInspector
void AssetTree::bindToInspector( const QModelIndexList& indexes )
{
	// More that one item selected - do nothing
	if( indexes.size() > 1 ) {
		return;
	}

	// Get the inspector widget
	InspectorQPtr inspector = qMainWindow->inspector();

	// The asset was deselected - hide inspector
	if( indexes.empty() ) {
		inspector->setModel( NULL );
		return;
	}

	// Get the asset file by index
	FileInfo file = m_proxy->assetFile( indexes[0] );

	// No meta data found - skip
	if( !model()->hasMetaData( file ) ) {
		return;
	}

	// Extract the UUID from file asset
	String uuid = model()->uuid( file );

#if 0
	// Find asset by UUID
	Scene::AssetWPtr asset = m_project->assets()->findAsset( uuid );
	DC_BREAK_IF( !asset.valid() );
	
	// Bind the selected asset to an object inspector.
	switch( asset->type() ) {
	case Scene::Asset::Material:	inspector->setModel( new MaterialModel( castTo<Scene::Material>( asset.get() ), this ) );
									break;
	default:						inspector->setModel( NULL );
	}
#else
    DC_NOT_IMPLEMENTED
#endif
}

// ** AssetTree::selection
FileInfoArray AssetTree::selection( void ) const
{
	FileInfoArray result;

	foreach( QModelIndex idx, selectedIndexes() ) {
		result.push_back( m_proxy->assetFile( idx ) );
	}

	return result;
}

// ** AssetTree::expandSelectedItems
void AssetTree::expandSelectedItems( void )
{
	foreach( QModelIndex idx, selectedIndexes() ) {
		expand( idx );
	}
}

} // namespace Ui

DC_END_COMPOSER