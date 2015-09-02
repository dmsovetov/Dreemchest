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

namespace scene {

// ** SpriteTransformRenderPass::SpriteTransformRenderPass
SpriteTransformRenderPass::SpriteTransformRenderPass( Ecs::Entities& entities, const Renderers& renderers ) : RenderPass( entities, "SpriteTransformRenderPass", renderers )
{
}

// ** SpriteTransformRenderPass::process
void SpriteTransformRenderPass::process( u32 currentTime, f32 dt, SceneObject& sceneObject, Sprite& sprite, Transform& transform )
{
	const Quat& rotation = transform.rotation();

	f32  x	  = transform.x();
	f32  y	  = transform.y();
	Vec3 left = rotation.rotate( Vec3( 10, 0, 0 ) );
	Vec3 up   = rotation.rotate( Vec3( 0, 10, 0 ) );

	m_renderers.m_renderer2d->line( x, y, x + left.x, y + left.y, Rgba( 1.0f, 0.0f, 0.0f ) );
	m_renderers.m_renderer2d->line( x, y, x + up.x,   y + up.y,   Rgba( 0.0f, 1.0f, 0.0f ) );
}

// ** SpriteTransformRenderPass::begin
bool SpriteTransformRenderPass::begin( u32 currentTime )
{
	m_renderers.m_renderer2d->begin( m_viewProj );
	return true;
}

// ** SpriteTransformRenderPass::end
void SpriteTransformRenderPass::end( void )
{
	m_renderers.m_renderer2d->end();
}

} // namespace scene

DC_END_DREEMCHEST