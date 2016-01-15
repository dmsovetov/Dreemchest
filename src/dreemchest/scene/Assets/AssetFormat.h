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

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Base class for all asset formats.
    class AbstractAssetFormat {
    public:
    };

    //! Generic base class for all asset format parsers.
    template<typename TAsset>
    class AssetFormat : public AbstractAssetFormat {
    public:

        //! Performs an asset data parsing from a stream.
        virtual bool    parse( Io::StreamPtr stream, Assets& assets, TAsset& asset ) = 0;
    };

    //! Loads an image from a raw pixel buffer format.
    class ImageFormatRaw : public AssetFormat<Image> {
    protected:

        //! Loads image data from an input stream.
        virtual bool    parse( Io::StreamPtr stream, Assets& assets, Image& image ) DC_DECL_OVERRIDE;
    };

    //! Loads a mesh from a raw binary format.
    class MeshFormatRaw : public AssetFormat<Mesh> {
    protected:

        //! Loads mesh data from an input stream.
        virtual bool    parse( Io::StreamPtr stream, Assets& assets, Mesh& image ) DC_DECL_OVERRIDE;
    };

    //! Loads a material from a key-value storage.
    class MaterialFormatKeyValue : public AssetFormat<Material> {
    protected:

        //! Loads material data from an input stream.
        virtual bool    parse( Io::StreamPtr stream, Assets& assets, Material& image ) DC_DECL_OVERRIDE;
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_AssetFormat_H__    */