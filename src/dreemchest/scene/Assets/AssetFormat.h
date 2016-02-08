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

#ifndef __DC_Scene_AssetFormat_H__
#define __DC_Scene_AssetFormat_H__

#include "../Scene.h"

#include "Image.h"
#include "Mesh.h"
#include "Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Base class for all asset format parsers.
    class AbstractAssetFormat {
    public:

        virtual         ~AbstractAssetFormat( void ) {}

        //! Loads data to a specified asset data handle.
        virtual bool    parse( Assets& assets, AssetHandle asset ) = 0;
    };

    //! Asset file format used for loading assets from files.
    class AbstractAssetFileFormat : public AbstractAssetFormat {
    public:

                        //! Constructs AbstractAssetFileFormat instance.
                        AbstractAssetFileFormat( const String& fileName );

        //! Opens the file stream and loads data from it.
        virtual bool    parse( Assets& assets, AssetHandle asset ) DC_DECL_OVERRIDE;

        //! Returns the source asset file name.
        const String&   fileName( void ) const;

    protected:

        //! This virtual method is used to dispatch the loading process to actual asset loading implementation.
        virtual bool    parseFromStream( Io::StreamPtr stream, Assets& assets, AssetHandle asset ) = 0;

    private:

        String          m_fileName; //!< Source file name to load asset from.
    };

    //! Generic base class for all asset file format parsers.
    template<typename TAsset>
    class AssetFileFormat : public AbstractAssetFileFormat {
    protected:

        //! Type casts an asset handle and dispatches the loading process to an implementation.
        virtual bool    parseFromStream( Io::StreamPtr stream, Assets& assets, AssetHandle asset ) DC_DECL_OVERRIDE;

        //! Performs an asset data parsing from a stream.
        virtual bool    parseFromStream( Io::StreamPtr stream, Assets& assets, TAsset& asset ) = 0;
    };

    // ** AssetFileFormat::parseFromStream
    template<typename TAsset>
    bool AssetFileFormat<TAsset>::parseFromStream( Io::StreamPtr stream, Assets& assets, AssetHandle asset )
    {
        AssetWriteLock<TAsset> lock = asset.writeLock<TAsset>();
        TAsset&                data = *lock;
        bool result = parseFromStream( stream, assets, data );
        return result;
    }

    //! Loads an image from a raw pixel buffer format.
    class ImageFormatRaw : public AssetFileFormat<Image> {
    protected:

        //! Loads image data from an input stream.
        virtual bool    parseFromStream( Io::StreamPtr stream, Assets& assets, Image& image ) DC_DECL_OVERRIDE;
    };

    //! Loads a mesh from a raw binary format.
    class MeshFormatRaw : public AssetFileFormat<Mesh> {
    protected:

        //! Loads mesh data from an input stream.
        virtual bool    parseFromStream( Io::StreamPtr stream, Assets& assets, Mesh& image ) DC_DECL_OVERRIDE;
    };

    //! Loads a material from a key-value storage.
    class MaterialFormatKeyValue : public AssetFileFormat<Material> {
    protected:

        //! Loads material data from an input stream.
        virtual bool    parseFromStream( Io::StreamPtr stream, Assets& assets, Material& image ) DC_DECL_OVERRIDE;
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_AssetFormat_H__    */