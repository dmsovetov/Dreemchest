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

	//! Basic scene asset
	class Asset : public RefCounted {
	friend class AssetBundle;
	public:

								ClassEnableTypeInfo( Asset )

		//! Available asset types.
		enum Type {
			  Image				= BIT( 0 )
			, Material			= BIT( 1 )
			, Mesh				= BIT( 2 )
			, Terrain			= BIT( 3 )
			, Prefab			= BIT( 4 )
			, Folder			= BIT( 5 )
			, TotalAssetTypes	= 6
			, All				= ~0
		};

		//! Asset loading state.
		enum State {
			Unknown,		//!< The asset state is unknown.
			Unloaded,		//!< The asset is unloaded.
			Loading,		//!< The asset is now loading.
			Loaded,			//!< The asset is loaded.
			Outdated,		//!< The asset should be reloaded.
			LoadingError	//! An error was encountered while loading an asset.
		};

								//! Constructs Asset instance.
								Asset( AssetBundle* bundle = NULL, Type type = TotalAssetTypes, const String& uuid = String(), const String& name = String() );

		//! Returns asset type.
		Type					type( void ) const;

		//! Returns the asset format.
		AssetFormat				format( void ) const;

		//! Sets the asset format.
		void					setFormat( AssetFormat value );

		//! Returns asset name.
		const String&			name( void ) const;

		//! Returns asset uuid.
		const String&			uuid( void ) const;

		//! Returns asset timestamp.
		u32						timestamp( void ) const;

		//! Sets asset timestamp.
		void					setTimestamp( u32 value );

		//! Returns asset state.
		State					state( void ) const;

		//! Sets asset state.
		void					setState( Asset::State value );

		//! Returns true if an asset needs to be loaded.
		bool					needsLoading( void ) const;

		//! Returns the root asset bundle.
		AssetBundleWPtr			bundle( void ) const;

		//! Returns asset data as key-value object.
		virtual Io::KeyValue	keyValue( void ) const;

		//! Reads asset data from key-value object.
		virtual bool			setKeyValue( const Io::KeyValue& value );

		//! Unloads asset data.
		virtual void			dispose( void );

		//! Returns asset name from type.
		static String			typeToString( Asset::Type type );

		//! Returns asset type from name.
		static Asset::Type		typeFromString( const String& type );

	private:

		//! Sets asset UUID.
		void					setUuid( const String& value );

		//! Sets parent asset bundle.
		void					setBundle( AssetBundleWPtr value );

		//! Sets asset name.
		void					setName( const String& value );

	private:

		AssetFormat				m_format;		//!< Asset format.
		AssetBundleWPtr			m_bundle;		//!< Parent asset bundle.
		Type					m_type;			//!< Asset type.
		String					m_name;			//!< Asset name.
		String					m_uuid;			//!< Asset UUID.
		State					m_state;		//!< Asset state.
		u32						m_timestamp;	//!< Last known asset file timestamp.
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
		Io::Path				assetPathByIdentifier( const String& name ) const;

		//! Creates the new Asset and loads it from key-value data.
		AssetPtr				createAssetFromData( const Io::KeyValue& data ) const;

		//! Creates new Asset by type.
		AssetPtr				createAssetByType( Asset::Type type ) const;

		//! Adds an asset to this bundle.
		void					addAsset( AssetPtr asset );

		//! Removes an Asset from bundle by UUID.
		void					removeAsset( const String& uuid );

		//! Sets asset name.
		bool					setAssetName( AssetWPtr asset, const String& value );

		//! Queues an asset for loading.
		void					queueForLoading( AssetWPtr asset );

		//! Removes an asset from loading queue.
		void					removeFromLoading( AssetWPtr asset );

		//! Returns an assets queued for loading.
		AssetSet				waitingForLoading( void ) const;

		//! Returns true if UUID file names should be used.
		bool					uuidFileNames( void ) const;

		//! Sets the UUID file names flag.
		void					setUuidFileNames( bool value );

		//! Creates an empty AssetBundle instance.
		static AssetBundlePtr	create( const String& name, const Io::Path& path = Io::Path() );

		//! Creates an AssetBundle instance and loads it from a key-value file.
		static AssetBundlePtr	createFromFile( const String& name, const Io::Path& path, const String& fileName );

		//! Creates an AssetBundle instance and loads it from a key-value string.
		static AssetBundlePtr	createFromString( const String& name, const Io::Path& path, const String& text );

	private:

								//! Constructs AssetBundle instance.
								AssetBundle( const String& name, const Io::Path& path );

		//! Loads assets from key-value string.
		bool					loadFromString( const String& text );

	private:

		//! Container type to store available assets.
		typedef Hash<AssetPtr>	Assets;

		//! Asset factory type.
		typedef AbstractFactory<Asset, Asset::Type>	AssetFactory;

		//! Container type to map from string to asset type.
		typedef Map<String, Asset::Type> AssetTypeByName;

		Io::Path				m_path;					//!< Asset bundle physical path.
		String					m_name;					//!< Asset bundle name.
		Assets					m_assets;				//!< Identifier to asset mapping.
		bool					m_uuidFileNames;		//!< Are the UUID file names used.
		AssetFactory			m_factory;				//!< Asset factory.
		AssetTypeByName			m_typeByName;			//!< Asset type by name.
		AssetSet				m_waitingForLoading;	//!< Assets waiting for loading.
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