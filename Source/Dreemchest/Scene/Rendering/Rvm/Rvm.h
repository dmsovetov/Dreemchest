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

#ifndef __DC_Scene_Rvm_H__
#define __DC_Scene_Rvm_H__

#include "../RenderScene.h"
#include "Ubershader.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Rendering virtual machine.
    class Rvm : public RefCounted {
    public:

        //! Displays a frame captured by a render scene.
        void                        display( const RenderFrameUPtr& frame );

        //! Creates an Rvm instance.
        static RvmPtr               create( RenderingContextWPtr context );

    private:

                                    //! Constructs an Rvm instance.
                                    Rvm( RenderingContextWPtr context );

        //! Resets rendering states to defaults.
        void                        reset( void );

        //! Executes a single command buffer.
        void                        execute( const RenderFrame& frame, const RenderCommandBuffer& commands );

        //! Binds a render target with specified viewport and executes a command buffer.
        void                        renderToTarget( const RenderFrame& frame, u8 renderTarget, const f32* viewport, const RenderCommandBuffer& commands );

        //! Unrolls a state stack an applies all state changes.
        void                        applyStates( const RenderFrame& frame, const RenderStateBlock* const * states, s32 count );

        //! Clears an active render target
        void                        clear( const f32* color, f32 depth, s32 stencil, u8 mask );

        //! Uploads data to a GPU constant buffer.
        void                        uploadConstantBuffer( u32 id, const void* data, s32 size );

        //! Uploads data to a GPU vertex buffer.
        void                        uploadVertexBuffer( u32 id, const void* data, s32 size );

        //! Sets an alpha testing state.
        void                        switchAlphaTest( const RenderFrame& frame, const RenderState& state );

        //! Sets a depth state.
        void                        switchDepthState( const RenderFrame& frame, const RenderState& state );

        //! Sets a blending state.
        void                        switchBlending( const RenderFrame& frame, const RenderState& state );

        //! Sets a render target.
        void                        switchRenderTarget( const RenderFrame& frame, const RenderState& state );

        //! Binds a shader program to a pipeline.
        void                        switchShader( const RenderFrame& frame, const RenderState& state );

        //! Binds a constant buffer to a pipeline.
        void                        switchConstantBuffer( const RenderFrame& frame, const RenderState& state );

        //! Binds a vertex buffer to a pipeline.
        void                        switchVertexBuffer( const RenderFrame& frame, const RenderState& state );

        //! Binds an index buffer to a pipeline.
        void                        switchIndexBuffer( const RenderFrame& frame, const RenderState& state );

        //! Binds an input layout to a pipeline.
        void                        switchInputLayout( const RenderFrame& frame, const RenderState& state );

        //! Binds a texture to a sampler.
        void                        switchTexture( const RenderFrame& frame, const RenderState& state );

        //! Sets a cull face mode.
        void                        switchCullFace( const RenderFrame& frame, const RenderState& state );

    private:

        //! State switcher function callback.
        typedef void ( Rvm::*StateSwitch )( const RenderFrame&, const RenderState& );

        //! A helper structure to store an active shader state.
        struct ActiveShader {
            UbershaderWPtr          shader;                 //!< A shader instance that should be used.
            UbershaderWPtr          activeShader;           //!< A shader instance that is now bound.
            Ubershader::Bitmask     features;               //!< An active permutation.
            Renderer::ShaderPtr     permutation;            //!< A shader permutation instance.

                                    //! Constructs an ActiveShader instance.
                                    ActiveShader( void )
                                        : features( 0 ) {}
        };

        //! A forward declaration of a stack type to store intermediate render targets.
        class IntermediateTargetStack;

        Renderer::HalWPtr                   m_hal;                                          //!< Rendering HAL to be used.
        RenderingContextWPtr                m_context;                                      //!< Parent rendering context.
        StateSwitch                         m_stateSwitches[RenderState::TotalStates];      //!< Function callbacks to switch states.
        u64                                 m_vertexAttributeFeatures;                      //!< A vertex attribute features.
        u64                                 m_resourceFeatures;                             //!< Active resource features.
        ActiveShader                        m_activeShader;                                 //!< An active shader instance.
        Stack<const f32*>                   m_viewportStack;                                //!< A viewport stack.
        AutoPtr<IntermediateTargetStack>    m_intermediateTargets;                          //!< An intermediate render target stack.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_H__    */