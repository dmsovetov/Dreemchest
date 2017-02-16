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

#ifndef __DC_Renderer_MetalRenderView_H__
#define __DC_Renderer_MetalRenderView_H__

#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>
#import "../Renderer.h"

DC_USE_DREEMCHEST

@interface MetalView : MTKView <MTKViewDelegate>

- (instancetype)initWithFrame:(CGRect)frameRect;

- (void)drawInMTKView:(MTKView *)view;
- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size;
- (void)beginFrame;
- (void)endFrame:(bool) wait;

@property Renderer::RenderView* owner;
@property (assign) id <MTLTexture> defaultFramebuffer;
@property (readonly) id <MTLCommandBuffer> commandBuffer;
@property (readonly) id <MTLCommandQueue> commandQueue;

@end

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    class MetalRenderView : public RenderView
    {
    public:
        
                        MetalRenderView(MetalView* nativeView);
        
        virtual s32     width() const NIMBLE_OVERRIDE;
        virtual s32     height() const NIMBLE_OVERRIDE;
        virtual bool    makeCurrent( void ) NIMBLE_OVERRIDE;
        virtual bool    beginFrame( void ) NIMBLE_OVERRIDE;
        virtual void    endFrame(bool wait) NIMBLE_OVERRIDE;
        MetalView*      nativeView() const;
        
    private:
        
        MetalView*      m_nativeView;
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_MetalRenderView_H__  */
