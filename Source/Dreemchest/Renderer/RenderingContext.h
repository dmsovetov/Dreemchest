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
        
        //! Cleans all allocated resources.
        virtual                                 ~RenderingContext( void );
        
        //! Sets a default state block.
        void                                    setDefaultStateBlock(const StateBlock& value);
        
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
        
        //! Queues a shader instance creation and returns it's index.
        Program                                 requestShader( const String& fileName );
        
        //! Queues a shader instance creation and returns it's index.
        Program                                 requestShader( const String& vertex, const String& fragment );
        
    protected:
        
                                                //! Constructs a RenderingContext instance.
                                                RenderingContext( void );
        
        //! Applies a specified state block.
        virtual PipelineFeatures                applyStateBlock(const RenderFrame& frame, const StateBlock& stateBlock) NIMBLE_ABSTRACT;
        
        //! Executes a specified command buffer.
        virtual void                            execute(const RenderFrame& frame, const CommandBuffer& commands) NIMBLE_ABSTRACT;
        
    private:
        
        //! Allocates a new render resource identifier of specified type.
        u16                                     allocateResourceIdentifier(RenderResourceType::Enum type);
        
    protected:
        
        //! A forward declaration of an internal command buffer type used for resource construction.
        class ConstructionCommandBuffer;
        
        //! A maximum number of input layout types
        enum { MaxInputLayouts = 255 };
        
        //! A container type to manage resource identifiers.
        typedef IndexManager<u16>               ResourceIdentifierManager;
        
        //! A unique pointer type for a vertex buffer layout instance.
        typedef UPtr<VertexBufferLayout>        VertexBufferLayoutUPtr;
        
        //! A unique pointer type for a pipeline feature layout instance.
        typedef UPtr<PipelineFeatureLayout>     PipelineFeatureLayoutUPtr;
        
        ResourceIdentifierManager               m_resourceIdentifiers[RenderResourceType::TotalTypes];  //!< An array of resource identifier managers.
        FixedArray<PipelineFeatureLayoutUPtr>   m_pipelineFeatureLayouts;                               //!< An array of constructed pipeline feature layouts.
        FixedArray<VertexBufferLayoutUPtr>      m_inputLayouts;                                         //!< Allocated input layouts.
        FixedArray<UbershaderPtr>               m_shaders;                                              //!< Allocated ubershaders.
        InputLayout                             m_inputLayoutCache[MaxInputLayouts];                    //!< A lookup table for input layout types.
        ConstructionCommandBuffer*              m_constructionCommandBuffer;                            //!< A command buffer that is used for resource construction commands.
        StateBlock                              m_defaultStateBlock;                                    //!< A default state block is applied after all commands were executed.
    };
    
    //! Creates a rendering context that uses a deprecated rendering HAL interface.
    RenderingContextPtr createDeprecatedRenderingContext( HalWPtr hal );
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_RenderingContext_H__  */
