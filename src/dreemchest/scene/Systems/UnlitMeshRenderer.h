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

#ifndef __DC_Scene_UnlitMeshRenderer_H__
#define __DC_Scene_UnlitMeshRenderer_H__

#include "RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! The mesh rendering pass
	class MeshRendererPass : public RenderPass<StaticMesh> {
	public:

									//! Constructs the MeshRendererPass instance.
									MeshRendererPass( ecs::Entities& entities, const Renderer& renderer );

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
		virtual void				process( u32 currentTime, f32 dt, SceneObject& sceneObject, StaticMesh& staticMesh, Transform& transform );

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
		
		renderer::ShaderPtr			m_shaders[TotalShaderTypes];	//!< Loaded shaders.
		ArrayAllocator<RenderOp>	m_renderOperations;				//!< Array allocator used to allocated render operations.
		RenderedFrame				m_frame;						//!< The frame to be rendered.
	};

	//! Define the unlit mesh render system
	typedef SinglePassRenderer<RenderUnlitMeshes, MeshRendererPass> UnlitMeshRenderer;

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_UnlitMeshRenderer_H__    */