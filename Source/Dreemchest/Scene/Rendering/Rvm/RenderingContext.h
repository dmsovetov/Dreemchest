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

#ifndef __DC_Scene_RenderingContext_H__
#define __DC_Scene_RenderingContext_H__

#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Rendering context.
    class RenderingContext : public RefCounted {
    public:

        //! Returns a parent rendering HAL instance.
        Renderer::HalWPtr                       hal( void ) const;

        //! Creates an input layout instance and returns it's index.
        RenderResource                          createInputLayout( const VertexFormat& vertexFormat );

        //! Creates a vertex buffer from a vertex array with a specified vertex format.
        RenderResource                          createVertexBuffer( const void* vertices, s32 count, const VertexFormat& dstFormat, const VertexFormat& srcFormat );

        //! Creates an index buffer from an index array.
        RenderResource                          createIndexBuffer( const u16* indices, s32 count );

        //! Creates a constant buffer with specified size and layout.
        RenderResource                          createConstantBuffer( s32 size, const Renderer::ConstantBufferLayout* layout );

        //! Interns a render target and returns it's integer identifier.
        s32                                     internRenderTarget( RenderTargetPtr renderTarget );

        //! Returns a render target by an index.
        const RenderTargetPtr&                  renderTarget( s32 identifier ) const;

        //! Returns a vertex buffer by an index.
        const Renderer::VertexBufferPtr&        vertexBuffer( RenderResource identifier ) const;

        //! Returns an index buffer by an index.
        const Renderer::IndexBufferPtr&         indexBuffer( RenderResource identifier ) const;

        //! Returns a constant buffer by an index.
        const Renderer::ConstantBufferPtr&      constantBuffer( RenderResource identifier ) const;

        //! Returns an input layout by an index.
        const Renderer::InputLayoutPtr&         inputLayout( RenderResource identifier ) const;
       
        //! Interns a texture and returns it's integer identifier.
        s32                                     internTexture( Renderer::TexturePtr texture );
        
        //! Returns a texture by an index.
        const Renderer::TexturePtr&             texture( s32 identifier ) const;

        //! Interns a shader and returns it's integer identifier.
        s32                                     internShader( UbershaderPtr shader );
        
        //! Returns a shader by an index.
        const UbershaderPtr&                    shader( s32 identifier ) const;

        //! Creates a RenderingContext instance.
        static RenderingContextPtr              create( Renderer::HalWPtr hal );

    private:

                                                //! Constructs a RenderingContext instance.
                                                RenderingContext( Renderer::HalWPtr hal );

    private:

        Renderer::HalWPtr                       m_hal;                  //!< A rendering HAL to be used.
        FixedArray<Renderer::VertexBufferPtr>   m_vertexBufferPool;     //!< Allocated vertex buffers.
        FixedArray<Renderer::IndexBufferPtr>    m_indexBufferPool;      //!< Allocated index buffers.
        FixedArray<Renderer::ConstantBufferPtr> m_constantBufferPool;   //!< Allocated constant buffers.
        FixedArray<Renderer::InputLayoutPtr>    m_inputLayoutPool;      //!< Allocated input layouts.

        IndexCache<RenderTargetPtr>             m_renderTargets;        //!< Interned render targets.
        IndexCache<Renderer::TexturePtr>        m_textures;             //!< Interned textures.
        IndexCache<UbershaderPtr>               m_shaders;              //!< Interned shaders.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContext_H__    */