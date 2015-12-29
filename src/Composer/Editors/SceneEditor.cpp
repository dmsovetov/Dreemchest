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
#include "../Systems/Transform/TranslationTool.h"
#include "../Systems/Transform/RotationTool.h"
#include "../Systems/Transform/ArcballRotationTool.h"
#include "../Systems/Terrain/TerrainEditing.h"
#include "../Systems/Rendering.h"
#include "../Project/Project.h"

DC_BEGIN_COMPOSER

namespace Editors {

// ** SceneEditor::initialize
bool SceneEditor::initialize( Project::ProjectWPtr project, const Scene::AssetPtr& asset, Ui::IDocumentWPtr document )
{
	if( !VisualEditor::initialize( project, asset, document ) ) {
		return false;
	}

	// Set the default transform tool
	setTransformTool( NoTransformTool );

	// Create rendering context.
	m_renderingContext = Scene::RenderingContext::create( hal() );

	// Create cursor binding
	m_cursorMovement = new Scene::Vec3Binding;

	// Create the scene.
	m_scene = Scene::Scene::create();
	m_scene->addSystem<Scene::AssetSystem>( m_project->assets() );

	// Create the scene model
	m_sceneModel = createSceneModel( m_project->assets(), m_scene );

	// Create terrain.
	{
		Scene::TerrainPtr terrain1 = new Scene::Terrain( m_project->assets().get(), "terrain1", "terrain1", 128 );
		m_sceneModel->placeTerrain( terrain1, Vec3( 0, 0, 0 ) );
		//Scene::TerrainPtr terrain2 = new Scene::Terrain( m_project->assets().get(), "terrain2", "terrain2", 128 );
		//m_sceneModel->placeTerrain( terrain2, Vec3( 128, 0, 0 ) );
		//Scene::TerrainPtr terrain3 = new Scene::Terrain( m_project->assets().get(), "terrain3", "terrain3", 128 );
		//m_sceneModel->placeTerrain( terrain3, Vec3( 0, 0, 128 ) );
		//Scene::TerrainPtr terrain4 = new Scene::Terrain( m_project->assets().get(), "terrain4", "terrain4", 128 );
		//m_sceneModel->placeTerrain( terrain4, Vec3( 128, 0, 128 ) );

		Scene::SceneObjectPtr tool = m_scene->createSceneObject();
		tool->attach<Scene::Transform>();
		tool->attach<TerrainTool>( terrain1, 10.0f );
		tool->attach<SceneEditorInternal>( tool, SceneEditorInternal::Private );
		m_scene->addSceneObject( tool );

		m_scene->addSystem<TerrainHeightmapSystem>( tool, viewport() );
	}

	// Create grid.
	{
		Scene::SceneObjectPtr grid = m_scene->createSceneObject();
		grid->attach<Scene::Grid>();
		grid->attach<Scene::Transform>();
		grid->attach<SceneEditorInternal>( grid, SceneEditorInternal::Private );
		m_scene->addSceneObject( grid );
	}

	// Create the camera.
	m_camera = Scene::SpectatorCamera::create( FrameTarget::create( document->renderingFrame() ), m_cursorMovement );
	m_camera->setMovementEnabled( false );
	m_camera->setRotationEnabled( false );
	m_camera->setClearColor( backgroundColor() );
	m_camera->setPosition( Vec3( 0.0f, 5.0f, 5.0f ) );
	m_camera->attach<SceneEditorInternal>( m_camera, SceneEditorInternal::Private );
	m_camera->attach<Scene::RenderWireframe>();
	m_camera->attach<Scene::RenderVertexNormals>();
	//m_camera->attach<Scene::RenderUnlit>();
	m_camera->attach<Scene::RenderBoundingVolumes>();
	m_camera->attach<RenderSceneHelpers>();
	m_scene->addSceneObject( m_camera );
	viewport()->setCamera( m_camera );

	// Add gizmo systems
	m_scene->addSystem<TranslationToolSystem>( viewport() );
	m_scene->addSystem<ArcballRotationToolSystem>( viewport() );
	m_scene->addSystem<RotationToolSystem>( viewport() );
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
	// Create the tool bar
	DC_BREAK_IF( m_tools.valid() );
	m_tools = window->addToolBar();

	m_tools->beginActionGroup();
	{
		m_tools->addAction( "Select", BindAction( SceneEditor::menuTransformSelect ), "", ":Scene/Scene/cursor.png", Ui::ItemCheckable | Ui::ItemChecked );
		m_tools->addAction( "Translate", BindAction( SceneEditor::menuTransformTranslate ), "", ":Scene/Scene/move.png", Ui::ItemCheckable );
		m_tools->addAction( "Rotate", BindAction( SceneEditor::menuTransformRotate ), "", ":Scene/Scene/rotate.png", Ui::ItemCheckable );
		m_tools->addAction( "Scale", BindAction( SceneEditor::menuTransformScale ), "", ":Scene/Scene/scale.png", Ui::ItemCheckable );
	}
	m_tools->endActionGroup();

	m_tools->addSeparator();

	m_tools->beginActionGroup();
	{
		m_tools->addAction( "Raise Terrain", BindAction( SceneEditor::menuTerrainRaise ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable | Ui::ItemChecked );
		m_tools->addAction( "Lower Terrain", BindAction( SceneEditor::menuTerrainLower ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable );
		m_tools->addAction( "Level Terrain", BindAction( SceneEditor::menuTerrainLevel ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable );
		m_tools->addAction( "Flatten Terrain", BindAction( SceneEditor::menuTerrainFlatten ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable );
		m_tools->addAction( "Smooth Terrain", BindAction( SceneEditor::menuTerrainSmooth ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable );
	}
	m_tools->endActionGroup();

	// Set this model
	window->sceneTree()->setModel( m_sceneModel );

	// Subscribe for event
	window->sceneTree()->subscribe<Ui::ISceneTree::DoubleClicked>( dcThisMethod( SceneEditor::handleSceneObjectDoubleClicked ) );
}

// ** SceneEditor::notifyEnterBackground
void SceneEditor::notifyEnterBackground( Ui::IMainWindowWPtr window )
{
	// Remove tool bar
	window->removeToolBar( m_tools );
	m_tools = Ui::IToolBarPtr();

	// Set empty model
	window->sceneTree()->setModel( SceneModelPtr() );

	// Unsubscribe for event
	window->sceneTree()->unsubscribe<Ui::ISceneTree::DoubleClicked>( dcThisMethod( SceneEditor::handleSceneObjectDoubleClicked ) );
}

// ** SceneEditor::menuTransformSelect
void SceneEditor::menuTransformSelect( Ui::IActionWPtr action )
{
	setTransformTool( NoTransformTool );
}

// ** SceneEditor::menuTransformTranslate
void SceneEditor::menuTransformTranslate( Ui::IActionWPtr action )
{
	setTransformTool( TransformTranslate );
}

// ** SceneEditor::menuTransformRotate
void SceneEditor::menuTransformRotate( Ui::IActionWPtr action )
{
	setTransformTool( TransformRotate );
}

// ** SceneEditor::menuTransformScale
void SceneEditor::menuTransformScale( Ui::IActionWPtr action )
{
	setTransformTool( TransformScale );
}

// ** SceneEditor::menuTerrainRaise
void SceneEditor::menuTerrainRaise( Ui::IActionWPtr action )
{
    m_scene->findByAspect( Ecs::Aspect::all<TerrainTool>() ).begin()->get()->get<TerrainTool>()->setType( TerrainTool::Raise );
}

// ** SceneEditor::menuTerrainLower
void SceneEditor::menuTerrainLower( Ui::IActionWPtr action )
{
    m_scene->findByAspect( Ecs::Aspect::all<TerrainTool>() ).begin()->get()->get<TerrainTool>()->setType( TerrainTool::Lower );
}

// ** SceneEditor::menuTerrainFlatten
void SceneEditor::menuTerrainFlatten( Ui::IActionWPtr action )
{
    m_scene->findByAspect( Ecs::Aspect::all<TerrainTool>() ).begin()->get()->get<TerrainTool>()->setType( TerrainTool::Flatten );
}

// ** SceneEditor::menuTerrainLevel
void SceneEditor::menuTerrainLevel( Ui::IActionWPtr action )
{
    m_scene->findByAspect( Ecs::Aspect::all<TerrainTool>() ).begin()->get()->get<TerrainTool>()->setType( TerrainTool::Level );
}

// ** SceneEditor::menuTerrainSmooth
void SceneEditor::menuTerrainSmooth( Ui::IActionWPtr action )
{
    m_scene->findByAspect( Ecs::Aspect::all<TerrainTool>() ).begin()->get()->get<TerrainTool>()->setType( TerrainTool::Smooth );
}

// ** SceneEditor::handleMousePress
void SceneEditor::handleMousePress( s32 x, s32 y, u8 buttons )
{
	VisualEditor::handleMousePress( x, y, buttons );

	if( buttons & Ui::RightMouseButton ) {
		m_camera->setRotationEnabled( true );
		m_camera->setMovementEnabled( true );
	}
}

// ** SceneEditor::handleMouseRelease
void SceneEditor::handleMouseRelease( s32 x, s32 y, u8 buttons )
{
	VisualEditor::handleMouseRelease( x, y, buttons );

	if( buttons & Ui::RightMouseButton ) {
		m_camera->setRotationEnabled( false );
		m_camera->setMovementEnabled( false );
	}
	else if( buttons & Ui::LeftMouseButton ) {
		// Get the scene object underneath the mouse cursor
		Scene::SceneObjectWPtr target = findSceneObjectAtPoint( x, y );

		// Select it
		selectSceneObject( target );
	}
}

// ** SceneEditor::handleMouseMove
void SceneEditor::handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, u8 buttons )
{
	// Run the base class method to update the cursor
	VisualEditor::handleMouseMove( x, y, dx, dy, buttons );

	// Update cursor movement
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
	// Get the scene object underneath the mouse cursor
	Scene::SceneObjectWPtr target = findSceneObjectAtPoint( x, y );

	// Highlight it
	highlightSceneObject( target );
}

// ** SceneEditor::handleDrop
void SceneEditor::handleDrop( IMimeDataWPtr mime, s32 x, s32 y )
{
	// Get the scene object underneath the mouse cursor
	Scene::SceneObjectWPtr target = findSceneObjectAtPoint( x, y );

	// Extract assets from MIME data
	Scene::AssetSet assets = Composer::instance()->assetsFromMime( mime );

	// Get the asset action
	SceneModel::AssetAction action = m_sceneModel->acceptableAssetAction( assets, target, m_camera->position() + constructViewRay( x, y ).direction() * 5.0f );

	if( action ) {
		m_sceneModel->performAssetAction( action );
	}

	// Reset the highlight indicator
	highlightSceneObject( Scene::SceneObjectWPtr() );
}

// ** SceneEditor::highlightSceneObject
void SceneEditor::highlightSceneObject( Scene::SceneObjectWPtr sceneObject )
{
	// This object is already highlighted - skip
	if( sceneObject == m_activeSceneObject ) {
		return;
	}

	// Only one scene object can be highlighted at a time
	if( m_activeSceneObject.valid() ) {
		m_activeSceneObject->get<SceneEditorInternal>()->setHighlighted( false );
	}

	// Store this object
	m_activeSceneObject = sceneObject;

	// Mark it as highlighted
	if( m_activeSceneObject.valid() ) {
		m_activeSceneObject->get<SceneEditorInternal>()->setHighlighted( true );
	}
}

// ** SceneEditor::selectSceneObject
void SceneEditor::selectSceneObject( Scene::SceneObjectWPtr sceneObject )
{
	// This object is already selected - skip
	if( sceneObject == m_selectedSceneObject ) {
		return;
	}

	// Only one scene object can be selected at a time
	if( m_selectedSceneObject.valid() ) {
		// Remove the selected flag
		m_selectedSceneObject->get<SceneEditorInternal>()->setSelected( false );
		
		// Ensure the deselected object has no gizmos
		bindTransformGizmo( m_selectedSceneObject, NoTransformTool );
	}

	// Store this object
	m_selectedSceneObject = sceneObject;

	// Mark it as selected
	if( m_selectedSceneObject.valid() ) {
		// Add the selected flag
		m_selectedSceneObject->get<SceneEditorInternal>()->setSelected( true );

		// Bind the gizmo for an active transformation tool
		bindTransformGizmo( m_selectedSceneObject, m_activeTransformTool );
	}
}

// ** SceneEditor::findSceneObjectAtPoint
Scene::SceneObjectWPtr SceneEditor::findSceneObjectAtPoint( s32 x, s32 y ) const
{
	// Query scene object by ray
	Scene::RayTracingResultArray sceneObjects = m_scene->queryRay( constructViewRay( x, y ) );

	// Get the hit scene object.
	Scene::SceneObjectWPtr target = !sceneObjects.empty() ? sceneObjects[0].sceneObject : Scene::SceneObjectWPtr();

	return target;
}

// ** SceneEditor::setTransformTool
void SceneEditor::setTransformTool( ActiveTransformTool tool )
{
	// This tool is already activated
	if( tool == m_activeTransformTool ) {
		return;
	}

	// Set active tool
	m_activeTransformTool = tool;

	// Bind the gizmo to selected object
	if( m_selectedSceneObject.valid() ) {
		bindTransformGizmo( m_selectedSceneObject, m_activeTransformTool );
	}
}

// ** SceneEditor::bindTransformGizmo
void SceneEditor::bindTransformGizmo( Scene::SceneObjectWPtr sceneObject, ActiveTransformTool tool ) const
{
	DC_BREAK_IF( !sceneObject.valid() );

	// First remove all transform tool gizmos
	if( sceneObject->has<TranslationTool>() )		sceneObject->detach<TranslationTool>();
	if( sceneObject->has<RotationTool>() )			sceneObject->detach<RotationTool>();
	if( sceneObject->has<ArcballRotationTool>() )	sceneObject->detach<ArcballRotationTool>();

	// Now add the gizmo
	switch( tool ) {
	case TransformTranslate:	sceneObject->attach<TranslationTool>();
								break;

	case TransformRotate:		sceneObject->attach<ArcballRotationTool>();
								sceneObject->attach<RotationTool>();
								break;
	}
}

} // namespace Editors

DC_END_COMPOSER