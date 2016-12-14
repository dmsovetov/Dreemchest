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

#include "CommandBuffer.h"
#include "RenderFrame.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** CommandBuffer::CommandBuffer
CommandBuffer::CommandBuffer( void )
    : m_renderTargetIndex( 0 )
{
}
    
// ** CommandBuffer::reset
void CommandBuffer::reset( void )
{
    m_commands.clear();
}

// ** CommandBuffer::clear
void CommandBuffer::clear( const Rgba& clearColor, u8 clearMask )
{
    OpCode opCode;
    opCode.type = OpCode::Clear;
    opCode.sorting = 0;
    opCode.clear.mask = clearMask;
    opCode.clear.depth = 1.0f;
    opCode.clear.stencil = 0;

    memcpy( opCode.clear.color, &clearColor, sizeof opCode.clear.color );
    push( opCode );
}

// ** CommandBuffer::execute
void CommandBuffer::execute( const CommandBuffer& commands )
{
    OpCode opCode;
    opCode.type = OpCode::Execute;
    opCode.sorting = 0;
    opCode.execute.commands = &commands;
    push( opCode );
}

// ** CommandBuffer::renderToTarget
CommandBuffer& CommandBuffer::renderToTarget( RenderFrame& frame, TransientRenderTarget index, const Rect& viewport )
{
    CommandBuffer& commands = frame.createCommandBuffer();

    OpCode opCode;
    opCode.type = OpCode::RenderTarget;
    opCode.sorting = 0;
    opCode.renderTarget.commands = &commands;
    opCode.renderTarget.index = index;
    opCode.renderTarget.viewport[0] = viewport.min().x;
    opCode.renderTarget.viewport[1] = viewport.min().y;
    opCode.renderTarget.viewport[2] = viewport.width();
    opCode.renderTarget.viewport[3] = viewport.height();
    push( opCode );

    return commands;
}
    
// ** CommandBuffer::renderToTarget
CommandBuffer& CommandBuffer::renderToTarget( RenderFrame& frame, const Rect& viewport )
{
    return renderToTarget(frame, TransientRenderTarget(), viewport);
}

// ** CommandBuffer::acquireRenderTarget
TransientRenderTarget CommandBuffer::acquireRenderTarget( s32 width, s32 height, PixelFormat format )
{
    NIMBLE_ABORT_IF( m_renderTargetIndex + 1 >= 255, "too much render targets used" );

    OpCode opCode;
    opCode.type = OpCode::AcquireRenderTarget;
    opCode.sorting = 0;
    opCode.intermediateRenderTarget.index  = ++m_renderTargetIndex;
    opCode.intermediateRenderTarget.width  = width;
    opCode.intermediateRenderTarget.height = height;
    opCode.intermediateRenderTarget.format = format;
    push( opCode );

    return TransientRenderTarget::create(opCode.intermediateRenderTarget.index);
}

// ** CommandBuffer::releaseRenderTarget
void CommandBuffer::releaseRenderTarget( TransientRenderTarget index )
{
    OpCode opCode;
    opCode.type = OpCode::ReleaseRenderTarget;
    opCode.intermediateRenderTarget.index = index;
    opCode.sorting = 0;
    push( opCode );
}

// ** CommandBuffer::uploadConstantBuffer
void CommandBuffer::uploadConstantBuffer( ConstantBuffer_ id, const void* data, s32 size )
{
    OpCode opCode;
    opCode.type = OpCode::UploadConstantBuffer;
    opCode.upload.id = id;
    opCode.upload.data = data;
    opCode.upload.size = size;
    push( opCode );
}

// ** CommandBuffer::uploadVertexBuffer
void CommandBuffer::uploadVertexBuffer( VertexBuffer_ id, const void* data, s32 size )
{
    OpCode opCode;
    opCode.type = OpCode::UploadVertexBuffer;
    opCode.upload.id = id;
    opCode.upload.data = data;
    opCode.upload.size = size;
    push( opCode );
}

// ** CommandBuffer::drawIndexed
void CommandBuffer::drawIndexed( u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateStack& stateStack )
{
    emitDrawCall( OpCode::DrawIndexed, sorting, primitives, first, count, stateStack.states(), MaxStateStackDepth );
}
    
// ** CommandBuffer::drawIndexed
void CommandBuffer::drawIndexed( u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock* stateBlock )
{
    emitDrawCall( OpCode::DrawIndexed, sorting, primitives, first, count, &stateBlock, 1 );
}

// ** CommandBuffer::drawPrimitives
void CommandBuffer::drawPrimitives( u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateStack& stateStack )
{
    emitDrawCall( OpCode::DrawPrimitives, sorting, primitives, first, count, stateStack.states(), MaxStateStackDepth );
}
    
// ** CommandBuffer::drawPrimitives
void CommandBuffer::drawPrimitives( u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock* stateBlock )
{
    emitDrawCall( OpCode::DrawPrimitives, sorting, primitives, first, count, &stateBlock, 1 );
}
    
// ** CommandBuffer::emitDrawCall
void CommandBuffer::emitDrawCall( OpCode::Type type, u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock** states, s32 stateCount )
{
    OpCode opCode;
    opCode.type                 = type;
    opCode.sorting              = sorting;
    opCode.drawCall.primitives  = primitives;
    opCode.drawCall.first       = first;
    opCode.drawCall.count       = count;
    
    memset( opCode.drawCall.states, 0, sizeof( opCode.drawCall.states ) );
    memcpy( opCode.drawCall.states, states, sizeof( StateBlock* ) * stateCount );
    
    push( opCode );
}
    
// ** CommandBuffer::push
void CommandBuffer::push(const OpCode& opCode)
{
    m_commands.push_back(opCode);
}

} // namespace Renderer

DC_END_DREEMCHEST
