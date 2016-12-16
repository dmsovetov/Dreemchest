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

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! A base class for all OpenGL-based rendering contexts.
    class OpenGLRenderingContext : public RenderingContext
    {
    protected:
        
                                                //! Constructs an OpenGLRenderingContext instance.
                                                OpenGLRenderingContext(RenderViewPtr view);
        
        //! Searches for a suitable shader program permutation.
        GLuint                                  lookupPermutation(Program program, PipelineFeatures features) const;
        
    protected:
    
        //! A container type to store program permutations.
        typedef HashMap<PipelineFeatures, GLuint> ProgramPermutations;
        
        FixedArray<GLuint>                      m_vertexBuffers;    //!< Allocated vertex buffers.
        FixedArray<GLuint>                      m_indexBuffers;     //!< Allocated index buffers.
        FixedArray<GLuint>                      m_constantBuffers;  //!< Allocated constant buffers.
        FixedArray<GLuint>                      m_textures;         //!< Allocated textures.
        mutable FixedArray<ProgramPermutations> m_permutations;     //!< Available program permutations.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_OpenGLRenderingContext_H__    */
