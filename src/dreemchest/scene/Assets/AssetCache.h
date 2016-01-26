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

#ifndef __DC_Scene_Assets_Cache_H__
#define __DC_Scene_Assets_Cache_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Base class for all asset cache types
    class AbstractAssetCache {
    public:

        virtual                     ~AbstractAssetCache( void ) {}
    };

    //! Generic class to store all loaded assets of type TAsset.
    template<typename TAsset>
    class AssetCache : public AbstractAssetCache {
    public:

        //! Creates an asset inside the pool.
        SlotIndex32                 add( void );

        //! Removes an asset from a cache.
        bool                        remove( SlotIndex32 slot );

        //! Returns an asset by it's id.
        const TAsset&               get( SlotIndex32 slot );

    protected:

        Slots<TAsset, SlotIndex32>  m_cache;   //!< Cached asset data is stored here.
    };

    // ** AssetCache::add
    template<typename TAsset>
    SlotIndex32 AssetCache<TAsset>::add( void )
    {
        return m_cache.reserve();
    }

    // ** AssetCache::remove
    template<typename TAsset>
    bool AssetCache<TAsset>::remove( SlotIndex32 slot )
    {
        return m_cache.remove( slot );
    }

    // ** AssetCache::get
    template<typename TAsset>
    const TAsset& AssetCache<TAsset>::get( SlotIndex32 slot )
    {
        return m_cache.get( slot );
    }

#if ASSET_DEPRECATED

    //! Base class for all asset pools.
    class AbstractAssetPool {
    public:

        virtual                     ~AbstractAssetPool( void ) {}
    };

    //! Generic class to store all available assets of type TAsset.
    template<typename TAsset>
    class AssetPool : public AbstractAssetPool {
    friend typename AssetHandle<TAsset>;
    friend typename AssetWriteLock<TAsset>;
    public:

        //! Alias for an asset handle type.
        typedef AssetHandle<TAsset> Handle;

        //! Creates an asset inside the pool.
        Handle                      add( const AssetId& id, AssetFormat<TAsset>* format );

        //! Removes an asset from a pool.
        bool                        remove( const AssetId& id );

        //! Returns an asset by it's id.
        Handle                      get( const AssetId& id );

    protected:

        //! Returns an asset data by slot handle.
        const TAsset&               assetAtSlot( SlotIndex32 slot ) const;

        //! Returns true if the specified asset slot handle is valid.
        bool                        isValidSlot( SlotIndex32 slot ) const;

        //! Updates last use timestamp of an asset at specified slot.
        void                        updateLastUsed( SlotIndex32 slot ) const;

        //! Updates last modified timestamp of an asset at specified slot.
        void                        updateLastModified( SlotIndex32 slot ) const;

    protected:

        //! Subclass of an asset to add internal pool data.
        class AssetInternal : public TAsset {
        public:

                                    //! Constructs AssetInternal instance.
                                    AssetInternal( void )
                                        : m_lastModified( 0 ), m_lastUsed( 0 ) {}

            //! Sets asset identifier.
            void                    setUniqueId( const AssetId& value ) { m_uniqueId = value; }

            //! Sets asset format.
            void                    setFormat( AssetFormat<TAsset>* value ) { m_format = value; }

            //! Set last used timestamp of an asset.
            void                    setLastUsed( u32 value ) const { m_lastUsed = value; }

            //! Updates last modified timestamp of an asset.
            void                    setLastModified( u32 value ) const { m_lastModified = value; }

        private:

            mutable u32             m_lastModified;     //!< Last time this asset was modified.
            mutable u32             m_lastUsed;         //!< Last time this asset was used.
            AssetId                 m_uniqueId;         //!< Unique asset id.
            AssetFormat<TAsset>*    m_format;           //!< Asset format used for loading this asset.
        };

        //! Container type to store unique id to an asset slot mapping.
        typedef Map<AssetId, SlotIndex32>   AssetSlotsById;

        Slots<AssetInternal, SlotIndex32>   m_assets;   //!< Asset data for each asset file is stored here.
        AssetSlotsById                      m_slotById; //!< AssetId to handle mapping.
    };

    // ** AssetPool::add
    template<typename TAsset>
    typename AssetPool<TAsset>::Handle AssetPool<TAsset>::add( const AssetId& id, AssetFormat<TAsset>* format )
    {
        DC_BREAK_IF( format == NULL );
        DC_BREAK_IF( m_slotById.find( id ) != m_slotById.end() );

        // First reserve the slot for an asset data.
        SlotIndex32 slot = m_assets.reserve();

        // Setup internal asset data.
        AssetInternal& internal = m_assets.get( slot );
        internal.setUniqueId( id );
        internal.setFormat( format );

        // Now register unique id associated with this asset slot.
        m_slotById[id] = slot;

        // Construct an asset handle.
        return Handle( *this, slot );
    }

    // ** AssetPool::remove
    template<typename TAsset>
    bool AssetPool<TAsset>::remove( const AssetId& id )
    {
        // Find handle by id.
        AssetSlotsById::iterator i = m_slotById.find( id );
        DC_BREAK_IF( i == m_slotById.end() );

        // Store the slot before and remove the handle mapping.
        SlotIndex32 slot = i->second;
        m_slotById.erase( i );

        // Now release an asset data
        return m_assets.remove( slot );
    }

    // ** AssetPool::get
    template<typename TAsset>
    typename AssetPool<TAsset>::Handle AssetPool<TAsset>::get( const AssetId& id )
    {
        AssetSlotsById::const_iterator i = m_slotById.find( id );

        if( i == m_slotById.end() ) {
            return Handle();
        }

        return Handle( *this, i->second );
    }

    // ** AssetPool::assetAtSlot
    template<typename TAsset>
    const TAsset& AssetPool<TAsset>::assetAtSlot( SlotIndex32 slot ) const
    {
        return m_assets.get( slot );
    }

    // ** AssetPool::isValidSlot
    template<typename TAsset>
    bool AssetPool<TAsset>::isValidSlot( SlotIndex32 slot ) const
    {
        return m_assets.has( slot );
    }

    // ** AssetPool::updateLastUsed
    template<typename TAsset>
    void AssetPool<TAsset>::updateLastUsed( SlotIndex32 slot ) const
    {
        DC_BREAK_IF( !isValidSlot( slot ) );

        const AssetInternal& asset = m_assets.get( slot );
        asset.setLastUsed( Platform::currentTime() );
    }

    // ** AssetPool::updateLastModified
    template<typename TAsset>
    void AssetPool<TAsset>::updateLastModified( SlotIndex32 slot ) const
    {
        DC_BREAK_IF( !isValidSlot( slot ) );

        const AssetInternal& asset = m_assets.get( slot );
        asset.setLastModified( Platform::currentTime() );
    }
#endif

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Assets_Cache_H__    */