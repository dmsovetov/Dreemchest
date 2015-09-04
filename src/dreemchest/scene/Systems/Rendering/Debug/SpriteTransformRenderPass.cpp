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

#include "SpriteTransformRenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** SpriteTransformRenderPass::SpriteTransformRenderPass
SpriteTransformRenderPass::SpriteTransformRenderPass( Ecs::Entities& entities, const Rendering& rendering ) : RenderPass2D( entities, "SpriteTransformRenderPass", rendering )
{
}

// ** SpriteTransformRenderPass::process
void SpriteTransformRenderPass::process( u32 currentTime, f32 dt, SceneObject& sceneObject, Sprite& sprite, Transform& transform )
{
	// Get the rotation
	f32  angle = radians( transform.rotationZ() );
	f32  s	   = sinf( angle );
	f32  c	   = cosf( angle );

	// Calculate the basis vectors from rotation
	Vec2 up   = Vec2( s, c );
	Vec2 side = Vec2( up.y, -up.x );

	f32  x = transform.x();
	f32  y = transform.y();

	m_rendering.m_renderer2d->line( x, y, x + side.x, y + side.y, Rgba( 1.0f, 0.0f, 0.0f ) );
	m_rendering.m_renderer2d->line( x, y, x + up.x,   y + up.y,   Rgba( 0.0f, 1.0f, 0.0f ) );
}

} // namespace Scene

DC_END_DREEMCHEST