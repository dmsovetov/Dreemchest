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

#ifndef __DC_Renderer_OpenGLFramebufferCache_H__
#define __DC_Renderer_OpenGLFramebufferCache_H__

#include "OpenGL2.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
    //! Allocates and holds OpenGL framebuffer objects.
    class OpenGLFramebufferCache
    {
    public:
        
                            //! Constructs a OpenGLFramebufferCache instance.
                            OpenGLFramebufferCache();
                            ~OpenGLFramebufferCache();
        
        //! Returns a framebuffer OpenGL identifier.
        GLuint              identifier(s32 index) const;
        
        //! Searches for a framebuffer and marks it a used.
        s32                 acquire(u16 width, u16 height, u32 options);
        
        //! Releases a framebuffer.
        void                release(s32 index);
        
    private:
        
        //! Completely frees a framebuffer object.
        void                free(s32 index);
        
    private:
        
        //! A framebuffer cache item.
        struct Item
        {
            GLuint          id;             //!< A framebuffer id.
            GLuint          depth;          //!< An attached depth renderbuffer.
            u16             width;          //!< A framebuffer width.
            u16             height;         //!< A framebuffer height.
            u32             options;        //!< A framebuffer options.
            bool            acquired;       //!< Indicates that a framebuffer is in use now.
        };
        
        FixedArray<Item>    m_items;        //!< An array of cached framebuffers.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_OpenGLFramebufferCache_H__    */
