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

#include "RenderFrame.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderFrame::createCommandBuffer
RenderCommandBuffer& RenderFrame::createCommandBuffer( void )
{
    m_commandBuffers.push_back( DC_NEW RenderCommandBuffer );
    return *m_commandBuffers.back().get();
}

// ** RenderFrame::internRenderTarget
s32 RenderFrame::internRenderTarget( RenderTargetPtr renderTarget )
{
    return m_renderTargets.add( renderTarget );
}

// ** RenderFrame::renderTarget
const RenderTargetPtr& RenderFrame::renderTarget( s32 identifier ) const
{
    return m_renderTargets.resolve( identifier );
}

// ** RenderFrame::internVertexBuffer
s32 RenderFrame::internVertexBuffer( Renderer::VertexBufferPtr vertexBuffer )
{
    return m_vertexBuffers.add( vertexBuffer );
}
        
// ** RenderFrame::vertexBuffer
const Renderer::VertexBufferPtr& RenderFrame::vertexBuffer( s32 identifier ) const
{
    return m_vertexBuffers.resolve( identifier );
}

// ** RenderFrame::internIndexBuffer
s32 RenderFrame::internIndexBuffer( Renderer::IndexBufferPtr indexBuffer )
{
    return m_indexBuffers.add( indexBuffer );
}
        
// ** RenderFrame::indexBuffer
const Renderer::IndexBufferPtr& RenderFrame::indexBuffer( s32 identifier ) const
{
    return m_indexBuffers.resolve( identifier );
}

// ** RenderFrame::internConstantBuffer
s32 RenderFrame::internConstantBuffer( Renderer::ConstantBufferPtr constantBuffer )
{
    return m_constantBuffers.add( constantBuffer );
}

// ** RenderFrame::constantBuffer
const Renderer::ConstantBufferPtr& RenderFrame::constantBuffer( s32 identifier ) const
{
    return m_constantBuffers.resolve( identifier );
}

// ** RenderFrame::internTexture
s32 RenderFrame::internTexture( Renderer::TexturePtr texture )
{
    return m_textures.add( texture );
}
        
// ** RenderFrame::texture
const Renderer::TexturePtr& RenderFrame::texture( s32 identifier ) const
{
    return m_textures.resolve( identifier );
}

// ** RenderFrame::internShader
s32 RenderFrame::internShader( Renderer::ShaderPtr shader )
{
    return m_shaders.add( shader );
}
        
// ** RenderFrame::shader
const Renderer::ShaderPtr& RenderFrame::shader( s32 identifier ) const
{
    return m_shaders.resolve( identifier );
}

} // namespace Scene

DC_END_DREEMCHEST