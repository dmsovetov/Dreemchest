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
    , resourceId( id )
{
}

// ** RenderState::RenderState
RenderState::RenderState( Renderer::TriangleFace face )
    : type( CullFace )
{
    cullFace = face;
}

// ** RenderState::RenderState
RenderState::RenderState( Renderer::Compare function, bool write )
    : type( DepthState )
{
    compareFunction = function;
    data.depthWrite = write;
}

// ** RenderState::RenderState
RenderState::RenderState( Renderer::Compare function, f32 reference )
    : type( AlphaTest )
{
    compareFunction     = function;
    data.alphaReference = static_cast<u8>( reference * 255 );
}

// ** RenderState::RenderState
RenderState::RenderState( s32 id, ConstantBufferType type )
    : type( ConstantBuffer )
{
    resourceId = id;
    data.index = type;
}

// ** RenderState::RenderState
RenderState::RenderState( Renderer::BlendFactor src, Renderer::BlendFactor dst )
    : type( Blending )
{
    data.blend = (src << 4) | dst;
}

// ** RenderState::RenderState
RenderState::RenderState( s32 id, TextureSampler sampler, Renderer::RenderTarget::Attachment attachment )
    : type( Texture )
{
    resourceId = id;
    data.index = sampler | (attachment << 4);
}

// -------------------------------------------------------------------------- RenderStateBlock -------------------------------------------------------------------------- //

// ** RenderStateBlock::RenderStateBlock
RenderStateBlock::RenderStateBlock( void )
    : m_mask( 0 )
    , m_features( 0 )
    , m_featureMask( ~0 )
    , m_count( 0 )
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

// ** RenderStateBlock::bindInputLayout
void RenderStateBlock::bindInputLayout( s32 id )
{
    pushState( RenderState( RenderState::InputLayout, id ), RenderState::InputLayout );
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

// ** RenderStateBlock::bindRenderedTexture
void RenderStateBlock::bindRenderedTexture( u8 renderTarget, RenderState::TextureSampler sampler, Renderer::RenderTarget::Attachment attachment )
{
    pushState( RenderState( -renderTarget, sampler, attachment ), RenderState::Texture + sampler );
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

// ** RenderStateBlock::enableFeatures
void RenderStateBlock::enableFeatures( u64 bits )
{
    m_features = m_features | bits;
}

// ** RenderStateBlock::disableFeatures
void RenderStateBlock::disableFeatures( u64 bits )
{
    m_featureMask = m_featureMask & ~bits;
}

// ** RenderStateBlock::setAlphaTest
void RenderStateBlock::setAlphaTest( Renderer::Compare function, f32 reference )
{
    pushState( RenderState( function, reference ), RenderState::AlphaTest );
}

// ** RenderStateBlock::setCullFace
void RenderStateBlock::setCullFace( Renderer::TriangleFace face )
{
    pushState( RenderState( face ), RenderState::CullFace );
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
    DC_ABORT_IF( m_count + 1 >= MaxStates, "state block overflow" );

    // Push a state to a state block
    m_states[m_count] = state;
    m_stateBits[m_count] = BIT( stateBit );
    m_count++;

    // Update a state block bitmask
    m_mask = m_mask | BIT( stateBit );
}

// ---------------------------------------------------------------------------- StateScope ------------------------------------------------------------------------------ //

// ** StateScope::StateScope
StateScope::StateScope( RenderStateStack& stack, RenderStateBlock* stateBlock )
    : m_stack( stack )
    , m_stateBlock( stateBlock )
{
}

// ** StateScope::StateScope
StateScope::StateScope( StateScope& other )
    : m_stack( other.m_stack )
    , m_stateBlock( other.m_stateBlock )
{
    other.m_stateBlock = NULL;
}

// ** StateScope::~StateScope
StateScope::~StateScope( void )
{
    if( m_stateBlock ) {
        m_stack.pop();
    }
}

// ** StateScope::operator =
const StateScope& StateScope::operator = ( StateScope& other )
{
    m_stack      = other.m_stack;
    m_stateBlock = other.m_stateBlock;
    other.m_stateBlock = NULL;
    return *this;
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

// ** RenderStateStack::newScope
StateScope RenderStateStack::newScope( void )
{
    DC_ABORT_IF( (size() + 1) >= MaxStateStackDepth, "stack overflow" );
    RenderStateBlock* block = new( m_allocator.allocate( sizeof( RenderStateBlock ) ) ) RenderStateBlock;
    return push( block );
}

// ** RenderStateStack::push
StateScope RenderStateStack::push( const RenderStateBlock* block )
{
    if( block == NULL ) {
        return StateScope( *this, NULL );
    }

    for( s32 i = m_size; i > 0; i-- ) {
        m_stack[i] = m_stack[i - 1];
    }
    m_stack[0] = block;
    m_size++;

    return StateScope( *this, const_cast<RenderStateBlock*>( block ) );
}

// ** RenderStateStack::pop
void RenderStateStack::pop( void )
{
    DC_ABORT_IF( size() == 0, "stack underflow" );

    for( s32 i = 0; i < m_size - 1; i++ ) {
        m_stack[i] = m_stack[i + 1];
    }

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