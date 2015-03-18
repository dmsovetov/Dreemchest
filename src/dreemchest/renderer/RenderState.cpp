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

#include    "RenderState.h"

DC_BEGIN_DREEMCHEST

namespace renderer
{

// ------------------------------- RasterizerState ------------------------------- //

// ** RasterizerState::RasterizerState
RasterizerState::RasterizerState( void ) {
    setAs( RSValueNotSet );
}

// ** RasterizerState::setAs
void RasterizerState::setAs( RenderStateValue value )
{
    m_fillMode   = value;
    m_cullMode   = value;
    m_alphaFunc  = value;
    m_alphaRef   = (int)value;
    m_scissor    = value;
    m_red        = value;
    m_green      = value;
    m_blue       = value;
    m_alpha      = value;
    m_depth      = value;
}

// ** RasterizerState::setCullMode
void RasterizerState::setCullMode( TriangleFace mode )
{
    m_cullMode = mode;
}

// ** RasterizerState::setFillMode
void RasterizerState::setFillMode( PolygonMode mode )
{
    m_fillMode = mode;
}

// ** RasterizerState::setCullMode
void RasterizerState::setAlphaTest( Compare func, float ref )
{
    m_alphaFunc = func;
    m_alphaRef  = ref;
}

// ** RasterizerState::setCullMode
void RasterizerState::setZWrite( bool write )
{
    m_depth = write ? 1 : 0;
}

// ** RasterizerState::setCullMode
void RasterizerState::setScissorEnable( bool enable )
{
    m_scissor = enable ? 1 : 0;
}

// ** RasterizerState::setCullMode
void RasterizerState::setColorWrite( bool red, bool green, bool blue, bool alpha )
{
    m_red   = red   ? 1 : 0;
    m_green = green ? 1 : 0;
    m_blue  = blue  ? 1 : 0;
    m_alpha = alpha ? 1 : 0;
}

// ------------------------------- BlendState ------------------------------- //


// ** BlendState::BlendState
BlendState::BlendState( void ) {
    setAs( RSValueNotSet );
}

// ** BlendState::setAs
void BlendState::setAs( RenderStateValue value )
{
    m_src   = value;
    m_dst   = value;
    m_op    = value;
}

// ** BlendState::setCullMode
void BlendState::setBlendFunc( BlendFactor src, BlendFactor dst )
{
    m_src = src;
    m_dst = dst;
}

// ** BlendState::setBlendEnabled
void BlendState::setBlendEnabled( bool enabled )
{
    m_blend = enabled ? 1 : 0;
}

// ------------------------------- DepthStencilState ------------------------------- //

// ** DepthStencilState::DepthStencilState(
DepthStencilState::DepthStencilState( void ) {
    setAs( RSValueNotSet );
}

// ** DepthStencilState::setAs
void DepthStencilState::setAs( RenderStateValue value )
{
    m_depthFunc     = value;
    m_stencilFail   = value, m_stencilZFail = value, m_stencilZPass = value;
    m_stencilFunc   = value;
    m_stencilValue  = value, m_stencilMask = value;
    m_stencilEnable = value;
}

// ** DepthStencilState::setDepthFunc
void DepthStencilState::setDepthFunc( Compare func )
{
    m_depthFunc = func;
}

// ** DepthStencilState::setStencilFunc
void DepthStencilState::setStencilFunc( Compare func, int value, int mask )
{
    m_stencilFunc   = func;
    m_stencilValue  = value;
    m_stencilMask   = mask;
}

// ** DepthStencilState::setStencilOp
void DepthStencilState::setStencilOp( StencilAction fail, StencilAction zFail, StencilAction zPass )
{
    m_stencilFail     = fail;
    m_stencilZFail    = zFail;
    m_stencilZPass    = zPass;
}

// ** DepthStencilState::setStencilMask
void DepthStencilState::setStencilMask( int mask )
{
    m_stencilMask = mask;
}

// ** DepthStencilState::setStencilEnabled
void DepthStencilState::setStencilEnabled( bool enabled )
{
    m_stencilEnable = enabled ? 1 : 0;
}
    
} // namespace renderer

DC_END_DREEMCHEST