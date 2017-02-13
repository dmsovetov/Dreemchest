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

#include "../Dreemchest.h"

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

        //! Returns an asset that is referenced by this handle.
        const Asset&                    asset( void ) const;
        Asset&                          asset( void );

        //! Returns asset index.
        const Index&                    index( void ) const;

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
                                        Handle( AssetsWPtr assets, Index index );

        //! Set an asset handle.
        void                            setHandle( AssetsWPtr assets, Index index );

        //! Returns writable asset data.
        template<typename TAsset>
        TAsset&                         writableData( void );

    protected:

        AssetsWPtr                      m_assets;   //!< An assets manager that issued this handle.
        Index                           m_index;    //!< Asset index.
    #ifdef DC_DEBUG
        Asset*                          m_asset;    //!< Pointer to an asset is here to simplify debugging.
    #endif  /*  DC_DEBUG    */
    };

    //! This asset data handle exposes actual asset data. All data handles are weak references and become invalid once asset is unloaded.
    template<typename TAsset>
    class DataHandle : public Handle {
    public:

                                        //! Constructs an empty DataHandle instance.
                                        DataHandle( void );

                                        //! Constructs DataHandle instance from Handle by casting it's type.
                                        DataHandle( const Handle& asset );

        //! Copies an asset handle.
        const DataHandle<TAsset>&       operator = ( const DataHandle<TAsset>& other );

        //! This operator is used for read-only access to actual asset data.
        const TAsset*                   operator -> ( void ) const;

        //! This operator is used to return a const reference to actual asset data.
        const TAsset&                   operator * ( void ) const;

        //! Returns the read-only asset data and updates the last usage timestamp.
        const TAsset&                   readLock( void ) const;

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
                                    WriteLock( const DataHandle<TAsset>& asset );

    private:

        DataHandle<TAsset>          m_asset;    //!< Handle to an asset being modified.
    };

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_AssetHandle_H__    */
