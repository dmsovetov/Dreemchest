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

    //! Vertex attributes that are passed to a shader.
    enum VertexAttributeFeatures
    {
          VertexNormal
        , VertexColor
        , VertexTangent
        , VertexBitangent
        , VertexPointSize
        , VertexTexCoord0
        , VertexTexCoord1
        , VertexTexCoord2
        , VertexTexCoord3
        , VertexTexCoord4
        , VertexTexCoord5
        , VertexTexCoord6
        , VertexTexCoord7
        , TotalVertexAttributeFeatures
    };

    //! An offset of sampler features.
    enum { SamplerFeaturesOffset = TotalVertexAttributeFeatures };

    //! Texture samplers that can be enabled upon rendering.
    enum SamplerFeatures
    {
          TextureSampler0
        , TextureSampler1
        , TextureSampler2
        , TextureSampler3
        , TextureSampler4
        , TextureSampler5
        , TextureSampler6
        , TextureSampler7
        , TotalSamplerFeatures
    };

    //! An offset of constant buffer features.
    enum { CBufferFeaturesOffset = SamplerFeaturesOffset + TotalSamplerFeatures };

    //! Constant buffers that can be bound upon rendering.
    enum ConstantBufferFeatures
    {
          ConstantBuffer0
        , ConstantBuffer1
        , ConstantBuffer2
        , ConstantBuffer3
        , ConstantBuffer4
        , ConstantBuffer5
        , ConstantBuffer6
        , ConstantBuffer7
        , TotalConstantBufferFeatures
    };

    //! An offset of user-defined shader features.
    enum { UserDefinedFeaturesOffset = CBufferFeaturesOffset + TotalConstantBufferFeatures };

    //! A total number of available user-defined feature bits.
    enum { MaxUserDefinedFeatureBits = 64 - UserDefinedFeaturesOffset };

    //! Available user-defined shader features used by a scene renderer. 
    enum SceneShaderFeatures
    {
          ShaderAmbientColor        = 0x1
        , ShaderEmissionColor       = 0x2
        , ShaderPointLight          = 0x4
        , ShaderSpotLight           = 0x8
        , ShaderDirectionalLight    = 0xC
        , ShaderFogColor            = 0x10
        , ShaderSpecularColor       = 0x20
        , ShaderRimLight            = 0x40
        , ShaderShadowFiltering1    = 0x80
        , ShaderShadowFiltering2    = 0x100
        , ShaderShadowFiltering3    = 0x180
    };

    // ------------------------------------------------------------------------------------------------

    //! A maximum number of state blocks that can be pushed onto a state stack.
    enum { MaxStateStackDepth = 10 };

    //! Render state defines a single state change.
    struct State
    {
        //! Available constant buffers.
        enum ConstantBufferType
        {
              GlobalConstants       //!< A constant buffer that stores global scene settings (ambient color, fog type, etc.).
            , PassConstants         //!< A constant buffer that stores a pass variables (view-projection matrix, light color, etc.).
            , InstanceConstants     //!< A constant buffer that stores instance variables (model matrix, instance color, etc.).
            , MaterialConstants     //!< A constant buffer that stores material variables (diffuse color, emission, etc.).
            , LightConstants        //!< A constant buffer that stores light variables (color, position, etc.).
            , ShadowConstants       //!< A constant buffer that stores shadow variables (transform, near, far, etc.).
            , ClippingPlanes        //!< A constant buffer that stores clipping planes.
            , MaxConstantBuffers    //!< A maximum number of supported constant buffers.
        };

        //! Available texture samplers.
        enum TextureSampler
        {
              Texture0
            , Texture1
            , Texture2
            , Texture3
            , Texture4
            , Texture5
            , Texture6
            , Texture7
            , MaxTextureSamplers    //!< A maximum number of supported texture samplers.
        };

        //! Available render state types.
        enum Type
        {
              VertexBuffer      = 0                                         //!< Binds an input vertex buffer.
            , IndexBuffer       = VertexBuffer      + 1                     //!< Binds an input index buffer.
            , InputLayout       = IndexBuffer       + 1                     //!< Binds an input layout.
            , ConstantBuffer    = InputLayout       + 1                     //!< Binds a constant buffer.
            , Shader            = ConstantBuffer    + MaxConstantBuffers    //!< Binds a program instance.
            , Blending          = Shader            + 1                     //!< Sets a blend function.
            , PolygonOffset     = Blending          + 1                     //!< Sets a polygon offset fill.
            , DepthState        = PolygonOffset     + 1                     //!< Sets a depth test function and a reference value.
            , AlphaTest         = DepthState        + 1                     //!< Sets an alpha test function and a reference value.
            , CullFace          = AlphaTest         + 1                     //!< Sets a cull face mode.
            , Texture           = CullFace          + 1                     //!< Binds a texture to a sampler #(Type.Texture + index).
            , TotalStates       = Texture           + MaxTextureSamplers    //!< A total number of available render states.
        };

                                        //! Constructs an empty State instance.
                                        State( void );

                                        //! Constructs a State instance of specified type.
                                        State( Type type, s32 id );

                                        //! Constructs a cull face render state instance.
                                        State( TriangleFace face );

                                        //! Constructs a depth render state instance.
                                        State( Compare function, bool write );

                                        //! Constructs an alpha test render state instance.
                                        State( Renderer::Compare function, f32 reference );

                                        //! Constructs a constant buffer binding state.
                                        State( s32 id, ConstantBufferType type );

                                        //! Constructs a blend function render state.
                                        State( BlendFactor src, BlendFactor dst );

                                    #if DEV_DEPRECATED_HAL
                                        //! Constructs a texture binding state.
                                        State( s32 id, TextureSampler sampler, RenderTarget::Attachment attachment = RenderTarget::Depth );
                                    #else
                                        //! Constructs a texture binding state.
                                        State( s32 id, TextureSampler sampler, u8 attachment = 0 );
                                    #endif  /*  #if DEV_DEPRECATED_HAL  */

                                        //! Constructs a polygon offset state.
                                        State( f32 factor, f32 units );

        union
        {
            u16                         resourceId;         //!< Resource identifier to be bound to a pipeline.
            u16                         compareFunction;    //!< A compare function value.
            u16                         cullFace;           //!< A face value.
            struct
            {
                s8                      factor;             //!< A polygon offset factor.
                s8                      units;              //!< A polygon offset units.
            } polygonOffset;
        };

        union
        {
            u8                          blend;              //!< Source blend factor in high bits and destination blend factor in low bits.
            u8                          index;              //!< A constant buffer binding slot or sampler index.
            bool                        depthWrite;         //!< Enables or disables writing to a depth buffer.
            u8                          alphaReference;     //!< An alpha test function reference value.
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
        void                            bindVertexBuffer( s32 id );

        //! Binds an index buffer to a pipeline.
        void                            bindIndexBuffer( s32 id );

        //! Binds an input layout to a pipeline.
        void                            bindInputLayout( s32 id );

        //! Binds a constant buffer to a pipeline.
        void                            bindConstantBuffer( s32 id, State::ConstantBufferType type );

        //! Binds a program to a pipeline.
        void                            bindProgram( s32 id );

        //! Binds a texture to a specified sampler.
        void                            bindTexture( s32 id, State::TextureSampler sampler );

    #if DEV_DEPRECATED_HAL
        //! Binds a rendered texture to a specified sampler.
        void                            bindRenderedTexture( u8 renderTarget, State::TextureSampler sampler, RenderTarget::Attachment attachment = RenderTarget::Color0 );
    #else
        //! Binds a rendered texture to a specified sampler.
        void                            bindRenderedTexture( u8 renderTarget, State::TextureSampler sampler, u8 attachment = 0 );
    #endif  /*  #if DEV_DEPRECATED_HAL  */

        //! Sets a blend function.
        void                            setBlend( BlendFactor src, BlendFactor dst );

        //! Sets a depth state.
        void                            setDepthState( Compare function, bool write );

        //! Enables a ubershader features.
        void                            enableFeatures( u64 bits );

        //! Disables a ubershader features.
        void                            disableFeatures( u64 bits );

        //! Sets a polygon offset values.
        void                            setPolygonOffset( f32 factor, f32 units );

        //! Disables a polygon offset.
        void                            disablePolygonOffset( void );

        //! Sets an alpha test function.
        void                            setAlphaTest( Compare function, f32 reference );

        //! Sets a cull face side.
        void                            setCullFace( TriangleFace face );

        //! Disables an alpha testing.
        void                            disableAlphaTest( void );

        //! Disables blending
        void                            disableBlending( void );

        //! Returns a state block mask.
        u32                             mask( void ) const;

        //! Returns a total number of states inside this block.
        s32                             stateCount( void ) const;

        //! Returns a state at specified index.
        const State&                    state( s32 index ) const;

        //! Returns a state bit at specified index.
        u32                             stateBit( s32 index ) const;

        //! Returns a feature set enabled by a state block.
        u64                             features( void ) const;

        //! Returns a feature mask exposed by a state block.
        u64                             featureMask( void ) const;

    private:

        //! Pushes a new state to a block.
        void                            pushState( const State& state, u32 stateBit );

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
        
        //! Clears a render state stack.
        void                        clear( void );

        //! Returns the stack pointer.
        const StateBlock**    states( void ) const;

    private:

        //! Pops a state block from a top of the stack.
        void                        pop( void );

    private:

        LinearAllocator             m_allocator;    //!< Allocates a state block instances.
        const StateBlock**          m_stack;        //!< State blocks pushed onto a stack.
        s32                         m_size;         //!< Current stack size.
    };

} // namespace Renderer

DC_END_DREEMCHEST

#endif    /*    !__DC_Renderer_RenderState_H__    */
