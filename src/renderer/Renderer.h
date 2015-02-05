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

#ifndef __DC_Renderer_H__
#define __DC_Renderer_H__

#include "../Common.h"

DC_BEGIN_DREEMCHEST

namespace renderer {

    DECLARE_LOG( log )

    class RenderView;
    class Hal;
    class BatchRenderer;
    class BlendState;
    class RasterizerState;
    class DepthStencilState;
    class RenderResource;
        class Shader;
        class VertexBuffer;
        class IndexBuffer;
        class VertexDeclaration;
        class Texture;
            class Texture1D;
            class Texture2D;
                class RenderTarget;
            class Texture3D;
            class TextureCube;

    // ** enum Renderer
    //! Hardware rendering API.
    enum Renderer {
        OpenGL,
        Direct3D
    };

    // ** enum ClearMask
    //! Clear mask bits that are passed to Hal::clear method.
    enum ClearMask {
        ClearColor		= BIT( 0 ),                                 //!< Clear color buffer.
        ClearDepth		= BIT( 1 ),                                 //!< Clear depth buffer.
        ClearStencil	= BIT( 2 ),                                 //!< Clear stencil buffer.
        ClearAll		= ClearColor | ClearDepth | ClearStencil    //!< Clear all buffers.
    };

    // ** enum StencilAction
    enum StencilAction {
        StencilDecSaturate,
        StencilDecWrap,
        StencilIncSaturate,
        StencilIncWrap,
        StencilInvert,
        StencilKeep,
        StencilSet,
        StencilZero,
    };

    // ** enum BlendFactor
    enum BlendFactor {
        BlendDisabled,
        BlendZero,
        BlendOne,
        BlendSrcColor,
        BlendInvSrcColor,
        BlendDstColor,
        BlendInvDstColor,
        BlendSrcAlpha,
        BlendInvSrcAlpha,
        BlendDstAlpha,
        BlendInvDstAlpha,
    };

    // ** enum ColorMask
    //! Color mask bits used as an argument for Hal::setColorMask method.
    enum ColorMask {
        ColorMaskRed	= BIT( 0 ),                                                         //!< Render only RED channel.
        ColorMaskGreen	= BIT( 1 ),                                                         //!< Render only GREEN channel.
        ColorMaskBlue	= BIT( 2 ),                                                         //!< Render only BLUE channel.
        ColorMaskAlpha	= BIT( 3 ),                                                         //!< Render only ALPHA channel.
        ColorMaskAll	= ColorMaskRed | ColorMaskGreen | ColorMaskBlue | ColorMaskAlpha,   //!< Render all color channels.
        ColorMaskRgb	= ColorMaskRed | ColorMaskGreen | ColorMaskBlue,                    //!< Render RED, GREEN and BLUE color channels.
    };

    // ** enum TextureWrap
    //! Texture wrapping control.
    enum TextureWrap {
        Clamp,              //!< Clamp texture.
        Repeat              //!< Repeat texture.
    };

    // ** enum TextureFilter
    //! Texture filtering.
    enum TextureFilter {
        FilterNearest,      //!< The texture filtering is disabled.
        FilterLinear,       //!< Interpolate texels.
        FilterMipNearest,   //!< Use texture mip-maps.
        FilterMipLinear,    //!< Use texture mip-map and interpolate between the mip levels.
    };

    // ** enum RenderStateValue
    enum RenderStateValue {
        RSValueDefault = -1,
        RSValueNotSet  = -2
    };

    // ** enum PolygonMode
    enum PolygonMode {
        PolygonWire = 0,
        PolygonFill
    };

    // ** enum TriangleFace
    //! Triangle face enum (used in Hal::setCulling, Hal::setStencilAction, etc.)
    enum TriangleFace {
        TriangleFaceFront,          //!< Front face.
        TriangleFaceBack,           //!< Back face.
        TriangleFaceFrontAndBack,   //!< Front and back face.
        TriangleFaceNone            //!< Nothing.
    };

