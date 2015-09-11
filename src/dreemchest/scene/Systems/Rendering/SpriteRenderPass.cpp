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

namespace Scene {

// ** SpriteRenderPass::SpriteRenderPass
SpriteRenderPass::SpriteRenderPass( Ecs::Entities& entities, const Rendering& rendering ) : RenderPass2D( entities, "SpriteRenderPass", rendering )
{
}

// ** SpriteRenderPass::process
void SpriteRenderPass::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, Sprite& sprite, Transform& transform )
{
	// Get the sprite texture
	ImagePtr			 image   = sprite.image();
	AssetTexturePtr		 data    = image->data();
	Renderer::TexturePtr texture = data.valid() ? data->texture : Renderer::TexturePtr();

	// Get the rotation
	f32  angle = radians( transform.rotationZ() );
	f32  s	   = sinf( angle );
	f32  c	   = cosf( angle );

	// Calculate the basis vectors from rotation
	Vec2 up   = Vec2( s, c );
	Vec2 side = Vec2( up.y, -up.x );

	// Calculate the sprite dimensions
	f32 width  = image->width()  * transform.scaleX() * 0.5f;
	f32 height = image->height() * transform.scaleY() * 0.5f;

	// Render oriented quad
	m_rendering.m_renderer2d->orientedQuad( texture, transform.x(), transform.y(), width,height, up, side, sprite.color() );
}

} // namespace Scene

DC_END_DREEMCHEST