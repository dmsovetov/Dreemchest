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

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Material class.
	class Material {
	public:

		//! Material features.
		enum Feature {
			  FeatureDiffuse	= BIT( 1 )
		};

		//! Material lighting model.
		enum Model {
			  Unlit
			, Ambient
			, Phong
			, TotalModels
		};

		//! Material layers.
        enum Layer {
              Diffuse				//!< Diffuse color layer.
            , Specular				//!< Specular color layer.
			, Normal				//!< Normal map layer.
			, Heightmap				//!< Heightmap layer.
			, AmbientOcclusion		//!< Ambient occlusion layer.
			, Emission				//!< Emission color layer.
			, DiffuseDetail			//!< Diffuse map detail layer.
			, NormalDetail			//!< Normal map detail layer.
			, Tint					//!< Tint color layer.

            , TotalMaterialLayers	//!< The total number of material layers.
        };

									//! Constructs Material instance.
									Material( void );

		//! Returns material rendering mode.
		RenderingMode				renderingMode( void ) const;

		//! Sets material rendering mode.
		void						setRenderingMode( RenderingMode value );

		//! Returns material light model.
		Model						model( void ) const;

		//! Sets material light model.
		void						setModel( Model value );

		//! Returns material feature set.
		u32							features( void ) const;

        //! Returns a material color.
        const Rgba&					color( Layer layer ) const;

		//! Sets material color.
		void						setColor( Layer layer, const Rgba& value );

        //! Returns a material texture.
        ImageHandle			        texture( Layer layer ) const;

		//! Sets material texture.
		void						setTexture( Layer layer, ImageHandle value );

		//! Returns the diffuse material texture.
		ImageHandle					diffuse( void ) const;

		//! Sets the diffuse material texture.
		void						setDiffuse( ImageHandle value );

	private:

		//! Updates material features.
		void						updateMaterialFeatures( void );

	private:

		Model						m_model;						//!< Material light model.
		RenderingMode				m_renderingMode;				//!< Material blending.
		u32							m_features;						//!< Used material features.
        Rgba						m_color[TotalMaterialLayers];	//!< Material colors.
		ImageHandle					m_texture[TotalMaterialLayers];	//!< Material textures.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Material_H__    */