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

#include "SceneEditor.h"
#include "../Project/Project.h"

DC_BEGIN_COMPOSER

namespace Editors {

// ** SceneEditor::initialize
bool SceneEditor::initialize( Project::ProjectWPtr project, const Scene::AssetPtr& asset, Ui::IDocumentWPtr document )
{
	if( !VisualEditor::initialize( project, asset, document ) ) {
		return false;
	}

	// Create rendering context.
	m_renderingContext = Scene::RenderingContext::create( hal() );

	// Create cursor binding
	m_cursorMovement = new Scene::Vec3Binding;

	// Create the scene.
	m_scene = Scene::Scene::create();
	m_scene->addSystem<Scene::AssetSystem>( m_project->assets() );

	// Create terrain.
	Scene::TerrainPtr terrain = new Scene::Terrain( m_project->assets().get(), "terrain", "terrain", 128 );

	{
		Scene::SceneObjectPtr chunk = m_scene->createSceneObject();
		Scene::MeshPtr		  mesh  = terrain->createMesh();
		chunk->attach<Scene::StaticMesh>( mesh );
		chunk->attach<Scene::Transform>();
	}

/*	for( u32 i = 0; i < terrain->chunkCount(); i++ ) {
		for( u32 j = 0; j < terrain->chunkCount(); j++ ) {
			Scene::SceneObjectPtr chunk = m_scene->createSceneObject();
			Scene::MeshPtr		  mesh  = terrain->createChunkMesh( j, i );

			chunk->attach<Scene::StaticMesh>( mesh );
			chunk->attach<Scene::Transform>( j * Scene::Terrain::kChunkSize, -4, i * Scene::Terrain::kChunkSize, Scene::Transform::WPtr() );
		}
	}*/

	// Create the camera.
	m_camera = Scene::SpectatorCamera::create( FrameTarget::create( document->renderingFrame() ), m_cursorMovement );
	m_camera->setMovementEnabled( false );
	m_camera->setRotationEnabled( false );
	m_camera->setClearColor( backgroundColor() );
	m_camera->attach<Scene::RenderWireframe>();
	//m_camera->attach<Scene::RenderForwardLit>();
	m_camera->attach<Scene::RenderBoundingVolumes>();
	m_scene->addSceneObject( m_camera );

	return true;
}

// ** SceneEditor::render
void SceneEditor::render( f32 dt )
{
	// Update the scene
	m_scene->update( 0, dt );

	// Render the scene
	m_scene->render( m_renderingContext );

	// Reset the cursor movement
	m_cursorMovement->set( Vec3() );
}

// ** SceneEditor::handleMousePress
void SceneEditor::handleMousePress( s32 x, s32 y, u8 button )
{
	if( button == Ui::RightMouseButton ) {
		m_camera->setRotationEnabled( true );
		m_camera->setMovementEnabled( true );
	}
}

// ** SceneEditor::handleMouseRelease
void SceneEditor::handleMouseRelease( s32 x, s32 y, u8 button )
{
	if( button == Ui::RightMouseButton ) {
		m_camera->setRotationEnabled( false );
		m_camera->setMovementEnabled( false );
	}
}

// ** SceneEditor::handleMouseMove
void SceneEditor::handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, u8 buttons )
{
	m_cursorMovement->set( Vec3( -dy, -dx, 0 ) );
}

// ** SceneEditor::handleDragEnter
bool SceneEditor::handleDragEnter( IMimeDataWPtr mime )
{
	return mime->hasFormat( Composer::kAssetMime );
}

// ** SceneEditor::handleDragMove
void SceneEditor::handleDragMove( IMimeDataWPtr mime, s32 x, s32 y )
{
}

// ** SceneEditor::handleDrop
void SceneEditor::handleDrop( IMimeDataWPtr mime, s32 x, s32 y )
{
	// Extract assets from MIME data
	Scene::AssetSet assets = assetsFromMime( mime );

	// Places assets to scene
	for( Scene::AssetSet::const_iterator i = assets.begin(), end = assets.end(); i != end; ++i ) {
		Scene::AssetPtr asset = *i;

		switch( asset->type() ) {
		case Scene::Asset::Mesh:	placeMesh( castTo<Scene::Mesh>( asset.get() ), m_camera->get<Scene::Transform>()->position() );
									break;
		}
	}
}

// ** SceneEditor::assetsFromMime
Scene::AssetSet SceneEditor::assetsFromMime( IMimeDataWPtr mime ) const
{
	// Resulting asset set
	Scene::AssetSet result;

	// Get an array of attached assets
	StringArray assets = mime->strings();

	// Add assets to scene
	for( s32 i = 0, n = ( s32 )assets.size(); i < n; i++ ) {
		// Read an attached meta data
		Io::KeyValue meta = m_project->assetsModel()->metaData( assets[i] );
		DC_BREAK_IF( !meta.isObject() );

		// Find asset by UUID.
		Scene::AssetWPtr asset = m_project->assets()->findAsset( meta.get( "uuid", "" ).asString() );
		DC_BREAK_IF( !asset.valid() );

		// Add to set.
		result.insert( asset );
	}

	return result;
}

// ** SceneEditor::placeMesh
Scene::SceneObjectPtr SceneEditor::placeMesh( Scene::MeshPtr mesh, const Vec3& point )
{
	Scene::SceneObjectPtr sceneObject = m_scene->createSceneObject();
	sceneObject->attach<Scene::Identifier>( mesh->name() );
	sceneObject->attach<Scene::StaticMesh>( mesh );
	sceneObject->attach<Scene::Transform>( point.x, point.y, point.z, Scene::TransformWPtr() );
	m_scene->addSceneObject( sceneObject );

	return sceneObject;
}

} // namespace Editors

DC_END_COMPOSER