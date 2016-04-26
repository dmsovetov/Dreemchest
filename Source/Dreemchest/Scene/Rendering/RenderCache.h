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

#ifndef __DC_Scene_RenderCache_H__
#define __DC_Scene_RenderCache_H__

#include "RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Manages a render scene resource cache.
    class AbstractRenderCache : public RefCounted {
    public:

        //! A material node contains a cached material data (like constant buffer, allocated material parameters, etc.)
        struct MaterialNode {
            //! A material node constant buffer type alias.
            typedef RenderScene::CBuffer::Material  CBuffer;

            RenderResource                      constantBuffer; //!< A material constant buffer handle.
            CBuffer                             data;           //!< Material constant buffer.
            RenderStateBlock                    states;         //!< Material states that is bound prior an instance state block.
        };

        virtual                                 ~AbstractRenderCache( void ) {}

        //! Creates a material node instance for a specified material or returns a cached one.
        virtual const MaterialNode*             requestMaterial( const MaterialHandle& material ) NIMBLE_ABSTRACT;

        //! Creates an input layout for a specified vertex format or returns a cached one.
        virtual RenderResource                  requestInputLayout( const VertexFormat& vertexFormat ) NIMBLE_ABSTRACT;

        //! Requests a new vertex buffer for a mesh asset or returns a cached one.
        virtual RenderResource                  requestVertexBuffer( const MeshHandle& mesh ) NIMBLE_ABSTRACT;

        //! Requests a new index buffer for a mesh asset or returns a cached one.
        virtual RenderResource                  requestIndexBuffer( const MeshHandle& mesh ) NIMBLE_ABSTRACT;
    };


    //! Manages a render scene resource cache.
    class TestRenderCache : public AbstractRenderCache {
    friend class RenderScene;
    public:

        //! Creates an input layout for a specified vertex format or returns a cached one.
        virtual RenderResource                  requestInputLayout( const VertexFormat& vertexFormat ) NIMBLE_OVERRIDE;

        //! Requests a new vertex buffer for a mesh asset or returns a cached one.
        virtual RenderResource                  requestVertexBuffer( const MeshHandle& mesh ) NIMBLE_OVERRIDE;

        //! Requests a new index buffer for a mesh asset or returns a cached one.
        virtual RenderResource                  requestIndexBuffer( const MeshHandle& mesh ) NIMBLE_OVERRIDE;

        //! Creates a material node instance for a specified material or returns a cached one.
        virtual const MaterialNode*             requestMaterial( const MaterialHandle& material ) NIMBLE_OVERRIDE;

        //! Requests a new texture or returns a cached one.
        RenderResource                          requestTexture( const ImageHandle& image );

        //! Creates a RenderCache instance.
        static RenderCachePtr                   create( Assets::AssetsWPtr assets, RenderingContextWPtr context );

    private:

                                                //! Constructs a TestRenderCache instance.
                                                TestRenderCache( Assets::AssetsWPtr assets, RenderingContextWPtr context );

    private:

        //! Container type to store mapping from a vertex format to a previously created input layout.
        typedef HashMap<u8, RenderResource>     InputLayouts;

        //! Container type to store mapping from an asset id to a previously created render resource.
        typedef HashMap<Assets::AssetId, RenderResource> RenderResources;

        //! Container type to store a material state block cache.
        typedef HashMap<Assets::AssetId, AutoPtr<MaterialNode>> MaterialNodeCache;

        Assets::AssetsWPtr                      m_assets;                   //!< Parent assets instance.
        RenderingContextWPtr                    m_context;                  //!< Parent rendering context.
        InputLayouts                            m_inputLayouts;             //!< Input layout cache.
        RenderResources                         m_vertexBuffers;            //!< Vertex buffer cache.
        RenderResources                         m_indexBuffers;             //!< Index buffer cache.
        RenderResources                         m_textures;                 //!< Texture cache.
        MaterialNodeCache                       m_materials;                //!< Material render state block cache.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderCache_H__    */