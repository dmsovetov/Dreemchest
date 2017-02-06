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

#include "EmscriptenOpenGLView.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** createOpenGLView
RenderViewPtr createOpenGLView(void* window, u32 options)
{
    EmscriptenOpenGLView* view = DC_NEW EmscriptenOpenGLView;
    view->initialize(window, options);
    LogVerbose( "opengl", "%s", "Emscripten OpenGL viewport created\n" );
    return view;
}

// ** EmscriptenOpenGLView::~EmscriptenOpenGLView
EmscriptenOpenGLView::~EmscriptenOpenGLView( void )
{

}

// ** EmscriptenOpenGLView::initialize
bool EmscriptenOpenGLView::initialize(void* window, u32 options)
{
    return false;
}

// ** EmscriptenOpenGLView::beginFrame
bool EmscriptenOpenGLView::beginFrame( void )
{
	return makeCurrent();
}

// ** EmscriptenOpenGLView::endFrame
void EmscriptenOpenGLView::endFrame(bool wait)
{
    if (wait)
    {
        glFinish();
    }
    
    // Swap here
}

// ** EmscriptenOpenGLView::makeCurrent
bool EmscriptenOpenGLView::makeCurrent( void )
{
    return false;
}

} // namespace Renderer

DC_END_DREEMCHEST
