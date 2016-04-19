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

#ifndef __DC_Scene_RenderFrame_H__
#define __DC_Scene_RenderFrame_H__

#include "Commands.h"
#include "RenderState.h"
#include "../RenderTarget.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Render frame contains all required frame data captured by a render scene.
    class RenderFrame {
    public:
                        
                                                //! Constructs a RenderFrame instance.
                                                RenderFrame( void );

        //! Returns a total number of captured command buffers.
        s32                                     commandBufferCount( void ) const;

        //! Returns a command buffer at specified index.
        const RenderCommandBuffer&              commandBufferAt( s32 index ) const;

        //! Creates a new command buffer.
        RenderCommandBuffer&                    createCommandBuffer( void );

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
       

        //! Interns a texture and returns it's integer identifier.
        s32                                     internTexture( Renderer::TexturePtr texture );
        
        //! Returns a texture by an index.
        const Renderer::TexturePtr&             texture( s32 identifier ) const;

        //! Interns a shader and returns it's integer identifier.
        s32                                     internShader( Renderer::ShaderPtr shader );
        
        //! Returns a shader by an index.
        const Renderer::ShaderPtr&              shader( s32 identifier ) const;

        //! Returns a state stack.
        RenderStateStack&                       stateStack( void );

    private:

        //! Container type to store recorded command buffers.
        typedef Array<RenderCommandBufferUPtr> Commands;

        Commands                                m_commandBuffers;       //!< An array of recorded commands buffers.
        IndexCache<RenderTargetPtr>             m_renderTargets;        //!< Interned render targets.
        IndexCache<Renderer::VertexBufferPtr>   m_vertexBuffers;        //!< Interned vertex buffers.
        IndexCache<Renderer::IndexBufferPtr>    m_indexBuffers;         //!< Interned index buffers.
        IndexCache<Renderer::ConstantBufferPtr> m_constantBuffers;      //!< Interned constant buffers.
        IndexCache<Renderer::TexturePtr>        m_textures;             //!< Interned textures.
        IndexCache<Renderer::ShaderPtr>         m_shaders;              //!< Interned shaders.
        RenderStateStack                        m_stateStack;           //!< Current state stack.
    };

    //! Returns a total number of captured command buffers.
    NIMBLE_INLINE s32 RenderFrame::commandBufferCount( void ) const
    {
        return static_cast<s32>( m_commandBuffers.size() );
    }

    //! Returns a command buffer at specified index.
    NIMBLE_INLINE const RenderCommandBuffer& RenderFrame::commandBufferAt( s32 index ) const
    {
        DC_ABORT_IF( index < 0 || index >= commandBufferCount(), "index is out of range" );
        return *m_commandBuffers[index].get();
    }

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderFrame_H__    */