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

#ifndef __DC_Renderer_RenderingContext_H__
#define __DC_Renderer_RenderingContext_H__

#include "Ubershader.h"
#include "RenderState.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Rendering context.
    class RenderingContext : public RefCounted
    {
    public:
        
        //! Displays a frame captured by a render scene.
        virtual void                        display( RenderFrame& frame ) = 0;
        
        //! Queues an input layout instance for creation and returns it's index.
        virtual InputLayout                 requestInputLayout( const VertexFormat& vertexFormat ) = 0;
        
        //! Queues a vertex buffer instance for creation and returns it's index.
        virtual VertexBuffer_               requestVertexBuffer( const void* data, s32 size ) = 0;
        
        //! Queues an index buffer instance for creation and returns it's index.
        virtual IndexBuffer_                requestIndexBuffer( const void* data, s32 size ) = 0;
        
        //! Queues a constant buffer instance for creation and returns it's index.
        virtual ConstantBuffer_             requestConstantBuffer( const void* data, s32 size, const ConstantBufferLayout* layout ) = 0;
        
        //! Queues a texture instance for creation and returns it's index.
        virtual Texture_                    requestTexture( const void* data, u16 width, u16 height, PixelFormat format ) = 0;
        
        //! Queues a pipeline feature layout instance for creation and returns it's index.
        virtual FeatureLayout               requestPipelineFeatureLayout(const PipelineFeature* features) = 0;
        
        //! Queues a shader instance creation and returns it's index.
        virtual Program                     requestShader( const String& fileName ) = 0;
        
        //! Queues a shader instance creation and returns it's index.
        virtual Program                     requestShader( const String& vertex, const String& fragment ) = 0;
    };
    
    //! Creates a rendering context that uses a deprecated rendering HAL interface.
    RenderingContextPtr createDeprecatedRenderingContext( HalWPtr hal );
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_RenderingContext_H__  */
