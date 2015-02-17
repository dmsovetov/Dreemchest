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

#include "iOSOpenGLView.h"

DC_BEGIN_DREEMCHEST

namespace renderer {

// ** createOpenGLView
OpenGLView* createOpenGLView( void* window, PixelFormat depthStencil )
{
    iOSOpenGLView* view = DC_NEW iOSOpenGLView;
    view->initialize( reinterpret_cast<UIWindow*>( window ), depthStencil, nil );
    log::verbose( "iOS OpenGL viewport created\n" );
    return view;
}

// ** iOSOpenGLView::~iOSOpenGLView
iOSOpenGLView::~iOSOpenGLView( void )
{
    [m_view dealloc];
}

// ** iOSOpenGLView::initialize
bool iOSOpenGLView::initialize( UIWindow* window, PixelFormat depthStencil, id delegate )
{
    m_view = [[UIKitOpenGLView alloc] initWithWindow: window depthStencil:depthStencil];
    [window setContentView: m_view];
    return true;
}

// ** iOSOpenGLView::beginFrame
void iOSOpenGLView::beginFrame( void )
{
    [m_view beginFrame];
}

// ** iOSOpenGLView::endFrame
void iOSOpenGLView::endFrame( void )
{
    [m_view endFrame];
}

// ** iOSOpenGLView::makeCurrent
void iOSOpenGLView::makeCurrent( void )
{
    [m_view makeCurrent];
}

} // namespace renderer

DC_END_DREEMCHEST
