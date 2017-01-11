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
#include "PipelineFeatureLayout.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ---------------------------------------------------------------------------- State ---------------------------------------------------------------------------- //

// ** State::State
State::State( void )
    : type(TotalStates)
{
}

// ** State::State
State::State(VertexBuffer_ id)
    : resourceId(id)
    , type(BindVertexBuffer)
{
}

// ** State::State
State::State(IndexBuffer_ id)
    : resourceId(id)
    , type(BindIndexBuffer)
{
}

// ** State::State
State::State(FeatureLayout id)
    : resourceId(id)
    , type(SetFeatureLayout)
{
}

// ** State::State
State::State(Program id)
    : resourceId(id)
    , type(BindProgram)
{
}

// ** State::State
State::State(InputLayout id)
    : resourceId(id)
    , type(SetInputLayout)
{
}

// ** State::State
State::State(TriangleFace face)
    : type(CullFace)
{
    cullFace = face;
}

// ** State::State
State::State(PolygonMode mode)
    : type(Rasterization)
{
    rasterization = mode;
}

// ** State::State
State::State(Compare function, bool write)
    : type(DepthState)
{
    compareFunction = function;
    data.depthWrite = write;
}

// ** State::State
State::State(Compare function, f32 reference)
    : type(AlphaTest)
{
    compareFunction = function;
    data.ref        = static_cast<u8>(reference * 255);
}

// ** State::State
State::State(ConstantBuffer_ id, u8 index)
    : type(BindConstantBuffer)
{
    resourceId = id;
    data.index = index;
}

// ** State::State
State::State(BlendFactor src, BlendFactor dst)
    : type( Blending )
{
    data.blend = (src << 4) | dst;
}

// ** State::State
State::State(Texture_ id, u8 sampler)
    : type(BindTexture)
{
    resourceId = id;
    data.index = sampler;
}
    
// ** State::State
State::State(TransientTexture id, u8 sampler)
    : type(BindTransientTexture)
{
    resourceId = static_cast<u8>(id);
    data.index = sampler;
}

// ** State::State
State::State(f32 factor, f32 units)
    : type(PolygonOffset)
{
    polygonOffset.factor = factor * 128.0f;
    polygonOffset.units  = units * 128.0f;
}

// ** State::State
State::State(StencilAction sfail, StencilAction dpfail, StencilAction dppass)
    : type(StencilOp)
{
    stencilOp.sfail  = sfail;
    stencilOp.dpfail = dpfail;
    data.dppass      = dppass;
}

// ** State::State
State::State(Compare function, u8 ref, u8 mask)
    : type(StencilFunc)
{
    stencilFunction.op   = function;
    stencilFunction.mask = mask;
    data.ref             = ref;
}

// ** State::sourceBlendFactor
BlendFactor State::sourceBlendFactor() const
{
    return static_cast<BlendFactor>(data.blend >> 4);
}

// ** State::destBlendFactor
BlendFactor State::destBlendFactor() const
{
    return static_cast<BlendFactor>(data.blend & 0xF);
}
    
// ** State::alphaReference
f32 State::alphaReference() const
{
    return data.ref / 255.0f;
}
    
// ** State::polygonOffsetFactor
f32 State::polygonOffsetFactor() const
{
    return polygonOffset.factor / 128.0f;
}

// ** State::polygonOffsetUnits
f32 State::polygonOffsetUnits() const
{
    return polygonOffset.units / 128.0f;
}
    
// ** State::polygonMode
PolygonMode State::polygonMode() const
{
    return static_cast<PolygonMode>(rasterization);
}
    
// ** State::function
Compare State::function() const
{
    return static_cast<Compare>(compareFunction);
}
    
// ** State::depthFail
StencilAction State::depthFail() const
{
    return static_cast<StencilAction>(stencilOp.dpfail);
}


// ** State::stencilFail
StencilAction State::stencilFail() const
{
    return static_cast<StencilAction>(stencilOp.sfail);
}


// ** State::depthStencilPass
StencilAction State::depthStencilPass() const
{
    return static_cast<StencilAction>(data.dppass);
}
    
// ** State::samplerIndex
s32 State::samplerIndex() const
{
    return data.index & 0xF;
}
    
// ** State::bit
u32 State::bit() const
{
    switch (type)
    {
        case BindConstantBuffer:
            return type + data.index;
        case BindTexture:
        case BindTransientTexture:
            return type + MaxConstantBuffers + data.index;
        default:
            break;
    }
    
    return type;
}
    
// ** State::bitmask
u32 State::bitmask() const
{
    return BIT(bit());
}

