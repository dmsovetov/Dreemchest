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

#ifndef __DC_Scene_Assets_H__
#define __DC_Scene_Assets_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Asset data stores the loaded asset data (HAL texture, vertex buffers, etc.).
	class AssetData : public RefCounted {
	public:
	};

	//! The loaded HAL texture.
	class AssetTexture : public AssetData {
	public:
		Renderer::TexturePtr	texture;	//!< Reference to a loaded texture.
	};

	//! The loaded mesh data.
	class AssetMesh : public AssetData {
	public:
		Array<Renderer::VertexBufferPtr>	vertexBuffers;	//!< Mesh vertex buffers.
		Array<Renderer::IndexBufferPtr>		indexBuffers;	//!< Mesh index buffers.
	};

	//! Basic scene asset
	class Asset : public RefCounted {
	public:

								ClassEnableTypeInfo( Asset )

		//! Available asset types.
		enum Type {
			  Image				= BIT( 0 )
			, Mesh				= BIT( 1 )
			, Material			= BIT( 2 )
			, Scene				= BIT( 3 )
			, TotalAssetTypes	= 3
			, All				= ~0
		};

		//! Asset loading state.
		enum State {
			Unknown,		//!< The asset state is unknown.
			Unloaded,		//!< The asset is unloaded.
			Loading,		//!< The asset is now loading.
			Loaded,			//!< The asset is loaded.
			LoadingError	//! An error was encountered while loading an asset.
		};

								//! Constructs Asset instance.
								Asset( AssetBundle* bundle, Type type, const String& uuid, const String& name );

		//! Returns asset type.
		Type					type( void ) const;

		//! Returns the loader attached to an asset.
		const AssetLoaderPtr&	loader( void ) const;

		//! Sets the asset loader.
		void					setLoader( const AssetLoaderPtr& value );

		//! Returns asset name.
		const String&			name( void ) const;

		//! Returns asset uuid.
		const String&			uuid( void ) const;

		//! Returns asset state.
		State					state( void ) const;

		//! Returns true if an asset needs to be loaded.
		bool					needsLoading( void ) const;

		//! Returns the root asset bundle.
		const AssetBundleWPtr&	bundle( void ) const;

		//! Loads an asset.
		bool					load( const Renderer::HalPtr& hal );

		//! Unloads asset.
		virtual void			unload( void );

	private:

		AssetLoaderPtr			m_loader;	//!< Asset loader instance.
		AssetBundleWPtr			m_bundle;	//!< Parent asset bundle.
		Type					m_type;		//!< Asset type.
		String					m_name;		//!< Asset name.
		String					m_uuid;		//!< Asset UUID.
		State					m_state;	//!< Asset state.
	};

	//! Generic asset class to derive other asset types from it.
	template<typename TData>
	class AssetWithData : public Asset {
	public:

		//! Returns the asset data.
		const StrongPtr<TData>&	data( void ) const { return m_data; }

		//! Sets the asset data.
		void					setData( const StrongPtr<TData>& value ) { m_data = value; }

	protected:

								//! Constructs AssetWithData instance.
								AssetWithData( AssetBundle* bundle, Type type, const String& uuid, const String& name )
									: Asset( bundle, type, uuid, name ) {}

		//! Unloads the asset data.
		virtual void			unload( void ) { m_data = StrongPtr<TData>(); Asset::unload(); }

	private:

		StrongPtr<TData>		m_data;		//!< Loaded asset data.
	};

	//! Contains asset meta information & caches loaded assets.
	class AssetBundle : public RefCounted {
	public:

		//! Returns asset bundle name.
		const String&			name( void ) const;

		//! Returns an asset of template type T with specified name.
		template<typename T>
		StrongPtr<T>			find( const String& name );

		//! Returns asset by name.
		AssetPtr				findAsset( const String& name, u32 expectedType = Asset::All ) const;

		//! Returns the full asset path by an identifier.
		io::Path				assetPathByIdentifier( const String& name ) const;

		//! Creates the new Image asset inside this bundle.
		ImagePtr				addImage( const String& uuid, const String& name, u32 width, u32 height );

		//! Creates the new Material asset inside this bundle.
		MaterialPtr				addMaterial( const String& uuid, const String& name );

		//! Creates the new Asset inside this bundle.
		AssetPtr				addAsset( Asset::Type type, const String& uuid, const String& name );

		//! Creates the new Mesh asset inside this bundle.
		MeshPtr					addMesh( const String& uuid, const String& name );

		//! Creates an AssetBundle instance and loads it from a JSON file.
		static AssetBundlePtr	createFromJson( const String& name, const io::Path& path, const String& fileName );

	private:

								//! Constructs AssetBundle instance.
								AssetBundle( const String& name, const io::Path& path );

		//! Loads assets from JSON string.
		bool					loadFromJson( const String& json );

	private:

		//! Container type to store available assets.
		typedef Hash<AssetPtr>	Assets;

		io::Path				m_path;				//!< Asset bundle physical path.
		String					m_name;				//!< Asset bundle name.
		Assets					m_assets;			//!< Identifier to asset mapping.
	};

	// ** AssetBundle::find
	template<typename T>
	StrongPtr<T> AssetBundle::find( const String& name )
	{
		AssetPtr asset = findAsset( name );

		if( asset == AssetPtr() ) {
			return StrongPtr<T>();
		}

		T* casted = castTo<T>( asset.get() );

		return StrongPtr<T>( casted );
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Assets_H__    */