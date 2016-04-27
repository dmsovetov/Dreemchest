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

#include "Commands.h"
#include "RenderFrame.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ---------------------------------------------------------------------- RenderCommandBuffer --------------------------------------------------------------------- //

// ** RenderCommandBuffer::RenderCommandBuffer
RenderCommandBuffer::RenderCommandBuffer( RenderFrame& frame )
    : m_frame( frame )
{
}

// ** RenderCommandBuffer::clear
void RenderCommandBuffer::clear( const Rgba& clearColor, u8 clearMask )
{
    OpCode opCode;
    opCode.type = OpCode::Clear;
    opCode.sorting = 0;
    opCode.clear.mask = clearMask;
    opCode.clear.depth = 1.0f;
    opCode.clear.stencil = 0;

    memcpy( opCode.clear.color, &clearColor, sizeof opCode.clear.color );
    m_commands.push_back( opCode );
}

// ** RenderCommandBuffer::execute
void RenderCommandBuffer::execute( const RenderCommandBuffer& commands )
{
    OpCode opCode;
    opCode.type = OpCode::Execute;
    opCode.sorting = 0;
    opCode.execute.commands = &commands;
    m_commands.push_back( opCode );
}

// ** RenderCommandBuffer::renderToTarget
RenderCommandBuffer& RenderCommandBuffer::renderToTarget( const Rect& viewport )
{
    RenderCommandBuffer& commands = m_frame.createCommandBuffer();

    OpCode opCode;
    opCode.type = OpCode::RenderTarget;
    opCode.sorting = 0;
    opCode.renderTarget.commands = &commands;
    opCode.renderTarget.id = 0;
    opCode.renderTarget.viewport[0] = static_cast<u32>( viewport.min().x );
    opCode.renderTarget.viewport[1] = static_cast<u32>( viewport.min().y );
    opCode.renderTarget.viewport[2] = static_cast<u32>( viewport.width() );
    opCode.renderTarget.viewport[3] = static_cast<u32>( viewport.height() );
    m_commands.push_back( opCode );

    return commands;
}

// ** RenderCommandBuffer::uploadConstantBuffer
void RenderCommandBuffer::uploadConstantBuffer( u32 id, const void* data, s32 size )
{
    OpCode opCode;
    opCode.type = OpCode::UploadConstantBuffer;
    opCode.upload.id = id;
    opCode.upload.data = data;
    opCode.upload.size = size;
    m_commands.push_back( opCode );
}

// ** RenderCommandBuffer::drawIndexed
void RenderCommandBuffer::drawIndexed( u32 sorting, Renderer::PrimitiveType primitives, const RenderStateBlock* states[MaxStateStackDepth], s32 first, s32 count )
{
    OpCode opCode;
    opCode.type         = OpCode::DrawIndexed;
    opCode.sorting      = sorting;
    opCode.drawCall.primitives   = primitives;
    opCode.drawCall.first        = first;
    opCode.drawCall.count        = count;
    memcpy( opCode.drawCall.states, states, sizeof opCode.drawCall.states );

    m_commands.push_back( opCode );
}

// ** RenderCommandBuffer::drawPrimitives
void RenderCommandBuffer::drawPrimitives( u32 sorting, Renderer::PrimitiveType primitives, const RenderStateBlock* states[MaxStateStackDepth], s32 first, s32 count )
{
    OpCode opCode;
    opCode.type         = OpCode::DrawPrimitives;
    opCode.sorting      = sorting;
    opCode.drawCall.primitives   = primitives;
    opCode.drawCall.first        = first;
    opCode.drawCall.count        = count;
    memcpy( opCode.drawCall.states, states, sizeof opCode.drawCall.states );

    m_commands.push_back( opCode );
}

} // namespace Scene

DC_END_DREEMCHEST