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
		virtual bool		loadFromStream( AssetBundleWPtr assets, AssetWPtr asset, const Io::StreamPtr& stream ) { return false; }
	};

	//! Generic asset loader type to subclass custom asset loaders from.
	template<typename TAsset>
	class GenericAssetLoader : public AssetLoader {
	public:

		//! Starts loading an asset from input stream.
		virtual bool		loadFromStream( AssetBundleWPtr assets, AssetWPtr asset, const Io::StreamPtr& stream ) DC_DECL_OVERRIDE;

	protected:

		WeakPtr<TAsset>		m_asset;	//!< Target asset to load data into.
	};

	// ** GenericAssetLoader::loadFromStream
	template<typename TAsset>
	bool GenericAssetLoader<TAsset>::loadFromStream( AssetBundleWPtr assets, AssetWPtr asset, const Io::StreamPtr& stream )
	{
		DC_BREAK_IF( !asset.valid() );

		// Type cast an asset.
		m_asset = castTo<TAsset>( asset.get() );
		DC_BREAK_IF( !m_asset.valid() );

		return m_asset.valid();		
	}

	//! Loads an image from a raw pixel buffer format.
	class ImageLoaderRAW : public GenericAssetLoader<Image> {
	public:

		//! Loads image data from an input stream.
		virtual bool		loadFromStream( AssetBundleWPtr assets, AssetWPtr asset, const Io::StreamPtr& stream ) DC_DECL_OVERRIDE;
	};

	//! Loads a mesh from a raw format.
	class MeshLoaderRAW : public GenericAssetLoader<Mesh> {
	public:

		//! Fat mesh vertex.
		struct Vertex {
			f32				position[3];	//!< Vertex position.
			f32				normal[3];		//!< Vertex normal.
			f32				tex0[2];		//!< UV layer 0.
			f32				tex1[2];		//!< UV layer 1.
		};

		//! Loads image data from an input stream.
		virtual bool		loadFromStream( AssetBundleWPtr assets, AssetWPtr asset, const Io::StreamPtr& stream ) DC_DECL_OVERRIDE;
	};

	//! Loads a material from a JSON format.
	class MaterialLoader : public GenericAssetLoader<Material> {
	public:

		//! Loads image data from an input stream.
		virtual bool		loadFromStream( AssetBundleWPtr assets, AssetWPtr asset, const Io::StreamPtr& stream ) DC_DECL_OVERRIDE;
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Assets_Loaders_H__    */