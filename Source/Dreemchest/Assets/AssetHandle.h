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

#ifndef __DC_Assets_AssetHandle_H__
#define __DC_Assets_AssetHandle_H__

#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    //! This handles are issued by an Assets class and are the only way the outer world can access an asset.
    class Handle {
    friend class Assets;
    template<class TAsset> friend class WriteLock;
    public:

                                        //! Constructs an empty Handle instance.
                                        Handle( void );

                                        //! Constructs Handle instance from another one.
                                        Handle( const Handle& other );

        //! Copies an asset handle.
        const Handle&                   operator = ( const Handle& other );

        //! Compares two asset handles for equality.
        bool                            operator == ( const Handle& other ) const;

        //! Compares two asset handles.
        bool                            operator < ( const Handle& other ) const;

        //! This operator is used for read-only access to actual asset data.
        const Asset*                    operator -> ( void ) const;
        Asset*                          operator -> ( void );

        //! Returns true if this asset handle is still valid.
        bool                            isValid( void ) const;

        //! Returns true if this asset data is loaded to cache.
        bool                            isLoaded( void ) const;

        //! Forces an asset to be loaded and returns true if loading succeed.
        bool                            forceLoad( void );

        //! Returns an asset that is referenced by this handle.
        const Asset&                    asset( void ) const;
        Asset&                          asset( void );

        //! Returns asset index.
        Index                           index( void ) const;

        //! Returns asset manager that issued this handle.
        Assets*                         assets( void ) const;

        //! Returns cached read-only asset data.
        template<typename TAsset>
        const TAsset&                   readOnlyData( void ) const;

        //! Returns the read-only asset data and updates the last usage timestamp.
        template<typename TAsset>
        const TAsset&                   readLock( void ) const;

        //! Returns the write lock used to update an asset data.
        template<typename TAsset>
        WriteLock<TAsset>               writeLock( void );

    protected:

                                        //! Constructs the Handle instance.
                                        Handle( Assets* assets, Index index );

        //! Set an asset handle.
        void                            setHandle( Assets* assets, Index index );

        //! Returns writable asset data.
        template<typename TAsset>
        TAsset&                         writableData( void );

    protected:

        Assets*                         m_assets;   //!< An assets manager that issued this handle.
        Index                           m_index;    //!< Asset index.
    #ifdef DC_DEBUG
        Asset*                          m_asset;    //!< Pointer to an asset is here to simplify debugging.
    #endif  /*  DC_DEBUG    */
    };

    // ** Handle::readOnlyData
    template<typename TAsset>
    const TAsset& Handle::readOnlyData( void ) const
    {
        return assets()->readOnlyAssetData<TAsset>( asset() );
    }

    // ** Handle::writableData
    template<typename TAsset>
    TAsset& Handle::writableData( void )
    {
        return assets()->writableAssetData<TAsset>( asset() );
    }

    // ** Handle::index
    NIMBLE_INLINE Index Handle::index( void ) const
    {
        return m_index;
    }

    // ** Handle::isValid
    NIMBLE_INLINE bool Handle::isValid( void ) const
    {
        return m_assets && m_assets->isIndexValid( index() );
    }

    // ** Handle::isLoaded
    NIMBLE_INLINE bool Handle::isLoaded( void ) const
    {
        return isValid() && (operator->())->state() == Asset::Loaded;
    }

    // ** Handle::readLock
    template<typename TAsset>
    const TAsset& Handle::readLock( void ) const
    {
        return assets()->acquireReadLock<TAsset>( asset() );
    }

    // ** Handle::writeLock
    template<typename TAsset>
    WriteLock<TAsset> Handle::writeLock( void )
    {
        return assets()->acquireWriteLock<TAsset>( *this );
    }

    //! This generic asset handle exposes actual asset data. All data handles are weak references and become invalid once asset is unloaded.
    template<typename TAsset>
    class GenericHandle : public Handle {
    public:

                                        //! Constructs an empty GenericHandle instance.
                                        GenericHandle( void );

                                        //! Constructs GenericHandle instance from Handle by casting it's type.
                                        GenericHandle( const Handle& asset );

        //! Copies an asset handle.
        const GenericHandle<TAsset>&    operator = ( const GenericHandle<TAsset>& other );

        //! This operator is used for read-only access to actual asset data.
        const TAsset*                   operator -> ( void ) const;

        //! This operator is used to return a const reference to actual asset data.
        const TAsset&                   operator * ( void ) const;

        //! Returns the read-only asset data and updates the last usage timestamp.
        const TAsset&                   readLock( void ) const;

        //! Forces an asset to be loaded and returns resulting asset data.
        const TAsset&                   forceLoad( void );

        //! Returns the write lock used to update an asset data.
        WriteLock<TAsset>               writeLock( void );

        //! Returns asset name.
        const String&                   name( void ) const;

        //! Returns unique asset identifier.
        const AssetId&                  uniqueId( void ) const;

    #ifdef DC_DEBUG
    private:
        //! Set an asset handle.
        void                            setHandle( Assets* assets, Index index );
        const TAsset*                   m_data; //!< Pointer to asset data to simplify debugging.
    #endif  /*  DC_DEBUG    */
    };

    // ** GenericHandle::GenericHandle
    template<typename TAsset>
    GenericHandle<TAsset>::GenericHandle( void )
    {
    
    }

    // ** GenericHandle::GenericHandle
    template<typename TAsset>
    GenericHandle<TAsset>::GenericHandle( const Handle& asset )
    {
        DC_BREAK_IF( isValid() && asset.isValid() && !asset->type() == Assets::assetTypeId<TAsset>() );
        this->setHandle( asset.assets(), asset.index() );
    }

    // ** GenericHandle::operator =
    template<typename TAsset>
    const GenericHandle<TAsset>& GenericHandle<TAsset>::operator = ( const GenericHandle<TAsset>& other )
    {
        this->setHandle( other.assets(), other.index() );
        return *this;
    }

    // ** GenericHandle::operator ->
    template<typename TAsset>
    const TAsset* GenericHandle<TAsset>::operator -> ( void ) const
    {
        return &readOnlyData<TAsset>();
    }

    // ** GenericHandle::operator *
    template<typename TAsset>
    const TAsset& GenericHandle<TAsset>::operator * ( void ) const
    {
        return readOnlyData<TAsset>();
    }

