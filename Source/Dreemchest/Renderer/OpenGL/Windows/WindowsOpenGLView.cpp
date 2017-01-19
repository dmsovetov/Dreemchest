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

#include "WindowsOpenGLView.h"

DC_BEGIN_DREEMCHEST

Renderer::RenderViewWPtr renderView;

namespace Renderer {

// ** createOpenGLView
RenderViewPtr createOpenGLView(void* window, u32 options)
{
    WindowsOpenGLView* view = DC_NEW WindowsOpenGLView;
    view->initialize( reinterpret_cast<HWND>( window ), options );
    LogVerbose( "opengl", "Windows OpenGL viewport created\n" );
    renderView = view;
    return view;
}

// ** WindowsOpenGLView::~WindowsOpenGLView
WindowsOpenGLView::~WindowsOpenGLView( void )
{
	wglMakeCurrent( m_deviceContext, NULL );
	wglDeleteContext( m_renderingContext );
	ReleaseDC( m_window, m_deviceContext );
}

// ** WindowsOpenGLView::initialize
bool WindowsOpenGLView::initialize( HWND window, u32 options )
{
	PIXELFORMATDESCRIPTOR   pfd;
	int                     pixelFormat;

	int stencil = Private::stencilBitsFromOptions(options);
	int depth   = Private::depthBitsFromOptions(options);

	memset( &pfd, 0, sizeof( pfd ) );
	pfd.nSize           = sizeof( pfd );
	pfd.nVersion        = 1;
	pfd.dwFlags         = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType      = PFD_TYPE_RGBA;
	pfd.iLayerType      = PFD_MAIN_PLANE;
	pfd.cColorBits      = 24;
	pfd.cAlphaBits      = 8;
	pfd.cDepthBits      = depth;
	pfd.cStencilBits    = stencil;

	m_window		= window;
	m_deviceContext = GetDC( window );

	if( !m_deviceContext ) {
		return false;
	}

	if( (pixelFormat = ChoosePixelFormat( m_deviceContext, &pfd )) == 0 ) {
		return false;
	}

	if( SetPixelFormat( m_deviceContext, pixelFormat, &pfd ) == FALSE ) {
		return false;
	}

	m_renderingContext = wglCreateContext( m_deviceContext );

	if( !m_renderingContext ) {
		return false;
	}

	return makeCurrent();
}

// ** WindowsOpenGLView::beginFrame
bool WindowsOpenGLView::beginFrame( void )
{
	return makeCurrent();
}

// ** WindowsOpenGLView::endFrame
void WindowsOpenGLView::endFrame(bool wait)
{
    if (wait)
    {
        glFinish();
    }

    SwapBuffers( m_deviceContext );
}

// ** WindowsOpenGLView::makeCurrent
bool WindowsOpenGLView::makeCurrent( void )
{
	bool result = wglMakeCurrent( m_deviceContext, m_renderingContext ) != FALSE;

    if( !result ) {
        LogError( "opengl", "failed to make context current" );
        NIMBLE_BREAK
    }

	return result;
}

} // namespace Renderer

DC_END_DREEMCHEST
