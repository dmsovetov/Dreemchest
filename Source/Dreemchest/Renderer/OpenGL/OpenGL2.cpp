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

#include "OpenGL2.h"
#include "../VertexBufferLayout.h"

#if defined (DC_PLATFORM_MACOS)
    #define GL_POINT_SIZE_ARRAY GL_POINT_SIZE_ARRAY_APPLE
    #define glPointSizePointer  glPointSizePointerAPPLE
#endif  //  #if defined( DC_PLATFORM_MACOS )

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** OpenGL2::createBuffer
GLuint OpenGL2::createBuffer(GLenum type, const void* data, s32 size, GLenum usage)
{
    DC_CHECK_GL;
    
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(type, id);
    glBufferData(type, size, data, usage);
    glBindBuffer(type, 0);
    return id;
}
    
// ** OpenGL2::clear
void OpenGL2::clear(const GLclampf* color, u8 mask, GLclampd depth, GLint stencil)
{
    DC_CHECK_GL;
    
    GLbitfield flags = 0;
    
    if(mask & ClearColor)
    {
        flags |= GL_COLOR_BUFFER_BIT;
    }
    if(mask & ClearDepth)
    {
        flags |= GL_DEPTH_BUFFER_BIT;
    }
    if(mask & ClearStencil)
    {
        flags |= GL_STENCIL_BUFFER_BIT;
    }
    
    glClearColor(color[0], color[1], color[2], color[3]);
    glClearDepth(depth);
    glClearStencil(stencil);
    glClear(flags);
}
    
// ** OpenGL2::bindBuffer
void OpenGL2::bindBuffer(GLenum type, GLuint id)
{
    DC_CHECK_GL;
    glBindBuffer(type, id);
}
    
// ** OpenGL2::bufferSubData
void OpenGL2::bufferSubData(GLenum target, GLuint id, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    DC_CHECK_GL;
    glBindBuffer(target, id);
    glBufferSubData(target, offset, size, data);
    glBindBuffer(target, 0);
}

// ** OpenGL2::deleteProgram
void OpenGL2::deleteProgram(GLuint id)
{
    DC_CHECK_GL;
    glDeleteProgram(id);
}

// ** OpenGL2::deleteShader
void OpenGL2::deleteShader(GLuint id)
{
    DC_CHECK_GL;
    glDeleteShader(id);
}
    
// ** OpenGL2::compileShader
GLuint OpenGL2::compileShader(GLenum type, CString source, s8* error, s32 maxErrorSize)
{
    DC_CHECK_GL;
    
    GLint result;
    
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    // Get a status of a compiled shader
    GLsizei errLogSize;
    glGetShaderInfoLog(id, maxErrorSize, &errLogSize, error);
    
    if (errLogSize)
    {
        StringArray messages = split( error, "\n" );
        
        for( s32 i = 0; i < messages.size(); i++ )
        {
            if( result == GL_FALSE )
            {
                LogError("opengl", "%s\n", messages[i].c_str());
            }
            else
            {
                LogWarning("opengl", "%s\n", messages[i].c_str());
            }
        }
    }
    
    return id;
}
    
// ** OpenGL2::createProgram
GLuint OpenGL2::createProgram(const GLuint* shaders, s32 count, s8* error, s32 maxErrorSize)
{
    DC_CHECK_GL;
    
    // Create a shader program
    GLuint program = glCreateProgram();
    
    // Attach all shaders
    for (s32 i = 0; i < count; i++)
    {
        glAttachShader(program, shaders[i]);
    }
    
    // Now link a program
    GLint result;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    
    // Get a status of a compiled shader
    GLsizei errLogSize;
    glGetProgramInfoLog(program, maxErrorSize, &errLogSize, error);
    
    if (errLogSize == 0)
    {
        return program;
    }
    
    if (errLogSize)
    {
        StringArray messages = split(error, "\n");
        
        for (s32 i = 0; i < messages.size(); i++)
        {
            if (result == GL_FALSE)
            {
                LogError("opengl", "%s\n", messages[i].c_str());
            }
            else
            {
                LogWarning("opengl", "%s\n", messages[i].c_str());
            }
        }
    }
    
    deleteProgram(program);
    return 0;
}
    