    // ** enum Compare
    //! Comparison function used for alpha, depth and stencil tests.
    enum Compare {
        Always,                     //!< Always passes.
        Equal,                      //!< Passes if the incoming value is equal to the stored value.
        Greater,                    //!< Passes if the incoming value is greater than the stored value.
        GreaterEqual,               //!< Passes if the incoming value is greater than or equal to the stored value.
        Less,                       //!< Passes if the incoming value is less than the stored value.
        LessEqual,                  //!< Passes if the incoming value is less than or equal to the stored value.
        Never,                      //!< Never passes.
        NotEqual,                   //!< Passes if the incoming value is not equal to the stored value.

        CompareDisabled             //!< The test is disabled.
    };

    // ** enum Transform
    enum Transform {
        TransformModel,
        TransformView,
        TransformProjection,

        TransformTotal
    };

    // ** enum PixelFormat
    //! Supported pixel format for textures and render targets.
    enum PixelFormat {
        PixelAlpha8,    //!< 8-bit alpha.
        PixelRgb8,      //!< 8-bit RGB color.
        PixelRgba8,     //!< 8-bit RGBA color.

        // ** Compressed
        // ** TODO: add ETC/ATITC/S3TC formats
        PixelDxtc1,     //!< Compressed DXT format with 8 bytes per block with 1-bit alpha.
        PixelDxtc3,     //!< Compressed DXT format with 16 bytes per block.
        PixelDxtc5,     //!< Compressed DXT format with 16 bytes per block.
        PixelPvrtc2,    //!< Compressed PVR format with 2-bits per pixel.
        PixelPvrtc4,    //!< Compressed PVR format with 4-bits per pixel.

        // ** Floating point textures
        PixelR16F,      //!< 16-bit floating point format with a single color channel.
        PixelRg16F,     //!< 16-bit floating point format with two color channels.
        PixelRgba16F,   //!< 16-bit floating point RGBA color.
        PixelR32F,      //!< 32-bit floating point format with a single color channel.
        PixelRg32F,     //!< 32-bit floating point RGBA color.
        PixelRgba32F,

        // ** Depth-stencil formats
        PixelD24X8,     //!< 32-bit depth stencil surface with 24-bit depth value and no stencil.
        PixelD24S8,     //!< 32-bit depth stencil surface with 24-bit depth value and 8-bit stencil.
    };

    // ** enum PrimitiveType
    //! A list of available primitive types.
    enum PrimitiveType {
        PrimLines,          //!< Render lines.
        PrimLineStrip,      //!< Render line strip.
        PrimTriangles,      //!< Render triangles.
        PrimTriangleStrip,  //!< Render triangle strip.
        PrimTriangleFan,    //!< Render triangle fan.
        PrimPoints,         //!< Render points.

        PrimTotal           //!< Total primitive types available.
    };

} // namespace renderer

    typedef renderer::Hal*                  dcHal;
    typedef renderer::BatchRenderer*        dcBatchRenderer;
    typedef renderer::Shader*               dcShader;
    typedef renderer::Texture*              dcTexture;
    typedef renderer::Texture1D*            dcTexture1D;
    typedef renderer::Texture2D*            dcTexture2D;
    typedef renderer::Texture3D*            dcTexture3D;
    typedef renderer::TextureCube*          dcTextureCube;
    typedef renderer::VertexBuffer*         dcVertexBuffer;
    typedef renderer::IndexBuffer*          dcIndexBuffer;
    typedef renderer::RenderTarget*         dcRenderTarget;
    typedef renderer::VertexDeclaration*    dcVertexDeclaration;
    typedef renderer::BlendState*           dcBlendState;
    typedef renderer::DepthStencilState*    dcDepthStencilState;
    typedef renderer::RasterizerState*      dcRasterizerState;

DC_END_DREEMCHEST


#ifndef DC_BUILD_LIBRARY
    #include "Hal.h"
    #include "RenderState.h"
    #include "BatchRenderer.h"
#endif

#endif
