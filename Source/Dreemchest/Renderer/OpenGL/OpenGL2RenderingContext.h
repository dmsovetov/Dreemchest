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

#ifndef __DC_Renderer_OpenGL2RenderingContext_H__
#define __DC_Renderer_OpenGL2RenderingContext_H__

#include "OpenGLRenderingContext.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! OpenGL 2 rendering context implementation.
    class OpenGL2RenderingContext : public OpenGLRenderingContext
    {
    public:
                                    //! Constructs an OpenGL2RenderingContext instance.
                                    OpenGL2RenderingContext(RenderViewPtr view);
        
    protected:
        
        //! Executes a specified command buffer.
        virtual void                executeCommandBuffer(const RenderFrame& frame, const CommandBuffer& commands) NIMBLE_OVERRIDE;
        
        //! Compiles the requested rendering state (activates a shader permuation that best matches active pipeline state, bind buffers, textures, etc.).
        void                        compilePipelineState(const PipelineState& state);
        
        //! Compiles and sets a matching shader permutation.
        const Permutation*          applyProgramPermutation(const PipelineState& state);
        
        //! Compiles a shader program permutation.
        const Permutation*          compileShaderPermutation(ResourceId program, PipelineFeatures features, const PipelineFeatureLayout* featureLayout);
        
        //! Update uniforms from active constant buffers.
        void                        updateUniforms(const PipelineState& state, const Permutation* permutation);

        //! Acquires a transient texture.
        ResourceId                  acquireTexture(u8 type, u16 width, u16 height, PixelFormat format);
        
        //! Releases a transient texture.
        void                        releaseTexture(ResourceId id);
        
        //! Allocates a new texture of specified type.
        ResourceId                  allocateTexture(u8 type, const void* data, u16 width, u16 height, u16 mipLevels, u16 pixelFormat, u8 filter, ResourceId id = 0);
        
    private:
        
        //! A software-emulated constant buffer
        struct ConstantBuffer
        {
            Array<u8>               data;       //!< Actual buffer data.
            Array<UniformElement>   layout;     //!< A constant buffer layout.
        #if DEV_RENDERER_UNIFORM_CACHING
            Revision                revision;   //!< A constant buffer revision number to track modifications.
        #endif  //  #if DEV_RENDERER_UNIFORM_CACHING
        };

        //! Generates a uniform structure from a cbuffer definition.
        class ShaderPreprocessor : public UniformBufferPreprocessor
        {
            //! Generates a uniform definition.
            virtual String          generateBufferDefinition(const RenderingContext& renderingContext, const String& type, const String& name, s32 slot) const NIMBLE_OVERRIDE;
        };
        
        FixedArray<ConstantBuffer>  m_constantBuffers;      //!< An array of allocated constant buffer instances.
        const VertexBufferLayout*   m_activeInputLayout;    //!< An active input layout.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_OpenGL2RenderingContext_H__    */