#ifdef DC_DEBUG
    // ** GenericHandle<TAsset>::setHandle
    template<typename TAsset>
    void GenericHandle<TAsset>::setHandle( Assets* assets, Index index )
    {
        Handle::setHandle( assets, index );
        m_data = &data<TAsset>();
    }
#endif  /*  DC_DEBUG    */

    // ** GenericHandle::name
    template<typename TAsset>
    const String& GenericHandle<TAsset>::name( void ) const
    {
        return asset().name();
    }

    // ** GenericHandle::uniqueId
    template<typename TAsset>
    const AssetId& GenericHandle<TAsset>::uniqueId( void ) const
    {
        return asset().uniqueId();
    }

    // ** GenericHandle::readLock
    template<typename TAsset>
    const TAsset& GenericHandle<TAsset>::readLock( void ) const
    {
        return Handle::readLock<TAsset>();
    }

    // ** GenericHandle::readLock
    template<typename TAsset>
    const TAsset& GenericHandle<TAsset>::forceLoad( void )
    {
        Handle::forceLoad();
        return readLock();
    }

    // ** GenericHandle::writeLock
    template<typename TAsset>
    WriteLock<TAsset> GenericHandle<TAsset>::writeLock( void )
    {
        return Handle::writeLock<TAsset>();
    }

    //! Asset write lock is used for updating asset data.
    template<typename TAsset>
    class WriteLock {
    friend class Assets;
    public:

                                    ~WriteLock( void );

        //! This operator is used for write access to actual asset data.
        TAsset*                     operator -> ( void );

        //! Returns a non-constant reference to asset data.
        TAsset&                     operator * ( void );

    private:

                                    //! Constructs WriteLock instance.
                                    WriteLock( const GenericHandle<TAsset>& asset );

    private:

        GenericHandle<TAsset>       m_asset;    //!< Handle to an asset being modified.
    };

    // ** WriteLock::WriteLock
    template<typename TAsset>
    WriteLock<TAsset>::WriteLock( const GenericHandle<TAsset>& asset ) : m_asset( asset )
    {
        DC_BREAK_IF( !asset.isValid() );
    }

    // ** WriteLock::WriteLock
    template<typename TAsset>
    WriteLock<TAsset>::~WriteLock( void )
    {
        m_asset.assets()->releaseWriteLock( m_asset );
    }

    // ** WriteLock::operator ->
    template<typename TAsset>
    TAsset* WriteLock<TAsset>::operator -> ( void )
    {
        return &m_asset.writableData<TAsset>();
    }

    // ** WriteLock::operator *
    template<typename TAsset>
    TAsset& WriteLock<TAsset>::operator * ( void )
    {
        return m_asset.writableData<TAsset>();
    }

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_AssetHandle_H__    */