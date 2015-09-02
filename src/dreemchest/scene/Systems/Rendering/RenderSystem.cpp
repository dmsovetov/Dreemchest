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

#include "RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ---------------------------------------- RenderSystemBase ---------------------------------------- //

// ** RenderSystemBase::process
void RenderSystemBase::process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity )
{
	// Get the camera component from entity
	Camera& camera    = *entity->get<Camera>();
	u8		clearMask = camera.clearMask();

	// Get the transform component from entity
	Transform& transform = *entity->get<Transform>();

	// Get the output viewport for this camera
	Rect viewport = camera.viewport();

	// Get HAL from a renderer
	Renderer::HalPtr& hal = m_renderers.m_hal;

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
	Matrix4 viewProjection = camera.calculateProjectionMatrix() * transform.matrix().inversed();

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
			(*i)->render( currentTime, dt, viewProjection );
		}

		hal->setScissorTest( false );
		hal->setViewport( 0, 0, ( u32 )vwidth, ( u32 )vheight );
	}
	view->end();
}

// ---------------------------------------- RenderPassBase ---------------------------------------- //

// ** RenderPassBase::render
void RenderPassBase::render( u32 currentTime, f32 dt, const Matrix4& viewProjection )
{
	m_viewProj = viewProjection;
	update( currentTime, dt );
}

// ** RenderPassBase::process
void RenderPassBase::process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity )
{
	DC_BREAK
}

} // namespace scene

DC_END_DREEMCHEST