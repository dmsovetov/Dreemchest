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

#ifndef __DC_Scene_SpriteRenderer_System_H__
#define __DC_Scene_SpriteRenderer_System_H__

#include "SceneSystem.h"

#include "../Components/Rendering.h"
#include "../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! The sprite rendering system
	class SpriteRenderer : public SceneSystem2<MeshRenderer, Transform2D> {
	public:

									//! Constructs the SpriteRenderer instance.
									SpriteRenderer( ecs::Entities& entities, renderer::Hal* hal );

		//! Sets the view/projection matrices used for rendering
		void						setViewProjection( const Matrix4& viewProjection );

	protected:

		//! Available shader types.
		enum ShaderType {
			  ShaderSolid = 0
			, TotalShaderTypes
		};

		//! Rendering operation emitted by scene object processing.
		struct RenderOp {
			u32						shader;			//!< Material shader.
			renderer::VertexBuffer*	vertexBuffer;	//!< Vertex buffer to be used for rendering.
			renderer::IndexBuffer*	indexBuffer;	//!< Index buffer to be used for rendering.
			renderer::Texture*		texture;		//!< Texture to be used for rendering.
			const Rgba*				diffuse;		//!< Diffuse color.
			Matrix4					mvp;			//!< Model/view/projection matrix.
		};

		//! Renders a single entity with a mesh
		virtual void				process( u32 currentTime, f32 dt, SceneObject& sceneObject, MeshRenderer& meshRenderer, Transform2D& transform );

		//! Called every frame before any entites are rendered.
		virtual bool				begin( u32 currentTime );

		//! Called every frame after all entities has been rendered.
		virtual void				end( void );

		//! Emits a new render operation.
		RenderOp*					emitRenderOp( void );

		//! Sets the shader for current render operation.
		void						setShader( const RenderOp* rop );

		//! Sorts render operations by shader used, then by texture, then by mesh data.
		static bool					sortByShaderTextureMesh( const RenderOp* a, const RenderOp* b );

	protected:

		//! Render operations list container
		typedef List<const RenderOp*> EmittedRenderOps;

		//! Internal structure to hold current rendering state.
		struct RenderedFrame {
			renderer::VertexBuffer*	m_vertexBuffer;			//!< Active vertex buffer.
			renderer::Texture*		m_texture;				//!< Active texture
			u32						m_materialShader;		//!< Active material shader
			EmittedRenderOps		m_renderOperations;		//!< List of emitted render operations.

			//! Clears the frame
			void clear( void )
			{
				m_vertexBuffer = NULL;
				m_texture = NULL;
				m_materialShader = -1;
				m_renderOperations.clear();
			}
		};
		
		renderer::Hal*				m_hal;							//!< Renderer HAL.
		renderer::Shader*			m_shaders[TotalShaderTypes];	//!< Loaded shaders.
		ArrayAllocator<RenderOp>	m_renderOperations;				//!< Array allocator used to allocated render operations.
		RenderedFrame				m_frame;						//!< The frame to be rendered.
		Matrix4						m_viewProjection;				//!< The view-projection matrix.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_SpriteRenderer_System_H__    */