// ** OpenGL2::enableInputLayout
void OpenGL2::enableInputLayout(GLbyte* pointer, const VertexBufferLayout& layout)
{
    DC_CHECK_GL;
    
    s32 stride = layout.vertexSize();
    
    const VertexBufferLayout::Element& normal     = layout.normal();
    const VertexBufferLayout::Element& position   = layout.position();
    const VertexBufferLayout::Element& color      = layout.color();
    const VertexBufferLayout::Element& pointSize  = layout.pointSize();
    
    if (normal)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, stride, pointer + normal.offset);
    }
    
    if (color)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(color.count, GL_UNSIGNED_BYTE, stride, pointer + color.offset);
    }
    
    if (pointSize)
    {
    #ifndef DC_PLATFORM_WINDOWS
        glEnableClientState(GL_POINT_SIZE_ARRAY);
        glPointSizePointer(GL_FLOAT, stride, pointer + pointSize.offset);
    #endif
    }
    
    for (s32 i = 0; i < VertexTexCoord4 - VertexTexCoord0; i++)
    {
        const VertexBufferLayout::Element& uv = layout.uv(i);
        
        if (uv)
        {
            glClientActiveTexture(GL_TEXTURE0 + i);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(uv.count, GL_FLOAT, stride, pointer + uv.offset);
        }
    }
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(position.count, GL_FLOAT, stride, pointer + position.offset);
}

// ** OpenGL2::disableInputLayout
void OpenGL2::disableInputLayout(const VertexBufferLayout& layout)
{
    DC_CHECK_GL;
    
    if(layout.normal())
    {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    
    if(layout.color())
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }
    
    if(layout.pointSize())
    {
    #ifndef DC_PLATFORM_WINDOWS
        glDisableClientState(GL_POINT_SIZE_ARRAY);
    #endif
    }
    
    for (s32 i = 0; i < VertexTexCoord4 - VertexTexCoord0; i++)
    {
        if(layout.uv(i))
        {
            glClientActiveTexture(GL_TEXTURE0 + i);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
    }
    
    glDisableClientState(GL_VERTEX_ARRAY);
}

// ** OpenGL2::drawElements
void OpenGL2::drawElements(PrimitiveType primType, GLenum type, u32 firstIndex, u32 count)
{
    DC_CHECK_GL;
    
    static GLenum mode[TotalPrimitiveTypes] =
    {
          GL_LINES
        , GL_LINE_STRIP
        , GL_TRIANGLES
        , GL_TRIANGLE_STRIP
        , GL_TRIANGLE_FAN
        , GL_POINTS
    };

    glDrawElements(mode[primType], count, type, static_cast<GLbyte*>(NULL) + firstIndex);
}

// ** OpenGL2::drawArrays
void OpenGL2::drawArrays(PrimitiveType primType, u32 offset, u32 count)
{
    DC_CHECK_GL;

    static GLenum mode[TotalPrimitiveTypes] =
    {
          GL_LINES
        , GL_LINE_STRIP
        , GL_TRIANGLES
        , GL_TRIANGLE_STRIP
        , GL_TRIANGLE_FAN
        , GL_POINTS
    };
    
    glDrawArrays(mode[primType], offset, count);
}

// ** OpenGL2::convertBlendFactor
GLenum OpenGL2::convertBlendFactor(BlendFactor value)
{
    switch (value)
    {
        case BlendDisabled:     return 0;
        case BlendZero:         return GL_ZERO;
        case BlendOne:          return GL_ONE;
        case BlendSrcColor:     return GL_SRC_COLOR;
        case BlendInvSrcColor:  return GL_ONE_MINUS_SRC_COLOR;
        case BlendDstColor:     return GL_DST_COLOR;
        case BlendInvDstColor:  return GL_ONE_MINUS_DST_COLOR;
        case BlendSrcAlpha:     return GL_SRC_ALPHA;
        case BlendInvSrcAlpha:  return GL_ONE_MINUS_SRC_ALPHA;
        case BlendDstAlpha:     return GL_DST_ALPHA;
        case BlendInvDstAlpha:  return GL_ONE_MINUS_DST_ALPHA;
        default:                NIMBLE_NOT_IMPLEMENTED;
    }

    return 0;
}
    
// ** OpenGL2::convertTriangleFace
GLenum OpenGL2::convertTriangleFace(TriangleFace value)
{
    switch (value)
    {
        case TriangleFaceBack:          return GL_BACK;
        case TriangleFaceFront:         return GL_FRONT;
        case TriangleFaceFrontAndBack:  return GL_FRONT_AND_BACK;
        default:                        NIMBLE_NOT_IMPLEMENTED;
    }

    return 0;
}
    
// ** OpenGL2::convertCompareFunction
GLenum OpenGL2::convertCompareFunction(Compare value)
{
    switch (value)
    {
        case Always:        return GL_ALWAYS;
        case Equal:         return GL_EQUAL;
        case Greater:       return GL_GREATER;
        case GreaterEqual:  return GL_GEQUAL;
        case Less:          return GL_LESS;
        case LessEqual:     return GL_LEQUAL;
        case Never:         return GL_NEVER;
        case NotEqual:      return GL_NOTEQUAL;
        default:            NIMBLE_NOT_IMPLEMENTED
    }

    return 0;
}

} // namespace Renderer

DC_END_DREEMCHEST