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

#ifndef __DC_Scene_StaticMeshRopEmitter_H__
#define __DC_Scene_StaticMeshRopEmitter_H__

#include "RopEmitter.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Emits render operations for static meshes in scene.
	class StaticMeshRopEmitter : public RopEmitter<StaticMesh> {
	public:

		//! Features that are extracted from material.
		enum MaterialFeature {
			  DiffuseTexture = BIT( 0 )												//!< Use material diffuse texture.
			, Shader		 = BIT( 1 )												//!< Use material shader.
			, LightModel	 = BIT( 2 )												//!< Use material light model.
			, RenderingMode  = BIT( 3 )												//!< Use material rendering mode.
			, Distance		 = BIT( 4 )												//!< Calculate the distance to rendered object.
			, Unlit			 = DiffuseTexture | Shader | RenderingMode | Distance	//!< Material features used by unlit renderer.
			, All			 = ~0													//!< Use all material features.
		};

								//! Constructs StaticMeshEmitter instance.
								StaticMeshRopEmitter( Ecs::EcsWPtr ecs, u32 features = 0, Material::Model model = Material::Unlit );

	private:

		//! Emits render operations for all static meshes in scene.
		virtual void			emit( Rvm& rvm, ShaderCache& shaders, const StaticMesh& staticMesh, const Transform& transform );

	private:

		FlagSet32				m_features;		//!< Extracted material features mask.
		FlagSet8				m_renderModes;	//!< Render mode mask.
		Material::Model			m_model;		//!< Default material model.

	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_StaticMeshRopEmitter_H__    */