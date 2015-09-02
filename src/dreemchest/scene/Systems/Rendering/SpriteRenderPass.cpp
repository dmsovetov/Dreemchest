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

#include "SpriteRenderPass.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ** SpriteRenderPass::SpriteRenderPass
SpriteRenderPass::SpriteRenderPass( Ecs::Entities& entities, const Renderer& renderer ) : RenderPass( entities, "SpriteRenderPass", renderer )
{
}

// ** SpriteRenderPass::process
void SpriteRenderPass::process( u32 currentTime, f32 dt, SceneObject& sceneObject, Sprite& sprite, Transform& transform )
{
	// Get the sprite texture
	ImagePtr image = sprite.image();
	renderer::Texture2DPtr texture = image->requestTexture( m_renderer.m_hal.get() );

	// Get the rotation
	f32  angle = radians( transform.rotationZ() );
	f32  s	   = sinf( angle );
	f32  c	   = cosf( angle );

	// Calculate the basis vectors from rotation
	Vec2 up   = Vec2( s, c );
	Vec2 side = Vec2( up.y, -up.x );

	// Render oriented quad
	m_renderer.m_renderer2d->orientedQuad( texture, transform.x(), transform.y(), image->width() * 0.5f, image->height() * 0.5f, up, side, sprite.color() );
}

// ** SpriteRenderPass::begin
bool SpriteRenderPass::begin( u32 currentTime )
{
	m_renderer.m_renderer2d->begin( m_viewProj );
	return true;
}

// ** SpriteRenderPass::end
void SpriteRenderPass::end( void )
{
	m_renderer.m_renderer2d->end();
}

} // namespace scene

DC_END_DREEMCHEST