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

#ifndef __DC_Assets_H__
#define __DC_Assets_H__

#include "../Dreemchest.h"

#include <Platform/Time.h>
#include <Io/streams/Stream.h>
#include <Io/DiskFileSystem.h>

DC_BEGIN_DREEMCHEST

namespace Assets {

    NIMBLE_LOGGER_TAG( Assets )

    //! Opaque 32 bit handle.
    typedef OpaqueHandle<12, 20> Index;

    //! Forward declaration of an AssetDataHandle type.
    template<typename TAsset> class GenericHandle;

    //! Forward declaration of an asset WriteLock type.
    template<typename TAsset> class WriteLock;

    //! Asset identifier type.
    typedef String AssetId;

    //! Asset type identifier type.
    typedef u16 TypeId;

    //! Set of assets.
    typedef Set<class Handle> AssetSet;

    //! List of assets.
    typedef List<class Handle> AssetList;

    // Unique ptr for asset source.
    typedef AutoPtr<class AbstractSource> SourceUPtr;

    //! Asset class instance stores info about a single asset.
    class Asset {
    friend class Assets;
    public:

        //! Available asset states.
        enum State {
              Unloaded  //!< Asset is now unloaded.
            , Loading   //!< Asset is now loading.
            , Loaded    //!< Asset is loaded and ready to use.
            , Error     //!< Asset was failed to load.
        };

                                    //! Constructs empty Asset instance.
                                    Asset( void );

        //! Returns asset unique id.
        const AssetId&              uniqueId( void ) const;

        //! Returns asset type.
        const TypeId&               type( void ) const;

        //! Returns asset name.
        const String&               name( void ) const;

        //! Sets asset name.
        void                        setName( const String& value );

        //! Returns an asset source.
        AbstractSource*             source( void ) const;

        //! Returns asset state.
        State                       state( void ) const;

        //! Returns the last asset modification timestamp.
        u32                         lastModified( void ) const;

        //! Returns the last asset use timestamp.
        u32                         lastUsed( void ) const;

        //! Returns the last asset construction timestamp.
        u32                         lastConstructed( void ) const;

        //! Returns true if an asset is of specified type.
        template<typename TAsset>
        bool                        is( void ) const;

    private:

                                    //! Constructs Asset instance.
                                    Asset( const TypeId& type, void* cache, const Index& value, const AssetId& uniqueId, SourceUPtr source );

        //! Switches an asset to a specified state.
        void                        switchToState( State value );

        //! Sets the data handle.
        void                        setData( Index value );

        //! Returns the data handle.
        Index                       data( void ) const;

        //! Returns true if an asset has valid data.
        bool                        hasData( void ) const;

    private:

        SourceUPtr                  m_source;           //!< Asset source used for asset construction.
        TypeId                      m_type;             //!< Asset type.
        AssetId                     m_uniqueId;         //!< Unique asset id.
        String                      m_name;             //!< Asset name.
        State                       m_state;            //!< Current asset state.
        Index                       m_data;             //!< Asset data slot.
        void*                       m_cache;            //!< Asset data cache pointer.
        mutable u32                 m_lastConstructed;  //!< Last time this asset was constructed from source.
        mutable u32                 m_lastModified;     //!< Last time this asset was modified.
        mutable u32                 m_lastUsed;         //!< Last time this asset was used.
    };

    // ** Asset::is
    template<typename TAsset>
    NIMBLE_INLINE bool Asset::is( void ) const
    {
        return m_type == Assets::assetTypeId<TAsset>();
    }

    //! Root interface to access all available assets.
    class Assets : public RefCounted {
    friend class Handle;
    template<typename TAsset> friend class WriteLock;
    public:

                                    //! Constructs Assets instance.
                                    Assets( void );
                                    ~Assets( void );

        //! Adds a new asset of specified type.
        template<typename TAsset>
        GenericHandle<TAsset>       add( const AssetId& uniqueId, SourceUPtr source );

        //! Returns an asset of specified type.
        template<typename TAsset>
        GenericHandle<TAsset>       find( const AssetId& uniqueId ) const;

        //! Sets the default placeholder for unloaded assets of specified type.
        template<typename TAsset>
        void                        setPlaceholder( const TAsset& value );

        //! Adds new asset with unique id.
        Handle                      addAsset( const TypeId& type, const AssetId& uniqueId, SourceUPtr source );

        //! Removes asset by a unique id.
        bool                        removeAsset( const AssetId& uniqueId );

        //! Returns an asset by it's id.
        Handle                      findAsset( const AssetId& id ) const;

        //! Updates an asset manager (performs loading, unloading, etc).
        void                        update( f32 dt );

        //! Returns an asset type by it's name.
        TypeId                      typeFromName( const String& name ) const;

        //! Returns asset type name.
        String                      assetTypeName( const TypeId& type ) const;

        //! Returns an asset type id.
        template<typename TAsset>
        static TypeId               assetTypeId( void );

        //! Registers an asset type.
        template<typename TAsset>
        void                        registerType( void );

    private:

        //! Forward declaration of an abstract asset cache type.
        struct AbstractAssetCache;

        //! Forward declaration of generic asset cache type.
        template<typename TAsset>   class AssetCache;

        //! Returns an asset by index.
        const Asset&                assetAtIndex( Index index ) const;

        //! Returns true if the specified asset index is valid.
        bool                        isIndexValid( Index index ) const;

        //! Puts an asset to a loading queue.
        void                        queueForLoading( const Asset& asset ) const;

        //! Constructs an asset handle for a specified asset.
        Handle                      createHandle( const Asset& asset ) const;