// ** State::nameFromType
String State::nameFromType(Type type)
{
    switch (type)
    {
        case BindVertexBuffer:      return "BindVertexBuffer";
        case BindIndexBuffer:       return "BindIndexBuffer";
        case SetInputLayout:        return "SetInputLayout";
        case SetFeatureLayout:      return "SetFeatureLayout";
        case BindProgram:           return "BindProgram";
        case Blending:              return "Blending";
        case PolygonOffset:         return "PolygonOffset";
        case DepthState:            return "DepthState";
        case AlphaTest:             return "AlphaTest";
        case CullFace:              return "CullFace";
        case Rasterization:         return "Rasterization";
        case StencilOp:             return "StencilOp";
        case StencilFunc:           return "StencilFunc";
        case StencilMask:           return "StencilMask";
        case ColorMask:             return "ColorMask";
        case BindConstantBuffer:    return "BindConstantBuffer";
        case BindTexture:           return "BindTexture";
        case BindTransientTexture:  return "BindTransientTexture";
        default:
            NIMBLE_NOT_IMPLEMENTED
    }
    
    return "";
}

// -------------------------------------------------------------------------- StateBlock -------------------------------------------------------------------------- //

// ** StateBlock::StateBlock
StateBlock::StateBlock(State* states, s16 maxStates)
    : m_mask(0)
    , m_userDefined(0)
    , m_userDefinedMask(~0)
    , m_resourceFeatures(0)
    , m_count(0)
    , m_maxStates(maxStates)
    , m_states(states)
{
}

// ** StateBlock::bindVertexBuffer
void StateBlock::bindVertexBuffer(VertexBuffer_ id)
{
    pushState(State(id));
}

// ** StateBlock::bindIndexBuffer
void StateBlock::bindIndexBuffer(IndexBuffer_ id)
{
    pushState(State(id));
}

// ** StateBlock::bindInputLayout
void StateBlock::bindInputLayout(InputLayout id)
{
    pushState(State(id));
}
    
// ** StateBlock::bindFeatureLayout
void StateBlock::bindFeatureLayout(FeatureLayout id)
{
    pushState(State(id));
}

// ** StateBlock::bindConstantBuffer
void StateBlock::bindConstantBuffer(ConstantBuffer_ id, u8 index)
{
    pushState(State(id, index));
    m_resourceFeatures = m_resourceFeatures | PipelineFeature::constantBuffer(index);
}

// ** StateBlock::bindProgram
void StateBlock::bindProgram(Program id)
{
    pushState(State(id));
}

// ** StateBlock::bindTexture
void StateBlock::bindTexture(Texture_ id, u8 sampler)
{
    pushState(State(id, sampler));
    m_resourceFeatures = m_resourceFeatures | PipelineFeature::sampler(sampler);
}

// ** StateBlock::bindTexture
void StateBlock::bindTexture(TransientTexture id, u8 sampler)
{
    pushState(State(id, sampler));
    m_resourceFeatures = m_resourceFeatures | PipelineFeature::sampler(sampler);
}

// ** StateBlock::setBlend
void StateBlock::setBlend(BlendFactor src, BlendFactor dst)
{
    pushState(State(src, dst));
}

// ** StateBlock::setDepthState
void StateBlock::setDepthState(Compare function, bool write)
{
    pushState(State(function, write));
}

// ** StateBlock::enableFeatures
void StateBlock::enableFeatures(PipelineFeatures features)
{
    m_userDefined = m_userDefined | PipelineFeature::user(features);
}

// ** StateBlock::disableFeatures
void StateBlock::disableFeatures(PipelineFeatures mask)
{
    m_userDefinedMask = m_userDefinedMask & ~PipelineFeature::user(mask);
}

// ** StateBlock::setPolygonOffset
void StateBlock::setPolygonOffset(f32 factor, f32 units)
{
    pushState(State(factor, units));
}

// ** StateBlock::disablePolygonOffset
void StateBlock::disablePolygonOffset( void )
{
    setPolygonOffset(0.0f, 0.0f);
}
    
// ** StateBlock::setPolygonMode
void StateBlock::setPolygonMode(PolygonMode value)
{
    pushState(State(value));
}

// ** StateBlock::setStencilOp
void StateBlock::setStencilOp(StencilAction sfail, StencilAction dfail, StencilAction dppass)
{
    pushState(State(sfail, dfail, dppass));
}

// ** StateBlock::setStencilMask
void StateBlock::setStencilMask(u8 value)
{
    State state;
    state.type = State::StencilMask;
    state.stencilFunction.mask = value;
    pushState(state);
}

// ** StateBlock::setStencilFunction
void StateBlock::setStencilFunction(Compare function, u8 ref, u8 value)
{
    pushState(State(function, ref, value));
}
    
// ** StateBlock::disableStencilTest
void StateBlock::disableStencilTest( void )
{
    setStencilFunction(CompareDisabled, 0, 0);
}

// ** StateBlock::setColorMask
void StateBlock::setColorMask(u8 value)
{
    State state;
    state.type = State::ColorMask;
    state.mask = value;
    pushState(state);
}

