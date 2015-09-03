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

#ifndef __DC_Scene_StaticMeshRenderPass_H__
#define __DC_Scene_StaticMeshRenderPass_H__

#include "RenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! The mesh rendering pass
	class StaticMeshRenderPass : public RenderPass<StaticMesh> {
	public:

									//! Constructs the StaticMeshRenderPass instance.
									StaticMeshRenderPass( Ecs::Entities& entities, const Renderers& renderers );

	protected:

		//! Available shader types.
		enum ShaderType {
			  ShaderInvalid
			, ShaderSolid
			, TotalShaderTypes
		};

		//! Renders a single entity with a mesh
		virtual void				process( u32 currentTime, f32 dt, SceneObject& sceneObject, StaticMesh& staticMesh, Transform& transform );

		//! Called every frame before any entites are rendered.
		virtual bool				begin( u32 currentTime );

		//! Called every frame after all entities has been rendered.
		virtual void				end( void );

	protected:
		
		Rvm							m_rvm;							//!< The rendering virtual machine that performs rendering.
		Renderer::ShaderPtr			m_shaders[TotalShaderTypes];	//!< Loaded shaders.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_UnlitMeshRenderer_H__    */