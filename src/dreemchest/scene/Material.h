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

#ifndef __DC_Scene_Material_H__
#define __DC_Scene_Material_H__

#include "Scene.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! Mesh material class.
	class Material : public RefCounted {
	public:

		//! Material layers.
        enum Layer {
            Diffuse,
            Specular,
			Tint,
            TotalMaterialLayers
        };

		//! Material shader.
		enum Shader {
			Unknown,
			Solid,
			Transparent,
			Additive,
			TotalMaterialShaders
		};

		//! Returns material shader.
		Shader						shader( void ) const;

		//! Sets material shader.
		void						setShader( Shader value );

        //! Returns a material color.
        const Rgba&					color( Layer layer ) const;

		//! Sets material color.
		void						setColor( Layer layer, const Rgba& value );

        //! Returns a material texture.
        const renderer::TexturePtr&	texture( Layer layer ) const;

		//! Sets material texture.
		void						setTexture( Layer layer, const renderer::TexturePtr& value );

		//! Creates a new Material instance.
		static MaterialPtr			create( void );

	private:

									//! Constructs Material instance.
									Material( void );

	private:

		Shader						m_shader;						//!< Material shader.
        Rgba						m_color[TotalMaterialLayers];	//!< Material colors.
		renderer::TexturePtr		m_texture[TotalMaterialLayers];	//!< Material textures.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Material_H__    */