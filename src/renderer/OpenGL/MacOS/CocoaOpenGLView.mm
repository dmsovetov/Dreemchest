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

// ** initWithFrame
- ( id ) initWithFrame : ( NSRect )frameRect
{
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAStencilSize, 8,
        // Must specify the 3.2 Core Profile to use OpenGL 3.2
#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
#endif
        0
    };

    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

    if( !pf ) {
        NSLog(@"No OpenGL pixel format");
    }

    self = [super initWithFrame:frameRect pixelFormat:[pf autorelease]];
    [[self openGLContext] makeCurrentContext];

    return self;
}

// ** beginFrame
- ( void ) beginFrame
{
    [[self openGLContext] makeCurrentContext];
    CGLLockContext( ( CGLContextObj )[[self openGLContext] CGLContextObj] );
}

// ** endFrame
- ( void ) endFrame
{
    CGLFlushDrawable( ( CGLContextObj )[[self openGLContext] CGLContextObj] );
    CGLUnlockContext( ( CGLContextObj )[[self openGLContext] CGLContextObj] );
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
    [[self openGLContext] makeCurrentContext];

    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
}

// ** drawView
- ( void ) drawView
{
    // ?? Notify update here
}

// ** drawRect
- ( void ) drawRect : ( NSRect )rect {
    [self drawView];
}

// ** dealoc
- ( void ) dealloc
{
    // Release the display link
    CVDisplayLinkRelease( m_displayLink );
    [super dealloc];
}

// ** mouseDown
- ( void )mouseDown : ( NSEvent* )event
{
    printf( "CocoaOpenGLView::mouseDown\n" );
}

// ** mouseUp
- ( void )mouseUp : ( NSEvent* )event
{
    printf( "CocoaOpenGLView::mouseUp\n" );
}

// ** mouseMoved
- ( void )mouseMoved : ( NSEvent* )event
{
    printf( "CocoaOpenGLView::mouseMoved\n" );
}

// ** mouseDragged
- ( void )mouseDragged : ( NSEvent* )event
{
    printf( "CocoaOpenGLView::mouseDragged\n" );
    [self mouseMoved: event];
}

// ** keyDown
- ( void )keyDown : ( NSEvent* )event
{
    printf( "CocoaOpenGLView::keyDown\n" );
}

// ** keyUp
- ( void )keyUp : ( NSEvent* )event
{
    printf( "CocoaOpenGLView::keyUp\n" );
}

// ** acceptsFirstResponder
- ( BOOL )acceptsFirstResponder {
    return YES;
}

@end