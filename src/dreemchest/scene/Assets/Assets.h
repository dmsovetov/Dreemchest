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

#include "AssetHandle.h"
#include "AssetPool.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Asset type id.
    class AssetType {
    public:

        //! Invalid asset type constant.
        static const AssetType      Invalid;


                                    //! Constructs invalid Asset type.
                                    AssetType( void );

                                    //! Copies AssetType instance.
                                    AssetType( const AssetType& other );

        //! Compares two asset types.
        bool                        operator == ( const AssetType& other ) const;

        //! Compares two asset type.
        bool                        operator < ( const AssetType& other ) const;

        //! Returns true if this is a valid asset type.
        bool                        isValid( void ) const;

        //! Returns an asset type for specified class.
        template<typename TAsset>
        static AssetType            fromClass( void );

    private:

                                    //! Constructs AssetType instance.
                                    AssetType( TypeIdx type );

    private:

        TypeIdx                     m_type; //!< Actual asset type value.
    };

    // ** Assets::fromClass
    template<typename TAsset>
    AssetType AssetType::fromClass( void )
    {
        return GroupedTypeIndex<TAsset, AssetType>::idx();
    }

    //! Asset class instance stores info about a single asset.
    class Asset {
    public:

                                    //! Constructs empty Asset instance.
                                    Asset( void );

                                    //! Constructs Asset instance.
                                    Asset( AssetType type, const AssetId& uniqueId, const String& fileName );

        //! Returns asset unique id.
        const AssetId&              uniqueId( void ) const;

        //! Returns asset type.
        const AssetType&            type( void ) const;

        //! Returns asset name.
        const String&               name( void ) const;

        //! Sets asset name.
        void                        setName( const String& value );

    private:

        AssetType                   m_type;     //!< Asset type.
        AssetId                     m_uniqueId; //!< Unique asset id.
        String                      m_fileName; //!< Asset file name.
    };

    //! This handle are issued by an Assets class and are the only way the outer world can access an asset.
    class AssetHandle {
    friend class Assets;
    public:

                                    //! Constructs an empty AssetHandle instance.
                                    AssetHandle( void );

                                    //! Constructs AssetHandle instance from another one.
                                    AssetHandle( const AssetHandle& other );

        //! Copies an asset handle.
        const AssetHandle&          operator = ( const AssetHandle& other );

        //! This operator is used for read-only access to actual asset data.
        const Asset*                operator -> ( void ) const;

        //! Returns true if this asset handle is still valid.
        bool                        isValid( void ) const;

    private:

                                    //! Constructs the AssetHandle instance.
                                    AssetHandle( const Assets* assets, SlotIndex32 slot );

    private:

        const Assets*               m_assets;   //!< An assets manager that issued this handle.
        SlotIndex32                 m_slot;     //!< Asset slot.
    };

    //! Root interface to access all available assets.
    class Assets : public RefCounted {
    friend class AssetHandle;
    public:

        //! Adds new asset with unique id.
        AssetHandle                 addAsset( const AssetType& type, const AssetId& uniqueId, const String& fileName );

        //! Removes asset by a unique id.
        bool                        removeAsset( const AssetId& uniqueId );

        //! Returns an asset by it's id.
        AssetHandle                 findAsset( const AssetId& id ) const;

    private:

        //! Returns an asset by slot handle.
        const Asset&                assetAtSlot( SlotIndex32 slot ) const;

        //! Returns true if the specified asset slot handle is valid.
        bool                        isValidSlot( SlotIndex32 slot ) const;

    private:

        //! Container type to store unique id to an asset slot mapping.
        typedef Map<AssetId, SlotIndex32>   AssetSlotsById;

        Slots<Asset, SlotIndex32>   m_assets;   //!< All available assets.
        AssetSlotsById              m_slotById; //!< AssetId to handle mapping.
    };

