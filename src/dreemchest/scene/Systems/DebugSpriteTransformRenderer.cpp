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

#include "DebugSpriteTransformRenderer.h"

#include "../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ** DebugSpriteTransformRenderPass::DebugSpriteTransformRenderPass
DebugSpriteTransformRenderPass::DebugSpriteTransformRenderPass( ecs::Entities& entities, const Renderer& renderer ) : RenderPass( entities, "DebugSpriteTransformRenderPass", renderer )
{
}

// ** DebugSpriteTransformRenderPass::process
void DebugSpriteTransformRenderPass::process( u32 currentTime, f32 dt, SceneObject& sceneObject, Sprite& sprite, Transform& transform )
{
	m_renderer.m_renderer2d->point( transform.x(), transform.y(), Rgba( 0.0f, 1.0f, 0.0f, 1.0f ) );
}

// ** DebugSpriteTransformRenderPass::begin
bool DebugSpriteTransformRenderPass::begin( u32 currentTime )
{
	m_renderer.m_renderer2d->begin( m_viewProj );
	return true;
}

// ** DebugSpriteTransformRenderPass::end
void DebugSpriteTransformRenderPass::end( void )
{
	m_renderer.m_renderer2d->end();
}

} // namespace scene

DC_END_DREEMCHEST