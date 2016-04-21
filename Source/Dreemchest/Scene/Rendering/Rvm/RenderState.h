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

#ifndef __DC_Scene_RenderState_H__
#define __DC_Scene_RenderState_H__

#include "../../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    // ------------------------------------------------------------------------------------------------
    enum ShaderInputFeatures {
          ShaderInputNormal
        , ShaderInputColor
        , ShaderInputUv0
        , ShaderInputUv1
        , ShaderInputUv2
        , ShaderInputUv3
        , ShaderInputUv4
        , TotalInputFeatures
        , InputFeaturesOffset = 0
    };

    enum ShaderResourceFeatures {
          ShaderTexture0
        , ShaderTexture1
        , ShaderTexture2
        , ShaderTexture3
        , TotalResourceFeatures
        , ResourceFeaturesOffset = TotalInputFeatures
    };

    enum ShaderMaterialFeatures {
          ShaderAmbientColor
        , ShaderPointLight
        , TotalMaterialFeatures
        , MaterialFeaturesOffset = ResourceFeaturesOffset + TotalResourceFeatures
    };

    enum UbershaderFeatures {
          FeatureInputNormal        = BIT( ShaderInputNormal     + InputFeaturesOffset )
        , FeatureInputColor         = BIT( ShaderInputColor      + InputFeaturesOffset )
        , FeatureInputUv0           = BIT( ShaderInputUv0        + InputFeaturesOffset )
        , FeatureInputUv1           = BIT( ShaderInputUv1        + InputFeaturesOffset )
        , FeatureInputUv2           = BIT( ShaderInputUv2        + InputFeaturesOffset )
        , FeatureInputUv3           = BIT( ShaderInputUv3        + InputFeaturesOffset )
        , FeatureInputUv4           = BIT( ShaderInputUv4        + InputFeaturesOffset )
        , FeatureTexture0           = BIT( ShaderTexture0        + ResourceFeaturesOffset )  
        , FeatureTexture1           = BIT( ShaderTexture1        + ResourceFeaturesOffset )  
        , FeatureTexture2           = BIT( ShaderTexture2        + ResourceFeaturesOffset )  
        , FeatureTexture3           = BIT( ShaderTexture3        + ResourceFeaturesOffset )  
        , FeatureAmbientColor       = BIT( ShaderAmbientColor    + MaterialFeaturesOffset )
    };
    // ------------------------------------------------------------------------------------------------

    //! A maximum number of state blocks that can be pushed onto a state stack.
    enum { MaxStateStackDepth = 4 };

    //! Render state defines a single state change.
    struct RenderState {
        //! Available constant buffers.
        enum ConstantBufferType {
              GlobalConstants       //!< A constant buffer that stores global scene settings (ambient color, fog type, etc.).
            , PassConstants         //!< A constant buffer that stores a pass variables (view-projection matrix, light color, etc.).
            , InstanceConstants     //!< A constant buffer that stores instance variables (model matrix, instance color, etc.).
            , MaterialConstants     //!< A constant buffer that stores material variables (diffuse color, emission, etc.).
            , LightConstants        //!< A constant buffer that stores light variables (color, position, etc.).
            , MaxConstantBuffers    //!< A maximum number of supported constant buffers.
        };

        //! Available texture samplers.
        enum TextureSampler {
              Texture0
            , Texture1
            , Texture2
            , MaxTextureSamplers    //!< A maximum number of supported texture samplers.
        };

        //! Available render state types.
        enum Type {
              VertexBuffer      = 0                                     //!< Binds an input vertex buffer.
            , IndexBuffer       = VertexBuffer      + 1                    //!< Binds an input index buffer.
            , InputLayout       = IndexBuffer       + 1                    //!< Binds an input layout.
            , ConstantBuffer    = InputLayout       + 1                    //!< Binds a constant buffer.
            , Shader            = ConstantBuffer    + MaxConstantBuffers   //!< Binds a program instance.
            , RenderTarget      = Shader            + 1                    //!< Binds a render target.
            , Blending          = RenderTarget      + 1                    //!< Sets a blend function
            , DepthState        = Blending          + 1                    //!< Sets a depth test function and a reference value.
            , AlphaTest         = DepthState        + 1                    //!< Sets an alpha test function and a reference value.
            , Texture           = AlphaTest         + 1                    //!< Binds a texture to a sampler #(Type.Texture + index).
            , TotalStates       = Texture           + MaxTextureSamplers   //!< A total number of available render states.
        };

                                        //! Constructs an empty RenderState instance.
                                        RenderState( void );

                                        //! Constructs a RenderState instance of specified type.
                                        RenderState( Type type, s32 id );

                                        //! Constructs a depth render state instance.
                                        RenderState( Renderer::Compare function, bool write );

                                        //! Constructs an alpha test render state instance.
                                        RenderState( Renderer::Compare function, f32 reference );

                                        //! Constructs a constant buffer binding state.
                                        RenderState( s32 id, ConstantBufferType type );

                                        //! Constructs a blend function render state.
                                        RenderState( Renderer::BlendFactor src, Renderer::BlendFactor dst );

                                        //! Constructs a blend function render state.
                                        RenderState( s32 id, TextureSampler sampler );

                                        //! Constructs a render target state.
                                        RenderState( s32 id, const Rect& viewport );

        Type                            type;           //!< Render state type.
        union {
            s16                         id;             //!< A resource identifier to be bound to a pipeline.

            struct {
                s32                     id;             //!< Buffer ID to be bound.
                ConstantBufferType      type;           //!< Constant buffer index.
            } constantBuffer;                           //!< Bind constant buffer data.

            struct {
                Renderer::BlendFactor   src;            //!< A source blend factor.
                Renderer::BlendFactor   dst;            //!< A destination blend factor.
            } blend;                                    //!< Blend mode to be set.

            struct {
                Renderer::Compare       function;       //!< A depth tesing function.
                bool                    write;          //!< Enables or disables writing to a depth buffer.
            } depth;                                    //!< A depth testing state to be set.

            struct {
                Renderer::Compare       function;       //!< An alpha test function.
                f32                     reference;      //!< An alpha test function reference value.
            } alpha;                                    //!< An alpha testing state to be set.

            struct {
                s32                     id;             //!< Texture ID to be bound.
                TextureSampler          sampler;        //!< A sampler index to bind texture to.
            } texture;                                  //!< A texture sampler data.

            struct {
                s32                     id;             //!< Render target identifier.
                f32                     viewport[4];    //!< A target viewport.
            } renderTarget;                             //!< A render target state to be set.                              
        };
    };

    //! Render state block agregates a set of state changes.
    class RenderStateBlock {
    public:

                                        //! Constructs a RenderStateBlock block.
                                        RenderStateBlock( void );

        //! Binds a vertex buffer to a pipeline.
        void                            bindVertexBuffer( s32 id );

        //! Binds an index buffer to a pipeline.
        void                            bindIndexBuffer( s32 id );

        //! Binds an input layout to a pipeline.
        void                            bindInputLayout( s32 id );

        //! Binds a constant buffer to a pipeline.
        void                            bindConstantBuffer( s32 id, RenderState::ConstantBufferType type );

        //! Binds a program to a pipeline.
        void                            bindProgram( s32 id );

        //! Binds a texture to a specified sampler.
        void                            bindTexture( s32 id, RenderState::TextureSampler sampler );

        //! Sets a blend function.
        void                            setBlend( Renderer::BlendFactor src, Renderer::BlendFactor dst );

        //! Sets a depth state.
        void                            setDepthState( Renderer::Compare function, bool write );

        //! Enables a ubershader features.
        void                            enableFeatures( u64 bits );

        //! Disables a ubershader features.
        void                            disableFeatures( u64 bits );

        //! Sets an alpha test function.
        void                            setAlphaTest( Renderer::Compare function, f32 reference );

        //! Sets a render target.
        void                            setRenderTarget( s32 id, const Rect& viewport );

        //! Disables an alpha testing.
        void                            disableAlphaTest( void );

        //! Disables blending
        void                            disableBlending( void );

        //! Returns a state block mask.
        u32                             mask( void ) const;

        //! Returns a total number of states inside this block.
        s32                             stateCount( void ) const;

        //! Returns a state at specified index.
        const RenderState&              state( s32 index ) const;

        //! Returns a state bit at specified index.
        u32                             stateBit( s32 index ) const;

        //! Returns a feature set enabled by a state block.
        u64                             features( void ) const;

        //! Returns a feature mask exposed by a state block.
        u64                             featureMask( void ) const;

    private:

        //! Pushes a new state to a block.
        void                            pushState( const RenderState& state, u32 stateBit );

    private:

        u32                             m_mask;         //!< A bit mask of state changes that are preset inside this state block.
        u64                             m_features;     //!< A shader feature set.
        u64                             m_featureMask;  //!< A shader feature mask.
        Array<u32>                      m_stateBits;    //!< An array of state bits.
        Array<RenderState>              m_states;       //!< An array of state changes.
    };

    // ** RenderStateBlock::mask
    NIMBLE_INLINE u32 RenderStateBlock::mask( void ) const
    {
        return m_mask;
    }

    //! Returns a feature set enabled by a state block.
    NIMBLE_INLINE u64 RenderStateBlock::features( void ) const
    {
        return m_features;
    }

    //! Returns a feature mask exposed by a state block.
    NIMBLE_INLINE u64 RenderStateBlock::featureMask( void ) const
    {
        return m_featureMask;
    }

    // ** RenderStateBlock::stateCount
    NIMBLE_INLINE s32 RenderStateBlock::stateCount( void ) const
    {
        return static_cast<s32>( m_states.size() );
    }

    // ** RenderStateBlock::state
    NIMBLE_INLINE const RenderState& RenderStateBlock::state( s32 index ) const
    {
        DC_ABORT_IF( index < 0 || index >= stateCount(), "index is out of range" );
        return m_states[index];
    }

    // ** RenderStateBlock::stateBit
    NIMBLE_INLINE u32 RenderStateBlock::stateBit( s32 index ) const
    {
        DC_ABORT_IF( index < 0 || index >= stateCount(), "index is out of range" );
        return m_stateBits[index];
    }

    //! Render state stack.
    class RenderStateStack {
    public:

                                    //! Constructs a RenderStateStack instance.
                                    RenderStateStack( s32 maxStateBlocks, s32 maxStackSize );

        //! Allocates and pushes a new state block onto the stack.
        RenderStateBlock&           push( void );

        //! Pops a state block from a top of the stack.
        void                        pop( void );

        //! Returns the stack size.
        s32                         size( void ) const;

        //! Returns the stack pointer.
        const RenderStateBlock**    states( void ) const;

    private:

        LinearAllocator             m_allocator;    //!< Allocates a state block instances.
        const RenderStateBlock**    m_stack;        //!< State blocks pushed onto a stack.
        s32                         m_size;         //!< Current stack size.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderState_H__    */