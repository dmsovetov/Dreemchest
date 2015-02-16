//
//  Filename:	OpenGLExtensions.cpp
//	Created:	26:6:2013   21:12

/*
=========================================================================================

			HEADERS & DEFS

=========================================================================================
*/

#include	"OpenGLExtensions.h"

/*
=========================================================================================

			CODE

=========================================================================================
*/

DC_BEGIN_DREEMCHEST

namespace renderer {

#ifdef DC_PLATFORM_WINDOWS
    
// ** GL_ARB_multitexture
PFNGLACTIVETEXTUREARBPROC			glActiveTexture = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTexture = NULL;

// ** GL_ARB_vertex_buffer_object
PFNGLBINDBUFFERARBPROC				glBindBuffer = NULL;
PFNGLDELETEBUFFERSARBPROC			glDeleteBuffers = NULL;
PFNGLGENBUFFERSARBPROC				glGenBuffers = NULL;
PFNGLBUFFERDATAARBPROC				glBufferData = NULL;
PFNGLMAPBUFFERARBPROC				glMapBuffer = NULL;
PFNGLUNMAPBUFFERARBPROC				glUnmapBuffer = NULL;

// ** GL_ARB_shading_language_100 & GL_ARB_shader_objects
PFNGLCREATEPROGRAMPROC				glCreateProgram = NULL;
PFNGLCREATESHADERPROC				glCreateShader = NULL;
PFNGLLINKPROGRAMPROC				glLinkProgram = NULL;
PFNGLDELETEPROGRAMPROC				glDeleteProgram = NULL;
PFNGLDELETESHADERPROC				glDeleteShader = NULL;
PFNGLATTACHSHADERPROC				glAttachShader = NULL;
PFNGLCOMPILESHADERPROC				glCompileShader = NULL;
PFNGLSHADERSOURCEPROC				glShaderSource = NULL;
PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog = NULL;
PFNGLGETPROGRAMIVPROC				glGetProgramiv = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC		glGetUniformLocation = NULL;
PFNGLUNIFORM4FARBPROC				glUniform4f = NULL;
PFNGLUNIFORM3FARBPROC				glUniform3f = NULL;
PFNGLUNIFORM2FARBPROC				glUniform2f = NULL;
PFNGLUNIFORM1IARBPROC				glUniform1i = NULL;
PFNGLUNIFORM1FARBPROC				glUniform1f = NULL;
PFNGLUNIFORM1FVARBPROC				glUniform1fv = NULL;
PFNGLUNIFORM2FVARBPROC				glUniform2fv = NULL;
PFNGLUNIFORM3FVARBPROC				glUniform3fv = NULL;
PFNGLUNIFORM4FVARBPROC				glUniform4fv = NULL;
PFNGLUNIFORMMATRIX4FVARBPROC		glUniformMatrix4fv = NULL;

// ** GL_framebuffer_object
PFNGLDELETEFRAMEBUFFERSPROC			glDeleteFramebuffers = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC		glFramebufferTexture2D = NULL;
PFNGLGENFRAMEBUFFERSPROC			glGenFramebuffers = NULL;

	// ** GL_ARB_texture_compression
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC	glCompressedTexImage2D = NULL;

// ** loadOpenGLExtensions
bool loadOpenGLExtensions( void )
{
	glActiveTexture			= ( PFNGLACTIVETEXTUREARBPROC )         wglGetProcAddress( "glActiveTextureARB" );
	glClientActiveTexture	= ( PFNGLCLIENTACTIVETEXTUREARBPROC )   wglGetProcAddress( "glClientActiveTextureARB" );

	glBindBuffer			= ( PFNGLBINDBUFFERARBPROC )            wglGetProcAddress( "glBindBufferARB" );
	glDeleteBuffers			= ( PFNGLDELETEBUFFERSARBPROC )         wglGetProcAddress( "glDeleteBuffersARB" );
	glGenBuffers			= ( PFNGLGENBUFFERSARBPROC )            wglGetProcAddress( "glGenBuffersARB" );
	glBufferData			= ( PFNGLBUFFERDATAARBPROC )            wglGetProcAddress( "glBufferDataARB" );
	glMapBuffer				= ( PFNGLMAPBUFFERARBPROC )             wglGetProcAddress( "glMapBufferARB" );
	glUnmapBuffer			= ( PFNGLUNMAPBUFFERARBPROC )           wglGetProcAddress( "glUnmapBufferARB" );

	glGetUniformLocation	= ( PFNGLGETUNIFORMLOCATIONARBPROC )    wglGetProcAddress( "glGetUniformLocationARB" );
	glUniform4f				= ( PFNGLUNIFORM4FARBPROC )             wglGetProcAddress( "glUniform4fARB" );
	glUniform3f				= ( PFNGLUNIFORM3FARBPROC )             wglGetProcAddress( "glUniform3fARB" );
	glUniform2f				= ( PFNGLUNIFORM2FARBPROC )             wglGetProcAddress( "glUniform2fARB" );
	glUniform1i				= ( PFNGLUNIFORM1IARBPROC )             wglGetProcAddress( "glUniform1iARB" );
	glUniform1f				= ( PFNGLUNIFORM1FARBPROC )             wglGetProcAddress( "glUniform1fARB" );
	glUniform1fv			= ( PFNGLUNIFORM1FVARBPROC )            wglGetProcAddress( "glUniform1fvARB" );
	glUniform2fv			= ( PFNGLUNIFORM2FVARBPROC )            wglGetProcAddress( "glUniform2fvARB" );
	glUniform3fv			= ( PFNGLUNIFORM3FVARBPROC )            wglGetProcAddress( "glUniform3fvARB" );
	glUniform4fv			= ( PFNGLUNIFORM4FVARBPROC )            wglGetProcAddress( "glUniform4fvARB" );
	glUniformMatrix4fv		= ( PFNGLUNIFORMMATRIX4FVARBPROC )      wglGetProcAddress( "glUniformMatrix4fvARB" );

	glCompressedTexImage2D	= ( PFNGLCOMPRESSEDTEXIMAGE2DARBPROC )  wglGetProcAddress( "glCompressedTexImage2DARB" );

	return true;
}

#else
// ** loadOpenGLExtensions
bool loadOpenGLExtensions( void )
{
    return true;
}
#endif

} // namespace renderer

DC_END_DREEMCHEST