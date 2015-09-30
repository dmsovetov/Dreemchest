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

#ifndef __DC_Scene_Assets_Loaders_H__
#define __DC_Scene_Assets_Loaders_H__

#include "Assets.h"

#include "Mesh.h"
#include "Image.h"
#include "Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Base class for all asset loaders.
	class AssetLoader : public RefCounted {
	public:

		//! Loads an asset data from an input stream.
		virtual bool		loadFromStream( AssetBundleWPtr assets, Renderer::HalPtr hal, const io::StreamPtr& stream ) const { return false; }
	};

	//! Loads an image from a raw pixel buffer format.
	class RawImageLoader : public AssetLoader {
	public:

							//! Constructs the RawImageLoader instance.
							RawImageLoader( ImagePtr image )
								: m_image( image ) {}

		//! Loads image data from an input stream.
		virtual bool		loadFromStream( AssetBundleWPtr assets, Renderer::HalPtr hal, const io::StreamPtr& stream ) const;

		//! Attaches this loader to an asset.
		static void			attachTo( ImagePtr image ) {
			AssetLoaderPtr loader( DC_NEW RawImageLoader( image ) );
			image->setLoader( loader );
		}

	private:

		mutable ImagePtr	m_image;	//!< Target image to load data into.
	};

	//! Loads a mesh from a raw format.
	class RawMeshLoader : public AssetLoader {
	public:

		//! Fat mesh vertex.
		struct Vertex {
			f32				position[3];	//!< Vertex position.
			f32				normal[3];		//!< Vertex normal.
			f32				tex0[2];		//!< UV layer 0.
			f32				tex1[2];		//!< UV layer 1.
		};

							//! Constructs the RawMeshLoader instance.
							RawMeshLoader( MeshPtr mesh )
								: m_mesh( mesh ) {}

		//! Loads image data from an input stream.
		virtual bool		loadFromStream( AssetBundleWPtr assets, Renderer::HalPtr hal, const io::StreamPtr& stream ) const;

		//! Attaches this loader to an asset.
		static void			attachTo( MeshPtr mesh ) {
			AssetLoaderPtr loader( DC_NEW RawMeshLoader( mesh ) );
			mesh->setLoader( loader );
		}

	private:

		mutable MeshPtr		m_mesh;	//!< Target mesh to load data into
	};

#ifdef DC_JSON_ENABLED

	//! Loads a material from a JSON format.
	class JsonMaterialLoader : public AssetLoader {
	public:

							//! Constructs the JsonMaterialLoader instance.
							JsonMaterialLoader( MaterialPtr material )
								: m_material( material ) {}

		//! Loads image data from an input stream.
		virtual bool		loadFromStream( AssetBundleWPtr assets, Renderer::HalPtr hal, const io::StreamPtr& stream ) const;

		//! Attaches this loader to an asset.
		static void			attachTo( MaterialPtr material ) {
			AssetLoaderPtr loader( DC_NEW JsonMaterialLoader( material ) );
			material->setLoader( loader );
		}

	private:

		mutable MaterialPtr	m_material;	//!< Target material to load data into
	};

#endif	/*	DC_JSON_ENABLED	*/

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Assets_Loaders_H__    */