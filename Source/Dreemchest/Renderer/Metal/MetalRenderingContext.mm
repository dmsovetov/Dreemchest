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

#include "MetalRenderingContext.h"
#include "MetalRenderView.h"
#include "../VertexFormat.h"
#include "../VertexBufferLayout.h"
#include "../Commands/CommandBuffer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
static CString s_defaultLibrarySource =
    "using namespace metal;                                         \n"
    "                                                               \n"
    "struct Vertex                                                  \n"
    "{                                                              \n"
    "    float4 position [[attribute(0)]];                          \n"
    "};                                                             \n"
    "                                                               \n"
    "struct OutputVertex                                            \n"
    "{                                                              \n"
    "    float4 position [[position]];                              \n"
    "};                                                             \n"
    "                                                               \n"
    "vertex OutputVertex vertex_main(Vertex in [[stage_in]])        \n"
    "{                                                              \n"
    "    OutputVertex out;                                          \n"
    "    out.position = in.position;                                \n"
    "    return out;                                                \n"
    "}                                                              \n"
    "                                                               \n"
    "fragment float4 fragment_main(OutputVertex in [[stage_in]])    \n"
    "{                                                              \n"
    "    return float4(1.0, 1.0, 1.0, 1.0);                         \n"
    "}                                                              \n"
    ;

// ** createMetalRenderingContext
RenderingContextPtr createMetalRenderingContext(RenderViewPtr view)
{
    MetalRenderView* metalView  = static_cast<MetalRenderView*>(view.get());
    return DC_NEW MetalRenderingContext(metalView);
}

// ** MetalRenderingContext::MetalRenderingContext
MetalRenderingContext::MetalRenderingContext(MetalRenderView* view)
    : RenderingContext(view)
    , m_device(view->nativeView().device)
    , m_metal(view->nativeView())
{
    // Create the command queue
    m_commandQueue = [m_device newCommandQueue];
    
    // Create the default library
    m_library = [m_device newLibraryWithSource:[NSString stringWithUTF8String:s_defaultLibrarySource]
                                       options:0
                                         error:nil];
    
    // Emplace invalid resources
    m_inputLayouts.emplace(0, nil);
    m_vertexBuffers.emplace(0, nil);
}
    
// ** MetalRenderingContext::executeCommandBuffer
void MetalRenderingContext::executeCommandBuffer(const CommandBuffer& commands)
{
    MTLRenderPassDescriptor* activeRenderPass = [MTLRenderPassDescriptor renderPassDescriptor];

    for (s32 i = 0, n = commands.size(); i < n; i++)
    {
        // Get a render operation at specified index
        const OpCode& opCode = commands.opCodeAt(i);
        
        // Perform a draw call
        switch(opCode.type)
        {
            case OpCode::Clear:
                activeRenderPass.colorAttachments[0].texture     = m_metal.defaultFramebuffer;
                activeRenderPass.colorAttachments[0].loadAction  = MTLLoadActionClear;
                activeRenderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
                activeRenderPass.colorAttachments[0].clearColor  = MTLClearColorMake(  opCode.clear.color[0]
                                                                                     , opCode.clear.color[1]
                                                                                     , opCode.clear.color[2]
                                                                                     , opCode.clear.color[3]);
                break;
                
            case OpCode::Execute:
                execute(*opCode.execute.commands);
                break;
                
            case OpCode::UploadConstantBuffer:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::UploadVertexBuffer:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::CreateInputLayout:
                m_inputLayouts.emplace(opCode.createInputLayout.id, createVertexBufferLayout(opCode.createInputLayout.format));
                m_vertexDescriptors.emplace(opCode.createInputLayout.id, createVertexDescriptor(*m_inputLayouts[opCode.createInputLayout.id]));
                break;
                
            case OpCode::CreateTexture:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::CreateIndexBuffer:
                m_indexBuffers.emplace(opCode.createBuffer.id, createBuffer(opCode.createBuffer.buffer.data, opCode.createBuffer.buffer.size));
                break;
                
            case OpCode::CreateVertexBuffer:
                m_vertexBuffers.emplace(opCode.createBuffer.id, createBuffer(opCode.createBuffer.buffer.data, opCode.createBuffer.buffer.size));
                break;
                
            case OpCode::CreateConstantBuffer:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::DeleteConstantBuffer:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::DeleteProgram:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::PrecompilePermutation:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::AcquireTexture:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::ReleaseTexture:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::RenderToTexture:
            case OpCode::RenderToTransientTexture:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::DrawIndexed:
                NIMBLE_NOT_IMPLEMENTED;
                break;
                
            case OpCode::DrawPrimitives:
            {
                if (m_debugPipeline == nil)
                {
                    MTLRenderPipelineDescriptor* pipelineDescriptor = [MTLRenderPipelineDescriptor new];
                    pipelineDescriptor.vertexFunction   = [m_library newFunctionWithName:@"vertex_main"];
                    pipelineDescriptor.fragmentFunction = [m_library newFunctionWithName:@"fragment_main"];
                    pipelineDescriptor.vertexDescriptor = m_vertexDescriptors[1];
                    pipelineDescriptor.colorAttachments[0].pixelFormat = m_metal.colorPixelFormat;
                    
                    m_debugPipeline = [m_device newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                                               error:NULL];
                }
                
                id <MTLCommandBuffer>        commandBuffer = [m_commandQueue commandBuffer];
                id <MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:activeRenderPass];
                
                [encoder setRenderPipelineState:m_debugPipeline];
                [encoder setVertexBuffer:m_vertexBuffers[1] offset:0 atIndex:0];
                [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                            vertexStart:opCode.drawCall.first
                            vertexCount:opCode.drawCall.count
                          instanceCount:1];
                [encoder endEncoding];
                [commandBuffer presentDrawable: m_metal.currentDrawable];
                [commandBuffer commit];
            }
                break;
                
            default:
                NIMBLE_NOT_IMPLEMENTED;
        }
    }
}
    
// ** MetalRenderingContext::createBuffer
id <MTLBuffer> MetalRenderingContext::createBuffer(const void* data, s32 size) const
{
    return [m_device newBufferWithBytes:data
                                 length:size
                                options:MTLResourceOptionCPUCacheModeDefault];
}

// ** MetalRenderingContext::createVertexDescriptor
MTLVertexDescriptor* MetalRenderingContext::createVertexDescriptor(const VertexBufferLayout& layout) const
{
    MTLVertexDescriptor* descriptor = [MTLVertexDescriptor vertexDescriptor];
    
    descriptor.layouts[0].stride = layout.vertexSize();
    descriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

    for (s32 i = 0; i < MaxVertexAttributes; i++)
    {
        const VertexBufferLayout::Element& element = layout[i];
        
        if (element)
        {
            if (i == VertexColor)
            {
                descriptor.attributes[i].format  = MTLVertexFormatUChar4;
            }
            else
            {
                descriptor.attributes[i].format  = static_cast<MTLVertexFormat>(MTLVertexFormatFloat + element.count - 1);
            }

            descriptor.attributes[i].offset      = element.offset;
            descriptor.attributes[i].bufferIndex = 0;
        }
    }
    
    return descriptor;
}

} // namespace Renderer

DC_END_DREEMCHEST
