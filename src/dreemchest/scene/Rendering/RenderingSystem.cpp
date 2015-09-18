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
#include "Rvm.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderingSystemBase::RenderingSystemBase
RenderingSystemBase::RenderingSystemBase( Ecs::Entities& entities, const String& name, const Ecs::Aspect& aspect ) : m_entities( entities ), m_name( name )
{
	m_cameras = Ecs::Family::create( entities, "Cameras", aspect );
}

// ** RenderingSystemBase::render
void RenderingSystemBase::render( RenderingContextPtr context )
{
	const Ecs::EntitySet& entities = m_cameras->entities();

	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		renderFromCamera( context, *(*i)->get<Camera>(), *(*i)->get<Transform>() );
	}
}

// ** RenderingSystemBase::renderFromCamera
void RenderingSystemBase::renderFromCamera( RenderingContextPtr context, Camera& camera, Transform& transform )
{
	// Get the camera component from entity
	u8 clearMask = camera.clearMask();

	// Get the output viewport for this camera
	Rect viewport = camera.viewport();

	// Get RVM from context
	RvmPtr rvm = context->rvm();

	// Get HAL from a renderer
	Renderer::HalPtr hal = context->hal();

	// Get the view
	const ViewPtr& view = camera.view();

	// Get the viewport bounds
	const Vec2& min		= viewport.min();
	const Vec2& max		= viewport.max();
	u32			width	= static_cast<u32>( max.x - min.x );
	u32			height	= static_cast<u32>( max.y - min.y );
	f32			vwidth  = static_cast<f32>( view->width() );
	f32			vheight = static_cast<f32>( view->height() );

	// Calculate the view-projection matrix
	Matrix4 viewProjection = camera.calculateProjectionMatrix() * transform.matrix().inversed() /*Matrix4::lookAt( Vec3( 5, 5, 5 ), Vec3( 0, 0, 0 ), Vec3( 0, 1, 0 ) )*/;

	view->begin();
	{
		hal->setViewport( ( u32 )min.x, ( u32 )min.y,  width, height );
		hal->setScissorTest( true, ( u32 )min.x, ( u32 )min.y,  width, height );

		if( (clearMask & Camera::ClearDisabled) == 0 ) {
			u32 mask = ( clearMask & Camera::ClearColor ? Renderer::ClearColor : 0 ) | ( clearMask & Camera::ClearDepth ? Renderer::ClearDepth : 0 );
			hal->clear( camera.clearColor(), 1.0f, 0, mask );
			camera.setClearMask( mask | Camera::ClearDisabled ); // Clear the viewport only once per frame.
		}

		for( RenderPasses::iterator i = m_passes.begin(), end = m_passes.end(); i != end; ++i ) {
			RenderPassBasePtr& pass = *i;

			pass->begin( context, camera, transform, viewProjection );
			pass->render( context );
			pass->end( context );
		}

		hal->setScissorTest( false );
		hal->setViewport( 0, 0, ( u32 )vwidth, ( u32 )vheight );
	}
	view->end();
}

} // namespace Scene

DC_END_DREEMCHEST