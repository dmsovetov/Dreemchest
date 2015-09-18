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

#ifndef __DC_Scene_RenderingContext_H__
#define __DC_Scene_RenderingContext_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Rendering context.
	class RenderingContext : public RefCounted {
	public:

								//! Constructs the RenderingContext instance.
								RenderingContext( RvmPtr rvm, ShaderCachePtr shaders, Renderer::HalPtr hal, Renderer::Renderer2DPtr renderer )
									: m_rvm( rvm ), m_shaders( shaders ), m_hal( hal ), m_renderer( renderer ) {}

		//! Returns RVM.
		RvmPtr					rvm( void ) const { return m_rvm; }

		//! Returns shader cache.
		ShaderCachePtr			shaders( void ) const { return m_shaders; }

		//! Returns hal.
		Renderer::HalPtr		hal( void ) const { return m_hal; }

		//! Returns renderer.
		Renderer::Renderer2DPtr	renderer( void ) const { return m_renderer; }

	private:

		RvmPtr					m_rvm;		//!< Rendering virtual machine.
		ShaderCachePtr			m_shaders;	//!< Shaders cache.
		Renderer::HalPtr		m_hal;		//!< Rendering HAL.
		Renderer::Renderer2DPtr	m_renderer;	//!< Rendering interface.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContext_H__    */