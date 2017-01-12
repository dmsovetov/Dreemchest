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

#ifndef __DC_Renderer_RenderState_H__
#define __DC_Renderer_RenderState_H__

#include "Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

    //! A maximum number of state blocks that can be pushed onto a state stack.
    enum { MaxStateStackDepth = 10 };

    //! Render state defines a single state change.
    struct State
    {
        //!< A maximum number of supported constant buffers.
        enum { MaxConstantBuffers = 8 };

        //!< A maximum number of supported texture samplers.
        enum { MaxTextureSamplers = 8 };

        //! Available render state types.
        enum Type
        {
              BindVertexBuffer      //!< Binds an input vertex buffer.
            , BindIndexBuffer       //!< Binds an input index buffer.
            , SetInputLayout        //!< Binds an input layout.
            , SetFeatureLayout      //!< Activates a pipeline feature layout.
            , BindProgram           //!< Binds a program instance.
            , Blending              //!< Sets a blend function.
            , PolygonOffset         //!< Sets a polygon offset fill.
            , DepthState            //!< Sets a depth test function and a reference value.
            , AlphaTest             //!< Sets an alpha test function and a reference value.
            , CullFace              //!< Sets a cull face mode.
            , Rasterization         //!< Sets a polygon rasterization mode.
            , StencilOp             //!< Sets stencil test actions.
            , StencilFunc           //!< Sets function and reference value for stencil testing.
            , StencilMask           //!< Control the writing of individual bits in the stencil planes
            , ColorMask             //!< Sets a color buffer write mask.
            , BindConstantBuffer    //!< Binds a constant buffer.
            , BindTexture           //!< Binds a persistent texture to a sampler.
            , BindTransientTexture  //!< Binds a transient texture to a sampler.
            , TotalStates
        };

                                        //! Constructs an empty State instance.
                                        State( void );

                                        //! Constructs a vertex buffer state instance.
                                        State(VertexBuffer_ id);
        
                                        //! Constructs an index buffer state instance.
                                        State(IndexBuffer_ id);
        
                                        //! Constructs an input layout state instance.
                                        State(InputLayout id);
                                        
                                        //! Constructs a feature layout state instance.
                                        State(FeatureLayout id);
        
                                        //! Constructs a program state instance.
                                        State(Program id);

                                        //! Constructs a cull face render state instance.
                                        State(TriangleFace face);
        
                                        //! Constructs a stencil op render state instance.
                                        State(StencilAction sfail, StencilAction dpfail, StencilAction dppass);
        
                                        //! Constructs a stencil function render state instance.
                                        State(Compare function, u8 ref, u8 mask);
        
                                        //! Constructs a polygon mode state instance.
                                        State( PolygonMode mode );

                                        //! Constructs a depth render state instance.
                                        State( Compare function, bool write );

                                        //! Constructs an alpha test render state instance.
                                        State( Compare function, f32 reference );

                                        //! Constructs a constant buffer binding state.
                                        State( ConstantBuffer_ id, u8 index );

                                        //! Constructs a blend function render state.
                                        State( BlendFactor src, BlendFactor dst );

                                        //! Constructs a transient texture binding state.
                                        State(TransientTexture id, u8 sampler);
        
                                        //! Constructs a rendered texture binding state.
                                        State( Texture_ id, u8 sampler );

                                        //! Constructs a polygon offset state.
                                        State( f32 factor, f32 units );
        
        //! Returns a source blend factor stored in a state.
        BlendFactor                     sourceBlendFactor() const;

        //! Returns a destination blend factor stored in a state.
        BlendFactor                     destBlendFactor() const;
        
        //! Returns an alpha reference value converted to a float.
        f32                             alphaReference() const;
        
        //! Returns a polygon offset factor value.
        f32                             polygonOffsetFactor() const;
        
        //! Returns a polygon offset units value.
        f32                             polygonOffsetUnits() const;
        
        //! Returns a stored polygon mode.
        PolygonMode                     polygonMode() const;
        
        //! Returns a rendering state compare function.
        Compare                         function() const;
        
        //! Returns a depth fail stencil action.
        StencilAction                   depthFail() const;
        
        //! Returns a stencil fail stencil action.
        StencilAction                   stencilFail() const;
        
        //! Returns a depth and stencil pass action.
        StencilAction                   depthStencilPass() const;
        
        //! Returns a sampler index.
        s32                             samplerIndex() const;
        
        //! Returns a rendering state bit index.
        u32                             bit() const;
        
        //! Returns a rendering state bit mask.
        StateMask                       bitmask() const;
        
        //! Returns a rendering state name from an identifier (used for debugging purposes).
        static String                   nameFromType(Type type);
        
        union
        {
            ResourceId                  resourceId;         //!< Resource identifier to be bound to a pipeline.
            u16                         compareFunction;    //!< A compare function value.
            u16                         cullFace;           //!< A face value.
            u16                         rasterization;      //!< A polygon rasterization mode.
            u16                         mask;               //!< A color buffer write mask.
            struct
            {
                s8                      factor;             //!< A polygon offset factor.
                s8                      units;              //!< A polygon offset units.
            } polygonOffset;
            struct
            {
                u8                      op;                 //!< A stencil compare function.
                u8                      mask;               //!< A stencil mask value.
            } stencilFunction;
            struct
            {
                u8                      sfail;              //!< A stencil test fail operation.
                u8                      dpfail;             //!< A depth test fail operation.
            } stencilOp;
        };

        union
        {
            u8                          blend;              //!< Source blend factor in high bits and destination blend factor in low bits.
            u8                          index;              //!< A constant buffer binding slot or sampler index.
            bool                        depthWrite;         //!< Enables or disables writing to a depth buffer.
            u8                          ref;                //!< An alpha test function or stencil op reference value.
            u8                          dppass;             //!< Both depth and stencil test pass operation.
        } data;

        u8                              type;               //!< Render state type.
    };

    //! Render state block agregates a set of state changes.
    class StateBlock
    {
    public:

                                        //! Constructs a StateBlock block.
                                        StateBlock(State* states, s16 maxStates);

        //! Binds a vertex buffer to a pipeline.
        void                            bindVertexBuffer(VertexBuffer_ id);

        //! Binds an index buffer to a pipeline.
        void                            bindIndexBuffer(IndexBuffer_ id);

        //! Binds an input layout to a pipeline.
        void                            bindInputLayout(InputLayout id);
        
        //! Activates a pipeline feature layout.
        void                            bindFeatureLayout(FeatureLayout id);

        //! Binds a constant buffer to a pipeline.
        void                            bindConstantBuffer(ConstantBuffer_ id, u8 index);

        //! Binds a program to a pipeline.
        void                            bindProgram(Program id);

        //! Binds a texture to a specified sampler.
        void                            bindTexture(Texture_ id, u8 sampler);

        //! Binds a transient texture to a specified sampler.
        void                            bindTexture(TransientTexture id, u8 sampler);

        //! Sets a blend function.
        void                            setBlend(BlendFactor src, BlendFactor dst);

        //! Sets a depth state.
        void                            setDepthState(Compare function, bool write);

        //! Enables a ubershader features.
        void                            enableFeatures(PipelineFeatures features);

        //! Disables a ubershader features.
        void                            disableFeatures(PipelineFeatures mask);

        //! Sets a polygon offset values.
        void                            setPolygonOffset(f32 factor, f32 units);

        //! Disables a polygon offset.
        void                            disablePolygonOffset( void );
        
        //! Sets a polygon rasterization mode.
        void                            setPolygonMode(PolygonMode value);
        
        //! Sets a color buffer write mask.
        void                            setColorMask(u8 value);
        
        //! Sets a stencil operation state.
        void                            setStencilOp(StencilAction sfail, StencilAction dfail, StencilAction dppass);
        
        //! Sets a stencil mask value.
        void                            setStencilMask(u8 value);
    
        //! Sets a stencil mask value.
        void                            setStencilFunction(Compare function, u8 ref, u8 value = 0xFF);
        
        //! Disables a stencil buffer test.
        void                            disableStencilTest( void );

        //! Sets an alpha test function.
        void                            setAlphaTest(Compare function, f32 reference);

        //! Sets a cull face side.
        void                            setCullFace(TriangleFace face);

        //! Disables an alpha testing.
        void                            disableAlphaTest( void );

        //! Disables blending
        void                            disableBlending( void );

        //! Returns a state block mask.
        StateMask                       mask( void ) const;

        //! Returns a total number of states inside this block.
        s32                             stateCount( void ) const;

        //! Returns a state at specified index.
        const State&                    state(s32 index) const;

        //! Returns a feature set enabled by a state block.
        PipelineFeatures                userDefined( void ) const;

        //! Returns a feature mask exposed by a state block.
        PipelineFeatures                userDefinedMask( void ) const;
        
        //! Returns a feature mask exposed by a state block.
        PipelineFeatures                resourceFeatures( void ) const;

    private:

        //! Pushes a new state to a block.
        void                            pushState(const State& state);

    protected:

        StateMask                       m_mask;                 //!< A bit mask of state changes that are preset inside this state block.
        PipelineFeatures                m_userDefined;          //!< A shader feature set.
        PipelineFeatures                m_userDefinedMask;      //!< A shader feature mask.
        PipelineFeatures                m_resourceFeatures;     //!< 
        s16                             m_count;                //!< A total number of states stored inside a block.
        s16                             m_maxStates;            //!< A maximum number of states that can be stored inside a block.
        State*                          m_states;               //!< An array of state changes.
    };

    // ** StateBlock::mask
    NIMBLE_INLINE StateMask StateBlock::mask( void ) const
    {
        return m_mask;
    }

    // ** StateBlock::userDefined
    NIMBLE_INLINE PipelineFeatures StateBlock::userDefined( void ) const
    {
        return m_userDefined;
    }

    // ** StateBlock::userDefinedMask
    NIMBLE_INLINE PipelineFeatures StateBlock::userDefinedMask( void ) const
    {
        return m_userDefinedMask;
    }
    
    // ** StateBlock::resourceFeatures
    NIMBLE_INLINE PipelineFeatures StateBlock::resourceFeatures( void ) const
    {
        return m_resourceFeatures;
    }

    // ** StateBlock::stateCount
    NIMBLE_INLINE s32 StateBlock::stateCount( void ) const
    {
        return m_count;
    }

    // ** StateBlock::state
    NIMBLE_INLINE const State& StateBlock::state( s32 index ) const
    {
        NIMBLE_ABORT_IF( index < 0 || index >= stateCount(), "index is out of range" );
        return m_states[index];
    }
    
    //! Rendering state block of a fixed size.
    template<s32 TMaxStates>
    class FixedStateBlock : public StateBlock
    {
    public:
        
                                    //! Constructs a FixedStateBlock instance.
                                    FixedStateBlock();
        
                                    //! Constructs a copy of a FixedStateBlock instance.
                                    FixedStateBlock(const FixedStateBlock& other);
        
        //! Copies a fixed state block from another one.
        const FixedStateBlock&      operator = (const FixedStateBlock& other);
        
    private:
        
        State                       m_block[TMaxStates];    //!< An array of rendering states.
    };
    
    // ** FixedStateBlock::FixedStateBlock
    template<s32 TMaxStates>
    FixedStateBlock<TMaxStates>::FixedStateBlock()
        : StateBlock(m_block, TMaxStates)
    {
    }
    
    // ** FixedStateBlock::FixedStateBlock
    template<s32 TMaxStates>
    FixedStateBlock<TMaxStates>::FixedStateBlock(const FixedStateBlock& other)
        : StateBlock(m_block, TMaxStates)
    {
        this->operator = (other);
    }
    
    // ** FixedStateBlock::operator =
    template<s32 TMaxStates>
    const FixedStateBlock<TMaxStates>& FixedStateBlock<TMaxStates>::operator = (const FixedStateBlock& other)
    {
        m_mask              = other.m_mask;
        m_resourceFeatures  = other.m_resourceFeatures;
        m_userDefined       = other.m_userDefined;
        m_userDefinedMask   = other.m_userDefinedMask;
        m_count             = other.m_count;
        memcpy(m_block, other.m_block, sizeof(State) * other.stateCount());
        
        return *this;
    }
    
    typedef FixedStateBlock<4> StateBlock4;
    typedef FixedStateBlock<8> StateBlock8;
    typedef FixedStateBlock<12> StateBlock12;
    
    //! Forward declare a StateStack class.
    class StateStack;

    //! A render state stack scope.
    class StateScope
    {
    friend class StateStack;
    public:

                                    //! Performs a destructive copy of a state scope.
                                    StateScope( const StateScope& other );

                                    //! Pops a state block from a stack upon destruction.
                                    ~StateScope( void );

        //! Returns a pointer to a topmost state block.
        const StateBlock*           operator -> ( void ) const;
        StateBlock*                 operator -> ( void );

        //! Performs a destructive copy of a state scope.
        const StateScope&           operator = ( StateScope& other );

    private:

                                    //! Constructs a StateScope instance.
                                    StateScope( StateStack& stack, StateBlock* stateBlock );

    private:

        StateStack&                 m_stack;        //!< A state stack that issued this scope.
        StateBlock*                 m_stateBlock;   //!< A topmost state block.
    };

    // ** StateScope::operator ->
    NIMBLE_INLINE const StateBlock* StateScope::operator -> ( void ) const
    {
        return m_stateBlock;
    }

    // ** StateScope::operator ->
    NIMBLE_INLINE StateBlock* StateScope::operator -> ( void )
    {
        return m_stateBlock;
    }

    //! Render state stack.
    class StateStack
    {
    friend class StateScope;
    friend class RenderFrame;
    public:

                                    //! Constructs a StateStack instance.
                                    StateStack( s32 maxStateBlocks, s32 maxStackSize );

        //! Allocates and pushes a new state block onto the stack.
        StateScope                  newScope( void );

        //! Pushes a state block onto the stack.
        StateScope                  push( const StateBlock* block );

        //! Returns the stack size.
        s32                         size( void ) const;
        
        //! Resets a render state stack.
        void                        reset( void );

        //! Returns the stack pointer.
        const StateBlock**          states( void ) const;

    private:

        //! Pops a state block from a top of the stack.
        void                        pop( void );
        
        //! Pushes a new state block to stack.
        void                        push(const StateBlock& block);

    private:

        LinearAllocator             m_allocator;    //!< Allocates a state block instances.
        const StateBlock**          m_stack;        //!< State blocks pushed onto a stack.
        s32                         m_size;         //!< Current stack size.
        s32                         m_maxStackSize; //!< A maximum stack size.
    };

} // namespace Renderer

DC_END_DREEMCHEST

#endif    /*    !__DC_Renderer_RenderState_H__    */
