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

#ifndef __DC_Scene_AssetPool_H__
#define __DC_Scene_AssetPool_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Forward declaration of an abstract asset format.
    class AbstractAssetFormat;

    //! Base class for all asset pools.
    class AbstractAssetPool {
    protected:

        //! Info about a single asset file.
        struct File {
            AssetId                 uniqueId;   //!< Unique file id.
            AssetSlot               asset;      //!< Handle to an asset stored inside the pool.
            AbstractAssetFormat*    format;     //!< Asset format used for loading this asset.

                                    //! Constructs File instance.
                                    File( const AssetId& uniqueId = AssetId(), AssetSlot asset = AssetSlot(), AbstractAssetFormat* format = NULL )
                                        : uniqueId( uniqueId ), asset( asset ), format( format ) {}
        };

        //! Returns an asset file by it's id.
        const File&                 findFile( const AssetId& id ) const;

        //! Adds new file with a specified id and format.
        void                        addFile( const AssetId& id, AssetSlot asset, AbstractAssetFormat* format );

        //! Removes an asset file by it's id, returns the associated asset handle to release an asset data.
        AssetSlot                   removeFile( const AssetId& id );

    private:

        //! Container type to store all available asset files.
        typedef Map<AssetId, File>  AssetFiles;

        AssetFiles                  m_files;    //!< Available asset files.
    };

    //! Generic class to store all available assets of type TAsset.
    template<typename TAsset>
    class AssetPool : public AbstractAssetPool {
    friend typename AssetHandle<TAsset>;
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
        const TAsset&               assetAtSlot( AssetSlot slot ) const;

        //! Returns true if the specified asset slot handle is valid.
        bool                        isValidSlot( AssetSlot slot ) const;

    protected:

        Slots<TAsset, AssetSlot>    m_assets;    //!< Asset data for each asset file is stored here.
    };

    // ** AssetPool::add
    template<typename TAsset>
    typename AssetPool<TAsset>::Handle AssetPool<TAsset>::add( const AssetId& id, AssetFormat<TAsset>* format )
    {
        DC_BREAK_IF( format == NULL );

        // First reserve the slot for an asset data
        AssetSlot asset = m_assets.add( TAsset() );

        // Now add an asset file associated with this asset handle
        addFile( id, asset, format );

        // Construct an asset handle.
        return Handle( *this, asset );
    }

    // ** AssetPool::remove
    template<typename TAsset>
    bool AssetPool<TAsset>::remove( const AssetId& id )
    {
        // Remove the file by id and store the associated asset handle
        AssetSlot asset = removeFile( id );

        // Now release an asset data
        return m_assets.remove( asset );
    }

    // ** AssetPool::get
    template<typename TAsset>
    typename AssetPool<TAsset>::Handle AssetPool<TAsset>::get( const AssetId& id )
    {
        // First find an asset file by it.
        const File& file = findFile( id );
        DC_BREAK_IF( !m_assets.has( file.asset ) );

        // Now construct an asset handle.
        return Handle( *this, file.asset );
    }

    // ** AssetPool::assetAtSlot
    template<typename TAsset>
    const TAsset& AssetPool<TAsset>::assetAtSlot( AssetSlot slot ) const
    {
        return m_assets.get( slot );
    }

    // ** // ** AssetPool::isValidSlot
    template<typename TAsset>
    bool AssetPool<TAsset>::isValidSlot( AssetSlot slot ) const
    {
        return m_assets.has( slot );
    }

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_AssetPool_H__    */