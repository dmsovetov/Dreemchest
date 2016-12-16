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
    
} // namespace Renderer

DC_END_DREEMCHEST
