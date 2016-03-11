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

#ifndef __DC_Scene_RenderAssetSources_H__
#define __DC_Scene_RenderAssetSources_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Constructs renderable asset from a mesh asset.
    class RenderableMeshSource : public Assets::AssetSource<Renderable, Mesh> {
    public:

                                //! Constructs RenderableMeshSource instance.
                                RenderableMeshSource( MeshHandle mesh, RenderingContextWPtr context );

    protected:

        //! Constructs a renderable asset from a mesh source.
        virtual bool            constructFromAsset( const Mesh& mesh, Assets::Assets& assets, Renderable& renderable ) DC_DECL_OVERRIDE;

    private:

        RenderingContextWPtr    m_context;  //!< Parent context instance.
    };

    //! Constructs texture asset from an image.
    class TextureImageSource : public Assets::AssetSource<Texture, Image> {
    public:

                                //! Constructs TextureImageSource instance.
                                TextureImageSource( ImageHandle image, RenderingContextWPtr context );

    protected:

        //! Constructs a texture asset from an image source.
        virtual bool            constructFromAsset( const Image& image, Assets::Assets& assets, Texture& texture ) DC_DECL_OVERRIDE;

    private:

        RenderingContextWPtr    m_context;  //!< Parent context instance.   
    };

    //! Constructs technique asset from a material.
    class TechniqueMaterialSource : public Assets::AssetSource<Technique, Material> {
    public:

                                //! Constructs TechniqueMaterialSource instance.
                                TechniqueMaterialSource( MaterialHandle material, RenderingContextWPtr context );

    protected:

        //! Constructs a technique asset from a material source.
        virtual bool            constructFromAsset( const Material& material, Assets::Assets& assets, Technique& technique ) DC_DECL_OVERRIDE;

    private:

        RenderingContextWPtr    m_context;  //!< Parent context instance.   
    };

    //! Compiles the shader permutation instance.
    class ProgramShaderSource : public Assets::AssetSource<Program, Shader> {
    public:

                                //! Constructs ProgramShaderSource instance.
                                ProgramShaderSource( ShaderHandle shader, RenderingContextWPtr context );

    protected:

        //! Constructs a technique asset from a material source.
        virtual bool            constructFromAsset( const Shader& shader, Assets::Assets& assets, Program& program ) DC_DECL_OVERRIDE;

    private:

        RenderingContextWPtr    m_context;  //!< Parent context instance.    
    };

    //! Loads a shader from a text file
    class ShaderFormatText : public Assets::FileSource<Shader> {
    protected:

        //! Loads shader source from an input stream.
        virtual bool    constructFromStream( Io::StreamPtr stream, Assets::Assets& assets, Shader& shader ) DC_DECL_OVERRIDE;
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderAssetSources_H__    */