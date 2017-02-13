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
#include "iOSOpenGLView.h"

DC_USE_DREEMCHEST

// ** UIKitOpenGLView
@implementation UIKitOpenGLView

@synthesize width = _width;
@synthesize height = _height;

// ** layerClass
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

// ** initWithView
- ( id ) initWithView: ( Renderer::iOSOpenGLView* )view bounds:( CGRect )bounds options:( unsigned int )options scaleFactor:(float)scaleFactor
{
    if ((self = [super initWithFrame: bounds]))
    {
        self.contentScaleFactor = scaleFactor;
        self.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
        
        [self setupLayer];
        if (![self setupContext: kEAGLRenderingAPIOpenGLES2])
        {
            [self release];
            return nil;
        }
        
        [self setupFrameBuffer];
        [self setupDisplayLink];
        [self resizeFromLayer: _layer];
        
        _view = view;
        return self;
    }

    return nil;
}

// ** setupLayer
- (void) setupLayer
{
    _layer = ( CAEAGLLayer* )self.layer;
    _layer.contentsScale = self.contentScaleFactor;
    _layer.opaque = YES;
}

// ** setupContext
- (BOOL) setupContext: (EAGLRenderingAPI) api
{
    _context = [[EAGLContext alloc] initWithAPI:api];
    
    if (!_context)
    {
        Renderer::Log::error(NIMBLE_LOGGER_CONTEXT, "opengles", "failed to create context with a requested API level\n");
        return NO;
    }
    
    if (![EAGLContext setCurrentContext:_context])
    {
        Renderer::Log::error(NIMBLE_LOGGER_CONTEXT, "opengles", "failed set context\n");
        return NO;
    }
    
    return YES;
}

// ** setupFrameBuffer
- (BOOL) setupFrameBuffer
{
    // Create color renderbuffer
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);

    // Create depth renderbuffer
    glGenRenderbuffers(1, &_depthRenderBuffer);

    // Configure the framebuffer
    glGenFramebuffers(1, &_defaultFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFramebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
    
    return YES;
}

// ** setupDisplayLink
- (void) setupDisplayLink
{
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

// ** resizeFromLayer
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer
{
    // Allocate color buffer backing based on the current layer size
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    
    if( ![_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer] )
    {
        Renderer::Log::error(NIMBLE_LOGGER_CONTEXT, "opengles", "failed to create renderbuffer from layer\n");
        return NO;
    }
    
    // Get the viewport dimensions
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_height);
    
    // Update the depth buffer dimensions
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, _width, _height);
    
    Renderer::Log::verbose(NIMBLE_LOGGER_CONTEXT, "opengles", "renderbuffer resized to %dx%d\n", _width, _height);
    return YES;
}

// ** render
- (void)render: (CADisplayLink*)displayLink
{
    if (_view)
    {
        _view->notifyUpdate();
    }
}

// ** layoutSubviews
- (void) layoutSubviews
{
    [self resizeFromLayer: _layer];
}

// ** dealoc
- (void) dealloc
{
    if (_defaultFramebuffer)
    {
        glDeleteFramebuffersOES(1, &_defaultFramebuffer);
        _defaultFramebuffer = 0;
    }

    if (_colorRenderBuffer)
    {
        glDeleteRenderbuffersOES(1, &_colorRenderBuffer);
        _colorRenderBuffer = 0;
    }

    if (_depthRenderBuffer)
    {
        glDeleteRenderbuffersOES(1, &_depthRenderBuffer);
        _depthRenderBuffer = 0;
    }

    if ([EAGLContext currentContext] == _context)
    {
        [EAGLContext setCurrentContext: nil];
    }

    [_context release];
    _context = nil;

    [super dealloc];
}

// ** makeCurrent
- ( BOOL ) makeCurrent
{
    return [EAGLContext setCurrentContext: _context];
}

// ** beginFrame
- ( void ) beginFrame
{
    [self makeCurrent];

    glBindFramebufferOES( GL_FRAMEBUFFER_OES, _defaultFramebuffer );
    glViewport(0, 0, self.width, self.height);
}

// ** endFrame
- ( void ) endFrame: (BOOL) wait
{
    if (wait)
    {
        glFinish();
    }
    
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, _colorRenderBuffer);
    [_context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

@end
