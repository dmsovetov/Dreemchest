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

#ifndef __DC_Scene_RenderTarget_H__
#define __DC_Scene_RenderTarget_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Scene rendering target.
	class RenderTarget : public RefCounted {
	public:

		virtual						~RenderTarget( void ) {}

		//! Returns the viewport split by it's coordinates.
		static Rect					calculateSplitRect( u32 x, u32 y, u32 nx, u32 ny );

		//! Returns the render target rect.
		Rect						rect( void ) const { return Rect( 0.0f, 0.0f, ( f32 )width(), ( f32 )height() ); }

		//! Returns the render target width.
		virtual u32					width( void ) const { return 0; }

		//! Returns the render target height.
		virtual u32					height( void ) const { return 0; }

		//! Begins rendering to this target.
		virtual void				begin( Renderer::HalWPtr hal ) const;

		//! Ends rendering to this target.
		virtual void				end( Renderer::HalWPtr hal ) const;
	};

	//! WindowTarget is used for rendering the scene to window.
	class WindowTarget : public RenderTarget {
	public:

		//! Returns the window width.
		virtual u32					width( void ) const DC_DECL_OVERRIDE;

		//! Returns the window height.
		virtual u32					height( void ) const DC_DECL_OVERRIDE;

		//! Creates the WindowView instance.
		static WindowTargetPtr		create( const Platform::WindowWPtr& window );

	private:

									//! Constructs the WindowTarget instance.
									WindowTarget( const Platform::WindowWPtr& window );

	private:

		Platform::WindowWPtr		m_window;	//!< The output window.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderTarget_H__    */