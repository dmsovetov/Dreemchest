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

// ------------------------------------------------ OpenGLRenderingContext::ShaderVersionPreprocessor -------------------------------------------- //

// ** OpenGLRenderingContext::ShaderVersionPreprocessor::ShaderVersionPreprocessor
OpenGLRenderingContext::ShaderVersionPreprocessor::ShaderVersionPreprocessor(s32 version)
    : m_version(version)
{
}

// ** OpenGLRenderingContext::ShaderVersionPreprocessor::preprocess
bool OpenGLRenderingContext::ShaderVersionPreprocessor::preprocess(const RenderingContext& renderingContext, String& shader) const
{
    shader = "#version " + toString(m_version) + "\n" + shader;
    return true;
}

// ------------------------------------------------------ OpenGLRenderingContext --------------------------------------------------- //

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
bool OpenGLRenderingContext::lookupPermutation(ResourceId program, PipelineFeatures features, const Permutation** permutation) const
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
const OpenGLRenderingContext::Permutation* OpenGLRenderingContext::savePermutation(ResourceId program, PipelineFeatures features, GLuint id)
{
    Permutation permutation;
    permutation.program = id;
    
    OpenGL2::Program::Uniform uniformInfo;
    OpenGL2::Program::use(id);
    
    for (GLuint i = 0, n = OpenGL2::Program::uniformCount(id); i < n; i++)
    {
        OpenGL2::Program::uniformAt(id, i, uniformInfo);
        
        Permutation::Uniform uniform;
        uniform.location = OpenGL2::Program::uniformLocation(id, uniformInfo.name);
        uniform.type     = uniformInfo.type;
        uniform.size     = uniformInfo.size;
        
        switch (uniformInfo.type)
        {
            case GL_SAMPLER_1D:
            case GL_SAMPLER_2D:
            case GL_SAMPLER_3D:
            case GL_SAMPLER_CUBE:
                uniform.index = uniformInfo.name[uniformInfo.length - 1] - '0';
                uniform.hash  = String32();
                OpenGL2::Program::uniform1i(uniform.location, uniform.index);
                continue;
                break;
            default:
                uniform.index = uniformInfo.name[3] - '0';
                uniform.hash  = String32(uniformInfo.name + 5, uniformInfo.length - 5 - ((uniformInfo.name[uniformInfo.length - 1] == ']') ? 3 : 0));
        }
        
        LogDebug("opengl", "\tuniform '%s' location %d\n", uniformInfo.name, uniform.location);
        
        permutation.uniforms.push_back(uniform);
    }
    
    m_permutations[program][features] = permutation;
    return &m_permutations[program][features];
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
