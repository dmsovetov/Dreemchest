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

#include "Asset.h"
#include "AssetCache.h"
#include "AssetLoadingQueue.h"
#include "AssetHandle.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    //! Root interface to access all available assets.
    class Assets : public InjectEventEmitter<RefCounted> {
    friend class Handle;
    friend class LoadingQueue;
    template<typename TAsset> friend class WriteLock;
    public:

                                    //! Constructs Assets instance.
                                    Assets( void );
                                    ~Assets( void );

        //! Adds a new asset of specified type.
        template<typename TAsset>
        DataHandle<TAsset>          add( const AssetId& uniqueId, SourceUPtr source );

        //! Returns an asset of specified type.
        template<typename TAsset>
        DataHandle<TAsset>          find( const AssetId& uniqueId ) const;

        //! Returns asset cache of specified type.
        template<typename TAsset>
        const AssetCache<TAsset>&   assetCache( void ) const;

        //! Returns asset cache of specified type.
        template<typename TAsset>
        AssetCache<TAsset>&         assetCache( void );

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

        //! Returns the total number of bytes allocated for asset data.
        s32                         totalBytesUsed( void ) const;

        //! Forces an asset to be loaded and returns true if loading succeed.
        bool                        forceLoad( const Handle& asset );

        //! Forces an asset to be unloaded.
        void                        forceUnload( Handle asset );

        //! Registers an asset type.
        template<typename TAsset>
        AssetCache<TAsset>&         registerType( void );

        //! This event is emitted when an asset was loaded.
        struct Loaded {
                                    //! Constructs a Loaded event instance.
                                    Loaded( const Assets& sender, const Asset& asset )
                                        : sender( sender )
                                        , asset( asset ) {}
            const Assets&           sender; //!< An asset manager that sent this event.
            const Asset&            asset;  //!< An asset instance that was loaded.
        };

        //! This event is emitted when an asset was unloaded.
        struct Unloaded {
                                    //! Constructs a Unloaded event instance.
                                    Unloaded( const Assets& sender, const Asset& asset )
                                        : sender( sender )
                                        , asset( asset ) {}
            const Assets&           sender; //!< An asset manager that sent this event.
            const Asset&            asset;  //!< An asset instance that was unloaded.
        };

    private:

        //! Returns an asset by index.
        const Asset&                assetAtIndex( Index index ) const;
        Asset&                      assetAtIndex( Index index );

        //! Returns true if the specified asset index is valid.
        bool                        isIndexValid( Index index ) const;

        //! Constructs an asset handle for a specified asset.
        Handle                      createHandle( const Asset& asset ) const;

        //! Requests an asset cache for a specified asset type.
        AbstractAssetCache&         findAssetCache( const TypeId& type ) const;

        //! Returns cached read-only asset data.
        template<typename TAsset>
        const TAsset&               readOnlyAssetData( const Asset& asset ) const;

        //! Returns writable asset data (used internally by write locks).
        template<typename TAsset>
        TAsset&                     writableAssetData( const Asset& asset ) const;

        //! Locks an asset for reading and updates last used timestamp.
        template<typename TAsset>
        const TAsset&               acquireReadLock( const Asset& asset ) const;

        //! Locks an asset for writing.
        template<typename TAsset>
        WriteLock<TAsset>           acquireWriteLock( const Handle& asset );

        //! Unlocks an asset after writing and updates last modified timestamp.
        void                        releaseWriteLock( const Handle& asset );

        //! Queues a loaded asset for notification.
        void                        queueLoaded( const Handle& asset );

        //! Queues an unloaded asset for notification.
        void                        queueUnloaded( const Handle& asset );

    private:

        //! Container type to store unique id to an asset slot mapping.
        typedef Map<AssetId, Index> AssetIndexById;

        //! Container type to store asset cache for an asset type.
        typedef Map<TypeId, AbstractAssetCache*> AssetCaches;

        Pool<Asset, Index>          m_assets;           //!< All available assets.
        AssetIndexById              m_indexById;        //!< AssetId to asset index mapping.
        u32                         m_currentTime;      //!< Cached current time.
        Map<String, TypeId>         m_nameToType;       //!< Maps asset name to type.
        Map<TypeId, String>         m_typeToName;       //!< Maps asset type to name.
        AssetList                   m_loadedAssets;     //!< A list of assets that were loaded and are waiting for notification.
        AssetList                   m_unloadedAssets;   //!< A list of assets that were loaded and are waiting for notification.
        mutable LoadingQueueUPtr    m_loadingQueue;     //!< Asset loading queue.
        mutable AssetCaches         m_cache;            //!< Asset cache by an asset type.
    };

    // ** Assets::assetCache
    template<typename TAsset>
    AssetCache<TAsset>& Assets::assetCache( void )
    {
        return static_cast<AssetCache<TAsset>&>( findAssetCache( Asset::typeId<TAsset>() ) );
    }

    // ** Assets::assetCache
    template<typename TAsset>
    const AssetCache<TAsset>& Assets::assetCache( void ) const
    {
        return const_cast<Assets*>( this )->assetCache<TAsset>();
    }

    // ** Assets::registerType
    template<typename TAsset>
    AssetCache<TAsset>& Assets::registerType( void )
    {
        // Get the type & name
        TypeId type = Asset::typeId<TAsset>();
        String name = TypeInfo<TAsset>::name();

        // Register type & name
        m_nameToType[name] = type;
        m_typeToName[type] = name;

        // Create an asset cache for this type of asset
        NIMBLE_BREAK_IF( m_cache.count( type ) );
        AssetCache<TAsset>* cache = DC_NEW AssetCache<TAsset>();
        m_cache[type] = cache;

        return *cache;
    }

    //! Adds a new asset of specified type.
    template<typename TAsset>
    DataHandle<TAsset> Assets::add( const AssetId& uniqueId, SourceUPtr source )
    {
        Handle handle = addAsset( Asset::typeId<TAsset>(), uniqueId, source );
        return handle;
    }

    // ** Assets::find
    template<typename TAsset>
    DataHandle<TAsset> Assets::find( const AssetId& uniqueId ) const
    {
        Handle handle = findAsset( uniqueId );
        return handle;
    }

    // ** Assets::readOnlyAssetData
    template<typename TAsset>
    const TAsset& Assets::readOnlyAssetData( const Asset& asset ) const
    {
        // Request an asset cache for this type of asset
        AssetCache<TAsset>& cache = static_cast<AssetCache<TAsset>&>( asset.cache() );

        // Use the placeholder for unloaded assets
        if( !asset.isLoaded() ) {
            return cache.placeholder();
        }

        // This asset is already loaded, so we can just return a const reference to a writable data
        return writableAssetData<TAsset>( asset );
    }

    // ** Assets::writableAssetData
    template<typename TAsset>
    TAsset& Assets::writableAssetData( const Asset& asset ) const
    {
        NIMBLE_BREAK_IF( !asset.dataIndex().isValid(), "asset data is invalid" );

        // Get asset cache
        AssetCache<TAsset>& cache = static_cast<AssetCache<TAsset>&>( asset.cache() );

        // Now lookup an asset data
        TAsset& data = cache.get( asset.dataIndex() );
        return data;
    }

    // ** Assets::acquireReadLock
    template<typename TAsset>
    const TAsset& Assets::acquireReadLock( const Asset& asset ) const
    {
        if( asset.state() == Asset::Unloaded ) {
            m_loadingQueue->queue( createHandle( asset ) );
        }

        const TAsset& data = readOnlyAssetData<TAsset>( asset );
        asset.m_timestamp.used = m_currentTime;
        return data;
    }

    // ** Assets::acquireWriteLock
    template<typename TAsset>
    NIMBLE_INLINE WriteLock<TAsset> Assets::acquireWriteLock( const Handle& asset )
    {
        return WriteLock<TAsset>( asset );
    }

    // ** Assets::assetAtIndex
    NIMBLE_INLINE const Asset& Assets::assetAtIndex( Index index ) const
    {
        return m_assets.get( index );
    }

    // ** Assets::assetAtIndex
    NIMBLE_INLINE Asset& Assets::assetAtIndex( Index index )
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

#include "AssetHandle.hpp"

#endif    /*    !__DC_Assets_H__    */
