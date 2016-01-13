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

    //! This handles are issued by an Assets class and are the only way the outer world can access an asset.
    template<typename TAsset>
    class AssetHandle {
    friend class Assets;
    public:

        typedef Slots<TAsset>       Pool;   //!< Alias the asset pool type.
        typedef typename Pool::ID   ID;     //!< Alias the asset id type.

        //! This operator is used to access actual asset data.
        TAsset*                     operator -> ( void );
        const TAsset*               operator -> ( void ) const;

        //! Returns true if this asset handle is still valid.
        bool                        isValid( void ) const;

    private:

                                    //! Constructs the AssetHandle instance.
                                    AssetHandle( const Pool* pool, ID id );

    private:

        const Pool*                 m_pool; //!< An asset pool that issued this handle.
        ID                          m_id;   //!< Actual asset id.
    };

    // ** AssetHandle::AssetHandle
    template<typename TAsset>
    AssetHandle<TAsset>::AssetHandle( const Pool* pool, ID id ) : m_pool( pool ), m_id( id )
    {
    
    }

    // ** AssetHandle::operator ->
    template<typename TAsset>
    TAsset* AssetHandle<TAsset>::operator -> ( void )
    {
        return isValid() ? &m_pool->get( m_id ) : NULL;
    }

    // ** AssetHandle::operator ->
    template<typename TAsset>
    const TAsset* AssetHandle<TAsset>::operator -> ( void ) const
    {
        return isValid() ? &m_pool->get( m_id ) : NULL;
    }

    // ** AssetHandle::isValid
    template<typename TAsset>
    bool AssetHandle<TAsset>::isValid( void ) const
    {
        return m_pool->has( m_id );
    }

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_AssetHandle_H__    */