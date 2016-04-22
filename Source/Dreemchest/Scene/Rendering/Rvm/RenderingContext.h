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

        //! Interns a render target and returns it's integer identifier.
        s32                                     internRenderTarget( RenderTargetPtr renderTarget );

        //! Returns a render target by an index.
        const RenderTargetPtr&                  renderTarget( s32 identifier ) const;

        //! Interns a vertex buffer and returns it's integer identifier.
        s32                                     internVertexBuffer( Renderer::VertexBufferPtr vertexBuffer );
        
        //! Returns a vertex buffer by an index.
        const Renderer::VertexBufferPtr&        vertexBuffer( s32 identifier ) const;

        //! Interns an index buffer and returns it's integer identifier.
        s32                                     internIndexBuffer( Renderer::IndexBufferPtr indexBuffer );

        //! Returns an index buffer by an index.
        const Renderer::IndexBufferPtr&         indexBuffer( s32 identifier ) const;

        //! Interns a constant returns it's integer identifier.
        s32                                     internConstantBuffer( Renderer::ConstantBufferPtr constantBuffer );

        //! Returns a constant buffer by an index.
        const Renderer::ConstantBufferPtr&      constantBuffer( s32 identifier ) const;

        //! Interns an input layout returns it's integer identifier.
        s32                                     internInputLayout( Renderer::InputLayoutPtr inputLayout );

        //! Returns an input layout by an index.
        const Renderer::InputLayoutPtr&         inputLayout( s32 identifier ) const;
       
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

        Renderer::HalWPtr                       m_hal;  //!< A rendering HAL to be used.
        IndexCache<RenderTargetPtr>             m_renderTargets;        //!< Interned render targets.
        IndexCache<Renderer::VertexBufferPtr>   m_vertexBuffers;        //!< Interned vertex buffers.
        IndexCache<Renderer::IndexBufferPtr>    m_indexBuffers;         //!< Interned index buffers.
        IndexCache<Renderer::ConstantBufferPtr> m_constantBuffers;      //!< Interned constant buffers.
        IndexCache<Renderer::InputLayoutPtr>    m_inputLayouts;         //!< Interned input layouts.
        IndexCache<Renderer::TexturePtr>        m_textures;             //!< Interned textures.
        IndexCache<UbershaderPtr>               m_shaders;              //!< Interned shaders.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContext_H__    */