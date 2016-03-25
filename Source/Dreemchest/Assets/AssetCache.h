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

#ifndef __DC_Assets_AssetCache_H__
#define __DC_Assets_AssetCache_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    //! Abstract asset cache.
    class AbstractAssetCache {
    public:

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
    class AssetCache : public AbstractAssetCache {
    public:

        //! Asset size evaluation function.
        typedef cClosure<s32(const TAsset& asset)> AllocatedAssetMemory;

        //! Reserves the slot handle inside cache.
        virtual Index           reserve( void );

        //! Returns the total cache size.
        virtual s32             size( void ) const;

        //! Returns the total number of bytes used by an asset cache.
        virtual s32             allocatedBytes( void ) const;

        //! Sets allocated asset memory callback function.
        void                    setAllocatedAssetMemoryCallback( const AllocatedAssetMemory& value );

        //! Sets the default placeholder for unloaded assets of specified type.
        void                    setPlaceholder( const DataHandle<TAsset>& value );

        //! Returns a placeholder asset data.
        const TAsset&           placeholder( void ) const;

        //! Returns cached data at specified index.
        const TAsset&           get( const Index& index ) const;
        TAsset&                 get( const Index& index );

    private:

        TAsset                  m_builtInPlaceholder;   //!< Built-in placeholder asset.
        DataHandle<TAsset>      m_placeholder;          //!< Default placeholder that is returned for unloaded assets.
        Pool<TAsset, Index>     m_pool;                 //!< Cached asset data is stored here.
        AllocatedAssetMemory    m_allocatedAssetMemory; //!< Function to evaluate memory allocated by an asset instance.
    };

    // ** AssetCache::reserve
    template<typename TAsset>
    Index AssetCache<TAsset>::reserve( void )
    {
        return m_pool.reserve();
    }

    // ** AssetCache::size
    template<typename TAsset>
    s32 AssetCache<TAsset>::size( void ) const
    {
        return m_pool.size();
    }

    // ** AssetCache::allocatedBytes
    template<typename TAsset>
    s32 AssetCache<TAsset>::allocatedBytes( void ) const
    {
        if( !m_allocatedAssetMemory ) {
            LogWarning( "cache", "an amount of allocated bytes requested, but no size evaluator set\n" );
            return 0;
        }

        s32 result = 0;
        for( s32 i = 0, n = m_pool.size(); i < n; i++ ) {
            result += m_allocatedAssetMemory( m_pool.dataAt( i ) );
        }

        return result;
    }

    // ** AssetCache::setAllocatedAssetMemoryCallback
    template<typename TAsset>
    void AssetCache<TAsset>::setAllocatedAssetMemoryCallback( const AllocatedAssetMemory& value )
    {
        m_allocatedAssetMemory = value;
    }

    // ** Assets::setPlaceholder
    template<typename TAsset>
    void AssetCache<TAsset>::setPlaceholder( const DataHandle<TAsset>& value )
    {
        DC_BREAK_IF( !value.isLoaded(), "unloaded asset could not be set as a placeholder" );
        m_placeholder = value;
    }

    // ** AssetCache::placeholder
    template<typename TAsset>
    const TAsset& AssetCache<TAsset>::placeholder( void ) const
    {
        if( m_placeholder.isLoaded() ) {
            return get( m_placeholder.asset().dataIndex() );
        }

        return m_builtInPlaceholder;
    }

    // ** Assets::get
    template<typename TAsset>
    NIMBLE_INLINE const TAsset& AssetCache<TAsset>::get( const Index& index ) const
    {
        return m_pool.get( index );
    }

    // ** Assets::get
    template<typename TAsset>
    NIMBLE_INLINE TAsset& AssetCache<TAsset>::get( const Index& index )
    {
        return m_pool.get( index );
    }

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_AssetCache_H__    */