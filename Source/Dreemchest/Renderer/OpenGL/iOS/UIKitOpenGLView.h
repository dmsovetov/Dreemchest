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

#ifndef __DC_Renderer_UIKitOpenGLView_H__
#define __DC_Renderer_UIKitOpenGLView_H__

#include <UIKit/UIKit.h>
#include "../OpenGL2.h"

DC_BEGIN_DREEMCHEST
namespace Renderer
{
    class RenderView;
}
DC_END_DREEMCHEST

DC_USE_DREEMCHEST

// ** UIKitOpenGLView
@interface UIKitOpenGLView : UIView
{
    @private

        UIWindow*               m_window;
        Renderer::RenderView*   m_owner;

        BOOL                    m_animating;
        NSInteger               m_animationFrameInterval;
        id                      m_displayLink;

        EAGLContext*            m_context;
        GLint                   m_backingWidth;
        GLint                   m_backingHeight;
        GLuint                  m_defaultFramebuffer, m_colorRenderbuffer, m_depthBuffer;
    }

    - ( id )    initWithWindow: ( UIWindow* )window options:( unsigned int )options;
    - ( void )  setOwner: ( Renderer::RenderView* )view;
    - ( void )  beginFrame;
    - ( void )  endFrame;
    - ( BOOL )  makeCurrent;
@end

#endif /*   !defined( __DC_Renderer_UIKitOpenGLView_H__ ) */
