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

#include "OpenGLRenderingContext.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** OpenGLRenderingContext::OpenGLRenderingContext
OpenGLRenderingContext::OpenGLRenderingContext(RenderViewPtr view)
    : RenderingContext(view)
{
    m_framebuffers.push(Framebuffer());
}
    
// ** OpenGLRenderingContext::deleteProgram
void OpenGLRenderingContext::deleteProgram(ResourceId id)
{
    ProgramPermutations& permutations = m_permutations[id];
    
    for (ProgramPermutations::iterator i = permutations.begin(), end = permutations.end(); i != end; ++i)
    {
        OpenGL2::Program::deleteProgram(i->second.program);
    }
    
    m_permutations.emplace(id, ProgramPermutations());
}

// ** OpenGLRenderingContext::lookupPermutation
bool OpenGLRenderingContext::lookupPermutation(Program program, PipelineFeatures features, const Permutation** permutation) const
{
    if (static_cast<s32>(program) >= m_permutations.count())
    {
        m_permutations.emplace(program, ProgramPermutations());
        return false;
    }
    
    ProgramPermutations& permutations = m_permutations[program];
    
    // Now lookup a permutation cache
    ProgramPermutations::const_iterator i = permutations.find(features);
    
    if (i != permutations.end())
    {
        *permutation = &i->second;
        return true;
    }
    
    return false;
}
    
// ** OpenGLRenderingContext::savePermutation
void OpenGLRenderingContext::savePermutation(Program program, PipelineFeatures features, GLuint id)
{
    Permutation permutation;
    permutation.program = id;
    m_permutations[program][features] = permutation;
}
 
// ** OpenGLRenderingContext::findUniformLocation
GLint OpenGLRenderingContext::findUniformLocation(Program program, PipelineFeatures features, const FixedString& name)
{
    Permutation& permutation = m_permutations[program][features];
    UniformLocations& uniforms = permutation.uniforms;
    
    UniformLocations::const_iterator i = uniforms.find(name.hash());
    
    if (i != uniforms.end())
    {
        return i->second;
    }
    
    GLint location = OpenGL2::Program::uniformLocation(permutation.program, name);
    uniforms[name.hash()] = location;
    
    return location;
}

// ** OpenGLRenderingContext::acquireFramebuffer
s32 OpenGLRenderingContext::acquireFramebuffer(u16 width, u16 height)
{
    for (s32 i = 0, n = m_framebuffers.count(); i < n; i++)
    {
        if (!m_framebuffers[i].acquired && m_framebuffers[i].width == width && m_framebuffers[i].height == height)
        {
            m_framebuffers[i].acquired = true;
            return i;
        }
    }
    
    return 0;
}
    
// ** OpenGLRenderingContext::releaseFramebuffer
void OpenGLRenderingContext::releaseFramebuffer(s32 index)
{
    m_framebuffers[index].acquired = false;
}

// ** OpenGLRenderingContext::allocateFramebuffer
s32 OpenGLRenderingContext::allocateFramebuffer(GLuint id, GLuint depth, u16 width, u16 height)
{
    Framebuffer framebuffer;
    framebuffer.id = id;
    framebuffer.depth = depth;
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.acquired = true;
    return m_framebuffers.push(framebuffer);
}
    
} // namespace Renderer

DC_END_DREEMCHEST
