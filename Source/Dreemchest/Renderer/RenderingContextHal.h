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

#ifndef __DC_Renderer_RenderingContextHal_H__
#define __DC_Renderer_RenderingContextHal_H__

#include "RenderingContext.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Rendering context.
    class RenderingContextHal : public RenderingContext
    {
    public:
        
                                    //! Constructs a RenderingContextHal instance.
                                    RenderingContextHal( HalWPtr hal );

    protected:

        //! Resets rendering states to defaults.
        virtual PipelineFeatures    applyStateBlock(const RenderFrame& frame, const StateBlock& stateBlock) NIMBLE_OVERRIDE;
        
        //! Executes a specified command buffer.
        virtual void                executeCommandBuffer(const RenderFrame& frame, const CommandBuffer& commands) NIMBLE_OVERRIDE;
        
        //! Returns an intermediate render target.
        RenderTargetWPtr            intermediateRenderTarget( TransientRenderTarget id ) const;

        //! Binds a render target with specified viewport and executes a command buffer.
        void                        renderToTarget( const RenderFrame& frame, u8 renderTarget, const f32* viewport, const CommandBuffer& commands );

        //! Unrolls a state stack an applies all state changes, returns an up-to-date pipeline feature mask.
        PipelineFeatures            applyStates( const RenderFrame& frame, const StateBlock* const * stateBlocks, s32 count );

        //! Clears an active render target
        void                        clear( const f32* color, f32 depth, s32 stencil, u8 mask );

        //! Sets an alpha testing state.
        void                        switchAlphaTest( const RenderFrame& frame, const State& state );

        //! Sets a depth state.
        void                        switchDepthState( const RenderFrame& frame, const State& state );

        //! Sets a blending state.
        void                        switchBlending( const RenderFrame& frame, const State& state );

        //! Sets a render target.
        void                        switchRenderTarget( const RenderFrame& frame, const State& state );

        //! Binds a shader program to a pipeline.
        void                        switchShader( const RenderFrame& frame, const State& state );

        //! Binds a constant buffer to a pipeline.
        void                        switchConstantBuffer( const RenderFrame& frame, const State& state );

        //! Binds a vertex buffer to a pipeline.
        void                        switchVertexBuffer( const RenderFrame& frame, const State& state );

        //! Binds an index buffer to a pipeline.
        void                        switchIndexBuffer( const RenderFrame& frame, const State& state );

        //! Binds an input layout to a pipeline.
        void                        switchInputLayout( const RenderFrame& frame, const State& state );
        
        //! Activates a pipeline feature layout.
        void                        switchPipelineFeatureLayout( const RenderFrame& frame, const State& state );

        //! Binds a texture to a sampler.
        void                        switchTexture( const RenderFrame& frame, const State& state );

        //! Sets a cull face mode.
        void                        switchCullFace( const RenderFrame& frame, const State& state );

        //! Sets a polygon offset value
        void                        switchPolygonOffset( const RenderFrame& frame, const State& state );
        
        //! Handles a command to upload data to a GPU constant buffer.
        void                        commandUploadConstantBuffer(ConstantBuffer_ id, const void* data, s32 size);
        
        //! Handles a command to upload data to a GPU vertex buffer.
        void                        commandUploadVertexBuffer(VertexBuffer_ id, const void* data, s32 size);
        
        //! Handles an input layout creation command.
        void                        commandCreateInputLayout(InputLayout id, u8 vertexFormat);
        
        //! Handles a vertex buffer creation command.
        void                        commandCreateVertexBuffer(VertexBuffer_ id, const void* data, s32 size);
        
        //! Handles an index buffer creation command.
        void                        commandCreateIndexBuffer(IndexBuffer_ id, const void* data, s32 size);
        
        //! Handles an index buffer creation command.
        void                        commandCreateConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, const ConstantBufferLayout* layout);
        
        //! Handles a texture creation command.
        void                        commandCreateTexture(Texture_ id, u16 width, u16 height, const void* data, PixelFormat format);
        
        //! Acquires an intermediate render target.
        TransientRenderTarget    acquireRenderTarget( u16 width, u16 height, PixelFormat format );
        
        //! Releases an intermediate render target.
        void                        releaseRenderTarget( TransientRenderTarget id );
        
        //! Activates a shader permuation that best matches active pipeline state.
        void                        activateShaderPermutation( PipelineFeatures features );
        
        //! Compiles a shader permutation.
        ShaderPtr                   compileShaderPermutation( UbershaderPtr shader, PipelineFeatures features, const PipelineFeatureLayout* featureLayout );

    private:

        //! State switcher function callback.
        typedef void ( RenderingContextHal::*StateSwitch )( const RenderFrame&, const State& );


        Renderer::HalWPtr                   m_hal;                                  //!< Rendering HAL to be used.
        FixedArray<VertexBufferPtr>         m_vertexBuffers;                        //!< Allocated vertex buffers.
        FixedArray<IndexBufferPtr>          m_indexBuffers;                         //!< Allocated index buffers.
        FixedArray<ConstantBufferPtr>       m_constantBuffers;                      //!< Allocated constant buffers.
        FixedArray<TexturePtr>              m_textures;                             //!< Allocated textures.

        StateSwitch                         m_stateSwitches[State::TotalStates];    //!< Function callbacks to switch states.
        ShaderPtr                           m_activeProgram;                        //!< An active program permutation.
        Stack<const f32*>                   m_viewportStack;                        //!< A viewport stack.
        
        //! A helper struct that hold info about an intermediate render target.
        struct TransientRenderTarget_
        {
        #if DEV_DEPRECATED_HAL
            RenderTargetPtr                 renderTarget;                       //!< A GPU render target instance.
        #endif  /*  #if DEV_DEPRECATED_HAL  */
            u16                             width;                              //!< Render target width.
            u16                             height;                             //!< Render target height.
            PixelFormat                     format;                             //!< Render target internal format.
            bool                            isFree;                             //!< Indicates that this render target is free.
        };
        
        Array<TransientRenderTarget_>    m_renderTargets;            //!< An array of intermediate render targets.
    };

} // namespace Renderer

DC_END_DREEMCHEST

#endif    /*    !__DC_Renderer_RenderingContext_H__    */
