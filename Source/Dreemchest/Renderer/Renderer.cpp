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

#include "Renderer.h"
#include "RenderingContext.h"

DC_BEGIN_DREEMCHEST

namespace Renderer {
    
// ** RenderingApplicationDelegate::initialize
bool RenderingApplicationDelegate::initialize(s32 width, s32 height)
{
    // First create a window
    if (!WindowedApplicationDelegate::initialize(width, height))
    {
        return false;
    }
    
    // Create a rendering view.
    RenderViewPtr view = createOpenGLView(m_window->handle(), Renderer::PixelD24S8);
    
    // Now create a rendering context that is attached to a created window instance
#if 0
    // Now create the main renderer interface called HAL (hardware abstraction layer).
    m_renderingContext = createDeprecatedRenderingContext(view, Hal::create(OpenGL, view));
#else
    m_renderingContext = createOpenGL2RenderingContext(view);
#endif
    
    return m_renderingContext.valid();
}

// ** RenderingApplicationDelegate::handleWindowUpdate
void RenderingApplicationDelegate::handleWindowUpdate(const Platform::Window::Update& e)
{
    handleRenderFrame(e);
}
    
} // namespace Renderer

DC_END_DREEMCHEST
