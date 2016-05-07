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

    class SpriteRenderSystem : public RenderSystemBase {
    public:

                                        SpriteRenderSystem( RenderingContext& context, RenderScene& renderScene, f32 scaleFactor = 1.0f );

    protected:

        virtual void			        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform ) NIMBLE_OVERRIDE;
    
		//! Allocates and initializes an index buffer of a specified size.
		u16*							allocateTrianleIndexBuffer( s32 triangleCount ) const;

		//! Writes transformed vertices of a single sprite instance.
		void							emitSpriteVertices( void* vertices, s32 offset, const Matrix4& transform, s32 width, s32 height, const Rgba& color ) const;

	private:

		//! A maximum number of sprites that can be rendered in a single batch
		enum { MaxSpritesInBatch = 1000 };

		f32								m_scaleFactor;
		VertexFormat					m_vertexFormat;
		RenderResource					m_vertexBuffer;
		RenderResource					m_indexBuffer;
		RenderResource					m_inputLayout;
		u16*							m_indices;
		void*							m_vertices;
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_SpriteRenderSystem_H__    */