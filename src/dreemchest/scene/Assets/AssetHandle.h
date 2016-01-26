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

#ifndef __DC_Scene_AssetHandle_H__
#define __DC_Scene_AssetHandle_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

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

        //! Compares two asset handles.
        bool                        operator < ( const AssetHandle& other ) const;

        //! This operator is used for read-only access to actual asset data.
        const Asset*                operator -> ( void ) const;
        Asset*                      operator -> ( void );

        //! Returns true if this asset handle is still valid.
        bool                        isValid( void ) const;

    private:

                                    //! Constructs the AssetHandle instance.
                                    AssetHandle( const Assets* assets, SlotIndex32 slot );

    private:

        const Assets*               m_assets;   //!< An assets manager that issued this handle.
        SlotIndex32                 m_slot;     //!< Asset slot.
    };

    //! This generic asset handle exposes actual asset data. All data handles are weak references and become invalid once asset is unloaded.
    template<typename TAsset>
    class AssetDataHandle {
    public:

                                        //! Constructs an empty AssetDataHandle instance.
                                        AssetDataHandle( void );

                                        //! Constructs AssetDataHandle instance from another one.
                                        AssetDataHandle( const AssetDataHandle<TAsset>& other );

                                        //! Constructs AssetDataHandle instance from AssetHandle by casting it's type.
                                        AssetDataHandle( const AssetHandle& asset );

        //! Copies an asset handle.
        const AssetDataHandle<TAsset>&  operator = ( const AssetDataHandle<TAsset>& other );

        //! This operator is used for read-only access to actual asset data.
        const TAsset*                   operator -> ( void ) const;

        //! Returns true if this asset handle is still valid.
        bool                            isValid( void ) const;
    };

    // ** AssetDataHandle::AssetDataHandle
    template<typename TAsset>
    AssetDataHandle<TAsset>::AssetDataHandle( void )
    {
    
    }

    // ** AssetDataHandle::AssetDataHandle
    template<typename TAsset>
    AssetDataHandle<TAsset>::AssetDataHandle( const AssetDataHandle<TAsset>& other )
    {
    
    }

    // ** AssetDataHandle::AssetDataHandle
    template<typename TAsset>
    AssetDataHandle<TAsset>::AssetDataHandle( const AssetHandle& asset )
    {
    
    }

    // ** AssetDataHandle::operator =
    template<typename TAsset>
    const AssetDataHandle<TAsset>& AssetDataHandle<TAsset>::operator = ( const AssetDataHandle<TAsset>& other )
    {
        return *this;
    }

    // ** AssetDataHandle::operator ->
    template<typename TAsset>
    const TAsset* AssetDataHandle<TAsset>::operator -> ( void ) const
    {
        return NULL;
    }

    // ** AssetDataHandle::isValid
    template<typename TAsset>
    bool AssetDataHandle<TAsset>::isValid( void ) const
    {
        return false;
    }

#if ASSET_DEPRECATED
    //! This handles are issued by an Assets class and are the only way the outer world can access an asset.
    template<typename TAsset>
    class AssetHandle {
    friend typename AssetPool<TAsset>;
    public:

                                    //! Constructs an empty AssetHandle instance.
                                    AssetHandle( void );

                                    //! Constructs AssetHandle instance from another one.
                                    AssetHandle( const AssetHandle<TAsset>& other );

        //! Copies an asset handle.
        const AssetHandle<TAsset>&  operator = ( const AssetHandle<TAsset>& other );

        //! This operator is used for read-only access to actual asset data.
        const TAsset*               operator -> ( void ) const;

        //! Returns the read-only asset data and updates the last usage timestamp.
        const TAsset&               readLock( void ) const;

        //! Returns the write lock used to update an asset data.
        AssetWriteLock<TAsset>      writeLock( void );

        //! Returns true if this asset handle is still valid.
        bool                        isValid( void ) const;

    private:

                                    //! Constructs the AssetHandle instance.
                                    AssetHandle( const AssetPool<TAsset>& pool, SlotIndex32 slot );

    private:

        const AssetPool<TAsset>*    m_pool; //!< An asset pool that issued this handle.
        SlotIndex32                 m_slot; //!< Asset slot.
    };

    // ** AssetHandle::AssetHandle
    template<typename TAsset>
    AssetHandle<TAsset>::AssetHandle( void ) : m_pool( NULL )
    {
    
    }

    // ** AssetHandle::AssetHandle
    template<typename TAsset>
    AssetHandle<TAsset>::AssetHandle( const AssetPool<TAsset>& pool, SlotIndex32 slot ) : m_pool( &pool ), m_slot( slot )
    {
    
    }

    // ** AssetHandle::AssetHandle
    template<typename TAsset>
    AssetHandle<TAsset>::AssetHandle( const AssetHandle<TAsset>& other ) : m_pool( other.m_pool ), m_slot( other.m_slot )
    {
    
    }

    // ** AssetHandle::operator ->
    template<typename TAsset>
    const AssetHandle<TAsset>& AssetHandle<TAsset>::operator = ( const AssetHandle<TAsset>& other )
    {
        m_pool = other.m_pool;
        m_slot = other.m_slot;
        return *this;
    }

    // ** AssetHandle::operator ->
    template<typename TAsset>
    const TAsset* AssetHandle<TAsset>::operator -> ( void ) const
    {
        return isValid() ? &m_pool->assetAtSlot( m_slot ) : NULL;
    }

    // ** AssetHandle::isValid
    template<typename TAsset>
    bool AssetHandle<TAsset>::isValid( void ) const
    {
        return m_pool && m_pool->isValidSlot( m_slot );
    }

    // ** AssetHandle::readLock
    template<typename TAsset>
    const TAsset& AssetHandle<TAsset>::readLock( void ) const
    {
        DC_BREAK_IF( !isValid() );

        // Update the used timestamp
        m_pool->updateLastUsed( m_slot );

        // Resolve reference
        return *operator->();
    }

    // ** AssetHandle::writeLock
    template<typename TAsset>
    AssetWriteLock<TAsset> AssetHandle<TAsset>::writeLock( void )
    {
        return AssetWriteLock<TAsset>( m_pool, m_slot );
    }

    //! Asset write lock is used for updating asset data.
    template<typename TAsset>
    class AssetWriteLock {
    friend typename AssetHandle<TAsset>;
    public:

                                    ~AssetWriteLock( void );

        //! This operator is used for write access to actual asset data.
        TAsset*                     operator -> ( void );

    private:

                                    //! Constructs AssetWriteLock instance.
                                    AssetWriteLock( const AssetPool<TAsset>* pool, SlotIndex32 slot );

    private:

        const AssetPool<TAsset>*    m_pool; //!< An asset pool that issued this handle.
        SlotIndex32                 m_slot; //!< Asset slot.
    };

    // ** AssetWriteLock::AssetWriteLock
    template<typename TAsset>
    AssetWriteLock<TAsset>::AssetWriteLock( const AssetPool<TAsset>* pool, SlotIndex32 slot ) : m_pool( pool ), m_slot( slot )
    {
        DC_BREAK_IF( m_pool == NULL )
    }

    // ** AssetWriteLock::AssetWriteLock
    template<typename TAsset>
    AssetWriteLock<TAsset>::~AssetWriteLock( void )
    {
        m_pool->updateLastModified( m_slot );
    }

    // ** AssetWriteLock::operator ->
    template<typename TAsset>
    TAsset* AssetWriteLock<TAsset>::operator -> ( void )
    {
        return &const_cast<TAsset&>( m_pool->assetAtSlot( m_slot ) );
    }
#endif

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_AssetHandle_H__    */