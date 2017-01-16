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

#include "OpenGLFramebufferCache.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** OpenGLFramebufferCache::OpenGLFramebufferCache
OpenGLFramebufferCache::OpenGLFramebufferCache()
{
    m_items.push(Item());
}
    
// ** OpenGLFramebufferCache::~OpenGLFramebufferCache
OpenGLFramebufferCache::~OpenGLFramebufferCache()
{
    for (s32 i = 0, n = m_items.count(); i < n; i++)
    {
        free(i);
    }
}
    
// ** OpenGLFramebufferCache::identifier
GLuint OpenGLFramebufferCache::identifier(s32 index) const
{
    return m_items[index].id;
}

// ** OpenGLFramebufferCache::acquire
s32 OpenGLFramebufferCache::acquire(u16 width, u16 height, u32 options)
{
    for (s32 i = 0, n = m_items.count(); i < n; i++)
    {
        if (!m_items[i].acquired && m_items[i].width == width && m_items[i].height == height && m_items[i].options == options)
        {
            m_items[i].acquired = true;
            return i;
        }
    }
    
    LogVerbose("opengl", "allocating a framebuffer of size %dx%d\n", width, height);
    
    Item item;
    item.id       = OpenGL2::Framebuffer::create();
    item.width    = width;
    item.height   = height;
    item.options  = options;
    item.acquired = true;
    
    s32 depthBits = Private::depthBitsFromOptions(options);
    if (depthBits)
    {
        item.depth = OpenGL2::Framebuffer::renderbuffer(item.id, width, height, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT);
    }
    
    s32 index = m_items.push(item);
    return index;
}

// ** OpenGLFramebufferCache::release
void OpenGLFramebufferCache::release(s32 index)
{
    m_items[index].acquired = false;
}

// ** OpenGLFramebufferCache::free
void OpenGLFramebufferCache::free(s32 index)
{
    Item& item = m_items[index];
    NIMBLE_ABORT_IF(item.id, "invalid framebuffer object index");
    
    if (item.depth)
    {
        OpenGL2::Framebuffer::destroyRenderBuffer(item.depth);
        item.depth = 0;
    }
    
    if (item.id)
    {
        OpenGL2::Framebuffer::destroy(item.id);
        item.id = 0;
    }
}

} // namespace Renderer

DC_END_DREEMCHEST
