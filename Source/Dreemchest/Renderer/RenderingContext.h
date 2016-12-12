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

#ifndef __DC_Renderer_RenderingContext_H__
#define __DC_Renderer_RenderingContext_H__

#include "Ubershader.h"
#include "RenderState.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Rendering context.
    class RenderingContext : public RefCounted
    {
    public:

        //! Displays a frame captured by a render scene.
        void                        display( RenderFrame& frame );
        
        //! Queues an input layout instance for creation and returns it's index.
        InputLayout                 requestInputLayout( const VertexFormat& vertexFormat );
        
        //! Queues a vertex buffer instance for creation and returns it's index.
        VertexBuffer_               requestVertexBuffer( const void* data, s32 size );
        
        //! Queues an index buffer instance for creation and returns it's index.
        IndexBuffer_                requestIndexBuffer( const void* data, s32 size );
        
        //! Queues a constant buffer instance for creation and returns it's index.
        ConstantBuffer_             requestConstantBuffer( const void* data, s32 size, const ConstantBufferLayout* layout );
        
        //! Queues a texture instance for creation and returns it's index.
        Texture_                    requestTexture( const void* data, u16 width, u16 height, PixelFormat format );
        
        //! Queues a shader feature layout instance for creation and returns it's index.
        FeatureLayout               requestFeatureLayout(const ShaderFeature* features);
        
        //! Queues a shader instance creation and returns it's index.
        Program                     requestShader( const String& fileName );
        
        //! Queues a shader instance creation and returns it's index.
        Program                     requestShader( const String& vertex, const String& fragment );

    #if DEV_DEPRECATED_HAL
        //! Creates an RenderingContext instance.
        static RenderingContextPtr  create( HalWPtr hal );
    #endif  /*  #if DEV_DEPRECATED_HAL  */

    protected:

                                    //! Constructs a RenderingContext instance.
                                    RenderingContext( HalWPtr hal );

    #if DEV_DEPRECATED_HAL
        //! Resets rendering states to defaults.
        void                        reset( void );
        
        //! Returns an intermediate render target.
        RenderTargetWPtr            intermediateRenderTarget( IntermediateRenderTarget id ) const;
    #else
        //! Resets rendering states to defaults.
        virtual void                reset( void ) = 0;
    #endif  /*  #if DEV_DEPRECATED_HAL */
    
        //! Executes a single command buffer.
        void                        execute( const RenderFrame& frame, const CommandBuffer& commands );
        
    #if !DEV_DEPRECATED_HAL
        //! Executes a single command buffer.
        virtual void                executeCommandBuffer( const RenderFrame& frame, const CommandBuffer& commands ) = 0;
    #endif  /*  #if DEV_DEPRECATED_HAL  */

        //! Binds a render target with specified viewport and executes a command buffer.
        void                        renderToTarget( const RenderFrame& frame, u8 renderTarget, const f32* viewport, const CommandBuffer& commands );

        //! Unrolls a state stack an applies all state changes, returns an up-to-date pipeline feature mask.
        PipelineFeatures            applyStates( const RenderFrame& frame, const StateBlock* const * states, s32 count );

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
        
        //! Activates a feature layout.
        void                        switchFeatureLayout( const RenderFrame& frame, const State& state );

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
        
        //! Allocates a new input layout handle.
        virtual InputLayout         allocateInputLayout( void );
        
        //! Allocates a new vertex buffer handle.
        virtual VertexBuffer_       allocateVertexBuffer( void );
        
        //! Allocates a new index buffer handle.
        virtual IndexBuffer_        allocateIndexBuffer( void );
        
        //! Allocates a new constant buffer handle.
        virtual ConstantBuffer_     allocateConstantBuffer( void );
        
        //! Allocates a new texturehandle.
        virtual Texture_            allocateTexture( void );
        
        //! Acquires an intermediate render target.
        IntermediateRenderTarget    acquireRenderTarget( u16 width, u16 height, PixelFormat format );
        
        //! Releases an intermediate render target.
        void                        releaseRenderTarget( IntermediateRenderTarget id );
        
        //! Activates a shader permuation that best matches active pipeline state.
        void                        activateShaderPermutation( PipelineFeatures features );

    private:

        //! State switcher function callback.
        typedef void ( RenderingContext::*StateSwitch )( const RenderFrame&, const State& );

        //! A helper structure to store an active shader state.
        struct ActiveShader
        {
            UbershaderWPtr              shader;                 //!< A shader instance that should be used.
            UbershaderWPtr              activeShader;           //!< A shader instance that is now bound.
            PipelineFeatures            features;               //!< An active permutation.
            ShaderPtr                   permutation;            //!< A shader permutation instance.
            const ShaderFeatureLayout*  featureLayout;
            const ShaderFeatureLayout*  activeFeatureLayout;

                                        //! Constructs an ActiveShader instance.
                                        ActiveShader( void )
                                            : features( 0 ), featureLayout( NULL ), activeFeatureLayout( NULL ) {}
        };
        
        //! A maximum number of input layout types
        enum { MaxInputLayouts = 255 };

        //! A forward declaration of a stack type to store intermediate render targets.
        class IntermediateTargetStack;
        
        //! A forward declaration of an internal command buffer type used for resource construction.
        class ConstructionCommandBuffer;
        
#if DEV_DEPRECATED_HAL
        typedef Ptr<VertexBufferLayout>     VertexBufferLayoutUPtr;
#else
        typedef UPtr<VertexBufferLayout>    VertexBufferLayoutUPtr;
#endif  /*  #if DEV_DEPRECATED_HAL  */
        typedef UPtr<ShaderFeatureLayout>   ShaderFeatureLayoutUPtr;
        
#if DEV_DEPRECATED_HAL
        FixedArray<VertexBufferPtr>         m_vertexBuffers;                        //!< Allocated vertex buffers.
        FixedArray<IndexBufferPtr>          m_indexBuffers;                         //!< Allocated index buffers.
        FixedArray<ConstantBufferPtr>       m_constantBuffers;                      //!< Allocated constant buffers.
        FixedArray<TexturePtr>              m_textures;                             //!< Allocated textures.
#endif  /*  #if DEV_DEPRECATED_HAL  */
        FixedArray<ShaderFeatureLayoutUPtr> m_featureLayouts;
        FixedArray<VertexBufferLayoutUPtr>  m_inputLayouts;                         //!< Allocated input layouts.
        FixedArray<UbershaderPtr>           m_shaders;                              //!< Allocated ubershaders.
        InputLayout                         m_inputLayoutCache[MaxInputLayouts];    //!< A lookup table for input layout types.
        ConstructionCommandBuffer*          m_constructionCommandBuffer;            //!< A command buffer that is used for resource construction commands.

    #if DEV_DEPRECATED_HAL
        Renderer::HalWPtr                   m_hal;                                  //!< Rendering HAL to be used.
    #endif  /*  #if DEV_DEPRECATED_HAL  */
        StateSwitch                         m_stateSwitches[State::TotalStates];    //!< Function callbacks to switch states.
        PipelineFeatures                    m_vertexAttributeFeatures;              //!< A vertex attribute features.
        PipelineFeatures                    m_resourceFeatures;                     //!< Active resource features.
        ActiveShader                        m_activeShader;                         //!< An active shader instance.
        Stack<const f32*>                   m_viewportStack;                        //!< A viewport stack.
        UPtr<IntermediateTargetStack>       m_intermediateTargets;                  //!< An intermediate render target stack.
        
        //! A helper struct that hold info about an intermediate render target.
        struct IntermediateRenderTarget_
        {
        #if DEV_DEPRECATED_HAL
            RenderTargetPtr                 renderTarget;                       //!< A GPU render target instance.
        #endif  /*  #if DEV_DEPRECATED_HAL  */
            u16                             width;                              //!< Render target width.
            u16                             height;                             //!< Render target height.
            PixelFormat                     format;                             //!< Render target internal format.
            bool                            isFree;                             //!< Indicates that this render target is free.
        };
        
        Array<IntermediateRenderTarget_>    m_renderTargets;            //!< An array of intermediate render targets.
    };

} // namespace Renderer

DC_END_DREEMCHEST

#endif    /*    !__DC_Renderer_RenderingContext_H__    */
