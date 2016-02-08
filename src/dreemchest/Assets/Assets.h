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

#include "AssetType.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    DECLARE_LOG( log )

    //! Opaque 32 bit handle.
    typedef OpaqueHandle<12, 20> Index;

    //! Forward declaration of an AssetDataHandle type.
    template<typename TAsset> class GenericHandle;

    //! Forward declaration of an asset WriteLock type.
    template<typename TAsset> class WriteLock;

    //! Asset identifier type.
    typedef String AssetId;

    //! Set of assets.
    typedef Set<class Handle> AssetSet;

    //! List of assets.
    typedef List<class Handle> AssetList;

    // Unique ptr for asset format.
    typedef AutoPtr<class Format> FormatUPtr;

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

                                    //! Constructs Asset instance.
                                    Asset( Type type, const AssetId& uniqueId, FormatUPtr format );

        //! Returns asset unique id.
        const AssetId&              uniqueId( void ) const;

        //! Returns asset type.
        const Type&                 type( void ) const;

        //! Returns asset name.
        const String&               name( void ) const;

        //! Sets asset name.
        void                        setName( const String& value );

        //! Returns an asset format.
        Format*                     format( void ) const;

        //! Returns asset state.
        State                       state( void ) const;

        //! Returns the last asset modification timestamp.
        u32                         lastModified( void ) const;

        //! Returns the last asset use timestamp.
        u32                         lastUsed( void ) const;

    private:

        //! Switches an asset to a specified state.
        void                        switchToState( State value );

        //! Sets the cache handle.
        void                        setCache( Index value );

    private:

        FormatUPtr                  m_format;       //!< Asset format parser used for loading.
        Type                        m_type;         //!< Asset type.
        AssetId                     m_uniqueId;     //!< Unique asset id.
        String                      m_name;         //!< Asset name.
        State                       m_state;        //!< Current asset state.
        Index                       m_cache;        //!< Asset data cache slot.
        mutable u32                 m_lastModified; //!< Last time this asset was modified.
        mutable u32                 m_lastUsed;     //!< Last time this asset was used.
    };

    //! Root interface to access all available assets.
    class Assets : public RefCounted {
    friend class Handle;
    template<typename TAsset> friend class WriteLock;
    public:

                                    //! Constructs Assets instance.
                                    Assets( void );
                                    ~Assets( void );

        //! Returns an asset of specified type.
        template<typename TAsset>
        GenericHandle<TAsset>       find( const AssetId& uniqueId ) const;

        //! Sets the default placeholder for unloaded assets of specified type.
        template<typename TAsset>
        void                        setPlaceholder( const TAsset& value );

        //! Adds new asset with unique id.
        Handle                      addAsset( const Type& type, const AssetId& uniqueId, FormatUPtr format );

        //! Removes asset by a unique id.
        bool                        removeAsset( const AssetId& uniqueId );

        //! Returns an asset by it's id.
        Handle                      findAsset( const AssetId& id ) const;

        //! Updates an asset manager (performs loading, unloading, etc).
        void                        update( f32 dt );

    private:

        //! Forward declaration of generic asset cache type.
        template<typename TAsset>   class AssetCache;

        //! Returns an asset by index.
        const Asset&                assetAtIndex( Index index ) const;

        //! Returns true if the specified asset index is valid.
        bool                        isIndexValid( Index index ) const;

        //! Puts an asset to a loading queue.
        void                        queueForLoading( const Handle& asset ) const;

        //! Requests an asset cache for a specified asset type.
        template<typename TAsset>
        AssetCache<TAsset>&         requestAssetCache( void ) const;

        //! Returns cached asset data.
        template<typename TAsset>
        const TAsset&               assetData( const Handle& asset ) const;

        //! Reserves asset data handle inside the cache.
        Index                       reserveAssetData( const Type& type );

        //! Locks an asset for reading and updates last used timestamp.
        template<typename TAsset>
        const TAsset&               acquireReadLock( const Handle& asset ) const;

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
        typedef Map<AssetId, Index>   AssetIndexById;

        //! Container type to store asset cache for an asset type.
        typedef Map<Type, AbstractAssetCache*> AssetCaches;

        Slots<Asset, Index>         m_assets;       //!< All available assets.
        AssetIndexById              m_indexById;    //!< AssetId to asset index mapping.
        mutable AssetCaches         m_cache;        //!< Asset cache by an asset type.
        mutable AssetList           m_loadingQueue; //!< All assets waiting for loading are put in this queue.
    };

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
        AssetCache<TAsset>& cache = requestAssetCache<TAsset>();
        cache.placeholder = value;
    }

    // ** Assets::requestAssetCache
    template<typename TAsset>
    Assets::AssetCache<TAsset>& Assets::requestAssetCache( void ) const
    {
        Type type = Type::fromClass<TAsset>();
        AssetCaches::iterator i = m_cache.find( type );

        if( i != m_cache.end() ) {
            return *static_cast<AssetCache<TAsset>*>( i->second );
        }

        AssetCache<TAsset>* cache = DC_NEW AssetCache<TAsset>();
        m_cache[type] = cache;

        return *cache;
    }

    // ** Assets::assetData
    template<typename TAsset>
    const TAsset& Assets::assetData( const Handle& asset ) const
    {
        // Request an asset cache for this type of asset
        AssetCache<TAsset>& cache = requestAssetCache<TAsset>();

        // Use the placeholder for unloaded assets
        if( !asset->m_cache.isValid() ) {
            return cache.placeholder;
        }

        // Now lookup an asset data
        TAsset& data = cache.slots.get( asset->m_cache );
        return data;
    }

    // ** Assets::acquireReadLock
    template<typename TAsset>
    const TAsset& Assets::acquireReadLock( const Handle& asset ) const
    {
        DC_BREAK_IF( !asset.isValid() );

        if( !asset.isLoaded() ) {
            queueForLoading( asset );
        }

        const TAsset& data = assetData<TAsset>( asset );
        asset->m_lastUsed = Platform::currentTime();
        return data;
    }

    // ** Assets::acquireWriteLock
    template<typename TAsset>
    WriteLock<TAsset> Assets::acquireWriteLock( const Handle& asset )
    {
        return WriteLock<TAsset>( asset );
    }

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_H__    */