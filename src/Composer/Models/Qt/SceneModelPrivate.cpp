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

#include "SceneModelPrivate.h"

#include "../../Widgets/Qt/MimeData.h"

DC_BEGIN_COMPOSER

// ** createSceneModel
SceneModelPtr createSceneModel( Scene::SceneWPtr scene )
{
	return new SceneModelPrivate( scene );
}

// ----------------------------------------------------- QSceneModel ----------------------------------------------------- //

// ** QSceneModel::QSceneModel
QSceneModel::QSceneModel( SceneModelPrivate* parentSceneModel, Scene::SceneWPtr scene, QObject* parent )
	: QGenericTreeModel( 1, parent ), m_parent( parentSceneModel ), m_scene( scene )
{
	m_scene->subscribe<Scene::Scene::SceneObjectAdded>( dcThisMethod( QSceneModel::handleSceneObjectAdded ) );
	m_scene->subscribe<Scene::Scene::SceneObjectRemoved>( dcThisMethod( QSceneModel::handleSceneObjectRemoved ) );
}

// ** QSceneModel::remove
void QSceneModel::remove( const QModelIndex& index )
{
	// First remove children
	for( s32 i = 0; i < rowCount( index ); i++ ) {
		remove( this->index( i, 0, index ) );
	}

	// Get the scene object by index
	Scene::SceneObjectWPtr sceneObject = dataAt( index );
	DC_BREAK_IF( !sceneObject.valid() );

	// Remove scene object
	m_scene->removeSceneObject( sceneObject );
}

// ** QSceneModel::flags
Qt::ItemFlags QSceneModel::flags( const QModelIndex& index ) const
{
	return QGenericTreeModel::flags( index ) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;
}

// ** QSceneModel::data
QVariant QSceneModel::data( const QModelIndex& index, int role ) const
{
	// Get the scene object by index
	Scene::SceneObjectWPtr sceneObject = dataAt( index );
	DC_BREAK_IF( !sceneObject.valid() );

	// Return the data according to requested role.
	switch( role ) {
	case Qt::DisplayRole:
	case Qt::EditRole:			return sceneObject->has<Scene::Identifier>() ? sceneObject->get<Scene::Identifier>()->name().c_str() : sceneObject->typeName();
	//case Qt::DecorationRole:	return m_iconProvider->icon( item->data() );
	}

	return QVariant();
}

// ** QSceneModel::setData
bool QSceneModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
	// Skip all roles except the editing one
	if( role != Qt::EditRole ) {
		return QGenericTreeModel::setData( index, value, role );
	}

	// Get the name from value
	String name = value.toString().toStdString();

	// Empty names are not allowed
	if( name.empty() ) {
		return false;
	}

	// Get the scene object by index
	Scene::SceneObjectWPtr sceneObject = dataAt( index );
	DC_BREAK_IF( !sceneObject.valid() );

	// Set the object identifier
	if( Scene::Identifier* identifier = sceneObject->has<Scene::Identifier>() ) {
		identifier->setName( name );
	} else {
		sceneObject->attach<Scene::Identifier>( name );
	}

	return true;
}

// ** QSceneModel::dropMimeData
bool QSceneModel::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent )
{
	// Check if we a trying to drop an asset.
	if( !data->hasFormat( Composer::kAssetMime.c_str() ) ) {
		return QGenericTreeModel::dropMimeData( data, action, row, column, parent );
	}

	// Get the acceptable drop action
	AssetAction assetAction = acceptableAssetAction( data, action, row, column, parent );

	if( !assetAction ) {
		return false;
	}

	// Get the target scene object.
	Scene::SceneObjectWPtr target = assetAction.sceneObject;

	switch( assetAction.type ) {
	case AssetAction::PlaceMesh:		break;
	case AssetAction::AssignMaterial:	{
											if( !target.valid() ) {
												return false;
											}

											// Cast asset to a material
											Scene::MaterialPtr material = castTo<Scene::Material>( assetAction.assets.begin()->get() );
											DC_BREAK_IF( !material.valid() );

											// Load material
											m_scene->system<Scene::AssetSystem>()->queueMaterialAsset( material );

											// Set mesh material
											target->get<Scene::StaticMesh>()->setMaterial( 0, material );
										}
										break;
	}

	return true;
}

