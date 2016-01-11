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

#include "Rvm.h"
#include "ShaderCache.h"

#include "../Assets/Mesh.h"
#include "../Assets/Image.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// Open Renderer namespace here
using namespace Renderer;

// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( RvmPtr rvm, ShaderCachePtr shaders, HalPtr hal, Renderer2DPtr renderer )
	: m_rvm( rvm ), m_shaders( shaders ), m_hal( hal ), m_renderer( renderer )
{
	m_rvm->setRenderingContext( this );
}

// ** RenderingContext::create
RenderingContextPtr RenderingContext::create( const HalPtr& hal )
{
	// Create rendering context.
	Renderer2DPtr  renderer2d = Renderer2D::create( hal, 1024 );
	RvmPtr		   rvm( DC_NEW Rvm( hal, 8000 ) );
	ShaderCachePtr shaders( DC_NEW ShaderCache( "D:\\BFG9000\\externals\\src\\dreemchest\\src\\dreemchest\\scene\\Rendering\\Shaders\\", hal ) );

	return RenderingContextPtr( DC_NEW RenderingContext( rvm, shaders, hal, renderer2d ) );
}

// ** RenderingContext::rvm
RvmPtr RenderingContext::rvm( void ) const
{
	return m_rvm;
}

// ** RenderingContext::shaders
ShaderCachePtr RenderingContext::shaders( void ) const
{
	return m_shaders;
}

// ** RenderingContext::hal
HalPtr RenderingContext::hal( void ) const
{
	return m_hal;
}

// ** RenderingContext::renderer
Renderer2DPtr RenderingContext::renderer( void ) const
{
	return m_renderer;
}

} // namespace Scene

DC_END_DREEMCHEST