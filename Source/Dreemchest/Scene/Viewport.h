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

#ifndef __DC_Scene_Viewport_H__
#define __DC_Scene_Viewport_H__

#include "Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Scene viewport interface.
	class AbstractViewport : public RefCounted {
	public:

		virtual						~AbstractViewport( void ) {}

		//! Returns the render target rect.
		Rect						rect( void ) const;

		//! Returns the render target width.
		virtual s32					width( void ) const NIMBLE_ABSTRACT;

		//! Returns the render target height.
		virtual s32					height( void ) const NIMBLE_ABSTRACT;
	};

	//! WindowViewport is used for attaching a scene viewport to a window instance.
	class WindowViewport : public AbstractViewport {
	public:

		//! Returns the window width.
		virtual s32					width( void ) const NIMBLE_OVERRIDE;

		//! Returns the window height.
		virtual s32					height( void ) const NIMBLE_OVERRIDE;

		//! Creates the WindowView instance.
		static WindowViewportPtr    create( const Platform::WindowWPtr& window );

	private:

									//! Constructs the WindowViewport instance.
									WindowViewport( const Platform::WindowWPtr& window );

	private:

		Platform::WindowWPtr		m_window;	//!< The output window.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Viewport_H__    */