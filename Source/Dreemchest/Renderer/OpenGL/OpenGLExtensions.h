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

#ifndef		__DC_OpenGLExtensions_H__
#define		__DC_OpenGLExtensions_H__

#include	"../Renderer.h"
#include	"OpenGLHal.h"

DC_BEGIN_DREEMCHEST

namespace Renderer {

#if defined( DC_PLATFORM_WINDOWS )
	// ** GL_ARB_multitexture
	extern PFNGLACTIVETEXTUREARBPROC			glActiveTexture;
	extern PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTexture;

	// ** GL_ARB_vertex_buffer_object
	extern PFNGLBINDBUFFERARBPROC				glBindBuffer;
	extern PFNGLDELETEBUFFERSARBPROC			glDeleteBuffers;
	extern PFNGLGENBUFFERSARBPROC				glGenBuffers;
	extern PFNGLBUFFERDATAARBPROC				glBufferData;
	extern PFNGLMAPBUFFERARBPROC				glMapBuffer;
	extern PFNGLUNMAPBUFFERARBPROC				glUnmapBuffer;

	// ** GL_ARB_shading_language_100 & GL_ARB_shader_objects
	extern PFNGLCREATEPROGRAMPROC				glCreateProgram;
	extern PFNGLCREATESHADERPROC				glCreateShader;
	extern PFNGLLINKPROGRAMPROC					glLinkProgram;
	extern PFNGLDELETEPROGRAMPROC				glDeleteProgram;
	extern PFNGLDELETESHADERPROC				glDeleteShader;
	extern PFNGLATTACHSHADERPROC				glAttachShader;
	extern PFNGLCOMPILESHADERPROC				glCompileShader;
	extern PFNGLSHADERSOURCEPROC				glShaderSource;
	extern PFNGLUSEPROGRAMPROC					glUseProgram;
	extern PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
    extern PFNGLGETPROGRAMINFOLOGPROC           glGetProgramInfoLog;
	extern PFNGLGETSHADERIVPROC					glGetShaderiv;
	extern PFNGLGETPROGRAMIVPROC				glGetProgramiv;
	extern PFNGLGETUNIFORMLOCATIONARBPROC		glGetUniformLocation;
	extern PFNGLUNIFORM4FARBPROC				glUniform4f;
	extern PFNGLUNIFORM3FARBPROC				glUniform3f;
	extern PFNGLUNIFORM2FARBPROC				glUniform2f;
	extern PFNGLUNIFORM1IARBPROC				glUniform1i;
	extern PFNGLUNIFORM1FARBPROC				glUniform1f;
	extern PFNGLUNIFORM1FVARBPROC				glUniform1fv;
	extern PFNGLUNIFORM2FVARBPROC				glUniform2fv;
	extern PFNGLUNIFORM3FVARBPROC				glUniform3fv;
	extern PFNGLUNIFORM4FVARBPROC				glUniform4fv;
	extern PFNGLUNIFORMMATRIX4FVARBPROC			glUniformMatrix4fv;

	// ** GL_framebuffer_object
	extern PFNGLDELETEFRAMEBUFFERSPROC			glDeleteFramebuffers;
	extern PFNGLFRAMEBUFFERTEXTURE2DPROC		glFramebufferTexture2D;
	extern PFNGLGENFRAMEBUFFERSPROC				glGenFramebuffers;
	extern PFNGLBINDFRAMEBUFFERPROC				glBindFramebuffer;
	extern PFNGLDELETERENDERBUFFERSPROC			glDeleteRenderbuffers;
	extern PFNGLGENRENDERBUFFERSPROC			glGenRenderbuffers;
	extern PFNGLBINDRENDERBUFFERPROC			glBindRenderbuffer;
	extern PFNGLRENDERBUFFERSTORAGEPROC			glRenderbufferStorage;
	extern PFNGLCHECKFRAMEBUFFERSTATUSPROC		glCheckFramebufferStatus;
	extern PFNGLFRAMEBUFFERRENDERBUFFERPROC		glFramebufferRenderbuffer;

	// ** GL_ARB_texture_compression
	extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC		glCompressedTexImage2D;
#elif defined( DC_PLATFORM_MACOS )
    #define GL_RGBA16F              GL_RGBA16F_ARB
    #define GL_RGBA32F              GL_RGBA32F_ARB
    #define GL_RGB32F               GL_RGB32F_ARB
    #define GL_POINT_SIZE_ARRAY     GL_POINT_SIZE_ARRAY_APPLE
    #define glPointSizePointer      glPointSizePointerAPPLE
#elif defined( DC_PLATFORM_IOS ) || defined( DC_PLATFORM_ANDROID ) || defined( DC_PLATFORM_HTML5 ) // ** OpenGL ES
    #define GL_CLAMP                GL_CLAMP_TO_EDGE
    #define GL_RGB8                 GL_RGB8_OES
    #define GL_RGBA8                GL_RGBA8_OES
	#if !defined( DC_PLATFORM_ANDROID )
		#define GL_RGBA16F          GL_RGBA16F_EXT
	#endif
    #define GL_HALF_FLOAT           GL_HALF_FLOAT_OES
    #define GL_WRITE_ONLY           GL_WRITE_ONLY_OES
    #define GL_POINT_SIZE_ARRAY     GL_POINT_SIZE_ARRAY_OES

    #define glClearDepth            glClearDepthf
    #define glMapBuffer             glMapBufferOES
    #define glUnmapBuffer           glUnmapBufferOES
    #define glPointSizePointer      glPointSizePointerOES
#endif

	bool loadOpenGLExtensions( void );

} // namespace Renderer

DC_END_DREEMCHEST

#endif	/*	!__DC_OpenGLExtensions_H__	*/