#if ASSET_DEPRECATED
    //! Root interface to access all available assets.
    class Assets : public RefCounted {
    public:

                            ~Assets( void );

        //! Adds new asset of specified type.
        template<typename TAsset>
        AssetHandle<TAsset> add( const AssetId& id, AssetFormat<TAsset>* format );

        //! Removes an asset of specified type.
        template<typename TAsset>
        void                remove( const AssetId& id );

        //! Returns an asset of specified type.
        template<typename TAsset>
        AssetHandle<TAsset> get( const AssetId& id ) const;

        //! Returns an asset type id.
        template<typename TAsset>
        static AssetTypeId  assetTypeId( void );

    private:

        //! Returns the asset pool by it's type or creates a new one.
        template<typename TAsset>
        AssetPool<TAsset>*  requestAssetPool( void );

        //! Returns the asset pool by it's type.
        template<typename TAsset>
        AssetPool<TAsset>*  findAssetPool( void ) const;

    private:

        //! Container type to store all available asset pools.
        typedef Map<TypeIdx, AbstractAssetPool*> AssetPools;

        AssetPools          m_assetPools;   //!< All added asset pools reside here.
    };

    // ** Assets::add
    template<typename TAsset>
    AssetHandle<TAsset> Assets::add( const AssetId& id, AssetFormat<TAsset>* format )
    {
        AssetPool<TAsset>* pool = requestAssetPool<TAsset>();
        DC_BREAK_IF( pool == NULL );
        return pool->add( id, format );
    }

    // ** Assets::remove
    template<typename TAsset>
    void Assets::remove( const AssetId& id )
    {
        AssetPool<TAsset>* pool = requestAssetPool<TAsset>();
        DC_BREAK_IF( pool == NULL );
        pool->remove( id );
    }

    // ** Assets::get
    template<typename TAsset>
    AssetHandle<TAsset> Assets::get( const AssetId& id ) const
    {
        if( AssetPool<TAsset>* pool = findAssetPool<TAsset>() ) {
            return pool->get( id );
        }
       
        return AssetHandle<TAsset>();
    }

    // ** Assets::requestAssetPool
    template<typename TAsset>
    AssetPool<TAsset>* Assets::requestAssetPool( void )
    {
        // Find existing asset pool
        if( AssetPool<TAsset>* existingAssetPool = findAssetPool<TAsset>() ) {
            return existingAssetPool;
        }

        // Create a new one
        AssetPool<TAsset>* pool = DC_NEW AssetPool<TAsset>;
        m_assetPools[idx] = pool;

        return pool;
    }

    // ** Assets::findAssetPool
    template<typename TAsset>
    AssetPool<TAsset>* Assets::findAssetPool( void ) const
    {
        // Generate asset type
        AssetTypeId assetType = assetTypeId<TAsset>();

        // Lookup the pool
        AssetPools::const_iterator i = m_assetPools.find( idx );

        if( i == m_assetPools.end() ) {
            return NULL;
        }

        return static_cast<AssetPool<TAsset>*>( i->second );
    }

    // ** Assets::assetTypeId
    template<typename TAsset>
    AssetTypeId Assets::assetTypeId( void )
    {
        return GroupedTypeIndex<TAsset, Assets>::idx();
    }
