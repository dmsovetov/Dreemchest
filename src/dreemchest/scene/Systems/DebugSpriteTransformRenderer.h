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

#ifndef __DC_Scene_Systems_DebugSpriteTransformRenderer_H__
#define __DC_Scene_Systems_DebugSpriteTransformRenderer_H__

#include "RenderSystem.h"

#include "../Components/Rendering.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! The sprite position rendering pass
	class DebugSpriteTransformRenderPass : public RenderPass<Sprite> {
	public:

									//! Constructs the DebugSpriteTransformRenderPass instance.
									DebugSpriteTransformRenderPass( ecs::Entities& entities, const Renderer& renderer );

	private:

		//! Renders a position point of a sprite.
		virtual void				process( u32 currentTime, f32 dt, SceneObject& sceneObject, Sprite& sprite, Transform& transform );

		//! Sets the view-projection matrix for 2D rendering.
		virtual bool				begin( u32 currentTime );

		//! Flushes the generated 2D mesh.
		virtual void				end( void );
	};

	//! Define the sprite position renderer
	typedef SinglePassRenderer<DebugRenderSpriteTransform, DebugSpriteTransformRenderPass> DebugSpriteTransformRenderer;

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Systems_DebugSpriteTransformRenderer_H__    */