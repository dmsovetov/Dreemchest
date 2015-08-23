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

#ifndef __DC_Scene_Renderer_H__
#define __DC_Scene_Renderer_H__

#include "SceneSystem.h"

#include "../Components/Rendering.h"
#include "../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! The scene rendering system
	class Renderer : public SceneSystem2<MeshRenderer, Transform2D> {
	public:

							//! Constructs a Renderer instance.
							Renderer( ecs::Entities& entities, renderer::Hal* hal );

		//! Renders a single entity with a mesh
		virtual void		process( u32 currentTime, f32 dt, SceneObject& sceneObject, MeshRenderer& meshRenderer, Transform2D& transform );

		//! Called every frame before any entites are rendered.
		virtual bool		begin( u32 currentTime );

		//! Called every frame after all entities has been rendered.
		virtual void		end( void );

		//! Sets the view/projection matrices used for rendering
		void				setViewProjection( const Matrix4& viewProjection );

	protected:

		//! Available shader types.
		enum ShaderType {
			  ShaderSolid = 0
			, TotalShaderTypes
		};

		//! Renderer HAL.
		renderer::Hal*		m_hal;

		//! Loaded shaders.
		renderer::Shader*	m_shaders[TotalShaderTypes];

		//! The view-projection matrix.
		Matrix4				m_viewProjection;
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Renderer_H__    */