        //! Requests an asset cache for a specified asset type.
        AbstractAssetCache*         findAssetCache( const TypeId& type ) const;

        //! Returns cached asset data.
        template<typename TAsset>
        const TAsset&               assetData( const Asset& asset ) const;

        //! Reserves asset data handle inside the cache.
        Index                       reserveAssetData( const TypeId& type );

        //! Locks an asset for reading and updates last used timestamp.
        template<typename TAsset>
        const TAsset&               acquireReadLock( const Asset& asset ) const;

        //! Locks an asset for writing.
        template<typename TAsset>
        WriteLock<TAsset>           acquireWriteLock( const Handle& asset );

        //! Unlocks an asset after writing and updates last modified timestamp.
        void                        releaseWriteLock( const Handle& asset );

        //! Loads an asset data to a cache.
        bool                        loadAssetToCache( Handle asset );

    private:

        //! Abstract asset cache.
        struct AbstractAssetCache {
            virtual                 ~AbstractAssetCache( void ) {}

            //! Reserves the slot handle inside cache.
            virtual Index           reserve( void ) = 0;
        };

        //! Generic asset cache that stores asset data of specified type.
        template<typename TAsset>
        struct AssetCache : public AbstractAssetCache {
            //! Reserves the slot handle inside cache.
            virtual Index           reserve( void ) { return slots.reserve(); }

            TAsset                  placeholder;    //!< Default placeholder that is returned for unloaded assets.
            Slots<TAsset, Index>    slots;          //!< Cached asset data is stored here.
        };

        //! Container type to store unique id to an asset slot mapping.
        typedef Map<AssetId, Index> AssetIndexById;

        //! Container type to store asset cache for an asset type.
        typedef Map<TypeId, AbstractAssetCache*> AssetCaches;

        Slots<Asset, Index>         m_assets;       //!< All available assets.
        AssetIndexById              m_indexById;    //!< AssetId to asset index mapping.
        u32                         m_currentTime;  //!< Cached current time.
        Map<String, TypeId>         m_nameToType;   //!< Maps asset name to type.
        Map<TypeId, String>         m_typeToName;   //!< Maps asset type to name.
        mutable AssetCaches         m_cache;        //!< Asset cache by an asset type.
        mutable AssetList           m_loadingQueue; //!< All assets waiting for loading are put in this queue.
    };

    // ** Assets::assetTypeId
    template<typename TAsset>
    TypeId Assets::assetTypeId( void )
    {
        return GroupedTypeIndex<TAsset, Assets>::idx();
    }

    // ** Assets::registerType
    template<typename TAsset>
    void Assets::registerType( void )
    {
        // Get the type & name
        TypeId type = assetTypeId<TAsset>();
        String name = TypeInfo<TAsset>::name();

        // Register type & name
        m_nameToType[name] = type;
        m_typeToName[type] = name;

        // Create an asset cache for this type of asset
        DC_BREAK_IF( m_cache.count( type ) );
        m_cache[type] = DC_NEW AssetCache<TAsset>();
    }

    //! Adds a new asset of specified type.
    template<typename TAsset>
    GenericHandle<TAsset> Assets::add( const AssetId& uniqueId, SourceUPtr source )
    {
        Handle handle = addAsset( assetTypeId<TAsset>(), uniqueId, source );
        return handle;
    }

    // ** Assets::find
    template<typename TAsset>
    GenericHandle<TAsset> Assets::find( const AssetId& uniqueId ) const
    {
        Handle handle = findAsset( uniqueId );
        return handle;
    }

    // ** Assets::setPlaceholder
    template<typename TAsset>
    void Assets::setPlaceholder( const TAsset& value )
    {
        AssetCache<TAsset>* cache = static_cast<AssetCache<TAsset>*>( findAssetCache( assetTypeId<TAsset>() ) );
        cache->placeholder = value;
    }

    // ** Assets::assetData
    template<typename TAsset>
    const TAsset& Assets::assetData( const Asset& asset ) const
    {
        DC_BREAK_IF( asset.m_cache == NULL );

        // Request an asset cache for this type of asset
        AssetCache<TAsset>* cache = static_cast<AssetCache<TAsset>*>( reinterpret_cast<AbstractAssetCache*>( asset.m_cache ) );

        // Use the placeholder for unloaded assets
        if( asset.state() == Asset::Unloaded ) {
            return cache->placeholder;
        }

        // Now lookup an asset data
        TAsset& data = cache->slots.get( asset.data() );
        return data;
    }

    // ** Assets::acquireReadLock
    template<typename TAsset>
    const TAsset& Assets::acquireReadLock( const Asset& asset ) const
    {
        if( asset.state() != Asset::Loaded ) {
            queueForLoading( asset );
        }

        const TAsset& data = assetData<TAsset>( asset );
        asset.m_lastUsed = m_currentTime;
        return data;
    }

    // ** Assets::acquireWriteLock
    template<typename TAsset>
    NIMBLE_INLINE WriteLock<TAsset> Assets::acquireWriteLock( const Handle& asset )
    {
        DC_BREAK_IF( asset->state() == Asset::Unloaded, "could not acquire write lock for an unloaded asset" );
        return WriteLock<TAsset>( asset );
    }

    // ** Assets::assetAtIndex
    NIMBLE_INLINE const Asset& Assets::assetAtIndex( Index index ) const
    {
        return m_assets.get( index );
    }

    // ** Assets::isIndexValid
    NIMBLE_INLINE bool Assets::isIndexValid( Index index ) const
    {
        return m_assets.has( index );
    }

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_H__    */