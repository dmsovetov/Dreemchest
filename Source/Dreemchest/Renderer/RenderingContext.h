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

#include "RenderState.h"
#include "PipelineFeatureLayout.h"
#include "ShaderLibrary.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Rendering context.
    class RenderingContext : public RefCounted
    {
    public:
        
        //! Cleans all allocated resources.
        virtual                                 ~RenderingContext( void );
        
        //! Sets a default state block.
        void                                    setDefaultStateBlock(const StateBlock& value);
        
        //! Sets a default program.
        void                                    setDefaultProgram(Program value);
        
        //! Displays a frame captured by a render scene.
        void                                    display( RenderFrame& frame );
        
        //! Queues an input layout instance for creation and returns it's index.
        InputLayout                             requestInputLayout( const VertexFormat& vertexFormat );
        
        //! Queues a vertex buffer instance for creation and returns it's index.
        VertexBuffer_                           requestVertexBuffer( const void* data, s32 size );
        
        //! Queues an index buffer instance for creation and returns it's index.
        IndexBuffer_                            requestIndexBuffer( const void* data, s32 size );
        
        //! Queues a constant buffer instance for creation and returns it's index.
        ConstantBuffer_                         requestConstantBuffer( const void* data, s32 size, const ConstantBufferLayout* layout );
        
        //! Queues a texture instance for creation and returns it's index.
        Texture_                                requestTexture( const void* data, u16 width, u16 height, PixelFormat format );
        
        //! Queues a pipeline feature layout instance for creation and returns it's index.
        FeatureLayout                           requestPipelineFeatureLayout(const PipelineFeature* features);
        
        //! Queues a program instance creation and returns it's index.
        Program                                 requestProgram(const ShaderProgramDescriptor& descriptor);
        
        //! Queues a program instance creation and returns it's index.
        Program                                 requestProgram(VertexShader vertexShader, FragmentShader fragmentShader = FragmentShader());
        
        //! Queues a program instance creation and returns it's index.
        Program                                 requestProgram(const String& vertex, const String& fragment);
        
        //! Queues a shader instance creation and returns it's index.
        Program                                 deprecatedRequestShader(const String& fileName);
        
        //! Merges a list of state blocks into a single array of render states.
        s32                                     mergeStateBlocks(const StateBlock* const * stateBlocks, s32 blockCount, State* states, s32 maxStates, PipelineFeatures& userDefined) const;
        
    protected:
        
                                                //! Constructs a RenderingContext instance.
                                                RenderingContext(RenderViewPtr view);
        
        //! Applies a specified state block.
        virtual PipelineFeatures                applyStateBlock(const RenderFrame& frame, const StateBlock& stateBlock) NIMBLE_ABSTRACT;
        
        //! Executes a specified command buffer.
        virtual void                            executeCommandBuffer(const RenderFrame& frame, const CommandBuffer& commands) NIMBLE_ABSTRACT;
        
        //! Executes a specified command buffer inside an intermediate render stack frame.
        void                                    execute( const RenderFrame& frame, const CommandBuffer& commands );
        
        //! Loads an intermediate render target to a specified slot.
        void                                    loadTransientTarget(u8 index, TransientRenderTarget id);
        
        //! Unloads an intermediate render target from a specified slot.
        void                                    unloadTransientTarget(u8 index);
        
        //! Returns an intermediate render target at specified slot.
        TransientRenderTarget                   intermediateTarget(u8 index);
        
        //! Creates a vertex buffer layout instance from a flexible vertex format.
        VertexBufferLayoutUPtr                  createVertexBufferLayout(VertexFormat vertexFormat) const;
        
        //! Starts a process of pipeline state configuration.
        s32                                     startPipelineConfiguration(const StateBlock* const * stateBlocks, s32 blockCount, State* states, s32 maxStates, PipelineFeatures& userDefined);
        
        //! Finishes a process of pipeline state configration.
        PipelineFeatures                        finishPipelineConfiguration(PipelineFeatures userDefined);
        
        //! Generates a shader source code.
        String                                  generateShaderCode(const String& source, PipelineFeatures features, const PipelineFeatureLayout* featureLayout) const;
        
    private:
        
        //! Allocates a new render resource identifier of specified type.
        u16                                     allocateResourceIdentifier(RenderResourceType::Enum type);
        
    protected:
        
        //! A forward declaration of an internal command buffer type used for resource construction.
        class ConstructionCommandBuffer;
        
        //! A forward declaration of a stack type to store intermediate render targets.
        class TransientTargetStack;
        
        //! A maximum number of input layout types
        enum { MaxInputLayouts = 255 };
        
        //! A maximum number of state changes that can be applied in a row.
        enum { MaxStateChanges = 16 };
        
        RenderViewPtr                           m_view;                                                 //!< A rendering viewport.
        PersistentResourceIdentifiers           m_resourceIdentifiers[RenderResourceType::TotalTypes];  //!< An array of resource identifier managers.
        FixedArray<PipelineFeatureLayoutUPtr>   m_pipelineFeatureLayouts;                               //!< An array of constructed pipeline feature layouts.
        FixedArray<VertexBufferLayoutUPtr>      m_inputLayouts;                                         //!< Allocated input layouts.
        FixedArray<ShaderProgramDescriptor>     m_programs;                                             //!< Allocated shader programs.
        InputLayout                             m_inputLayoutCache[MaxInputLayouts];                    //!< A lookup table for input layout types.
        ConstructionCommandBuffer*              m_constructionCommandBuffer;                            //!< A command buffer that is used for resource construction commands.
        TransientTargetStack*                   m_intermediateTargets;                                  //!< An intermediate render target stack.
        StateBlock                              m_defaultStateBlock;                                    //!< A default state block is applied after all commands were executed.
        Program                                 m_defaultProgram;                                       //!< A default program to be used.
        PipelineState                           m_pipeline;                                             //!< An active pipeline state.
        ShaderLibrary                           m_shaderLibrary;                                        //!< A shader library.
    };
    
    //! Creates a rendering context that uses a deprecated rendering HAL interface.
    RenderingContextPtr createDeprecatedRenderingContext(RenderViewPtr view, HalPtr hal);
    
#ifdef DC_OPENGL_ENABLED
    //! Platform-specific OpenGL view constructor.
    extern RenderViewPtr createOpenGLView(void* window, PixelFormat depthStencil);
    
    //! Creates a rendering context that uses an OpenGL 2 rendering API.
    RenderingContextPtr createOpenGL2RenderingContext(RenderViewPtr view);
#endif    /*    DC_OPENGL_ENABLED    */
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_RenderingContext_H__  */
