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

#include "SceneModel.h"

DC_BEGIN_COMPOSER

// ** SceneModel::SceneModel
SceneModel::SceneModel( Scene::Assets& assets, Scene::SceneWPtr scene, QObject* parent ) : GenericTreeModel( 1, parent ), m_assets( assets ), m_scene( scene )
{
    qCheckParent( parent )
	scene->subscribe<Scene::Scene::SceneObjectAdded>( dcThisMethod( SceneModel::handleSceneObjectAdded ) );
	scene->subscribe<Scene::Scene::SceneObjectRemoved>( dcThisMethod( SceneModel::handleSceneObjectRemoved ) );
}

// ** SceneModel::scene
Scene::SceneWPtr SceneModel::scene( void ) const
{
    return m_scene;
}

// ** SceneModel::remove
void SceneModel::remove( const QModelIndex& index )
{
	// First remove children
	for( s32 i = 0; i < rowCount( index ); i++ ) {
		remove( this->index( i, 0, index ) );
	}

	// Get the scene object by index
	Scene::SceneObjectWPtr sceneObject = dataAt( index );
	DC_BREAK_IF( !sceneObject.valid() );

	// Remove scene object
	scene()->removeSceneObject( sceneObject );
}

// ** SceneModel::flags
Qt::ItemFlags SceneModel::flags( const QModelIndex& index ) const
{
	return GenericTreeModel::flags( index ) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;
}

// ** SceneModel::data
QVariant SceneModel::data( const QModelIndex& index, int role ) const
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

// ** SceneModel::setData
bool SceneModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
	// Skip all roles except the editing one
	if( role != Qt::EditRole ) {
		return GenericTreeModel::setData( index, value, role );
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

// ** SceneModel::dropMimeData
bool SceneModel::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent )
{
	// Check if we a trying to drop an asset.
	if( !data->hasFormat( Composer::kAssetMime.c_str() ) ) {
		return GenericTreeModel::dropMimeData( data, action, row, column, parent );
	}

	// Extract an assets from MIME data
	Scene::AssetSet assets = qComposer->assetsFromMime( data );

	// Extract target sccene object from model index
	Scene::SceneObjectWPtr sceneObject = parent.isValid() ? dataAt( parent ) : Scene::SceneObjectWPtr();

	// Get the acceptable drop action
	AssetAction assetAction = acceptableAssetAction( assets, sceneObject, Vec3( 0, 0, 0 ) );

	if( !assetAction ) {
		return false;
	}

	// Perform an action.
	bool result = performAssetAction( assetAction );
	
	return result;
}

#ifdef DC_QT5_ENABLED

// ** SceneModel::canDropMimeData
bool SceneModel::canDropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) const
{
	// Check if we a trying to drop an asset.
	if( !data->hasFormat( Composer::kAssetMime.c_str() ) ) {
		return GenericTreeModel::canDropMimeData( data, action, row, column, parent );
	}

	// Extract an assets from MIME data
	Scene::AssetSet assets = qComposer->assetsFromMime( data );

	// Extract target sccene object from model index
	Scene::SceneObjectWPtr sceneObject = parent.isValid() ? dataAt( parent ) : Scene::SceneObjectWPtr();

	// Get the acceptable drop action
	AssetAction assetAction = acceptableAssetAction( assets, sceneObject, Vec3( 0, 0, 0 ) );

	if( !assetAction ) {
		return false;
	}

	return true;
}

#endif	/*	DC_QT5_ENABLED	*/

// ** SceneModel::handleSceneObjectAdded
void SceneModel::handleSceneObjectAdded( const Scene::Scene::SceneObjectAdded& e )
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

// ** SceneModel::handleSceneObjectRemoved
void SceneModel::handleSceneObjectRemoved( const Scene::Scene::SceneObjectRemoved& e )
{
	// Get the model index by scene object.
	QModelIndex idx = indexFromData( e.sceneObject );
	DC_BREAK_IF( !idx.isValid() );

	// Get item from index
	TreeItem* item = itemAtIndex( idx );

	// Remove data at index
	removeItem( item );
}

// ** SceneModel::moveItem
bool SceneModel::moveItem( Item* sourceParent, Item* destinationParent, Item* item, int destinationRow ) const
{
	changeSceneObjectParent( item->data(), destinationParent ? destinationParent->data() : Scene::SceneObjectWPtr() );
	return true;
}

// ** SceneModel::changeSceneObjectParent
void SceneModel::changeSceneObjectParent( Scene::SceneObjectWPtr sceneObject, Scene::SceneObjectWPtr parent ) const
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

