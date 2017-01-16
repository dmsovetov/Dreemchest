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
    , m_stateStack(frame.stateStack())
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
TransientTexture RenderCommandBuffer::acquireTexture2D(u16 width, u16 height, u32 options)
{
    NIMBLE_ABORT_IF( m_transientResourceIndex + 1 >= 255, "too transient resources used" );
    
    TransientTexture id = TransientTexture::create(++m_transientResourceIndex);
    
    OpCode opCode;
    opCode.type = OpCode::AcquireTexture;
    opCode.sorting = 0;
    opCode.transientTexture.id     = id;
    opCode.transientTexture.width  = width;
    opCode.transientTexture.height = height;
    opCode.transientTexture.options = options;
    opCode.transientTexture.type = TextureType2D;
    push( opCode );
    
    return id;
}

// ** RenderCommandBuffer::acquireTextureCube
TransientTexture RenderCommandBuffer::acquireTextureCube(u16 size, u32 options)
{
    NIMBLE_ABORT_IF( m_transientResourceIndex + 1 >= 255, "too transient resources used" );
    
    TransientTexture id = TransientTexture::create(++m_transientResourceIndex);
    
    OpCode opCode;
    opCode.type = OpCode::AcquireTexture;
    opCode.sorting = 0;
    opCode.transientTexture.id     = id;
    opCode.transientTexture.width  = size;
    opCode.transientTexture.height = size;
    opCode.transientTexture.options = options;
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
void RenderCommandBuffer::drawIndexed(u32 sorting, PrimitiveType primitives, s32 first, s32 count)
{
    emitDrawCall(OpCode::DrawIndexed, sorting, primitives, first, count, m_stateStack.states(), m_stateStack.size(), NULL);
}

// ** RenderCommandBuffer::drawIndexed
void RenderCommandBuffer::drawIndexed(u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock& stateBlock)
{
    emitDrawCall(OpCode::DrawIndexed, sorting, primitives, first, count, m_stateStack.states(), m_stateStack.size(), &stateBlock);
}

// ** RenderCommandBuffer::drawPrimitives
void RenderCommandBuffer::drawPrimitives(u32 sorting, PrimitiveType primitives, s32 first, s32 count)
{
    emitDrawCall(OpCode::DrawPrimitives, sorting, primitives, first, count, m_stateStack.states(), m_stateStack.size(), NULL);
}

// ** RenderCommandBuffer::drawPrimitives
void RenderCommandBuffer::drawPrimitives(u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock& stateBlock)
{
    emitDrawCall(OpCode::DrawPrimitives, sorting, primitives, first, count, m_stateStack.states(), m_stateStack.size(), &stateBlock);
}
    
// ** RenderCommandBuffer::drawItem
void RenderCommandBuffer::drawItem(u32 sorting, const RenderItem& item)
{
    emitDrawCall(item.indexed ? OpCode::DrawIndexed : OpCode::DrawPrimitives, sorting, item.primitives, item.first, item.count, m_stateStack.states(), m_stateStack.size(), &item.states);
}

// ** RenderCommandBuffer::drawItem
void RenderCommandBuffer::drawItem(u32 sorting, const RenderItem& item, const StateBlock& stateBlock)
{
    NIMBLE_NOT_IMPLEMENTED
}

// ** RenderCommandBuffer::emitDrawCall
void RenderCommandBuffer::emitDrawCall(OpCode::Type type, u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock** stateBlocks, s32 stateBlockCount, const StateBlock* overrideStateBlock)
{
    // Compile an array of state blocks
    OpCode::CompiledStateBlock* compiledStateBlock = (OpCode::CompiledStateBlock*)m_frame.allocate(sizeof(OpCode::CompiledStateBlock));
    
    s32 maxStates = (m_frame.allocationCapacity() - m_frame.allocatedBytes()) / sizeof(State);
    State* states = (State*)m_frame.allocate(sizeof(State));
    compiledStateBlock->states   = states;
    compiledStateBlock->size     = 0;
    compiledStateBlock->mask     = 0;
    compiledStateBlock->features = 0;
    
    // First write an override state block (if specified)
    if (overrideStateBlock)
    {
        compiledStateBlock->size = compileStateStack(&overrideStateBlock, 1, states, maxStates, compiledStateBlock);
    }
    
    // Now unroll the state stack
    compiledStateBlock->size += compileStateStack(stateBlocks, stateBlockCount, states + compiledStateBlock->size, maxStates - compiledStateBlock->size, compiledStateBlock);
    
    m_frame.allocate(sizeof(State) * (compiledStateBlock->size - 1));
    
    // Now push a draw call command
    OpCode opCode;
    memset(&opCode, 0, sizeof(opCode));
    opCode.type                 = type;
    opCode.sorting              = sorting;
    opCode.drawCall.primitives  = primitives;
    opCode.drawCall.first       = first;
    opCode.drawCall.count       = count;
    opCode.drawCall.stateBlock  = compiledStateBlock;
    push(opCode);
}
    
// ** RenderCommandBuffer::compileStateStack
s32 RenderCommandBuffer::compileStateStack(const StateBlock* const * stateBlocks, s32 count, State* states, s32 maxStates, OpCode::CompiledStateBlock* compiledStateBlock)
{
    PipelineFeatures userFeatures = 0;
    PipelineFeatures userFeaturesMask = ~0;
    PipelineFeatures resourceFeatures = 0;

    // A total number of states written to an output array
    s32 statesWritten = 0;
    
    for (s32 i = 0; i < count; i++)
    {
        // Get a state block at specified index
        const StateBlock* block = stateBlocks[i];
        
        // No more state blocks in a stack - break
        if( block == NULL )
        {
            break;
        }
        
        // Update feature set
        userFeatures     = userFeatures     | block->userDefined();
        userFeaturesMask = userFeaturesMask & block->userDefinedMask();
        resourceFeatures = resourceFeatures | block->resourceFeatures();
        
        // Skip redundant state blocks by testing a block bitmask against an active state mask
        if( (compiledStateBlock->mask ^ block->mask()) == 0 )
        {
            continue;
        }
        
        // Apply all states in a block
        for( s32 j = 0, n = block->stateCount(); j < n; j++ )
        {
            // Get a render state by an index
            const State& state = block->state(j);
            
            // Get a render state bit
            StateMask stateMask = state.bitmask();
            
            // Skip redundate state blocks by testing a state bitmask agains an active state mask
            if( compiledStateBlock->mask & stateMask )
            {
                continue;
            }
            
            NIMBLE_ABORT_IF(statesWritten >= maxStates, "to much render states");
            
            // Write a render state at specified index to an output array
            states[statesWritten++] = state;
            
            // Update an active state mask
            compiledStateBlock->mask = compiledStateBlock->mask | stateMask;
        }
    }
    
    // Compose a user defined feature mask
    compiledStateBlock->features = compiledStateBlock->features | (userFeatures & userFeaturesMask) | resourceFeatures;
    
    return statesWritten;
}
    
} // namespace Renderer

DC_END_DREEMCHEST
