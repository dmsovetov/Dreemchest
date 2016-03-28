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
#include "../Widgets/Menu.h"
#include "../Systems/Transform/TranslationTool.h"
#include "../Systems/Transform/RotationTool.h"
#include "../Systems/Transform/ArcballRotationTool.h"
#include "../Systems/Terrain/TerrainEditing.h"
#include "../Systems/Rendering.h"
#include "../Project/Project.h"

DC_BEGIN_COMPOSER

namespace Editors {

// ** SceneEditor::SceneEditor
SceneEditor::SceneEditor( void ) : m_tools( NULL )
{

}

// ** SceneEditor::initialize
bool SceneEditor::initialize( ProjectQPtr project, const FileInfo& asset, Ui::DocumentQPtr document )
{
	if( !VisualEditor::initialize( project, asset, document ) ) {
		return false;
	}

	// Create rendering context.
	m_renderingContext = Scene::RenderingContext::create( hal() );

	// Create cursor binding
	m_cursorMovement = new Scene::Vec3Binding;

	// Create the scene.
    m_scene = loadFromFile( m_asset.absoluteFilePath() );

	// Create the scene model
	m_sceneModel = new SceneModel( m_project->assets(), m_scene, this );

	// Create terrain.
	{
		//Scene::TerrainPtr terrain1 = new Scene::Terrain( m_project->assets().get(), "terrain1", "terrain1", 128 );
		//m_sceneModel->placeTerrain( terrain1, Vec3( 0, 0, 0 ) );
		//Scene::TerrainPtr terrain2 = new Scene::Terrain( m_project->assets().get(), "terrain2", "terrain2", 128 );
		//m_sceneModel->placeTerrain( terrain2, Vec3( 128, 0, 0 ) );
		//Scene::TerrainPtr terrain3 = new Scene::Terrain( m_project->assets().get(), "terrain3", "terrain3", 128 );
		//m_sceneModel->placeTerrain( terrain3, Vec3( 0, 0, 128 ) );
		//Scene::TerrainPtr terrain4 = new Scene::Terrain( m_project->assets().get(), "terrain4", "terrain4", 128 );
		//m_sceneModel->placeTerrain( terrain4, Vec3( 128, 0, 128 ) );

		m_terrainTool = m_scene->createSceneObject();
		m_terrainTool->attach<Scene::Transform>();
		m_terrainTool->attach<TerrainTool>( /*terrain1*/Scene::TerrainHandle(), 10.0f );
		m_terrainTool->attach<SceneEditorInternal>( m_terrainTool, SceneEditorInternal::Private );
		m_scene->addSceneObject( m_terrainTool );

		m_scene->addSystem<TerrainHeightmapSystem>( m_terrainTool, viewport() );
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

	// Set the default tool
	setTool( NoTool );

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

struct Null : public Ecs::Component<Null> {};

// ** SceneEditor::save
void SceneEditor::save( void )
{
    // Get the set of objects to be serialized
    Scene::SceneObjectSet objects = m_scene->findByAspect( Ecs::Aspect::exclude<Null>() );

    // Create serialization context
    Ecs::SerializationContext ctx( m_scene->ecs() );

    KeyValue kv;

    // Write each object to a root key-value archive
    for( Scene::SceneObjectSet::const_iterator i = objects.begin(), end = objects.end(); i != end; ++i ) {
        if( !(*i)->isSerializable() ) {
            continue;
        }

        Archive object;
        (*i)->serialize( ctx, object );
        kv.setValueAtKey( (*i)->id().toString(), object );
    }

    // Write the serialized data to file
    qComposer->fileSystem()->writeTextFile( m_asset.absoluteFilePath(), QString::fromStdString( Io::VariantTextStream::stringify( Variant::fromValue( kv ), true ) ) );
}

// ** SceneEditor::loadFromFile
Scene::ScenePtr SceneEditor::loadFromFile( const QString& fileName ) const
{
    // Create scene instance
    Scene::ScenePtr scene = Scene::Scene::create();

    // Read the file contents
    QString data = qComposer->fileSystem()->readTextFile( fileName );

    if( data.isEmpty() ) {
        return scene;
    }

    // Create serialization context
    Ecs::SerializationContext ctx( scene->ecs() );
    ctx.set<Assets::Assets>( &m_project->assets() );

    // Parse KeyValue from a text stream
    Archive  ar = Io::VariantTextStream::parse( data.toStdString() );
    KeyValue kv = ar.as<KeyValue>();

    // Read each object from a root key-value archive
    for( KeyValue::Properties::const_iterator i = kv.properties().begin(), end = kv.properties().end(); i != end; ++i ) {
        // Create entity instance by a type name
        Ecs::EntityPtr entity = ctx.createEntity( i->second.as<KeyValue>().get<String>( "Type" ) );
        entity->attach<SceneEditorInternal>( entity );

        // Read entity from data
        entity->deserialize( ctx, i->second );

        // Add entity to scene
        scene->addSceneObject( entity );
    }

    return scene;
}

// ** SceneEditor::navigateToObject
void SceneEditor::navigateToObject( Scene::SceneObjectWPtr sceneObject )
{
	// Remove the previous component
	if( m_camera->has<Scene::MoveTo>() ) {
		m_camera->detach<Scene::MoveTo>();
	}

	// Get the  mesh bounding box
	Bounds bounds = sceneObject->get<Scene::StaticMesh>()->worldSpaceBounds();

	// Calculate new camera position by subtracting
	// the view direction from scene object position
	Vec3 position = bounds.center() + m_camera->view() * max3( bounds.width(), bounds.height(), bounds.depth() ) + 1.0f;

	// Attach the moving component
	m_camera->attach<Scene::MoveTo>( new Scene::Vec3Binding( position ), false, Scene::MoveTo::Smooth, 16.0f );
}

// ** SceneEditor::notifyEnterForeground
void SceneEditor::notifyEnterForeground( Ui::MainWindowQPtr window )
{
	// Create the tool bar
	DC_BREAK_IF( m_tools );
	m_tools = window->addToolBar();

	m_tools->beginActionGroup();
	{
		m_tools->addAction( "Select", BindAction( SceneEditor::menuTransformSelect ), "", ":Scene/Scene/cursor.png", Ui::ItemCheckable | Ui::ItemChecked );
		m_tools->addAction( "Translate", BindAction( SceneEditor::menuTransformTranslate ), "", ":Scene/Scene/move.png", Ui::ItemCheckable );
		m_tools->addAction( "Rotate", BindAction( SceneEditor::menuTransformRotate ), "", ":Scene/Scene/rotate.png", Ui::ItemCheckable );
		m_tools->addAction( "Scale", BindAction( SceneEditor::menuTransformScale ), "", ":Scene/Scene/scale.png", Ui::ItemCheckable );
        m_tools->addSeparator();
        m_tools->addWidget( new QComboBox );
        m_tools->addSeparator();
        m_tools->addWidget( new QComboBox );
	    m_tools->addSeparator();
		m_tools->addAction( "Raise Terrain", BindAction( SceneEditor::menuTerrainRaise ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable | Ui::ItemChecked )->setChecked( false );
		m_tools->addAction( "Lower Terrain", BindAction( SceneEditor::menuTerrainLower ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable );
		m_tools->addAction( "Level Terrain", BindAction( SceneEditor::menuTerrainLevel ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable );
		m_tools->addAction( "Flatten Terrain", BindAction( SceneEditor::menuTerrainFlatten ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable );
		m_tools->addAction( "Smooth Terrain", BindAction( SceneEditor::menuTerrainSmooth ), "", ":Scene/Scene/magnet.png", Ui::ItemCheckable );
	}
	m_tools->endActionGroup();

	// Set this model
	window->sceneTree()->setModel( m_sceneModel.get() );

	// Subscribe for event
    connect( window->sceneTree(), SIGNAL(sceneObjectDoubleClicked(Scene::SceneObjectWPtr)), this, SLOT(navigateToObject(Scene::SceneObjectWPtr)) );
}

// ** SceneEditor::notifyEnterBackground
void SceneEditor::notifyEnterBackground( Ui::MainWindowQPtr window )
{
	// Remove tool bar
	window->removeToolBar( m_tools );
	m_tools = Ui::ToolBarQPtr();

	// Set empty model
	window->sceneTree()->setModel( NULL );

	// Unsubscribe for event
    disconnect( window->sceneTree(), SIGNAL(sceneObjectDoubleClicked(Scene::SceneObjectWPtr)), this, SLOT(navigateToObject(Scene::SceneObjectWPtr)) );
}

// ** SceneEditor::menuTransformSelect
void SceneEditor::menuTransformSelect( Ui::ActionQPtr action )
{
	setTool( NoTool );
}

// ** SceneEditor::menuTransformTranslate
void SceneEditor::menuTransformTranslate( Ui::ActionQPtr action )
{
	setTool( ToolTranslate );
}

// ** SceneEditor::menuTransformRotate
void SceneEditor::menuTransformRotate( Ui::ActionQPtr action )
{
	setTool( ToolRotate );
}

// ** SceneEditor::menuTransformScale
void SceneEditor::menuTransformScale( Ui::ActionQPtr action )
{
	setTool( ToolScale );
}

// ** SceneEditor::menuTerrainRaise
void SceneEditor::menuTerrainRaise( Ui::ActionQPtr action )
{
    setTool( ToolRaiseTerrain );
    m_terrainTool->get<TerrainTool>()->setType( TerrainTool::Raise );
}

// ** SceneEditor::menuTerrainLower
void SceneEditor::menuTerrainLower( Ui::ActionQPtr action )
{
    setTool( ToolLowerTerrain );
    m_terrainTool->get<TerrainTool>()->setType( TerrainTool::Lower );
}

// ** SceneEditor::menuTerrainFlatten
void SceneEditor::menuTerrainFlatten( Ui::ActionQPtr action )
{
    setTool( ToolFlattenTerrain );
    m_terrainTool->get<TerrainTool>()->setType( TerrainTool::Flatten );
}

// ** SceneEditor::menuTerrainLevel
void SceneEditor::menuTerrainLevel( Ui::ActionQPtr action )
{
    setTool( ToolLevelTerrain );
    m_terrainTool->get<TerrainTool>()->setType( TerrainTool::Level );
}

// ** SceneEditor::menuTerrainSmooth
void SceneEditor::menuTerrainSmooth( Ui::ActionQPtr action )
{
    setTool( ToolSmoothTerrain );
    m_terrainTool->get<TerrainTool>()->setType( TerrainTool::Smooth );
}

// ** SceneEditor::handleMousePress
void SceneEditor::handleMousePress( s32 x, s32 y, const Ui::MouseButtons& buttons )
{
	VisualEditor::handleMousePress( x, y, buttons );

	if( buttons & Ui::MouseButtons::Right ) {
		m_camera->setRotationEnabled( true );
		m_camera->setMovementEnabled( true );
	}
}

// ** SceneEditor::handleMouseRelease
void SceneEditor::handleMouseRelease( s32 x, s32 y, const Ui::MouseButtons& buttons )
{
	VisualEditor::handleMouseRelease( x, y, buttons );

	if( buttons & Ui::MouseButtons::Right ) {
		m_camera->setRotationEnabled( false );
		m_camera->setMovementEnabled( false );
	}
	else if( buttons & Ui::MouseButtons::Left ) {
		// Get the scene object underneath the mouse cursor
		Scene::SceneObjectWPtr target = findSceneObjectAtPoint( x, y );

		// Select it
		selectSceneObject( target );
	}
}

// ** SceneEditor::handleMouseMove
void SceneEditor::handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, const Ui::MouseButtons& buttons )
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
bool SceneEditor::handleDragEnter( MimeDataQPtr mime )
{
	return mime->hasFormat( QString::fromStdString( Composer::kAssetMime ) );
}

// ** SceneEditor::handleDragMove
void SceneEditor::handleDragMove( MimeDataQPtr mime, s32 x, s32 y )
{
	// Get the scene object underneath the mouse cursor
	Scene::SceneObjectWPtr target = findSceneObjectAtPoint( x, y );

	// Highlight it
	highlightSceneObject( target );
}

// ** SceneEditor::handleDrop
void SceneEditor::handleDrop( MimeDataQPtr mime, s32 x, s32 y )
{
	// Get the scene object underneath the mouse cursor
	Scene::SceneObjectWPtr target = findSceneObjectAtPoint( x, y );

	// Extract assets from MIME data
	Assets::AssetSet assets = qComposer->assetsFromMime( mime );

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
		bindTransformGizmo( m_selectedSceneObject, NoTool );
	}

	// Store this object
	m_selectedSceneObject = sceneObject;

	// Mark it as selected
	if( m_selectedSceneObject.valid() ) {
		// Add the selected flag
		m_selectedSceneObject->get<SceneEditorInternal>()->setSelected( true );

		// Bind the gizmo for an active transformation tool
		bindTransformGizmo( m_selectedSceneObject, m_activeTool );
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

// ** SceneEditor::setTool
void SceneEditor::setTool( ActiveTool tool )
{
	// This tool is already activated
	if( tool == m_activeTool ) {
		return;
	}

	// Set active tool
	m_activeTool = tool;

	// Bind the gizmo to selected object
	if( m_selectedSceneObject.valid() ) {
		bindTransformGizmo( m_selectedSceneObject, m_activeTool );
	}

    // Now the terrain tool
    if( tool < ToolRaiseTerrain ) {
        m_terrainTool->disable<TerrainTool>();
    } else {
        m_terrainTool->enable<TerrainTool>();
    }
}

// ** SceneEditor::bindTransformGizmo
void SceneEditor::bindTransformGizmo( Scene::SceneObjectWPtr sceneObject, ActiveTool tool ) const
{
	DC_BREAK_IF( !sceneObject.valid() );

	// First remove all transform tool gizmos
	if( sceneObject->has<TranslationTool>() )		sceneObject->detach<TranslationTool>();
	if( sceneObject->has<RotationTool>() )			sceneObject->detach<RotationTool>();
	if( sceneObject->has<ArcballRotationTool>() )	sceneObject->detach<ArcballRotationTool>();

	// Now add the gizmo
	switch( tool ) {
	case ToolTranslate:	sceneObject->attach<TranslationTool>();
						break;

	case ToolRotate:	sceneObject->attach<ArcballRotationTool>();
						sceneObject->attach<RotationTool>();
						break;
	}
}

} // namespace Editors

DC_END_COMPOSER