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

#if 0

#include "Scene.h"
#include "Material.h"
#include "Mesh.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! A basic scene renderer
	class Renderer : public RefCounted {
	friend class Scene;
	public:

								//! Constructs a Renderer instance.
								Renderer( Renderer::Hal* hal );
		virtual					~Renderer( void ) {}

		//! Renders a scene.
		virtual void			render( const Matrix4& view, const Matrix4& proj, const Scene* scene );

	private:

		//! Renders a scene object with a specified shader.
		void					render( const Matrix4& view, const Matrix4& proj, Renderer::Shader* shader, const SceneObjectPtr& sceneObject );

		//! Renders a mesh chunk to a screen.
		void					render( const Mesh::Chunk& chunk );

	private:

		Renderer::Hal*			m_hal;										//!< Renderer HAL.
		Renderer::Shader*		m_shaders[Material::TotalMaterialShaders];	//!< Material shaders.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif

#endif    /*    !__DC_Scene_Mesh_H__    */