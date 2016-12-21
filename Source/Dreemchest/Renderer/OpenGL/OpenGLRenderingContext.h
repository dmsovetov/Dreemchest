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

#ifndef __DC_Renderer_OpenGLRenderingContext_H__
#define __DC_Renderer_OpenGLRenderingContext_H__

#include "../RenderingContext.h"
#include "OpenGLHal.h"
#include "OpenGL2.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! A base class for all OpenGL-based rendering contexts.
    class OpenGLRenderingContext : public RenderingContext
    {
    protected:
        
        //! A container type to store cached uniform locations.
        typedef HashMap<String32, GLint> UniformLocations;
        
        //! A shader program permutation entry.
        struct Permutation
        {
            GLuint              program;
            UniformLocations    uniforms;
        };
        
                                                //! Constructs an OpenGLRenderingContext instance.
                                                OpenGLRenderingContext(RenderViewPtr view);
        
        //! Searches for a suitable shader program permutation.
        bool                                    lookupPermutation(Program program, PipelineFeatures features, const Permutation** permutation) const;
        
        //! Puts a new permutation to a cache.
        void                                    savePermutation(Program program, PipelineFeatures features, GLuint id);
        
        //! Searches a uniform location by a name.
        GLint                                   findUniformLocation(Program program, PipelineFeatures features, const FixedString& name);
        
    protected:
        
        //! A render target object used for offscreen rendering.
        struct RenderTarget
        {
            GLuint              id;                                         //!< A framebuffer object id.
            GLuint              depth;                                      //!< An attached depth buffer.
            Texture_            textures[RenderTargetAttachment::Total];    //!< An array of attached textures.
            u16                 width;                                      //!< Render target width.
            u16                 height;                                     //!< Render target height.
            PixelFormat         pixelFormat;                                //!< Render target pixel format.
            
                                RenderTarget()
                                    : id(0), width(0), height(0)
                                {
                                }
        };
        
        //! A texture instance descriptor.
        struct Texture
        {
            GLuint              id;             //!< A texture OpenGL id.
            GLenum              type;           //!< A texture type (1D, 2D, 3D, Cube).
            
                                //! Constructs an instance of texture
                                Texture()
                                    : id(0), type(0) {}
            
                                //! Constructs an instance of texture
                                Texture(GLuint id, GLenum type)
                                    : id(id), type(type) {}
        };
    
        //! A container type to store program permutations.
        typedef HashMap<PipelineFeatures, Permutation> ProgramPermutations;
        
        FixedArray<GLuint>                      m_vertexBuffers;        //!< Allocated vertex buffers.
        FixedArray<GLuint>                      m_indexBuffers;         //!< Allocated index buffers.
        FixedArray<GLuint>                      m_constantBuffers;      //!< Allocated constant buffers.
        FixedArray<Texture>                     m_textures;             //!< Allocated textures.
        FixedArray<RenderTarget>                m_renderTargets;        //!< Allocated render targets.
        List<TransientRenderTarget>             m_freeRenderTargets;    //!< A list of free render targets.
        mutable FixedArray<ProgramPermutations> m_permutations;         //!< Available program permutations.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_OpenGLRenderingContext_H__    */
