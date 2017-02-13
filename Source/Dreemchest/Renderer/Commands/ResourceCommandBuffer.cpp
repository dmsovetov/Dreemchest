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

#include "ResourceCommandBuffer.h"
#include "../VertexFormat.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
// ** ResourceCommandBuffer::ResourceCommandBuffer
ResourceCommandBuffer::ResourceCommandBuffer()
{
    
}

// ** ResourceCommandBuffer::createVertexBuffer
InputLayout ResourceCommandBuffer::createInputLayout(InputLayout id, const VertexFormat& vertexFormat)
{
    OpCode opCode;
    opCode.type = OpCode::CreateInputLayout;
    opCode.createInputLayout.id = id;
    opCode.createInputLayout.format = vertexFormat;
    push( opCode );
    return id;
}

// ** ResourceCommandBuffer::createVertexBuffer
VertexBuffer_ ResourceCommandBuffer::createVertexBuffer(VertexBuffer_ id, const void* data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::CreateVertexBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.buffer = adoptDataBuffer(data, size);
    push( opCode );
    return id;
}

// ** ResourceCommandBuffer::createIndexBuffer
IndexBuffer_ ResourceCommandBuffer::createIndexBuffer(IndexBuffer_ id, const void* data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::CreateIndexBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.buffer = adoptDataBuffer(data, size);
    push( opCode );
    return id;
}

// ** ResourceCommandBuffer::createConstantBuffer
ConstantBuffer_ ResourceCommandBuffer::createConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, UniformLayout layout)
{
    OpCode opCode;
    opCode.type = OpCode::CreateConstantBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.buffer = adoptDataBuffer(data, size);
    opCode.createBuffer.layout = layout;
    push( opCode );
    return id;
}

// ** ResourceCommandBuffer::createTexture2D
Texture_ ResourceCommandBuffer::createTexture2D(Texture_ id, const void* data, u16 width, u16 height, u32 options)
{
    PixelFormat format = Private::pixelFormatFromOptions(options);
    
    OpCode opCode;
    opCode.type = OpCode::CreateTexture;
    opCode.createTexture.id = id;
    opCode.createTexture.buffer = adoptDataBuffer(data, bytesPerMipChain(format, width, height, 1));
    opCode.createTexture.width = width;
    opCode.createTexture.height = height;
    opCode.createTexture.mipLevels = 1;
    opCode.createTexture.type = TextureType2D;
    opCode.createTexture.options = options;
    push( opCode );
    return id;
}

// ** ResourceCommandBuffer::createTextureCube
Texture_ ResourceCommandBuffer::createTextureCube(Texture_ id, const void* data, u16 size, u16 mipLevels, u32 options)
{
    PixelFormat format = Private::pixelFormatFromOptions(options);
        
    OpCode opCode;
    opCode.type = OpCode::CreateTexture;
    opCode.createTexture.id = id;
    opCode.createTexture.buffer = adoptDataBuffer(data, bytesPerMipChain(format, size, size, mipLevels) * 6);
    opCode.createTexture.width = size;
    opCode.createTexture.height = size;
    opCode.createTexture.options = options;
    opCode.createTexture.type = TextureTypeCube;
    opCode.createTexture.mipLevels = mipLevels;
    push( opCode );
    return id;
}

// ** ResourceCommandBuffer::deleteConstantBuffer
void ResourceCommandBuffer::deleteConstantBuffer(ConstantBuffer_ id)
{
    OpCode opCode;
    opCode.type = OpCode::DeleteConstantBuffer;
    opCode.id   = id;
    push(opCode);
}

// ** ResourceCommandBuffer::deleteConstantBuffer
void ResourceCommandBuffer::deleteProgram(Program id)
{
    OpCode opCode;
    opCode.type = OpCode::DeleteProgram;
    opCode.id   = id;
    push(opCode);
}

// ** ResourceCommandBuffer::precompilePermutation
void ResourceCommandBuffer::precompilePermutation(Program id, PipelineFeatures features)
{
    OpCode opCode;
    opCode.type                = OpCode::PrecompilePermutation;
    opCode.precompile.program  = id;
    opCode.precompile.features = features;
    push(opCode);
}
    
} // namespace Renderer

DC_END_DREEMCHEST
