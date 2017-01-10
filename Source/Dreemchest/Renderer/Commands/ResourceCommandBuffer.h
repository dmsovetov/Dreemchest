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

#ifndef __DC_Renderer_ResourceCommandBuffer_H__
#define __DC_Renderer_ResourceCommandBuffer_H__

#include "CommandBuffer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

    //! A command buffer type used for resource construction.
    class ResourceCommandBuffer : public CommandBuffer
    {
    friend class RenderingContext;
    public:
        
        //! Emits an input layout creation command.
        InputLayout                 createInputLayout(InputLayout id, const VertexFormat& vertexFormat);
        
        //! Emits a vertex buffer creation command.
        VertexBuffer_               createVertexBuffer(VertexBuffer_ id, const void* data, s32 size);
        
        //! Emits an index buffer creation command.
        IndexBuffer_                createIndexBuffer(IndexBuffer_ id, const void* data, s32 size);
        
        //! Emits a constant buffer creation command.
        ConstantBuffer_             createConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, UniformLayout layout);
        
        //! Emits a texture creation command.
        Texture_                    createTexture2D(Texture_ id, const void* data, u16 width, u16 height, PixelFormat format, TextureFilter filter);
        
        //! Emits a cube texture creation command.
        Texture_                    createTextureCube(Texture_ id, const void* data, u16 size, u16 mipLevels, PixelFormat format, TextureFilter filter);
        
        //! Emits a constant buffer destruction command.
        void                        deleteConstantBuffer(ConstantBuffer_ id);
        
        //! Emits a program destruction command.
        void                        deleteProgram(Program id);
        
    private:
        
                                    //! Constructs a ResourceCommandBuffer instance.
                                    ResourceCommandBuffer();
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_ResourceCommandBuffer_H__   */
