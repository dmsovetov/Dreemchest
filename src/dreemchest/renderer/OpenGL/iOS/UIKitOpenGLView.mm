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

#include "UIKitOpenGLView.h"
#include "../../Renderer.h"

DC_USE_DREEMCHEST

// ** UIKitOpenGLView
@implementation UIKitOpenGLView

// You must implement this method
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

// ** initWithWindow
- ( id ) initWithWindow: ( UIWindow* )window depthStencil:( int )depthStencil;
{
    if( (self = [super initWithFrame: window.bounds]) )
    {
        CAEAGLLayer* eaglLayer = ( CAEAGLLayer* )self.layer;

        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

        m_window                 = window;
        m_animating              = FALSE;
        m_animationFrameInterval = 1;
        m_displayLink            = nil;

        if( ![self createContext] ) {
            [self release];
            return nil;
        }

        [self startAnimation];
    }

    return self;
}

// ** dealoc
- (void) dealloc
{
    if( m_defaultFramebuffer )
    {
        glDeleteFramebuffersOES( 1, &m_defaultFramebuffer );
        m_defaultFramebuffer = 0;
    }

    if( m_colorRenderbuffer )
    {
        glDeleteRenderbuffersOES( 1, &m_colorRenderbuffer );
        m_colorRenderbuffer = 0;
    }

    if( m_depthBuffer )
    {
        glDeleteRenderbuffersOES( 1, &m_depthBuffer );
        m_depthBuffer = 0;
    }

    if( [EAGLContext currentContext] == m_context ) {
        [EAGLContext setCurrentContext: nil];
    }

    [m_context release];
    m_context = nil;

    [super dealloc];
}

// ** makeCurrent
- ( BOOL ) makeCurrent
{
    return [EAGLContext setCurrentContext: m_context];
}

// ** beginFrame
- ( void ) beginFrame
{
    [self makeCurrent];

    glBindFramebufferOES( GL_FRAMEBUFFER_OES, m_defaultFramebuffer );
    glViewport( 0, 0, m_backingWidth, m_backingHeight );
}

// ** endFrame
- ( void ) endFrame
{
    glBindRenderbufferOES( GL_RENDERBUFFER_OES, m_colorRenderbuffer );
    [m_context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

// ** createContext
- ( BOOL ) createContext
{
    m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    if( !m_context ) {
        return NO;
    }

    [self makeCurrent];

    glGenFramebuffersOES( 1, &m_defaultFramebuffer );
    glGenRenderbuffersOES( 1, &m_colorRenderbuffer );
    glGenRenderbuffers( 1, &m_depthBuffer );
    glBindFramebufferOES( GL_FRAMEBUFFER_OES, m_defaultFramebuffer );
    glBindRenderbufferOES( GL_RENDERBUFFER_OES, m_colorRenderbuffer );
    glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, m_colorRenderbuffer );

    return YES;
}

// ** resizeFromLayer
- ( BOOL ) resizeFromLayer: ( CAEAGLLayer* )layer
{
    glBindRenderbufferOES( GL_RENDERBUFFER_OES, m_colorRenderbuffer );
    [m_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];

    glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &m_backingWidth );
    glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &m_backingHeight );

    if( glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES )
    {
        NSLog( @"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES( GL_FRAMEBUFFER_OES ) );
        return NO;
    }

    // ** Setup depth-stencil buffer
    glBindRenderbuffer( GL_RENDERBUFFER, m_depthBuffer );
//  glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, samplesToUse_, depthFormat_,backingWidth_, backingHeight_);
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, m_backingWidth, m_backingHeight );

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer );

    glBindRenderbuffer( GL_RENDERBUFFER, m_colorRenderbuffer );

    return YES;
}

// ** drawView
- (void) drawView: ( id )sender
{
    [self beginFrame];
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    [self endFrame];
    if( [m_window respondsToSelector:@selector(update)] ) {
        [m_window update];
    }
}

// ** layoutSubviews
- (void) layoutSubviews
{
    [self resizeFromLayer: (CAEAGLLayer*)self.layer];
    [self drawView:nil];
}

// ** startAnimation
- (void) startAnimation
{
    if( m_animating ) {
        return;
    }

    m_displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
    [m_displayLink setFrameInterval:m_animationFrameInterval];
    [m_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

    m_animating = TRUE;
}

// ** stopAnimation
- (void)stopAnimation
{
    if( !m_animating ) {
        return;
    }

    [m_displayLink invalidate];
    m_displayLink = nil;

    m_animating = FALSE;
}

@end