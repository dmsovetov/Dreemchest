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

#ifndef __DC_Scene_Component_Rendering_H__
#define __DC_Scene_Component_Rendering_H__

#include "Component.h"

#include "../Assets/Mesh.h"
#include "../Assets/Image.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! This component is attached to a camera to render unlit meshes.
	class RenderUnlitMeshes : public SceneComponent {
		OverrideComponent( RenderUnlitMeshes, SceneComponent )
	};

	//! This component is attached to a camera to render sprites.
	class RenderSprites : public SceneComponent {
		OverrideComponent( RenderSprites, SceneComponent )
	};

	//! Holds the static mesh data with per-instance materials.
	class StaticMesh : public SceneComponent {
	public:

									OverrideComponent( StaticMesh, SceneComponent )

									//! Constructs StaticMesh instance.
									StaticMesh( const MeshPtr& mesh = MeshPtr() )
										: m_mesh( mesh ) {}

		//! Returns mesh to be rendered.
		const MeshPtr&				mesh( void ) const;

		//! Sets a mesh to be rendered.
		void						setMesh( const MeshPtr& value );

		//! Returns the total number of materials.
		u32							materialCount( void ) const;

		//! Returns the material by index.
		MaterialPtr					material( u32 index ) const;

		//! Sets the material by index.
		void						setMaterial( u32 index, const MaterialPtr& value );

		//! Returns a lightmap texture.
		const renderer::TexturePtr&	lightmap( void ) const;

		//! Sets a lightmap texture.
		void						setLightmap( const renderer::TexturePtr& value );

	private:

		MeshPtr						m_mesh;			//!< Mesh to be rendered.
		Array<MaterialPtr>			m_materials;	//!< Mesh materials array.
		renderer::TexturePtr		m_lightmap;		//!< Lightmap texture that is rendered for this mesh.
	};

	//! Holds the sprite rendering info.
	class Sprite : public SceneComponent {
	public:
			
									OverrideComponent( Sprite, SceneComponent )

									//! Constructs the Sprite instance.
									Sprite( const ImagePtr& image = ImagePtr(), const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) )
										: m_image( image ), m_color( color ) {}

		//! Returns the sprite image.
		const ImagePtr&				image( void ) const;

		//! Returns the sprite color.
		const Rgba&					color( void ) const;

	private:

		ImagePtr					m_image;	//!< Sprite image.
		Rgba						m_color;	//!< Sprite color.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Rendering_H__    */