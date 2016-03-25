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

DC_BEGIN_DREEMCHEST

namespace Assets {

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
        DataHandle<TAsset>          add( const AssetId& uniqueId, SourceUPtr source );

        //! Returns an asset of specified type.
        template<typename TAsset>
        DataHandle<TAsset>          find( const AssetId& uniqueId ) const;

        //! Sets the default placeholder for unloaded assets of specified type.
        template<typename TAsset>
        void                        setPlaceholder( const DataHandle<TAsset>& value );

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

        //! Returns the total number of bytes allocated for a specified asset type.
        template<typename TAsset>
        s32                         allocatedForType( void ) const;

        //! Sets asset size evaluation function.
        template<typename TAsset>
        void                        setSizeEvaluator( const cClosure<s32(const TAsset&)>& value );

        //! Returns an asset type id.
        template<typename TAsset>
        static TypeId               assetTypeId( void );

        //! Registers an asset type.
        template<typename TAsset>
        void                        registerType( const cClosure<s32(const TAsset&)>& sizeEvaluator = cClosure<s32(const TAsset&)>() );

    private:

        //! Forward declaration of an abstract asset cache type.
        struct AbstractAssetCache;

        //! Forward declaration of generic asset cache type.
        template<typename TAsset>   class AssetCache;

        //! Returns an asset by index.
        const Asset&                assetAtIndex( Index index ) const;
        Asset&                      assetAtIndex( Index index );

        //! Returns true if the specified asset index is valid.
        bool                        isIndexValid( Index index ) const;

        //! Puts an asset to a loading queue.
        void                        queueForLoading( Asset& asset );

        //! Constructs an asset handle for a specified asset.
        Handle                      createHandle( const Asset& asset ) const;

        //! Requests an asset cache for a specified asset type.
        AbstractAssetCache*         findAssetCache( const TypeId& type ) const;

        //! Returns the total number of bytes allocated by a specified asset type cache.
        s32                         assetCacheSize( const TypeId& type ) const;

        //! Returns cached read-only asset data.
        template<typename TAsset>
        const TAsset&               readOnlyAssetData( const Asset& asset ) const;

        //! Returns writable asset data (used internally by write locks).
        template<typename TAsset>
        TAsset&                     writableAssetData( const Asset& asset ) const;

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

            //! Returns the total cache size.
            virtual s32             size( void ) const = 0;

