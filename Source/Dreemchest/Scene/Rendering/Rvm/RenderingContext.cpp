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

#include "RenderingContext.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( Renderer::HalWPtr hal )
    : m_hal( hal )
{
}

// ** RenderingContext::hal
Renderer::HalWPtr RenderingContext::hal( void ) const
{
    return m_hal;
}

// ** RenderingContext::internRenderTarget
s32 RenderingContext::internRenderTarget( RenderTargetPtr renderTarget )
{
    return m_renderTargets.add( renderTarget );
}

// ** RenderingContext::renderTarget
const RenderTargetPtr& RenderingContext::renderTarget( s32 identifier ) const
{
    return m_renderTargets.resolve( identifier );
}

// ** RenderingContext::internVertexBuffer
s32 RenderingContext::internVertexBuffer( Renderer::VertexBufferPtr vertexBuffer )
{
    return m_vertexBuffers.add( vertexBuffer );
}
        
// ** RenderingContext::vertexBuffer
const Renderer::VertexBufferPtr& RenderingContext::vertexBuffer( s32 identifier ) const
{
    return m_vertexBuffers.resolve( identifier );
}

// ** RenderingContext::internIndexBuffer
s32 RenderingContext::internIndexBuffer( Renderer::IndexBufferPtr indexBuffer )
{
    return m_indexBuffers.add( indexBuffer );
}
        
// ** RenderingContext::indexBuffer
const Renderer::IndexBufferPtr& RenderingContext::indexBuffer( s32 identifier ) const
{
    return m_indexBuffers.resolve( identifier );
}

// ** RenderingContext::internConstantBuffer
s32 RenderingContext::internConstantBuffer( Renderer::ConstantBufferPtr constantBuffer )
{
    return m_constantBuffers.add( constantBuffer );
}

// ** RenderingContext::constantBuffer
const Renderer::ConstantBufferPtr& RenderingContext::constantBuffer( s32 identifier ) const
{
    return m_constantBuffers.resolve( identifier );
}

// ** RenderingContext::internInputLayout
s32 RenderingContext::internInputLayout( Renderer::InputLayoutPtr inputLayout )
{
    return m_inputLayouts.add( inputLayout );
}

// ** RenderingContext::inputLayout
const Renderer::InputLayoutPtr& RenderingContext::inputLayout( s32 identifier ) const
{
    return m_inputLayouts.resolve( identifier );
}

// ** RenderingContext::internTexture
s32 RenderingContext::internTexture( Renderer::TexturePtr texture )
{
    return m_textures.add( texture );
}
        
// ** RenderingContext::texture
const Renderer::TexturePtr& RenderingContext::texture( s32 identifier ) const
{
    return m_textures.resolve( identifier );
}

// ** RenderingContext::internShader
s32 RenderingContext::internShader( UbershaderPtr shader )
{
    return m_shaders.add( shader );
}
        
// ** RenderingContext::shader
const UbershaderPtr& RenderingContext::shader( s32 identifier ) const
{
    return m_shaders.resolve( identifier );
}

// ** RenderingContext::create
RenderingContextPtr RenderingContext::create( Renderer::HalWPtr hal )
{
    return DC_NEW RenderingContext( hal );
}

} // namespace Scene

DC_END_DREEMCHEST