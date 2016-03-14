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

#ifndef __DC_Scene_RasterizationOptions_H__
#define __DC_Scene_RasterizationOptions_H__

#include "../RenderingContext.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Rasterization options used by a rendering mode.
    struct RasterizationOptions {
        u8                          modes;      //!< Bitmask of rendering modes that are eligible to use this rasterization options.
        struct {
            Renderer::BlendFactor   src;        //!< Source blend factor.
            Renderer::BlendFactor   dst;        //!< Destination blend factor.
        } blend;

        struct {
            Renderer::Compare		function;   //!< Alpha test function to be used.
			f32						reference;  //!< Reference value for an alpha test.
        } alpha;

        struct {
            Renderer::Compare	    function;   //!< Depth test function to be used.          
  			bool					write;	    //!< Is writing to a depth buffer enabled or not.
        } depth;

		//! Returns rasterization options with modified z-write option.
		RasterizationOptions		overrideZWrite( bool value ) const;

		//! Returns rasterization options with modified blending.
		RasterizationOptions		overrideBlending( Renderer::BlendFactor src, Renderer::BlendFactor dst ) const;

        //! Returns reasterization options with modified alpha test settings.
        RasterizationOptions        overrideAlphaTest( Renderer::Compare function, f32 reference ) const;

        //! Returns the default rasterization options.
        static RasterizationOptions basic( void );

        //! Returns default opaque rasterization options.
        static RasterizationOptions opaque( void );

        //! Returns default cutout rasterization options.
        static RasterizationOptions cutout( void );

        //! Returns default translucent rasterization options.
        static RasterizationOptions translucent( void );

        //! Returns default additive rasterization options.
        static RasterizationOptions additive( void );
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RasterizationOptions_H__    */