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

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    typedef u32 RenderResource;

    //! Manages a render scene resource cache.
    class RenderCache : public RefCounted {
    friend class RenderScene;
    public:

        //! Requests a new input layout from a rendering context or returns a cached one.
        RenderResource                          findInputLayout( const VertexFormat& format );

        //! Requests a new vertex buffer for a mesh asset or returns a cached one.
        RenderResource                          findVertexBuffer( const MeshHandle& mesh );

        //! Requests a new index buffer for a mesh asset or returns a cached one.
        RenderResource                          findIndexBuffer( const MeshHandle& mesh );

        //! Requests a new material constant buffer or returns a cached one.
        RenderResource                          findConstantBuffer( const MaterialHandle& material );

        //! Creates a RenderCache instance.
        static RenderCachePtr                   create( Assets::AssetsWPtr assets, RenderingContextWPtr context );

    private:

                                                //! Constructs a RenderCache instance.
                                                RenderCache( Assets::AssetsWPtr assets, RenderingContextWPtr context );

    private:

        //! Container type to store mapping from a vertex format to a previously created input layout.
        typedef HashMap<u8, RenderResource>     InputLayouts;

        //! Container type to store mapping from an asset id to a previously created render resource.
        typedef HashMap<Assets::AssetId, RenderResource> RenderResources;

        Assets::AssetsWPtr                      m_assets;                   //!< Parent assets instance.
        RenderingContextWPtr                    m_context;                  //!< Parent rendering context.
        InputLayouts                            m_inputLayouts;             //!< Input layout cache.
        RenderResources                         m_vertexBuffers;            //!< Vertex buffer cache.
        RenderResources                         m_indexBuffers;             //!< Index buffer cache.
        RenderResources                         m_materialConstantBuffers;  //!< Material constant buffers cache.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderCache_H__    */