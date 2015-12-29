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
SceneModelPtr createSceneModel( Scene::AssetBundleWPtr assets, Scene::SceneWPtr scene )
{
	return new SceneModelPrivate( assets, scene );
}

// ----------------------------------------------------- QSceneModel ----------------------------------------------------- //

// ** QSceneModel::QSceneModel
QSceneModel::QSceneModel( SceneModelPrivate* parentSceneModel, Scene::SceneWPtr scene, QObject* parent )
	: QGenericTreeModel( 1, parent ), m_parent( parentSceneModel ), m_scene( scene )
{
	m_scene->subscribe<Scene::Scene::SceneObjectAdded>( dcThisMethod( QSceneModel::handleSceneObjectAdded ) );
	m_scene->subscribe<Scene::Scene::SceneObjectRemoved>( dcThisMethod( QSceneModel::handleSceneObjectRemoved ) );
}

// ** QSceneModel::scene
Scene::SceneWPtr QSceneModel::scene( void ) const
{
	return m_scene;
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

	// Extract an assets from MIME data
	Scene::AssetSet assets = Composer::instance()->assetsFromMime( new Ui::MimeData( data ) );

	// Extract target sccene object from model index
	Scene::SceneObjectWPtr sceneObject = parent.isValid() ? dataAt( parent ) : Scene::SceneObjectWPtr();

	// Get the acceptable drop action
	SceneModel::AssetAction assetAction = m_parent->acceptableAssetAction( assets, sceneObject, Vec3( 0, 0, 0 ) );

	if( !assetAction ) {
		return false;
	}

	// Perform an action.
	bool result = m_parent->performAssetAction( assetAction );
	
	return result;
}

#ifdef DC_QT5_ENABLED

// ** QSceneModel::canDropMimeData
bool QSceneModel::canDropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) const
{
	// Check if we a trying to drop an asset.
	if( !data->hasFormat( Composer::kAssetMime.c_str() ) ) {
		return QGenericTreeModel::canDropMimeData( data, action, row, column, parent );
	}

	// Extract an assets from MIME data
	Scene::AssetSet assets = Composer::instance()->assetsFromMime( new Ui::MimeData( data ) );

	// Extract target sccene object from model index
	Scene::SceneObjectWPtr sceneObject = parent.isValid() ? dataAt( parent ) : Scene::SceneObjectWPtr();

	// Get the acceptable drop action
	SceneModel::AssetAction assetAction = m_parent->acceptableAssetAction( assets, sceneObject, Vec3( 0, 0, 0 ) );

	if( !assetAction ) {
		return false;
	}

	return true;
}

#endif	/*	DC_QT5_ENABLED	*/