            //! Returns the total number of bytes used by an asset cache.
            virtual s32             allocatedBytes( void ) const = 0;
        };

        //! Generic asset cache that stores asset data of specified type.
        template<typename TAsset>
        struct AssetCache : public AbstractAssetCache {
            //! Asset size evaluation function.
            typedef cClosure<s32(const TAsset& asset)> SizeEvaluator;

            //! Reserves the slot handle inside cache.
            virtual Index           reserve( void ) { return pool.reserve(); }

            //! Returns the total cache size.
            virtual s32             size( void ) const { return pool.size(); }

            //! Returns the total number of bytes used by an asset cache.
            virtual s32             allocatedBytes( void ) const
            {
                if( !sizeEvaluator ) {
                    LogWarning( "cache", "an amount of allocated bytes requested, but no size evaluator set\n" );
                    return 0;
                }

                s32 result = 0;
                for( s32 i = 0, n = pool.size(); i < n; i++ ) {
                    result += sizeEvaluator( pool.dataAt( i ) );
                }

                return result;
            }

            TAsset                  builtInPlaceholder; //!< Built-in placeholder asset.
            DataHandle<TAsset>      placeholder;        //!< Default placeholder that is returned for unloaded assets.
            Pool<TAsset, Index>     pool;               //!< Cached asset data is stored here.
            SizeEvaluator           sizeEvaluator;      //!< Function to evaluate single asset size.
        };

        //! Container type to store unique id to an asset slot mapping.
        typedef Map<AssetId, Index> AssetIndexById;

        //! Container type to store asset cache for an asset type.
        typedef Map<TypeId, AbstractAssetCache*> AssetCaches;

        Pool<Asset, Index>          m_assets;       //!< All available assets.
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

    // ** Assets::allocatedForType
    template<typename TAsset>
    s32 Assets::allocatedForType( void ) const
    {
        return assetCacheSize( assetTypeId<TAsset>() );
    }

    // ** Assets::setSizeEvaluator
    template<typename TAsset>
    void Assets::setSizeEvaluator( const cClosure<s32(const TAsset&)>& value )
    {
        AbstractAssetCache* cache = findAssetCache( assetTypeId<TAsset>() );
        DC_ABORT_IF( cache == NULL, "unknown asset type" );

        static_cast<AssetCache<TAsset>*>( cache )->sizeEvaluator = value;
    }

    // ** Assets::registerType
    template<typename TAsset>
    void Assets::registerType( const cClosure<s32(const TAsset&)>& sizeEvaluator )
    {
        // Get the type & name
        TypeId type = assetTypeId<TAsset>();
        String name = TypeInfo<TAsset>::name();

        // Register type & name
        m_nameToType[name] = type;
        m_typeToName[type] = name;

        // Create an asset cache for this type of asset
        DC_BREAK_IF( m_cache.count( type ) );
        AssetCache<TAsset>* cache = DC_NEW AssetCache<TAsset>();
        cache->sizeEvaluator = sizeEvaluator;
        m_cache[type] = cache;
    }

    //! Adds a new asset of specified type.
    template<typename TAsset>
    DataHandle<TAsset> Assets::add( const AssetId& uniqueId, SourceUPtr source )
    {
        Handle handle = addAsset( assetTypeId<TAsset>(), uniqueId, source );
        return handle;
    }

    // ** Assets::find
    template<typename TAsset>
    DataHandle<TAsset> Assets::find( const AssetId& uniqueId ) const
    {
        Handle handle = findAsset( uniqueId );
        return handle;
    }

    // ** Assets::setPlaceholder
    template<typename TAsset>
    void Assets::setPlaceholder( const DataHandle<TAsset>& value )
    {
        // Request an asset cache for this type of asset
        AssetCache<TAsset>* cache = static_cast<AssetCache<TAsset>*>( findAssetCache( assetTypeId<TAsset>() ) );

        DC_BREAK_IF( !value.isLoaded(), "unloaded asset could not be set as a placeholder" );
        cache->placeholder = value;
    }

    // ** Assets::readOnlyAssetData
    template<typename TAsset>
    const TAsset& Assets::readOnlyAssetData( const Asset& asset ) const
    {
        DC_BREAK_IF( asset.m_cache == NULL );

        // Request an asset cache for this type of asset
        AssetCache<TAsset>* cache = static_cast<AssetCache<TAsset>*>( reinterpret_cast<AbstractAssetCache*>( asset.m_cache ) );

        // Use the placeholder for unloaded assets
        if( !asset.isLoaded() ) {
            if( !cache->placeholder.isLoaded() ) {
                return cache->builtInPlaceholder;
            }

            return readOnlyAssetData<TAsset>( cache->placeholder.asset() );
        }

        // This asset is already loaded, so we can just return a const reference to a writable data
        return writableAssetData<TAsset>( asset );
    }

    // ** Assets::writableAssetData
    template<typename TAsset>
    TAsset& Assets::writableAssetData( const Asset& asset ) const
    {
        DC_BREAK_IF( !asset.data().isValid(), "asset data is invalid" );
        DC_BREAK_IF( asset.m_cache == NULL, "no data cache associated with this asset" );

        // Get asset cache
        AssetCache<TAsset>* cache = static_cast<AssetCache<TAsset>*>( reinterpret_cast<AbstractAssetCache*>( asset.m_cache ) );

        // Now lookup an asset data
        TAsset& data = cache->pool.get( asset.data() );
        return data;
    }

    // ** Assets::acquireReadLock
    template<typename TAsset>
    const TAsset& Assets::acquireReadLock( const Asset& asset ) const
    {
        if( asset.state() == Asset::Unloaded ) {
            const_cast<Assets*>( this )->queueForLoading( const_cast<Asset&>( asset ) );
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

#ifndef DC_BUILD_LIBRARY
    #include "Asset.h"
#endif  /*  #ifndef DC_BUILD_LIBRARY    */

#endif    /*    !__DC_Assets_H__    */