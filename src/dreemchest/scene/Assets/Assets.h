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

namespace scene {

	//! Basic scene asset
	class Asset : public RefCounted {
	public:

								ClassEnableTypeInfo( Asset )

		//! Available asset types.
		enum Type {
			  Image				= BIT( 0 )
			, Mesh				= BIT( 1 )
			, Material			= BIT( 2 )
			, TotalAssetTypes	= 3
			, All				= ~0
		};

								//! Constructs Asset instance.
								Asset( AssetBundle* bundle, Type type, const String& name );

		//! Returns asset type.
		Type					type( void ) const;

		//! Returns asset name.
		const String&			name( void ) const;

		//! Returns the root asset bundle.
		const AssetBundleWPtr&	bundle( void ) const;

		//! Loads an asset.
		bool					load( void );

		//! Loads an asset from stream.
		virtual bool			loadFromStream( const io::StreamPtr& stream );

		//! Unloads asset.
		virtual void			unload( void );

	private:

		AssetBundleWPtr			m_bundle;	//!< Parent asset bundle.
		Type					m_type;		//!< Asset type.
		String					m_name;		//!< Asset name.
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
		AssetPtr				findAsset( const String& name, u32 expectedType = Asset::All );

		//! Returns the full asset path by an identifier.
		io::Path				assetPathByName( const String& name ) const;

		//! Creates the new Image asset inside this bundle.
		ImagePtr				addImage( const String& name, ImageFormat format, u32 width, u32 height );

		//! Creates the new Material asset inside this bundle.
		MaterialPtr				addMaterial( const String& name );

		//! Creates an AssetBundle instance.
		static AssetBundlePtr	create( const String& name, const io::Path& path, const String& json );

	private:

								//! Constructs AssetBundle instance.
								AssetBundle( const String& name, const io::Path& path );

		//! Loads assets from JSON string.
		bool					loadFromJson( const String& json );

	private:

		//! Container type to store available assets.
		typedef Hash<AssetPtr>	Assets;

		io::Path				m_path;		//!< Asset bundle physical path.
		String					m_name;		//!< Asset bundle name.
		Assets					m_assets;	//!< Available assets.
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

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Assets_H__    */