// ** QSceneModel::handleSceneObjectAdded
void QSceneModel::handleSceneObjectAdded( const Scene::Scene::SceneObjectAdded& e )
{
	DC_BREAK_IF( !e.sceneObject->has<Editors::SceneEditorInternal>() );

	// Ignore private scene objects.
	if( e.sceneObject->has<Editors::SceneEditorInternal>()->isPrivate() ) {
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
	m_parent->changeSceneObjectParent( item->data(), destinationParent ? destinationParent->data() : Scene::SceneObjectWPtr() );
	return true;
}

// -------------------------------------------------- SceneModelPrivate -------------------------------------------------- //

// ** SceneModelPrivate::SceneModelPrivate
SceneModelPrivate::SceneModelPrivate( Scene::AssetBundleWPtr assets, Scene::SceneWPtr scene ) : PrivateInterface( new QSceneModel( this, scene ) ), m_assets( assets )
{

}

// ** SceneModelPrivate::changeSceneObjectParent
void SceneModelPrivate::changeSceneObjectParent( Scene::SceneObjectWPtr sceneObject, Scene::SceneObjectWPtr parent )
{
	DC_BREAK_IF( !sceneObject.valid() );

	// Get item transform
	Scene::TransformWPtr childTransform = sceneObject->get<Scene::Transform>();

	// Get the parent transform
	Scene::TransformWPtr parentTransform = parent.valid() ? parent->get<Scene::Transform>() : Scene::TransformWPtr();

	//! WORKAROUND: convert to local space of a new parent
	Vec3 position = childTransform->worldSpacePosition() - (parent.valid() ? parentTransform->worldSpacePosition() : Vec3( 0, 0, 0 ));
	Quat rotation = -(parent.valid() ? parentTransform->rotation() : Quat()) * ((parent.valid() ? parentTransform->rotation() : Quat()) * childTransform->rotation());
	childTransform->setPosition( position );
	childTransform->setRotation( rotation );

	// Now change the parent transform of a child
	childTransform->setParent( parentTransform );
}

// ** SceneModelPrivate::acceptableDropAction
SceneModel::AssetAction SceneModelPrivate::acceptableAssetAction( const Scene::AssetSet& assets, Scene::SceneObjectWPtr target, const Vec3& point ) const
{
	// No valid assets - can't drop
	if( assets.empty() ) {
		return AssetAction::Invalid;
	}

	// Check if we have droppable assets
	AssetAction assetAction( AssetAction::Invalid, assets, target, point );

	for( Scene::AssetSet::const_iterator i = assets.begin(), end = assets.end(); i != end; i++ ) {
		switch( (*i)->type() ) {
		case Scene::Asset::Mesh:		assetAction.type = AssetAction::PlaceMesh;
										break;

		case Scene::Asset::Material:	if( !target.valid() || target->has<Scene::StaticMesh>() ) {
											assetAction.type = AssetAction::AssignMaterial;
										}
										break;
		}
	}

	return assetAction;
}

// ** SceneModelPrivate::performAssetAction
bool SceneModelPrivate::performAssetAction( const AssetAction& action )
{
	// Get the target scene object.
	Scene::SceneObjectWPtr target = action.sceneObject;

	switch( action.type ) {
	case AssetAction::PlaceMesh:		{
											Scene::SceneObjectWPtr object = placeStaticMesh( castTo<Scene::Mesh>( action.assets.begin()->get() ), action.point );
											DC_BREAK_IF( !object.valid() );

											if( target.valid() ) {
												object->get<Scene::Transform>()->setParent( target->get<Scene::Transform>() );
											}
										}
										break;
	case AssetAction::AssignMaterial:	{
											if( !target.valid() ) {
												return false;
											}

											applyMaterial( target, 0, castTo<Scene::Material>( action.assets.begin()->get() ) );
										}

										break;
	}

	return true;
}

// ** SceneModelPrivate::applyMaterial
void SceneModelPrivate::applyMaterial( Scene::SceneObjectWPtr target, s32 slot, Scene::MaterialWPtr material )
{
	DC_BREAK_IF( !target.valid() );
	DC_BREAK_IF( !target->has<Scene::StaticMesh>() );
	DC_BREAK_IF( !material.valid() );

	// Queue material for loading.
	material->bundle()->queueForLoading( material );

	// Set mesh material
	target->get<Scene::StaticMesh>()->setMaterial( slot, material );
}

// ** SceneModelPrivate::placeTerrain
Scene::SceneObjectWPtr SceneModelPrivate::placeTerrain( Scene::TerrainWPtr terrain, const Vec3& point )
{
	// Get the scene.
	Scene::SceneWPtr scene = m_private->scene();

	// Create root scene object.
	Scene::SceneObjectPtr root = scene->createSceneObject();
	root->attach<Editors::SceneEditorInternal>( root );
	root->attach<Scene::Transform>( point.x, point.y, point.z, Scene::TransformWPtr() );
	root->attach<Scene::Identifier>( "Terrain" );
	scene->addSceneObject( root );

	// Add chunks
	for( u32 z = 0; z < terrain->chunkCount(); z++ ) {
		for( u32 x = 0; x < terrain->chunkCount(); x++ ) {
			Scene::SceneObjectPtr chunk = scene->createSceneObject();
			Scene::MeshPtr		  mesh  = terrain->createChunkMesh( x, z );

			chunk->attach<Editors::SceneEditorInternal>( chunk, Editors::SceneEditorInternal::Private );
			chunk->attach<Scene::StaticMesh>( mesh );
			chunk->attach<Editors::TerrainChunk>( terrain, x, z );
			chunk->attach<Scene::Transform>( x * Scene::Terrain::kChunkSize, 0, z * Scene::Terrain::kChunkSize, root->get<Scene::Transform>() );
			
			scene->addSceneObject( chunk );
		}
	}

	return root;
}

// ** SceneModelPrivate::placeStaticMesh
Scene::SceneObjectWPtr SceneModelPrivate::placeStaticMesh( Scene::MeshWPtr mesh, const Vec3& point )
{
	DC_BREAK_IF( !mesh.valid() );

	// Get the scene
	Scene::SceneWPtr scene = m_private->scene();

	// Load the mesh
	scene->system<Scene::AssetSystem>()->load( mesh );

	// Construct scene object
	Scene::SceneObjectPtr sceneObject = scene->createSceneObject();
	sceneObject->attach<Scene::Identifier>( mesh->name() );
	sceneObject->attach<Scene::StaticMesh>( mesh );
	sceneObject->attach<Scene::Transform>( point.x, point.y, point.z, Scene::TransformWPtr() );
	sceneObject->attach<Editors::SceneEditorInternal>( sceneObject );
	scene->addSceneObject( sceneObject );

	// Get all materials
	Set<Scene::MaterialPtr> materials = m_assets->findByType<Scene::Material>();

	// Apply materials
	for( s32 i = 0; i < mesh->chunkCount(); i++ ) {
		// Get chunk texture name
		const String& texture = mesh->texture( i );

		// Find material by name
		for( Set<Scene::MaterialPtr>::const_iterator j = materials.begin(), end = materials.end(); j != end; ++j ) {
			if( j->get()->name().find( texture + ".material" ) == String::npos ) {
				continue;
			}

			sceneObject->get<Scene::StaticMesh>()->setMaterial( i, *j );
			break;
		}

		if( !sceneObject->get<Scene::StaticMesh>()->material( i ).valid() ) {
			qDebug() << "Material" << texture.c_str() << "not found";
		}
	}

	return sceneObject;
}

DC_END_COMPOSER