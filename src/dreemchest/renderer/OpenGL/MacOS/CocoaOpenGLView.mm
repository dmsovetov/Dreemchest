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

#include "CocoaOpenGLView.h"
#include "../../Renderer.h"

DC_USE_DREEMCHEST

// ** CocoaOpenGLView
@implementation CocoaOpenGLView

// ** getFrameForTime
- ( CVReturn ) getFrameForTime : ( const CVTimeStamp* )outputTime
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    //    [self drawView];
    [self setNeedsDisplay: YES];
    [pool release];

    return kCVReturnSuccess;
}

// ** OnDisplay
static CVReturn OnDisplay( CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext )
{
    CVReturn result = [(CocoaOpenGLView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

// ** initWithWindow
-( id ) initWithWindow: ( NSWindow* )window depthStencil:( int )depthStencil;
{
    m_window = window;

    int depth   = 24;
    int stencil = 8;

    switch( depthStencil ) {
    case renderer::PixelD24S8:  depth   = 24;
                                stencil = 8;
                                break;
    case renderer::PixelD24X8:  depth   = 24;
                                stencil = 0;
                                break;
    }

    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize,   depth,
        NSOpenGLPFAStencilSize, stencil,
        0
    };

    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

    if( !pf ) {
        NSLog(@"No OpenGL pixel format");
    }

    self = [super initWithFrame:m_window.frame pixelFormat:[pf autorelease]];
    [[self openGLContext] makeCurrentContext];

    return self;
}

// ** beginFrame
- ( void ) beginFrame
{
    [self makeCurrent];
    CGLLockContext( ( CGLContextObj )[[self openGLContext] CGLContextObj] );
}

// ** endFrame
- ( void ) endFrame
{
    CGLFlushDrawable( ( CGLContextObj )[[self openGLContext] CGLContextObj] );
    CGLUnlockContext( ( CGLContextObj )[[self openGLContext] CGLContextObj] );
}

// ** makeCurrent
- ( BOOL ) makeCurrent
{
    [[self openGLContext] makeCurrentContext];
    return YES;
}

// ** prepareOpenGL
- ( void ) prepareOpenGL
{
    [super prepareOpenGL];
    [self initGL];

    CVDisplayLinkCreateWithActiveCGDisplays( &m_displayLink );
    CVDisplayLinkSetOutputCallback( m_displayLink, &OnDisplay, self );

    // ** Set the display link for the current renderer
    CGLContextObj		cglContext		= ( CGLContextObj )[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj	cglPixelFormat	= ( CGLPixelFormatObj )[[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext( m_displayLink, cglContext, cglPixelFormat );

    // ** Activate the display link
    CVDisplayLinkStart( m_displayLink );
}

// ** initGL
- ( void ) initGL
{
    [self makeCurrent];

    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
}

// ** drawRect
- ( void ) drawRect : ( NSRect )rect
{
    [m_window update];
}

// ** dealoc
- ( void ) dealloc
{
    // Release the display link
    CVDisplayLinkRelease( m_displayLink );
    [super dealloc];
}

@end