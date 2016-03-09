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

#ifndef		__DC_R_RenderState_H__
#define		__DC_R_RenderState_H__

#include	"Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer {

    // ** class RasterizerState
    class RasterizerState {
    public:

        int             m_fillMode;
        int             m_cullMode;
        int             m_alphaFunc;
        float           m_alphaRef;
        int             m_scissor;
        int             m_red, m_green, m_blue, m_alpha, m_depth;

    public:

                        RasterizerState( void );

        void            setAs( RenderStateValue value );
        void            setCullMode( TriangleFace face );
        void            setFillMode( PolygonMode mode );
        void            setAlphaTest( Compare func, float ref );
        void            setZWrite( bool write );
        void            setScissorEnable( bool enable );
        void            setColorWrite( bool red, bool green, bool blue, bool alpha );
    };

    // ** class BlendState
    class BlendState {
    public:

        int            m_src, m_dst;
        int            m_op;
        int            m_blend;

    public:

                        BlendState( void );

        void            setAs( RenderStateValue value );
        void            setBlendEnabled( bool enabled );
        void            setBlendFunc( BlendFactor src, BlendFactor dst );
    };

    // ** class DepthStencilState
    class DepthStencilState {
    public:

        int            m_depthFunc;
        int            m_stencilFail, m_stencilZFail, m_stencilZPass;
        int            m_stencilFunc;
        int            m_stencilValue, m_stencilMask;
        int            m_stencilEnable;

    public:

                        DepthStencilState( void );

        void            setAs( RenderStateValue value );
        void            setDepthFunc( Compare func );
        void            setStencilFunc( Compare func, int value, int mask );
        void            setStencilOp( StencilAction fail, StencilAction zFail, StencilAction zPass );
        void            setStencilMask( int mask );
        void            setStencilEnabled( bool enabled );
    };

} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_R_RenderState_H__ */