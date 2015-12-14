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

DC_BEGIN_COMPOSER

namespace Editors {

// ** SceneEditor::initialize
bool SceneEditor::initialize( const Asset& asset, Ui::IDocumentWPtr document )
{
	if( !VisualEditor::initialize( asset, document ) ) {
		return false;
	}

	// Create rendering context.
	Renderer::HalWPtr hal = this->hal();
	Renderer::Renderer2DPtr renderer2d = Renderer::Renderer2D::create( hal, 1024 );

	Scene::RvmPtr rvm( DC_NEW Scene::Rvm( hal, 8000 ) );
	Scene::ShaderCachePtr shaders( DC_NEW Scene::ShaderCache( "D:\\BFG9000\\externals\\src\\dreemchest\\src\\dreemchest\\scene\\Rendering\\Shaders", hal ) );

	m_renderingContext = Scene::RenderingContextPtr( DC_NEW Scene::RenderingContext( rvm, shaders, hal, renderer2d ) );

	// Create cursor binding
	m_cursorMovement = new Scene::Vec3Binding;

	// Create the scene.
	m_scene = Scene::Scene::create();
	m_scene->addSystem<Scene::AssetSystem>( hal );

	// Create assets.
	m_assets = Scene::AssetBundle::create( "assets" );

	Scene::TerrainPtr terrain = m_assets->addTerrain( "terrain", "terrain", 128 );

	for( u32 i = 0; i < terrain->chunkCount(); i++ ) {
		for( u32 j = 0; j < terrain->chunkCount(); j++ ) {
			Scene::SceneObjectPtr chunk = m_scene->createSceneObject();
			Scene::MeshPtr		  mesh  = terrain->createChunkMesh( hal, j, i );

			if( !m_chunk.valid() ) {
				m_chunk = chunk;
			}

			chunk->attach<Scene::StaticMesh>( mesh );
			chunk->attach<Scene::Transform>( j * Scene::Terrain::kChunkSize, -4, i * Scene::Terrain::kChunkSize, ::Scene::Transform::WPtr() );
		}
	}

	// Create the camera.
	m_camera = Scene::SpectatorCamera::create( FrameTarget::create( document->renderingFrame() ), m_cursorMovement );
	m_camera->setClearColor( backgroundColor() );
	m_camera->attach<Scene::RenderWireframe>();
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
	const io::Bson& data = mime->data();
	DC_BREAK_IF( data.isNull() );

	const io::Bson::ValueArray& items = data.items();

	for( s32 i = 0, n = ( s32 )items.size(); i < n; i++ ) {
		qDebug() << "Will place" << items[i].get( "uuid", "" ).asString().c_str();
	}
}

} // namespace Editors

DC_END_COMPOSER