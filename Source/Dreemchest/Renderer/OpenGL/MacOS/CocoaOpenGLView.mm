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

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

DC_USE_DREEMCHEST

// ** CocoaOpenGLView
@implementation CocoaOpenGLView

// ** displayCallback
static CVReturn displayCallback( CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext )
{
    CocoaOpenGLView* view = reinterpret_cast<CocoaOpenGLView*>(displayLinkContext);
    [view renderForTime];
    return kCVReturnSuccess;
}

// ** renderForTime
- (void)renderForTime
{
    @autoreleasepool
    {
    //    [self setNeedsDisplay: YES];
        if (m_owner)
        {
            m_owner->notifyUpdate();
        }
    }
}

// ** initWithWindow
-( id ) initWithWindow: ( NSWindow* )window depthStencil:( unsigned int )options
{
    m_window        = window;
    m_displayLink   = nil;
    m_owner         = NULL;

    int depth   = Renderer::Private::depthBitsFromOptions(options);
    int stencil = Renderer::Private::stencilBitsFromOptions(options);

    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize,   static_cast<NSOpenGLPixelFormatAttribute>( depth ),
        NSOpenGLPFAStencilSize, static_cast<NSOpenGLPixelFormatAttribute>( stencil ),
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
- ( BOOL ) beginFrame
{
    if( m_displayLink == nil ) {
        return NO;
    }

    [[self openGLContext] makeCurrentContext];
    
    CGLError error = CGLLockContext( ( CGLContextObj )[[self openGLContext] CGLContextObj] );
    if( error != kCGLNoError ) {
        NSLog( @"CocoaOpenGLView::beginFrame : CGLLockContext error %x\n", error );
    }

    return YES;
}

// ** endFrame
- ( void ) endFrame
{
    CGLError error;

    error = CGLFlushDrawable( ( CGLContextObj )[[self openGLContext] CGLContextObj] );
    if( error != kCGLNoError ) {
        NSLog( @"CocoaOpenGLView::beginFrame : CGLFlushDrawable error %x\n", error );
    }

    error = CGLUnlockContext( ( CGLContextObj )[[self openGLContext] CGLContextObj] );
    if( error != kCGLNoError ) {
        NSLog( @"CocoaOpenGLView::beginFrame : CGLUnlockContext error %x\n", error );
    }
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

    CVReturn result;

    result = CVDisplayLinkCreateWithActiveCGDisplays( &m_displayLink );
    if( result != kCVReturnSuccess ) {
        NSLog( @"CocoaOpenGLView::prepareOpenGL : CVDisplayLinkCreateWithActiveCGDisplays result %d\n", result );
    }

    result = CVDisplayLinkSetOutputCallback( m_displayLink, displayCallback, self );
    if( result != kCVReturnSuccess ) {
        NSLog( @"CocoaOpenGLView::prepareOpenGL : CVDisplayLinkSetOutputCallback result %d\n", result );
    }

    // ** Set the display link for the current renderer
    CGLContextObj        cglContext        = ( CGLContextObj )[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj    cglPixelFormat    = ( CGLPixelFormatObj )[[self pixelFormat] CGLPixelFormatObj];

    result = CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext( m_displayLink, cglContext, cglPixelFormat );
    if( result != kCVReturnSuccess ) {
        NSLog( @"CocoaOpenGLView::prepareOpenGL : CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext result %d\n", result );
    }

    // ** Activate the display link
    result = CVDisplayLinkStart( m_displayLink );
    if( result != kCVReturnSuccess ) {
        NSLog( @"CocoaOpenGLView::prepareOpenGL : CVDisplayLinkStart result %d\n", result );
    }
}

// ** initGL
- ( void ) initGL
{
    [self makeCurrent];

    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
}

- ( void ) setOwner: ( Renderer::RenderView* )view
{
    m_owner = view;
}

// ** drawRect
//- ( void ) drawRect : ( NSRect )rect
//{
//    if (m_owner)
//   {
//        m_owner->notifyUpdate();
//    }
//}

// ** dealoc
- ( void ) dealloc
{
    // Release the display link
    CVDisplayLinkRelease( m_displayLink );
    [super dealloc];
}

@end