// ** StateBlock::setAlphaTest
void StateBlock::setAlphaTest(Compare function, f32 reference)
{
    pushState(State(function, reference));
}

// ** StateBlock::setCullFace
void StateBlock::setCullFace(TriangleFace face)
{
    pushState(State(face));
}

// ** StateBlock::disableBlending
void StateBlock::disableAlphaTest( void )
{
    setAlphaTest(CompareDisabled, 0.0f);
}

// ** StateBlock::disableBlending
void StateBlock::disableBlending( void )
{
    setBlend(BlendDisabled, BlendDisabled);
}

// ** StateBlock::pushState
void StateBlock::pushState(const State& state)
{
    NIMBLE_BREAK_IF(m_mask & BIT(state.bit()), "a state setting could not be overriden");
    NIMBLE_ABORT_IF(m_count + 1 > m_maxStates, "state block overflow");

    // Push a state to a state block
    m_states[m_count] = state;
    m_count++;

    // Update a state block bitmask
    m_mask = m_mask | BIT(state.bit());
}

// ---------------------------------------------------------------------------- StateScope ------------------------------------------------------------------------------ //

// ** StateScope::StateScope
StateScope::StateScope( StateStack& stack, StateBlock* stateBlock )
    : m_stack( stack )
    , m_stateBlock( stateBlock )
{
}

// ** StateScope::StateScope
StateScope::StateScope( const StateScope& other )
    : m_stack( other.m_stack )
    , m_stateBlock( other.m_stateBlock )
{
    NIMBLE_NOT_IMPLEMENTED
//    other.m_stateBlock = NULL;
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

// -------------------------------------------------------------------------- StateStack -------------------------------------------------------------------------- //

// ** StateStack::StateStack
StateStack::StateStack(s32 maxStateBlocks, s32 maxStackSize)
    : m_allocator(maxStateBlocks * sizeof(StateBlock) + sizeof(StateBlock*) * maxStackSize)
    , m_stack(NULL)
    , m_size(0)
    , m_maxStackSize(maxStackSize)
{
    reset();
}

// ** StateStack::newScope
StateScope StateStack::newScope( void )
{
    NIMBLE_ABORT_IF( (size() + 1) >= MaxStateStackDepth, "stack overflow" );

    void* allocated = m_allocator.allocate( sizeof( StateBlock ) );
    NIMBLE_ABORT_IF( allocated == NULL, "to much render state blocks allocated" );

    StateBlock* block = new( allocated ) StateBlock8;
    return push( block );
}

// ** StateStack::push
StateScope StateStack::push( const StateBlock* block )
{
    if( block == NULL ) {
        return StateScope( *this, NULL );
    }

    for( s32 i = m_size; i > 0; i-- ) {
        m_stack[i] = m_stack[i - 1];
    }
    m_stack[0] = block;
    m_size++;

    return StateScope( *this, const_cast<StateBlock*>( block ) );
}

// ** StateStack::pop
void StateStack::pop( void )
{
    NIMBLE_ABORT_IF( size() == 0, "stack underflow" );

    for( s32 i = 0; i < m_size - 1; i++ ) {
        m_stack[i] = m_stack[i + 1];
    }

    m_stack[m_size--] = NULL;
}

// ** StateStack::size
s32 StateStack::size( void ) const
{
    return m_size;
}

// ** StateStack::states
const StateBlock** StateStack::states( void ) const
{
    return m_stack;
}
    
// ** StateStack::reset
void StateStack::reset( void )
{
    memset(m_stack, 0, sizeof(m_stack[0]) * m_size);
    m_allocator.reset();
    m_stack = reinterpret_cast<const StateBlock**>(m_allocator.allocate(sizeof(StateBlock*) * m_maxStackSize));
    m_size = 0;
}
    
// ** StateStack::mergeBlocks
s32 StateStack::mergeBlocks(const StateBlock* const * stateBlocks, s32 count, State* states, s32 maxStates, StateMask& activeStateMask, PipelineFeatures& userDefined)
{
    PipelineFeatures userFeatures = 0;
    PipelineFeatures userFeaturesMask = ~0;
    PipelineFeatures resourceFeatures = 0;
    
    // Reset a bitmask of states that were already set
    activeStateMask = 0;
    
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
        if( (activeStateMask ^ block->mask()) == 0 )
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
            if( activeStateMask & stateMask )
            {
                continue;
            }
            
            NIMBLE_ABORT_IF(statesWritten >= maxStates, "to much render states");
            
            // Write a render state at specified index to an output array
            states[statesWritten++] = state;
            
            // Update an active state mask
            activeStateMask = activeStateMask | stateMask;
        }
    }
    
    // Compose a user defined feature mask
    userDefined = (userFeatures & userFeaturesMask) | resourceFeatures;
    
    return statesWritten;
}

} // namespace Renderer

DC_END_DREEMCHEST
