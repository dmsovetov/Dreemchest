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

#ifndef __DC_Scene_Rendering_SpriteRenderSystem_H__
#define __DC_Scene_Rendering_SpriteRenderSystem_H__

#include "../RenderSystem/RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Renders all sprites that reside in scene.
    class SpriteRenderSystem : public RenderSystem<SpriteRenderer> {
    public:

                                        SpriteRenderSystem( RenderingContext& context, RenderScene& renderScene );

    protected:

		//! Emits render operations for rendered sprites.
        virtual void			        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const SpriteRenderer& spriteRenderer ) NIMBLE_OVERRIDE;
    
		//! Allocates and initializes an index buffer of a specified size.
		u16*							allocateTrianleIndexBuffer( s32 triangleCount ) const;

		//! Writes transformed vertices of a single sprite instance.
		void							emitSpriteVertices( void* vertices, s32 offset, const Matrix4& transform, s32 width, s32 height, const Rgba& color, f32 scaleFactor ) const;

	private:

		//! A maximum number of sprites that can be rendered in a single batch
		enum { MaxSpritesInBatch = 1000 };

		VertexFormat					m_vertexFormat;		//!< A 2D vertex format.
		RenderResource					m_vertexBuffer;		//!< An intermediate vertex buffer used for batching.
		RenderResource					m_indexBuffer;		//!< A static index buffer with pre-allocated set of indices.
		RenderResource					m_inputLayout;		//!< An input layout constructed from a vertex format.
		u16*							m_indices;			//!< An allocated static index buffer.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_SpriteRenderSystem_H__    */