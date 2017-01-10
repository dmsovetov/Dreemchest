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

#include "RenderCommandBuffer.h"
#include "../RenderFrame.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** RenderCommandBuffer::RenderCommandBuffer
RenderCommandBuffer::RenderCommandBuffer(RenderFrame& frame)
    : m_frame(frame)
    , m_transientResourceIndex(0)
{
}

// ** RenderCommandBuffer::clear
void RenderCommandBuffer::clear(const Rgba& clearColor, u8 clearMask)
{
    OpCode opCode;
    opCode.type = OpCode::Clear;
    opCode.sorting = 0;
    opCode.clear.mask = clearMask;
    opCode.clear.depth = 1.0f;
    opCode.clear.stencil = 0;
    
    memcpy(opCode.clear.color, &clearColor, sizeof(opCode.clear.color));
    push(opCode);
}

// ** RenderCommandBuffer::renderToTexture
RenderCommandBuffer& RenderCommandBuffer::renderToTexture(TransientTexture id, const Rect& viewport)
{
    RenderCommandBuffer& commands = m_frame.createCommandBuffer();
    
    OpCode opCode;
    opCode.type = OpCode::RenderToTransientTexture;
    opCode.sorting = 0;
    opCode.renderToTextures.commands = &commands;
    opCode.renderToTextures.id = static_cast<u8>(id);
    opCode.renderToTextures.side = 255;
    opCode.renderToTextures.viewport.x = viewport.min().x;
    opCode.renderToTextures.viewport.y = viewport.min().y;
    opCode.renderToTextures.viewport.width = viewport.width();
    opCode.renderToTextures.viewport.height = viewport.height();
    push( opCode );
    
    return commands;
}

// ** RenderCommandBuffer::renderToCubeMap
RenderCommandBuffer& RenderCommandBuffer::renderToCubeMap(TransientTexture id, u8 side, const Rect& viewport)
{
    RenderCommandBuffer& commands = m_frame.createCommandBuffer();
    
    OpCode opCode;
    opCode.type = OpCode::RenderToTransientTexture;
    opCode.sorting = 0;
    opCode.renderToTextures.commands = &commands;
    opCode.renderToTextures.id = static_cast<u8>(id);
    opCode.renderToTextures.side = side;
    opCode.renderToTextures.viewport.x = viewport.min().x;
    opCode.renderToTextures.viewport.y = viewport.min().y;
    opCode.renderToTextures.viewport.width = viewport.width();
    opCode.renderToTextures.viewport.height = viewport.height();
    push( opCode );
    
    return commands;
}

// ** RenderCommandBuffer::renderToCubeMap
RenderCommandBuffer& RenderCommandBuffer::renderToCubeMap(Texture_ id, u8 side, const Rect& viewport)
{
    RenderCommandBuffer& commands = m_frame.createCommandBuffer();
    
    OpCode opCode;
    opCode.type = OpCode::RenderToTexture;
    opCode.sorting = 0;
    opCode.renderToTextures.commands = &commands;
    opCode.renderToTextures.id = id;
    opCode.renderToTextures.side = side;
    opCode.renderToTextures.viewport.x = viewport.min().x;
    opCode.renderToTextures.viewport.y = viewport.min().y;
    opCode.renderToTextures.viewport.width = viewport.width();
    opCode.renderToTextures.viewport.height = viewport.height();
    push( opCode );
    
    return commands;
}

// ** RenderCommandBuffer::renderToTarget
RenderCommandBuffer& RenderCommandBuffer::renderToTarget(const Rect& viewport)
{
    return renderToTexture(TransientTexture(), viewport);
}

// ** RenderCommandBuffer::acquireTexture2D
TransientTexture RenderCommandBuffer::acquireTexture2D(u16 width, u16 height, PixelFormat format)
{
    NIMBLE_ABORT_IF( m_transientResourceIndex + 1 >= 255, "too transient resources used" );
    
    TransientTexture id = TransientTexture::create(++m_transientResourceIndex);
    
    OpCode opCode;
    opCode.type = OpCode::AcquireTexture;
    opCode.sorting = 0;
    opCode.transientTexture.id     = id;
    opCode.transientTexture.width  = width;
    opCode.transientTexture.height = height;
    opCode.transientTexture.format = format;
    opCode.transientTexture.type = TextureType2D;
    push( opCode );
    
    return id;
}

// ** RenderCommandBuffer::acquireTextureCube
TransientTexture RenderCommandBuffer::acquireTextureCube(u16 size, PixelFormat format)
{
    NIMBLE_ABORT_IF( m_transientResourceIndex + 1 >= 255, "too transient resources used" );
    
    TransientTexture id = TransientTexture::create(++m_transientResourceIndex);
    
    OpCode opCode;
    opCode.type = OpCode::AcquireTexture;
    opCode.sorting = 0;
    opCode.transientTexture.id     = id;
    opCode.transientTexture.width  = size;
    opCode.transientTexture.height = size;
    opCode.transientTexture.format = format;
    opCode.transientTexture.type = TextureTypeCube;
    push( opCode );
    
    return id;
}

// ** RenderCommandBuffer::releaseTexture
void RenderCommandBuffer::releaseTexture(TransientTexture id)
{
    OpCode opCode;
    opCode.type = OpCode::ReleaseTexture;
    opCode.transientTexture.id = id;
    opCode.sorting = 0;
    push( opCode );
}

// ** RenderCommandBuffer::drawIndexed
void RenderCommandBuffer::drawIndexed(u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateStack& stateStack)
{
    emitDrawCall(OpCode::DrawIndexed, sorting, primitives, first, count, stateStack.states(), stateStack.size());
}

// ** RenderCommandBuffer::drawIndexed
void RenderCommandBuffer::drawIndexed(u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock* stateBlock)
{
    emitDrawCall(OpCode::DrawIndexed, sorting, primitives, first, count, &stateBlock, 1);
}

// ** RenderCommandBuffer::drawPrimitives
void RenderCommandBuffer::drawPrimitives(u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateStack& stateStack)
{
    emitDrawCall(OpCode::DrawPrimitives, sorting, primitives, first, count, stateStack.states(), stateStack.size());
}

// ** RenderCommandBuffer::drawPrimitives
void RenderCommandBuffer::drawPrimitives(u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock* stateBlock)
{
    emitDrawCall(OpCode::DrawPrimitives, sorting, primitives, first, count, &stateBlock, 1);
}

// ** RenderCommandBuffer::emitDrawCall
void RenderCommandBuffer::emitDrawCall( OpCode::Type type, u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock** states, s32 stateCount)
{
    OpCode opCode;
    opCode.type                 = type;
    opCode.sorting              = sorting;
    opCode.drawCall.primitives  = primitives;
    opCode.drawCall.first       = first;
    opCode.drawCall.count       = count;
    opCode.drawCall.states      = (const StateBlock**)m_frame.allocate(sizeof(StateBlock*) * (stateCount + 1));
    
    opCode.drawCall.states[stateCount] = NULL;
    memcpy(opCode.drawCall.states, states, sizeof(StateBlock*) * stateCount);

    push(opCode);
}
    
} // namespace Renderer

DC_END_DREEMCHEST
