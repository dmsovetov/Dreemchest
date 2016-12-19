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
#include "Hal.h"

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
              VertexBuffer      = 0                                         //!< Binds an input vertex buffer.
            , IndexBuffer       = VertexBuffer      + 1                     //!< Binds an input index buffer.
            , InputLayout       = IndexBuffer       + 1                     //!< Binds an input layout.
            , FeatureLayout     = InputLayout       + 1                     //!< Activates a pipeline feature layout.
            , ConstantBuffer    = FeatureLayout     + 1                     //!< Binds a constant buffer.
            , Shader            = ConstantBuffer    + MaxConstantBuffers    //!< Binds a program instance.
            , Blending          = Shader            + 1                     //!< Sets a blend function.
            , PolygonOffset     = Blending          + 1                     //!< Sets a polygon offset fill.
            , DepthState        = PolygonOffset     + 1                     //!< Sets a depth test function and a reference value.
            , AlphaTest         = DepthState        + 1                     //!< Sets an alpha test function and a reference value.
            , CullFace          = AlphaTest         + 1                     //!< Sets a cull face mode.
            , Texture           = CullFace          + 1                     //!< Binds a texture to a sampler #(Type.Texture + index).
            , Rasterization     = Texture           + MaxTextureSamplers    //!< Sets a polygon rasterization mode.
            , StencilOp         = Rasterization     + 1                     //!< Sets stencil test actions.
            , StencilFunc       = StencilOp         + 1                     //!< Sets function and reference value for stencil testing.
            , StencilMask       = StencilFunc       + 1                     //!< Control the writing of individual bits in the stencil planes.
            , ColorMask         = StencilMask       + 1                     //!< Sets a color buffer write mask.
            , TotalStates                                                   //!< A total number of available render states.
        };

                                        //! Constructs an empty State instance.
                                        State( void );

                                        //! Constructs a State instance of specified type.
                                        State( Type type, PersistentResourceId id );

                                        //! Constructs a cull face render state instance.
                                        State( TriangleFace face );
        
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

                                        //! Constructs a rendered texture binding state.
                                        State( TransientRenderTarget id, u8 sampler, RenderTargetAttachment attachment );
        
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
        
        union
        {
            PersistentResourceId        resourceId;         //!< Resource identifier to be bound to a pipeline.
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

        //! A maximum number of states that can be stored inside a single block.
        enum { MaxStates = 8 };

                                        //! Constructs a StateBlock block.
                                        StateBlock( void );

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

        //! Binds a rendered texture to a specified sampler.
        void                            bindRenderedTexture(TransientRenderTarget renderTarget, u8 sampler, RenderTargetAttachment attachment = RenderTargetColor0);

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
        u32                             mask( void ) const;

        //! Returns a total number of states inside this block.
        s32                             stateCount( void ) const;

        //! Returns a state at specified index.
        const State&                    state(s32 index) const;

        //! Returns a state bit at specified index.
        u32                             stateBit(s32 index) const;

        //! Returns a feature set enabled by a state block.
        u64                             features( void ) const;

        //! Returns a feature mask exposed by a state block.
        u64                             featureMask( void ) const;

    private:

        //! Pushes a new state to a block.
        void                            pushState(const State& state, u32 stateBit);

    private:

        u32                             m_mask;                 //!< A bit mask of state changes that are preset inside this state block.
        u64                             m_features;             //!< A shader feature set.
        u64                             m_featureMask;          //!< A shader feature mask.
        u32                             m_stateBits[MaxStates]; //!< An array of state bits.
        State                           m_states[MaxStates];    //!< An array of state changes.
        s16                             m_count;                //!< A total number of states stored inside a block.
    };

    // ** StateBlock::mask
    NIMBLE_INLINE u32 StateBlock::mask( void ) const
    {
        return m_mask;
    }

    //! Returns a feature set enabled by a state block.
    NIMBLE_INLINE u64 StateBlock::features( void ) const
    {
        return m_features;
    }

    //! Returns a feature mask exposed by a state block.
    NIMBLE_INLINE u64 StateBlock::featureMask( void ) const
    {
        return m_featureMask;
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

    // ** StateBlock::stateBit
    NIMBLE_INLINE u32 StateBlock::stateBit( s32 index ) const
    {
        NIMBLE_ABORT_IF( index < 0 || index >= stateCount(), "index is out of range" );
        return m_stateBits[index];
    }
    
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
        const StateBlock**    states( void ) const;

    private:

        //! Pops a state block from a top of the stack.
        void                        pop( void );

    private:

        LinearAllocator             m_allocator;    //!< Allocates a state block instances.
        const StateBlock**          m_stack;        //!< State blocks pushed onto a stack.
        s32                         m_size;         //!< Current stack size.
        s32                         m_maxStackSize; //!< A maximum stack size.
    };

} // namespace Renderer

DC_END_DREEMCHEST

#endif    /*    !__DC_Renderer_RenderState_H__    */
