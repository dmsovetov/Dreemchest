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
        
        //! A pipeline state that was constructed from an array of render state changes.
        struct RequestedState
        {
            VertexBuffer_           vertexBuffer;                               //!< A vertex buffer that should be set.
            IndexBuffer_            indexBuffer;                                //!< An index buffer that should be set.
            InputLayout             inputLayout;                                //!< An input layout that should be set.
            FeatureLayout           featureLayout;                              //!< A feature layout that should be set.
            Program                 program;                                    //!< A shader program that should be set.
            ConstantBuffer_         constantBuffer[State::MaxConstantBuffers];  //!< An array of bound constant buffers.
            PipelineFeatures        features;                                   //!< A bitmask of user-defined pipeline features.
            
                                    RequestedState() : features(0) {}
        };
        
        //! Applies a specified state block.
        virtual PipelineFeatures    applyStateBlock(const RenderFrame& frame, const StateBlock& stateBlock) NIMBLE_OVERRIDE;
        
        //! Executes a specified command buffer.
        virtual void                executeCommandBuffer(const RenderFrame& frame, const CommandBuffer& commands) NIMBLE_OVERRIDE;
        
        //! Unrolls a state stack an applies all state changes, returns an up-to-date pipeline feature mask.
        RequestedState              applyStates(const RenderFrame& frame, const StateBlock* const * stateBlocks, s32 count);
        
        //! Compiles the requested rendering state (activates a shader permuation that best matches active pipeline state, bind buffers, textures, etc.).
        void                        compilePipelineState(RequestedState state);
        
        //! Compiles a shader program permutation.
        GLuint                      compileShaderPermutation(Program program, PipelineFeatures features, const PipelineFeatureLayout* featureLayout);
        
        //! Update uniforms from active constant buffers.
        void                        updateUniforms(const RequestedState& state, PipelineFeatures features, Program program);
        
    private:
        
        //! A software-emulated constant buffer
        struct ConstantBuffer
        {
            Array<u8>               data;       //!< Actual buffer data.
            const UniformElement*   layout;     //!< A constant buffer layout.
        };
        
        RequestedState              m_activeState;      //!< An active rendering pipeline state.
        FixedArray<ConstantBuffer>  m_constantBuffers;  //!< An array of allocated constant buffer instances.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_OpenGL2RenderingContext_H__    */
