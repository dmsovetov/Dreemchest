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

#include "RenderingContext.h"

#include "../Components/Rendering.h"
#include "../Components/Transform.h"
#include "../Assets/Renderable.h"
#include "../Assets/RenderAssetSources.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( Assets::Assets& assets, Renderer::HalWPtr hal, SceneWPtr scene )
	: m_hal( hal )
    , m_scene( scene )
    , m_assets( assets )
{
    m_staticMeshes = scene->ecs()->requestIndex( "StaticMeshes", Ecs::Aspect::all<Transform, StaticMesh>() );
    m_lights       = scene->ecs()->requestIndex( "Lights", Ecs::Aspect::all<Transform, Light>() );
}

// ** RenderingContext::create
RenderingContextPtr RenderingContext::create( Assets::Assets& assets, Renderer::HalWPtr hal, SceneWPtr scene )
{
	return RenderingContextPtr( DC_NEW RenderingContext( assets, hal, scene ) );
}

// ** RenderingContext::hal
Renderer::HalWPtr RenderingContext::hal( void ) const
{
    return m_hal;
}

// ** RenderingContext::begin
void RenderingContext::begin( void )
{
    // Get all active scene cameras
    const Ecs::EntitySet& cameras = m_scene->cameras()->entities();

    // Clear each render target
    for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; i++ ) {
		Camera*			 camera = (*i)->get<Camera>();
		RenderTargetWPtr target = camera->target();

		if( !target.valid() ) {
			continue;
		}

		target->begin( this );
		{
			m_hal->setViewport( camera->viewport() );
			u32 mask = ( camera->clearMask() & Camera::ClearColor ? Renderer::ClearColor : 0 ) | ( camera->clearMask() & Camera::ClearDepth ? Renderer::ClearDepth : 0 );
			m_hal->clear( camera->clearColor(), 1.0f, 0, mask );
			m_hal->setViewport( target->rect() );
		}
		target->end( this );
	}

    RenderCommandBuffer commands;
    for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; i++ ) {
        renderFromCamera( commands, *(*i).get(), *(*i)->get<Camera>(), *(*i)->get<Transform>() );
    }
    commands.flush( m_hal );
}

// ** RenderingContext::end
void RenderingContext::end( void )
{

}

// ** RenderingContext::renderFromCamera
void RenderingContext::renderFromCamera( RenderCommandBuffer& commands, Ecs::Entity& entity, Camera& camera, Transform& transform )
{
	// Get the render target
	RenderTargetWPtr target = camera.target();

	// Calculate the view-projection matrix
	Matrix4 viewProjection = camera.calculateViewProjection( transform.matrix() );

	target->begin( this );
	{
        // Set the viewport rect
		m_hal->setViewport( camera.viewport() );

        // Push the view-projection transform
        m_hal->setTransform( Renderer::TransformProjection, camera.calculateProjectionMatrix().m );
        m_hal->setTransform( Renderer::TransformView, transform.matrix().inversed().m );

		renderStaticMeshes( commands );
		m_hal->setViewport( target->rect() );
	}
	target->end( this );
}

// ** RenderingContext::renderStaticMeshes
void RenderingContext::renderStaticMeshes( RenderCommandBuffer& commands )
{
    const Ecs::EntitySet& staticMeshes = m_staticMeshes->entities();

    for( Ecs::EntitySet::const_iterator i = staticMeshes.begin(), end = staticMeshes.end(); i != end; i++ ) {
        // Get the static mesh from an entity
        const StaticMesh* staticMesh = (*i)->get<StaticMesh>();

        // Get the static mesh transforms
        const Transform* transform = (*i)->get<Transform>();

        // Request the renderable asset for this mesh
        s32 renderable = requestRenderable( staticMesh->mesh() );
        m_renderableHandles[renderable].readLock();

        // Emit the rendering command
        commands.push( renderable );
     /*   RenderableHandle handle = requestRenderable( staticMesh->mesh() );
        DC_ABORT_IF( !handle.isValid(), "no valid renderable for a mesh" );

        // Read-lock renderable asset
        const Renderable& renderable = handle.readLock();

        // Render all chunks
        for( s32 j = 0; j < renderable.chunkCount(); j++ ) {
            m_hal->setVertexBuffer( renderable.vertexBuffer( j ) );
            m_hal->renderIndexed( renderable.primitiveType(), renderable.indexBuffer( 0 ), 0, renderable.indexBuffer( 0 )->size() );
        }*/
    }
}

// ** RenderingContext::requestRenderable
s32 RenderingContext::requestRenderable( MeshHandle mesh )
{
    // First lookup the renderable by a mesh handle
    RenderableByMesh::iterator i = m_renderableByMesh.find( mesh );

    // Next renderable index to be used
    s32 index = 0;

    // Found the renderable
    if( i != m_renderableByMesh.end() ) {
        // Ensure that mesh handle matches the stored one
        if( i->first.index() == mesh.index() ) {
            return i->second;
        }

        // Save this renderable asset index
        index = i->second;

        // Renderable is outdated - remove it
        m_renderableByMesh.erase( i );
    }

    // Resize an array of renderable assets
    if( index >= m_renderableHandles.size() ) {
        m_renderableHandles.resize( index + 1 );
    }

    // Read lock the mesh asset to queue it for loading
    mesh.readLock();

    // Create a renderable instance
    RenderableHandle handle = m_assets.add<Renderable>( mesh.uniqueId() + ".renderable", DC_NEW RenderableMeshSource( mesh, m_hal ) );
    DC_ABORT_IF( !handle.isValid(), "failed to create renderable asset" );
    handle.asset().setName( mesh.asset().name() + ".renderable" );

    // Save this handle
    m_renderableByMesh[mesh]   = index;
    m_renderableHandles[index] = handle;

    return index;
}

// ** RenderCommandBuffer::push
void RenderCommandBuffer::push( s32 renderable )
{

}

// ** RenderCommandBuffer::flush
void RenderCommandBuffer::flush( const Array<RenderableHandle>& renderables, Renderer::HalWPtr hal )
{
    std::sort( m_commands.begin(), m_commands.end() );
}

} // namespace Scene

DC_END_DREEMCHEST