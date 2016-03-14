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

#include "RasterizationOptions.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RasterizationOptions::overrideZWrite
RasterizationOptions RasterizationOptions::overrideZWrite( bool value ) const
{
	RasterizationOptions result = *this;
	result.depth.write = value;
	return result;
}

// ** RasterizationOptions::overrideBlending
RasterizationOptions RasterizationOptions::overrideBlending( Renderer::BlendFactor src, Renderer::BlendFactor dst ) const
{
	RasterizationOptions result = *this;
	result.blend.src = src;
	result.blend.dst = dst;
	return result;
}

// ** RasterizationOptions::overrideAlphaTest
RasterizationOptions RasterizationOptions::overrideAlphaTest( Renderer::Compare function, f32 reference ) const
{
	RasterizationOptions result = *this;
	result.alpha.function  = function;
	result.alpha.reference = reference;
	return result;
}

// ** RasterizationOptions::basic
RasterizationOptions RasterizationOptions::basic( void )
{
    RasterizationOptions options;

    options.blend.src       = Renderer::BlendDisabled;
    options.blend.dst       = Renderer::BlendDisabled;
    options.alpha.function  = Renderer::CompareDisabled;
    options.alpha.reference = 0.0f;
    options.depth.write     = true;
    options.depth.function  = Renderer::LessEqual;

    return options;
}

// ** Rvm::RasterizationOptions::opaque
RasterizationOptions RasterizationOptions::opaque( void )
{
    return basic();
}

// ** Rvm::RasterizationOptions::cutout
RasterizationOptions RasterizationOptions::cutout( void )
{
    return basic()
            .overrideAlphaTest( Renderer::Greater, 0.5f );
}

// ** Rvm::RasterizationOptions::translucent
RasterizationOptions RasterizationOptions::translucent( void )
{
    return basic()
            .overrideBlending( Renderer::BlendSrcAlpha, Renderer::BlendInvSrcAlpha )
            .overrideZWrite( false );
}

// ** RasterizationOptions::additive
RasterizationOptions RasterizationOptions::additive( void )
{
    return basic()
            .overrideBlending( Renderer::BlendOne, Renderer::BlendOne )
            .overrideZWrite( false );
}

} // namespace Scene

DC_END_DREEMCHEST