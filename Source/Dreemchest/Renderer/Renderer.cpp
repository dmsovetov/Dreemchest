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
#include "VertexFormat.h"
#include "../Platform/Application.h"

DC_BEGIN_DREEMCHEST

namespace Renderer {
    
// ---------------------------------------------------------------- RenderingApplicationDelegate ---------------------------------------------------------------- //
    
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
    
// ** bytesPerBlock
u32 bytesPerBlock(PixelFormat format)
{
    switch (format)
    {
        case PixelDxtc1:    return 8;
        case PixelDxtc3:    return 16;
        case PixelDxtc5:    return 16;
        case PixelPvrtc2:   return 32;
        case PixelPvrtc4:   return 16;
        default:            NIMBLE_NOT_IMPLEMENTED
    }
    
    return 0;
}

// ** bytesPerPixel
u32 bytesPerPixel(PixelFormat format)
{
    switch (format)
    {
        case PixelLuminance8:   return 1;
        case PixelRgb8:         return 3;
        case PixelRgba8:        return 4;
        case PixelR16F:         return 2;
        case PixelRg16F:        return 4;
        case PixelRgba16F:      return 8;
        case PixelR32F:         return 4;
        case PixelRg32F:        return 8;
        case PixelRgba32F:      return 16;
        default:                NIMBLE_NOT_IMPLEMENTED
    }
    
    return 0;
}

// ** bytesPerMip
u32 bytesPerMip(PixelFormat format, u16 width, u16 height)
{
    u32 size = 0;
    
    switch (format)
    {
        case PixelDxtc1:
        case PixelDxtc3:
        case PixelDxtc5:
            size = ((width + 3) >> 2) * ((height + 3) >> 2) * bytesPerBlock(format);
            break;
            
        case PixelPvrtc2:
            size = (width / 8) * (height / 4) * ((bytesPerBlock(format) * 4) / 8);
            break;
            
        case PixelPvrtc4:
            size = (width / 4) * (height / 4) * ((bytesPerBlock(format) * 4) / 8);
            break;
            
        default:
            size = width * height * bytesPerPixel(format);
    }
    
    return size;
}

// ** bytesPerMipChain
u32 bytesPerMipChain(PixelFormat format, u16 width, u16 height, u16 mipLevels)
{
    u32 size = 0;
    
    for (s32 i = 0; i < mipLevels; i++)
    {
        size += bytesPerMip(format, width, height);
        width = width >> 1;
        height = height >> 1;
    }
    
    return size;
}
    
} // namespace Renderer

DC_END_DREEMCHEST