#ifdef DC_QT5_ENABLED

// ** QSceneModel::canDropMimeData
bool QSceneModel::canDropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) const
{
	// Check if we a trying to drop an asset.
	if( !data->hasFormat( Composer::kAssetMime.c_str() ) ) {
		return QGenericTreeModel::canDropMimeData( data, action, row, column, parent );
	}

	// Get the acceptable drop action
	AssetAction assetAction = acceptableAssetAction( data, action, row, column, parent );

	if( !assetAction ) {
		return false;
	}

	return true;
}

#endif	/*	DC_QT5_ENABLED	*/

// ** QSceneModel::acceptableDropAction
QSceneModel::AssetAction QSceneModel::acceptableAssetAction( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) const
{
	// Extract an assets from MIME data
	Scene::AssetSet assets = Composer::instance()->assetsFromMime( new Ui::MimeData( data ) );
	
	// No valid asset found - can't drop
	if( assets.empty() ) {
		return AssetAction::Invalid;
	}

	// Get the target model index we are goind to drop an asset
	QModelIndex index = this->index( row, column, parent );
	qDebug() << index.isValid() << row << column << parent.isValid();

	// Get the scene object at current index
	//Scene::SceneObjectWPtr sceneObject = index.isValid() ? dataAt( index ) : Scene::SceneObjectWPtr();
	Scene::SceneObjectWPtr sceneObject = parent.isValid() ? dataAt( parent ) : Scene::SceneObjectWPtr();

	// Check if we have droppable assets
	AssetAction assetAction( AssetAction::Invalid, assets, sceneObject );

	for( Scene::AssetSet::const_iterator i = assets.begin(), end = assets.end(); i != end; i++ ) {
		switch( (*i)->type() ) {
		case Scene::Asset::Mesh:		assetAction.type = AssetAction::PlaceMesh;
										break;
		case Scene::Asset::Material:	if( !sceneObject.valid() || sceneObject->has<Scene::StaticMesh>() ) {
											assetAction.type = AssetAction::AssignMaterial;
										}
										break;
		}
	}

	return assetAction;
}

// ** QSceneModel::handleSceneObjectAdded
void QSceneModel::handleSceneObjectAdded( const Scene::Scene::SceneObjectAdded& e )
{
	// Ignore internal scene objects.
	if( e.sceneObject->has<Editors::SceneEditorInternal>() ) {
		return;
	}

	// Add scene object item.
	TreeItem* item = createItem( e.sceneObject );

	// Add it to model.
	addItem( item, NULL );
}

// ** QSceneModel::handleSceneObjectRemoved
void QSceneModel::handleSceneObjectRemoved( const Scene::Scene::SceneObjectRemoved& e )
{
	// Get the model index by scene object.
	QModelIndex idx = indexFromData( e.sceneObject );
	DC_BREAK_IF( !idx.isValid() );

	// Get item from index
	TreeItem* item = itemAtIndex( idx );

	// Remove data at index
	removeItem( item );
}

// ** QSceneModel::moveItem
bool QSceneModel::moveItem( Item* sourceParent, Item* destinationParent, Item* item, int destinationRow ) const
{
	// Get item transform
	Scene::TransformWPtr child = item->data()->get<Scene::Transform>();

	// Get the parent transform
	Scene::TransformWPtr parent = destinationParent ? destinationParent->data()->get<Scene::Transform>() : Scene::TransformWPtr();

	//! WORKAROUND: convert to local space of a new parent
	Vec3 position = child->worldSpacePosition() - (parent.valid() ? parent->worldSpacePosition() : Vec3( 0, 0, 0 ));
	Quat rotation = -(parent.valid() ? parent->rotation() : Quat()) * ((parent.valid() ? parent->rotation() : Quat()) * child->rotation());
	child->setPosition( position );
	child->setRotation( rotation );

	// Now change the parent transform of a child
	child->setParent( parent );

	return true;
}

// -------------------------------------------------- SceneModelPrivate -------------------------------------------------- //

// ** SceneModelPrivate::SceneModelPrivate
SceneModelPrivate::SceneModelPrivate( Scene::SceneWPtr scene ) : PrivateInterface( new QSceneModel( this, scene ) )
{

}

DC_END_COMPOSER