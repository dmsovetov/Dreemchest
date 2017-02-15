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

#import "MetalRenderView.h"

@implementation MetalView

@synthesize owner;
@synthesize defaultFramebuffer;

// ** initWithFrame
- (instancetype)initWithFrame:(CGRect)frameRect
{
    id <MTLDevice> device = MTLCreateSystemDefaultDevice();
    
    if (self = [super initWithFrame:frameRect device:device])
    {
        self.delegate         = self;
    #ifdef DC_PLATFORM_IOS
        self.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
    #endif  //  #ifdef DC_PLATFORM_IOS
        self.clearColor       = MTLClearColorMake(0.3, 0.3, 0.3, 1.0);
        return self;
    }
    
    return nil;
}

// ** beginFrame
- (void)beginFrame
{
    defaultFramebuffer = self.currentDrawable.texture;
}

// ** endFrame
- (void)endFrame: (bool)wait
{
    
}

// ** drawInMTKView
- (void)drawInMTKView:(MTKView *)view
{
    if (owner)
    {
        owner->notifyUpdate();
    }
}

// ** mtkView
- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size
{
//    NSLog(@"drawInMTKView\n");
}

@end

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
// ** MetalRenderView::MetalRenderView
MetalRenderView::MetalRenderView(MetalView* nativeView)
    : m_nativeView(nativeView)
{
}
    
// ** MetalRenderView::width
s32 MetalRenderView::width() const
{
    return m_nativeView.frame.size.width;
}
    
// ** MetalRenderView::height
s32 MetalRenderView::height() const
{
    return m_nativeView.frame.size.height;
}
    
// ** MetalRenderView::makeCurrent
bool MetalRenderView::makeCurrent( void )
{
    return true;
}
    
// ** MetalRenderView::beginFrame
bool MetalRenderView::beginFrame( void )
{
    [m_nativeView beginFrame];
    return true;
}
    
// ** MetalRenderView::endFrame
void MetalRenderView::endFrame(bool wait)
{
    [m_nativeView endFrame:wait];
}
    
// ** MetalRenderView::nativeView
MetalView* MetalRenderView::nativeView() const
{
    return m_nativeView;
}

// ** createMetalView
RenderViewPtr createMetalView(void* window, u32 options)
{
#if defined(DC_PLATFORM_MACOS)
    NSWindow*  nativeWindow = reinterpret_cast<NSWindow*>(window);
    MetalView* nativeView   = [[MetalView alloc] initWithFrame: nativeWindow.frame];
    [nativeWindow setContentView: nativeView];
    LogVerbose("metal", "%s", "macOS Metal render view created\n" );
#else
    UIWindow*  nativeWindow = reinterpret_cast<UIWindow*>(window);
    MetalView* nativeView   = [[MetalView alloc] initWithFrame: nativeWindow.bounds];
    [nativeWindow addSubview: nativeView];
    LogVerbose("metal", "%s", "iOS Metal render view created\n" );
#endif  //  #if defined(DC_PLATFORM_MACOS)
    
    MetalRenderView* view = DC_NEW MetalRenderView(nativeView);
    nativeView.owner = view;
    return view;
}

} // namespace Renderer

DC_END_DREEMCHEST
