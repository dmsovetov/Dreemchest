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

#ifndef __DC_Renderer_OpenGL2_H__
#define __DC_Renderer_OpenGL2_H__

#include "../Renderer.h"
#include "OpenGLHal.h"

// ** OpenGL headers and libraries
#if defined( DC_PLATFORM_WINDOWS )
    #include <windows.h>

    #include <gl/gl.h>
    #include <gl/glu.h>
    #include "Windows/glext.h"
    #include "Windows/wglext.h"
#endif  //  #if defined( DC_PLATFORM_WINDOWS )

#if defined( DC_PLATFORM_ANDROID )
    #define GL_GLEXT_PROTOTYPES

    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif  //  #if defined( DC_PLATFORM_ANDROID )

#if defined( DC_PLATFORM_HTML5 )
    #define GL_GLEXT_PROTOTYPES

    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif  //  #if defined( DC_PLATFORM_HTML5 )

#if defined( DC_PLATFORM_IOS )
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#endif  //  #if defined( DC_PLATFORM_IOS )

#if defined( DC_PLATFORM_MACOS )
    #include <OpenGL/gl.h>
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/glext.h>
#endif  //  #if defined( DC_PLATFORM_MACOS )

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! An OpenGL 2 API wrapper.
    class OpenGL2
    {
    public:
        
        //! Creates a buffer object with a specified type and returns it's identifier.
        static GLuint   createBuffer(GLenum type, const void* data, s32 size, GLenum usage);
        
        //! Clears an active viewport.
        static void     clear(const GLclampf* color, u8 mask, GLclampd depth, GLint stencil);
        
        //! Binds a buffer object to a pipeline.
        static void     bindBuffer(GLenum type, GLuint id);
        
        //! Updates a part of a buffer.
        static void     bufferSubData(GLenum target, GLuint id, GLintptr offset, GLsizeiptr size, const GLvoid* data);
        
        //! Converts a blend factor constant to an OpenGL value.
        static GLenum   convertBlendFactor(BlendFactor value);
        
        //! Converts a triangle face constant to an OpenGL value.
        static GLenum   convertTriangleFace(TriangleFace value);
        
        //! Converts a comparation function constant to an OpenGL value.
        static GLenum   convertCompareFunction(Compare value);
        
        //! Renders an indexed batch of primitives.
        static void     drawElements(PrimitiveType primType, GLenum type, u32 firstIndex, u32 count);

        //! Renders a batch of primitives.
        static void     drawArrays(PrimitiveType primType, u32 offset, u32 count);
        
        //! Enables a vertex buffer layout.
        static void     enableInputLayout(GLbyte* pointer, const VertexBufferLayout& layout);
        
        //! Disables a vertex buffer layout.
        static void     disableInputLayout(const VertexBufferLayout& layout);
        
        //! Compiles a shader program from a source code.
        static GLuint   compileShader(GLenum type, CString source, s8* error, s32 maxErrorSize);
        
        //! Creates a program from an array of shaders.
        static GLuint   createProgram(const GLuint* shaders, s32 count, s8* error, s32 maxErrorSize);
        
        //! Destroys a program.
        static void     deleteProgram(GLuint id);
        
        //! Destroys a shader.
        static void     deleteShader(GLuint id);
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_OpenGL2_H__  */
