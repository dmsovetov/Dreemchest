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

#ifndef __DC_Scene_ShaderCache_H__
#define __DC_Scene_ShaderCache_H__

#include "../Scene.h"
#include "../Assets/Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

#if DEV_DEPRECATED_SCENE_RENDERER

	//! All shaders used for scene rendering reside here.
	class ShaderCache : public RefCounted {
	public:

		//! Available shaders.
		enum ShaderId {
			  Null
			, ConstantColor
			, Normals
			, TotalShaders
		};

									//! Constructs the ShaderCache
									ShaderCache( const String& folder, const Renderer::HalPtr& hal );

		//! Returns the shader by name.
		const Renderer::ShaderPtr&	shaderById( ShaderId id );

		//! Returns the material shader with specified features.
		const Renderer::ShaderPtr&	materialShader( Material::Model model, u32 features = 0 );

		//! Loads the shader code from a file.
		static bool					loadFromFile( const String& fileName, String& vertex, String& fragment );

	private:

		//! Shader code container.
		struct Code {
			CString		m_vertex;	//!< Vertex shader code.
			CString		m_fragment;	//!< Fragment shader code.
		};

		static Code					s_shaderCode[TotalShaders];			//!< Shader code.
		static CString				s_shaderName[TotalShaders];			//!< Shader names.
		static CString				s_modelName[Material::TotalModels];	//!< Light model names.

		Renderer::HalPtr			m_hal;								//!< The rendering HAL.
		Renderer::ShaderPtr			m_shaders[TotalShaders];			//!< Cached shaders.
		ShaderPtr					m_material[Material::TotalModels];	//!< Unlit uber shader.
	};

	//! Scene shader with a support of permutations.
	class Shader : public RefCounted {
	public:

									//! Constructs the Shader instance.
									Shader( const String& vertex, const String& fragment );

		//! Returns the shader permutation.
		const Renderer::ShaderPtr&	permutation( Renderer::HalPtr hal, u32 features );

		//! Returns true if the shader permutation is loaded.
		bool						isLoaded( u32 features ) const;

		//! Adds a feature to shader.
		void						addFeature( u32 mask, const String& name );

	private:

		//! A single shader feature.
		struct Feature {
			u32						mask;		//!< Feature mask.
			String					name;		//!< Feature name.
		};

		String						m_vertex;	//!< Vertex shader code.
		String						m_fragment;	//!< Fragment shader code.
		Array<Feature>				m_features;	//!< Available shader features.
		Array<Renderer::ShaderPtr>	m_shaders;	//!< Compiled shader permutations.
	};

#endif  /*  DEV_DEPRECATED_SCENE_RENDERER   */

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_ShaderCache_H__    */