#endif
    // --------------------------------------------------------------------------------------------------------------------------------- //

 /*   //! Asset type id.
    typedef TypeIdx AssetTypeId;

	//! Basic scene asset
	class Asset : public RefCounted {
	friend class AssetBundle;
	public:

		//! Asset loading state.
		enum State {
			Unknown,		//!< The asset state is unknown.
			Loading,		//!< The asset is now loading.
			Loaded,			//!< The asset is loaded.
			LoadingError	//!< An error was encountered while loading an asset.
		};

								//! Constructs Asset instance.
								Asset( AssetBundle* bundle = NULL, const String& uuid = String(), const String& name = String() );
                                ~Asset( void );

		//! Returns asset type id.
		AssetTypeId			    type( void ) const;

		//! Returns the asset format.
		AssetFormat				format( void ) const;

		//! Sets the asset format.
		void					setFormat( AssetFormat value );

		//! Returns asset name.
		const String&			name( void ) const;

		//! Returns asset uuid.
		const String&			uuid( void ) const;

		//! Returns asset data timestamp.
		u32						timestamp( void ) const;

		//! Returns asset state.
		State					state( void ) const;

		//! Sets asset state.
		void					setState( State value );

		//! Returns true if an asset needs to be loaded.
		bool					needsLoading( void ) const;

        //! Returns an attached asset data.
        template<typename T>
        const T&                get( void ) const;

        //! Updates an attached asset data.
        template<typename T>
        void                    set( const T& value );

        //! Returns true if an asset's data matches the specified type.
        template<typename T>
        bool                    is( void ) const;

		//! Returns asset data as key-value object.
		//virtual Io::KeyValue	keyValue( void ) const;

		//! Reads asset data from key-value object.
		//virtual bool			setKeyValue( const Io::KeyValue& value );

		//! Unloads asset data.
		//virtual void			dispose( void );

        //! Returns an asset type id.
        template<typename T>
        static AssetTypeId      assetTypeId( void );

        //! Creates new asset instance.
        template<typename T>
        static AssetPtr         create( const T& value );

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
		String					m_name;			//!< Asset name.
		String					m_uuid;			//!< Unique asset identifier.
		State					m_state;		//!< Asset state.
		u32						m_timestamp;	//!< Last known asset file timestamp.
        void*                   m_data;         //!< Stored asset data.
        AssetTypeId             m_typeId;       //!< Asset type id.
	};

    // ** Asset::get
    template<typename T>
    const T& Asset::get( void ) const
    {
        DC_BREAK_IF( !is<T>() );
        return *reinterpret_cast<T*>( m_data );
    }

    // ** Asset::set
    template<typename T>
    void Asset::set( const T& value )
    {
        if( m_typeId == -1 ) {
            m_typeId = assetTypeId<T>();
            m_data = DC_NEW T( value );
        } else {
            DC_BREAK_IF( !is<T>() );
            *reinterpret_cast<T*>( m_data ) = value;
        }

        m_timestamp = Platform::currentTime();
    }

    // ** Asset::is
    template<typename T>
    bool Asset::is( void ) const
    {
        return m_typeId == assetTypeId<T>();
    }

    // ** Asset::assetTypeId
    template<typename T>
    AssetTypeId Asset::assetTypeId( void )
    {
        return GroupedTypeIndex<T, Asset>::idx();
    }

    // ** Asset::create
    template<typename T>
    AssetPtr Asset::create( const T& value )
    {
        Asset* instance = DC_NEW Asset;
        instance->set<T>( value );
        return instance;
    }

	//! Contains asset meta information & caches loaded assets.
	class AssetBundle : public RefCounted {
	public:

		//! Returns asset bundle name.
		const String&			name( void ) const;

		//! Returns an asset of template type T with specified name.
		//template<typename TAsset>
		//StrongPtr<TAsset>		find( const String& name );

		//! Returns asset set of specified type.
		//template<typename TAsset>
		//Set<StrongPtr<TAsset>>	findByType( void ) const;

		//! Returns asset by name.
		//AssetPtr				findAsset( const String& name, u32 expectedType = Asset::All ) const;

		//! Returns the full asset path by an identifier.
		//Io::Path				assetPathByIdentifier( const String& name ) const;

		//! Creates the new Asset and loads it from key-value data.
		//AssetPtr				createAssetFromData( const Io::KeyValue& data ) const;

		//! Creates new Asset by type.
		//AssetPtr				createAssetByType( AssetTypeId type ) const;

		//! Adds an asset to this bundle.
		void					addAsset( AssetPtr asset );

		//! Removes an Asset from bundle by UUID.
		void					removeAsset( const String& uuid );

		//! Sets asset name.
		bool					setAssetName( AssetWPtr asset, const String& value );

		//! Queues an asset for loading.
		//void					queueForLoading( AssetWPtr asset );

		//! Removes an asset from loading queue.
		//void					removeFromLoading( AssetWPtr asset );

		//! Returns an assets queued for loading.
		//AssetSet				waitingForLoading( void ) const;

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
		//bool					loadFromString( const String& text );

	private:

		//! Container type to store available assets.
		typedef Hash<AssetPtr>	Assets;

		//! Asset factory type.
		//typedef AbstractFactory<Asset, AssetTypeId>	AssetFactory;

		//! Container type to map from string to asset type.
		//typedef Map<String, Asset::Type> AssetTypeByName;

		Io::Path				m_path;					//!< Asset bundle physical path.
		String					m_name;					//!< Asset bundle name.
		Assets					m_assets;				//!< Identifier to asset mapping.
		bool					m_uuidFileNames;		//!< Are the UUID file names used.
		//AssetFactory			m_factory;				//!< Asset factory.
		//AssetTypeByName			m_typeByName;			//!< Asset type by name.
		//AssetSet				m_waitingForLoading;	//!< Assets waiting for loading.
	};

	// ** AssetBundle::find
	//template<typename T>
	//StrongPtr<T> AssetBundle::find( const String& name )
	//{
	//	AssetPtr asset = findAsset( name );

	//	if( asset == AssetPtr() ) {
	//		return StrongPtr<T>();
	//	}

	//	T* casted = castTo<T>( asset.get() );

	//	return StrongPtr<T>( casted );
	//}

	// ** AssetBundle::find
	//template<typename TAsset>
	//Set<StrongPtr<TAsset>> AssetBundle::findByType( void ) const
	//{
	//	Set<StrongPtr<TAsset>> result;

	//	for( Assets::const_iterator i = m_assets.begin(), end = m_assets.end(); i != end; ++i ) {
	//		StrongPtr<TAsset> asset = castTo<TAsset>( i->second.get() );

	//		if( asset.valid() ) {
	//			result.insert( asset );
	//		}
	//	}

	//	return result;
	//}*/

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Assets_H__    */