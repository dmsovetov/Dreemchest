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

#include "RenderScene.h"
#include "RenderAssets.h"
#include "RenderSystems/RenderSystem.h"

#include "../Components/Rendering.h"
#include "../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderScene::RenderScene
RenderScene::RenderScene( SceneWPtr scene, RenderingContextWPtr context )
    : m_scene( scene )
    , m_context( context )
{
    // Create entity caches
    m_meshes = scene->ecs()->createDataCache<MeshCache>( Ecs::Aspect::all<StaticMesh, Transform>(), dcThisMethod( RenderScene::createMeshFromEntity ) );
    m_lights = scene->ecs()->createDataCache<LightCache>( Ecs::Aspect::all<Light, Transform>(), dcThisMethod( RenderScene::createLightFromEntity ) );

    // Create camera index
    m_cameras = scene->ecs()->requestIndex( "Cameras", Ecs::Aspect::all<Camera>() );
}

// ** RenderScene::create
RenderScenePtr RenderScene::create( SceneWPtr scene, RenderingContextWPtr context )
{
    return DC_NEW RenderScene( scene, context );
}

// ** RenderScene::context
RenderingContextWPtr RenderScene::context( void ) const
{
    return m_context;
}

// ** RenderScene::scene
SceneWPtr RenderScene::scene( void ) const
{
    return m_scene;
}

// ** RenderScene::meshes
const RenderScene::Meshes& RenderScene::meshes( void ) const
{
    return m_meshes->data();
}

// ** RenderScene::captureFrame
RenderScene::Frame RenderScene::captureFrame( void )
{
    Frame frame;

    // Get all active scene cameras
    const Ecs::EntitySet& cameras = m_cameras->entities();

    // Clear each render target
    for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; i++ ) {
		Camera*			 camera = (*i)->get<Camera>();
		RenderTargetWPtr target = camera->target();

		if( !target.valid() ) {
			continue;
		}

		target->begin( m_context );
		{
			m_context->hal()->setViewport( camera->viewport() );
			u32 mask = ( camera->clearMask() & Camera::ClearColor ? Renderer::ClearColor : 0 ) | ( camera->clearMask() & Camera::ClearDepth ? Renderer::ClearDepth : 0 );
			m_context->hal()->clear( camera->clearColor(), 1.0f, 0, mask );
			m_context->hal()->setViewport( target->rect() );
		}
		target->end( m_context );
	}

    // Process all render systems
    for( s32 i = 0, n = static_cast<s32>( m_renderSystems.size() ); i < n; i++ ) {
        m_renderSystems[i]->render( frame );
    }

#if 0
    s32 allocated = 0;
    s32 used      = 0;

    // Calculate allocated space
    for( s32 i = 0, n = static_cast<s32>( frame.size() ); i < n; i++ ) {
        allocated += frame[i]->allocatedBytes();
        used += frame[i]->usedBytes();
    }

    LogDebug( "renderScene", "%2.2fkb allocated for a frame, %2.2fkb used (%2.2f percents wasted)\n", allocated / 1024.0f, used / 1024.0f, static_cast<f32>( allocated - used ) / allocated * 100.0f );
#endif

    return frame;
}

// ** RenderScene::display
void RenderScene::display( Frame& frame )
{
    for( s32 i = 0, n = static_cast<s32>( frame.size() ); i < n; i++ ) {
        // Sort all emitted commands
        frame[i]->sort();

    #if 0
        frame[i]->dump();
    #endif

        // Execute rendering commands
        m_context->rvm()->execute( *frame[i] );
    }
}

// ** RenderScene::createMeshFromEntity
RenderScene::RenderableMesh RenderScene::createMeshFromEntity( const Ecs::Entity& entity )
{
    const Transform*  transform  = entity.get<Transform>();
    const StaticMesh* staticMesh = entity.get<StaticMesh>();

    RenderableMesh mesh;
    mesh.transform  = transform;
    mesh.matrix     = &transform->matrix();
    mesh.material   = staticMesh->material( 0 );
    mesh.mesh       = staticMesh->mesh();
    mesh.renderable = m_context->requestRenderable( staticMesh->mesh() );
    mesh.technique  = m_context->requestTechnique( mesh.material );
    return mesh;
}

// ** RenderScene::createLightFromEntity
RenderScene::RenderableLight RenderScene::createLightFromEntity( const Ecs::Entity& entity )
{
    const Transform* transform = entity.get<Transform>();

    RenderableLight light;
    light.transform = transform;
    light.matrix    = &transform->matrix();

    return light;
}

} // namespace Scene

DC_END_DREEMCHEST