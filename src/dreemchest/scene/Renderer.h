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

#include "Scene.h"
#include "Material.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! A basic scene renderer
	class Renderer : public RefCounted {
	friend class Scene;
	public:

								//! Constructs a Renderer instance.
								Renderer( renderer::Hal* hal );
		virtual					~Renderer( void ) {}

		//! Renders a scene.
		virtual void			render( const Matrix4& view, const Matrix4& proj, const ScenePtr& scene );

	private:

		//! Renders a scene object with a specified shader.
		void					render( const Matrix4& view, const Matrix4& proj, renderer::Shader* shader, const SceneObjectPtr& sceneObject );

	private:

		renderer::Hal*			m_hal;										//!< Renderer HAL.
		renderer::Shader*		m_shaders[Material::TotalMaterialShaders];	//!< Material shaders.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Mesh_H__    */