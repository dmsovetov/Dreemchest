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
#include "Gizmo.h"
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

	// Create the scene model
	m_sceneModel = createSceneModel( m_scene );

	// Create terrain.
	Scene::TerrainPtr terrain = new Scene::Terrain( m_project->assets().get(), "terrain", "terrain", 128 );

	{
		Scene::SceneObjectPtr chunk = m_scene->createSceneObject();
		Scene::MeshPtr		  mesh  = terrain->createMesh();
		chunk->attach<Scene::StaticMesh>( mesh );
		chunk->attach<Scene::Transform>();
		chunk->attach<Scene::Identifier>( "Terrain" );
		m_scene->addSceneObject( chunk );
	// Create grid.
	{
		Scene::SceneObjectPtr grid = m_scene->createSceneObject();
		grid->attach<Grid>();
		grid->attach<Scene::Transform>();
		grid->attach<SceneEditorInternal>();
		m_scene->addSceneObject( grid );
	}

	// Create the camera.
	m_camera = Scene::SpectatorCamera::create( FrameTarget::create( document->renderingFrame() ), m_cursorMovement );
	m_camera->setMovementEnabled( false );
	m_camera->setRotationEnabled( false );
	m_camera->setClearColor( backgroundColor() );
	m_camera->attach<SceneEditorInternal>();
	m_camera->attach<Scene::RenderWireframe>();
	//m_camera->attach<Scene::RenderForwardLit>();
	m_camera->attach<Scene::RenderBoundingVolumes>();
	m_camera->attach<RenderGizmo>();
	m_scene->addSceneObject( m_camera );

	// Add gizmo systems
	m_scene->addSystem<TranslationGizmoSystem>( m_camera );
	m_scene->addRenderingSystem<SceneHelpersRenderer>();

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

// ** SceneEditor::handleSceneObjectDoubleClicked
void SceneEditor::handleSceneObjectDoubleClicked( const Ui::ISceneTree::DoubleClicked& e )
{
	// Remove the previous component
	if( m_camera->has<Scene::MoveTo>() ) {
		m_camera->detach<Scene::MoveTo>();
	}

	// Get the  mesh bounding box
	Bounds bounds = e.sceneObject->get<Scene::StaticMesh>()->worldSpaceBounds();

	// Calculate new camera position by subtracting
	// the view direction from scene object position
	Vec3 position = bounds.center() + m_camera->view() * max3( bounds.width(), bounds.height(), bounds.depth() ) + 1.0f;

	// Attach the moving component
	m_camera->attach<Scene::MoveTo>( new Scene::Vec3Binding( position ), false, Scene::MoveTo::Smooth, 16.0f );
}

// ** SceneEditor::notifyEnterForeground
void SceneEditor::notifyEnterForeground( Ui::IMainWindowWPtr window )
{
	// Set this model
	window->sceneTree()->setModel( m_sceneModel );

	// Subscribe for event
	window->sceneTree()->subscribe<Ui::ISceneTree::DoubleClicked>( dcThisMethod( SceneEditor::handleSceneObjectDoubleClicked ) );
}

// ** SceneEditor::notifyEnterBackground
void SceneEditor::notifyEnterBackground( Ui::IMainWindowWPtr window )
{
	// Set empty model
	window->sceneTree()->setModel( SceneModelPtr() );

	// Unsubscribe for event
	window->sceneTree()->unsubscribe<Ui::ISceneTree::DoubleClicked>( dcThisMethod( SceneEditor::handleSceneObjectDoubleClicked ) );
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

// ** SceneEditor::handleMouseWheel
void SceneEditor::handleMouseWheel( s32 delta )
{
	m_camera->setPosition( m_camera->position() - m_camera->view() * delta * 0.01f );
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
	// Construct the view ray from cursor position.
	Ray ray = m_camera->toWorldSpace( x, m_camera->get<Scene::Camera>()->target()->height() - y );

	// Extract assets from MIME data
	Scene::AssetSet assets = Composer::instance()->assetsFromMime( mime );

	// Places assets to scene
	for( Scene::AssetSet::const_iterator i = assets.begin(), end = assets.end(); i != end; ++i ) {
		Scene::AssetPtr asset = *i;

		switch( asset->type() ) {
		case Scene::Asset::Mesh:	placeMesh( castTo<Scene::Mesh>( asset.get() ), m_camera->position() + ray.direction() * 5.0f );
									break;
		}
	}
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