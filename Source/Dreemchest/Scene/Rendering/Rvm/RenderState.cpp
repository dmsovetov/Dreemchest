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

#include "RenderState.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ---------------------------------------------------------------------------- RenderState ---------------------------------------------------------------------------- //

// ** RenderState::RenderState
RenderState::RenderState( void )
    : type( TotalStates )
{
}

// ** RenderState::RenderState
RenderState::RenderState( Type type, s32 id )
    : type( type )
    , id( id )
{
}

// ** RenderState::RenderState
RenderState::RenderState( Renderer::Compare function, bool write )
    : type( DepthState )
{
    depth.function = function;
    depth.write    = write;
}

// ** RenderState::RenderState
RenderState::RenderState( Renderer::Compare function, f32 reference )
    : type( AlphaTest )
{
    alpha.function  = function;
    alpha.reference = reference;
}

// ** RenderState::RenderState
RenderState::RenderState( s32 id, ConstantBufferType type )
    : type( ConstantBuffer )
{
    constantBuffer.id   = id;
    constantBuffer.type = type;
}

// ** RenderState::RenderState
RenderState::RenderState( Renderer::BlendFactor src, Renderer::BlendFactor dst )
    : type( Blending )
{
    blend.src = src;
    blend.dst = dst;
}

// ** RenderState::RenderState
RenderState::RenderState( s32 id, TextureSampler sampler )
    : type( Texture )
{
    texture.id      = id;
    texture.sampler = sampler;
}

// ** RenderState::RenderState
RenderState::RenderState( s32 id, const Rect& viewport )
    : type( RenderTarget )
{
    renderTarget.id = id;
    renderTarget.viewport[0] = viewport.min().x;
    renderTarget.viewport[1] = viewport.min().y;
    renderTarget.viewport[2] = viewport.width();
    renderTarget.viewport[3] = viewport.height();
}

// -------------------------------------------------------------------------- RenderStateBlock -------------------------------------------------------------------------- //

// ** RenderStateBlock::RenderStateBlock
RenderStateBlock::RenderStateBlock( void )
    : m_mask( 0 )
{
}

// ** RenderStateBlock::bindVertexBuffer
void RenderStateBlock::bindVertexBuffer( s32 id )
{
    pushState( RenderState( RenderState::VertexBuffer, id ), RenderState::VertexBuffer );
}

// ** RenderStateBlock::bindIndexBuffer
void RenderStateBlock::bindIndexBuffer( s32 id )
{
    pushState( RenderState( RenderState::IndexBuffer, id ), RenderState::IndexBuffer );
}

// ** RenderStateBlock::bindConstantBuffer
void RenderStateBlock::bindConstantBuffer( s32 id, RenderState::ConstantBufferType type )
{
    pushState( RenderState( id, type ), RenderState::ConstantBuffer + type );
}

// ** RenderStateBlock::bindProgram
void RenderStateBlock::bindProgram( s32 id )
{
    pushState( RenderState( RenderState::Shader, id ), RenderState::Shader );
}

// ** RenderStateBlock::bindTexture
void RenderStateBlock::bindTexture( s32 id, RenderState::TextureSampler sampler )
{
    pushState( RenderState( id, sampler ), RenderState::Texture + sampler );
}

// ** RenderStateBlock::setBlend
void RenderStateBlock::setBlend( Renderer::BlendFactor src, Renderer::BlendFactor dst )
{
    pushState( RenderState( src, dst ), RenderState::Blending );
}

// ** RenderStateBlock::setDepthState
void RenderStateBlock::setDepthState( Renderer::Compare function, bool write )
{
    pushState( RenderState( function, write ), RenderState::DepthState );
}

// ** RenderStateBlock::setAlphaTest
void RenderStateBlock::setAlphaTest( Renderer::Compare function, f32 reference )
{
    pushState( RenderState( function, reference ), RenderState::AlphaTest );
}

// ** RenderStateBlock::setRenderTarget
void RenderStateBlock::setRenderTarget( s32 id, const Rect& viewport )
{
    pushState( RenderState( id, viewport ), RenderState::RenderTarget );
}

// ** RenderStateBlock::disableBlending
void RenderStateBlock::disableAlphaTest( void )
{
    setAlphaTest( Renderer::CompareDisabled, 0.0f );
}

// ** RenderStateBlock::disableBlending
void RenderStateBlock::disableBlending( void )
{
    setBlend( Renderer::BlendDisabled, Renderer::BlendDisabled );
}

// ** RenderStateBlock::pushState
void RenderStateBlock::pushState( const RenderState& state, u32 stateBit )
{
    DC_BREAK_IF( m_mask & BIT( stateBit ), "a state setting could not be overriden" );

    // Push a state to a state block
    m_states.push_back( state );

    // Push a state bit
    m_stateBits.push_back( BIT( stateBit ) );

    // Update a state block bitmask
    m_mask = m_mask | BIT( stateBit );
}

// -------------------------------------------------------------------------- RenderStateStack -------------------------------------------------------------------------- //

// ** RenderStateStack::RenderStateStack
RenderStateStack::RenderStateStack( s32 maxStateBlocks, s32 maxStackSize )
    : m_allocator( maxStateBlocks * sizeof( RenderStateBlock ) + sizeof( RenderStateBlock* ) * maxStackSize )
    , m_size( 0 )
    , m_stack( NULL )
{
    m_stack = reinterpret_cast<const RenderStateBlock**>( m_allocator.allocate( sizeof( RenderStateBlock* ) * maxStackSize ) );
}

// ** RenderStateStack::push
RenderStateBlock& RenderStateStack::push( void )
{
    DC_ABORT_IF( (size() + 1) >= MaxStateStackDepth, "stack overflow" );
    RenderStateBlock* block = new( m_allocator.allocate( sizeof( RenderStateBlock ) ) ) RenderStateBlock;
    m_stack[m_size++] = block;
    return *block;
}

// ** RenderStateStack::pop
void RenderStateStack::pop( void )
{
    DC_ABORT_IF( size() == 0, "stack underflow" );
    m_stack[m_size--] = NULL;
}

// ** RenderStateStack::size
s32 RenderStateStack::size( void ) const
{
    return m_size;
}

// ** RenderStateStack::states
const RenderStateBlock** RenderStateStack::states( void ) const
{
    return m_stack;
}

} // namespace Scene

DC_END_DREEMCHEST