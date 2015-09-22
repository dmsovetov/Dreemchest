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

#include "RenderingSystem.h"
#include "RenderTarget.h"
#include "Rvm.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderingSystemBase::RenderingSystemBase
RenderingSystemBase::RenderingSystemBase( Ecs::EcsWPtr ecs, const String& name, const Ecs::Aspect& aspect ) : m_ecs( ecs ), m_name( name )
{
	m_cameras = m_ecs->requestIndex( "Cameras", aspect );
}

// ** RenderingSystemBase::render
void RenderingSystemBase::render( RenderingContextPtr context )
{
	const Ecs::EntitySet& entities = m_cameras->entities();

	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		renderFromCamera( context, *i->get(), *(*i)->get<Camera>(), *(*i)->get<Transform>() );
	}
}

// ** RenderingSystemBase::renderFromCamera
void RenderingSystemBase::renderFromCamera( RenderingContextPtr context, Ecs::Entity& entity, Camera& camera, Transform& transform )
{
	// Get HAL from a renderer
	Renderer::HalPtr hal = context->hal();

	// Get the render target
	const RenderTargetPtr& target = camera.target();

	// Calculate the view-projection matrix
	Matrix4 viewProjection = camera.calculateViewProjection( transform.matrix() );

	target->begin( context );
	{
		hal->setViewport( camera.viewport() );

		for( RenderPasses::iterator i = m_passes.begin(), end = m_passes.end(); i != end; ++i ) {
			RenderPassBasePtr& pass = *i;

			pass->begin( context, camera, transform, viewProjection );
			pass->render( context );
			pass->end( context );
		}

		hal->setViewport( target->rect() );
	}
	target->end( context );
}

} // namespace Scene

DC_END_DREEMCHEST