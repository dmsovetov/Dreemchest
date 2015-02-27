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

#ifndef __DC_Renderer_WindowsOpenGLView_H__
#define __DC_Renderer_WindowsOpenGLView_H__

#include "../OpenGLHal.h"

#include <windows.h>

DC_BEGIN_DREEMCHEST

namespace renderer {

    // ** class WindowsOpenGLView
    class WindowsOpenGLView : public OpenGLView {
    public:

        virtual             ~WindowsOpenGLView( void );

        // ** IView
        virtual bool        makeCurrent( void );
        virtual bool        beginFrame( void );
        virtual void        endFrame( void );

        // ** WindowsOpenGLView
        bool                initialize( HWND window, PixelFormat depthStencil );

    private:

		//! Parent window handle.
		HWND				m_window;

		//! Device context handle.
        HDC					m_deviceContext;

		//! OpenGL rendering context.
		HGLRC				m_renderingContext;
    };

} // namespace renderer

DC_END_DREEMCHEST

#endif /*   !defined(__DC_Renderer_WindowsOpenGLView_H__) */
