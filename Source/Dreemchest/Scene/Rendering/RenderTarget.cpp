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

#include "RenderTarget.h"
#include "RenderAssets.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------ RenderTarget ------------------------------------------- //

// ** RenderTarget::calculateSplitRect
Rect RenderTarget::calculateSplitRect( u32 x, u32 y, u32 nx, u32 ny )
{
	// Calculate the viewport dimensions in NDC
	f32 width  = 1.0f / nx;
	f32 height = 1.0f / ny;

	// Calculate the NDC of a viewport
	Rect ndc = Rect( x * width, y * height, (x + 1) * width, (y + 1) * height );

	return ndc;
}

// ** RenderTarget::begin
void RenderTarget::begin( RenderingContextWPtr context ) const
{
}

// ** RenderTarget::end
void RenderTarget::end( RenderingContextWPtr context ) const
{
}

// ------------------------------------------ WindowTarget ------------------------------------------ //

// ** WindowTarget::WindowTarget
WindowTarget::WindowTarget( const Platform::WindowWPtr& window ) : m_window( window )
{
}

// ** WindowTarget::width
u32 WindowTarget::width( void ) const
{
	return m_window->width();
}

// ** WindowTarget::height
u32 WindowTarget::height( void ) const
{
	return m_window->height();
}

// ** WindowTarget::create
WindowTargetPtr WindowTarget::create( const Platform::WindowWPtr& window )
{
	return WindowTargetPtr( DC_NEW WindowTarget( window ) );
}

// --------------------------------------- TextureTarget --------------------------------------- //

// ** TextureTarget::TextureTarget
TextureTarget::TextureTarget( const Renderer::RenderTargetPtr& rt ) : m_rt( rt )
{
}

// ** TextureTarget::rt
Renderer::RenderTargetPtr TextureTarget::rt( void ) const
{
	return m_rt;
}

// ** TextureTarget::begin
void TextureTarget::begin( RenderingContextWPtr context ) const
{
	context->hal()->setRenderTarget( m_rt );
}

// ** TextureTarget::end
void TextureTarget::end( RenderingContextWPtr context ) const
{
	context->hal()->setRenderTarget( Renderer::RenderTargetPtr() );
}

// ** TextureTarget::width
u32 TextureTarget::width( void ) const
{
	return m_rt->width();
}

// ** TextureTarget::height
u32 TextureTarget::height( void ) const
{
	return m_rt->height();
}

// ** TextureTarget::create
RenderTargetPtr TextureTarget::create( const Renderer::RenderTargetPtr& rt )
{
	return TextureTargetPtr( DC_NEW TextureTarget( rt ) );
}

} // namespace Scene

DC_END_DREEMCHEST