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

#ifndef __DC_Assets_Asset_H__
#define __DC_Assets_Asset_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    //! Asset class instance stores info about a single asset.
    class Asset {
    friend class Assets;
    friend class LoadingQueue;
    public:

        //! Available asset states.
        enum State {
              Unloaded              //!< Asset is now unloaded.
            , WaitingForLoading     //!< Asset is now waiting to start loading.
            , Loading               //!< Asset is now loading.
            , Loaded                //!< Asset is loaded and ready to use.
            , Error                 //!< Asset was failed to load.
        };

        //! Asset data timestamp.
        struct Timestamp {
            u32                     constructed;  //!< Last time this asset was constructed from source.
            u32                     modified;     //!< Last time this asset was modified.
            u32                     used;         //!< Last time this asset was used.
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

        //! Returns the data cache index.
        const Index&                dataIndex( void ) const;

        //! Returns an asset source.
        AbstractSource&             source( void ) const;

        //! Returns an asset data cache.
        AbstractAssetCache&         cache( void ) const;

        //! Returns asset state.
        State                       state( void ) const;

        //! Returns true if an asset is loaded.
        bool                        isLoaded( void ) const;

        //! Returns true if an asset is up to date.
        bool                        isUpToDate( void ) const;

        //! Returns asset timestamp.
        const Timestamp&            timestamp( void ) const;

        //! Returns true if an asset is of specified type.
        template<typename TAsset>
        bool                        is( void ) const;
        
        //! Returns an asset type id.
        template<typename TAsset>
        static TypeId               typeId( void );

    private:

                                    //! Constructs Asset instance.
                                    Asset( const TypeId& type, AbstractAssetCache* cache, const Index& value, const AssetId& uniqueId, SourceUPtr source );

        //! Switches an asset to a specified state.
        void                        switchToState( State value );

    private:

        SourceUPtr                  m_source;           //!< Asset source used for asset construction.
        TypeId                      m_type;             //!< Asset type.
        AssetId                     m_uniqueId;         //!< Unique asset id.
        String                      m_name;             //!< Asset name.
        State                       m_state;            //!< Current asset state.
        Index                       m_data;             //!< Asset data slot.
        AbstractAssetCache*         m_cache;            //!< Asset data cache pointer.
        mutable Timestamp           m_timestamp;        //!< Asset data timestamp.
    };

    // ** Asset::is
    template<typename TAsset>
    NIMBLE_INLINE bool Asset::is( void ) const
    {
        return m_type == typeId<TAsset>();
    }
    
    // ** Assets::typeId
    template<typename TAsset>
    NIMBLE_INLINE TypeId Asset::typeId( void )
    {
        return GroupedTypeIndex<TAsset, Asset>::idx();
    }

    // ** Asset::isLoaded
    NIMBLE_INLINE bool Asset::isLoaded( void ) const
    {
        return m_state == Loaded;
    }

    // ** Asset::cache
    NIMBLE_INLINE AbstractAssetCache& Asset::cache( void ) const
    {
        NIMBLE_ABORT_IF( m_cache == NULL, "invalid asset cache" );
        return *m_cache;
    }

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_Asset_H__    */