// ** SceneModel::acceptableDropAction
SceneModel::AssetAction SceneModel::acceptableAssetAction( const Scene::AssetSet& assets, Scene::SceneObjectWPtr target, const Vec3& point ) const
{
	// No valid assets - can't drop
	if( assets.empty() ) {
		return AssetAction::Invalid;
	}

	// Check if we have droppable assets
	AssetAction assetAction( AssetAction::Invalid, assets, target, point );

	for( Scene::AssetSet::const_iterator i = assets.begin(), end = assets.end(); i != end; i++ ) {
        const Scene::AssetType& type = (*i)->type();

        if( type.is<Scene::Mesh>() ) {
            assetAction.type = AssetAction::PlaceMesh;
        }
        else if( type.is<Scene::Material>() ) {
            if( !target.valid() || target->has<Scene::StaticMesh>() ) {
                assetAction.type = AssetAction::AssignMaterial;
            }
        }
	}

	return assetAction;
}

// ** SceneModel::performAssetAction
bool SceneModel::performAssetAction( const AssetAction& action )
{
	// Get the target scene object.
	Scene::SceneObjectWPtr target = action.sceneObject;

	switch( action.type ) {
	case AssetAction::PlaceMesh:		{
											Scene::SceneObjectWPtr object = placeStaticMesh( *action.assets.begin(), action.point );
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

											applyMaterial( target, 0, *action.assets.begin() );
										}

										break;
	}

	return true;
}

// ** SceneModel::applyMaterial
void SceneModel::applyMaterial( Scene::SceneObjectWPtr target, s32 slot, Scene::MaterialHandle material )
{
	DC_BREAK_IF( !target.valid() );
	DC_BREAK_IF( !target->has<Scene::StaticMesh>() );
	DC_BREAK_IF( !material.isValid() );

	// Queue material for loading.
	//material->bundle()->queueForLoading( material );
    qWarning() << "SceneModel::applyMaterial : material is not queued for loading any more";

	// Set mesh material
	target->get<Scene::StaticMesh>()->setMaterial( slot, material );
}

// ** SceneModel::placeTerrain
Scene::SceneObjectWPtr SceneModel::placeTerrain( Scene::TerrainHandle terrain, const Vec3& point )
{
	// Get the scene.
	Scene::SceneWPtr scene = m_scene;

	// Create root scene object.
	Scene::SceneObjectPtr root = scene->createSceneObject();
    root->setSerializable( true );
	root->attach<Editors::SceneEditorInternal>( root );
	root->attach<Scene::Transform>( point.x, point.y, point.z, Scene::TransformWPtr() );
	root->attach<Scene::Identifier>( "Terrain" );
	scene->addSceneObject( root );

    DC_NOT_IMPLEMENTED

	// Add chunks
	//for( u32 z = 0; z < terrain->chunkCount(); z++ ) {
	//	for( u32 x = 0; x < terrain->chunkCount(); x++ ) {
	//		Scene::SceneObjectPtr chunk = scene->createSceneObject();
	//		Scene::Mesh		      mesh  = terrain->createChunkMesh( x, z );

	//		chunk->attach<Editors::SceneEditorInternal>( chunk, Editors::SceneEditorInternal::Private );
	//		chunk->attach<Scene::StaticMesh>( mesh );
	//		chunk->attach<Editors::TerrainChunk>( terrain, x, z );
	//		chunk->attach<Scene::Transform>( x * Scene::Terrain::kChunkSize, 0, z * Scene::Terrain::kChunkSize, root->get<Scene::Transform>() );
	//		
	//		scene->addSceneObject( chunk );
	//	}
	//}

	return root;
}

// ** SceneModel::placeStaticMesh
Scene::SceneObjectWPtr SceneModel::placeStaticMesh( Scene::MeshHandle mesh, const Vec3& point )
{
	DC_BREAK_IF( !mesh.isValid() );

#if 0
	// Construct scene object
	Scene::SceneObjectPtr sceneObject = m_scene->createSceneObject();
    sceneObject->setSerializable( true );
	sceneObject->attach<Scene::Identifier>( mesh->name() );
	sceneObject->attach<Scene::StaticMesh>( mesh );
	sceneObject->attach<Scene::Transform>( point.x, point.y, point.z, Scene::TransformWPtr() );
	sceneObject->attach<Editors::SceneEditorInternal>( sceneObject );
	m_scene->addSceneObject( sceneObject );

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
#else
    DC_NOT_IMPLEMENTED
    return Scene::SceneObjectWPtr();
#endif
}

DC_END_COMPOSER