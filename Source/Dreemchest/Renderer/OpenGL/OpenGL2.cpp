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

// --------------------------------------------------------------------------------------------------------------------------------------- //

#if defined(DC_PLATFORM_WINDOWS)

// GL_ARB_multitexture
static PFNGLACTIVETEXTUREARBPROC        glActiveTexture         = NULL;
static PFNGLCLIENTACTIVETEXTUREARBPROC  glClientActiveTexture   = NULL;

// GL_ARB_vertex_buffer_object
static PFNGLBINDBUFFERARBPROC           glBindBuffer            = NULL;
static PFNGLDELETEBUFFERSARBPROC        glDeleteBuffers         = NULL;
static PFNGLGENBUFFERSARBPROC           glGenBuffers            = NULL;
static PFNGLBUFFERDATAARBPROC           glBufferData            = NULL;
static PFNGLBUFFERSUBDATAARBPROC        glBufferSubData         = NULL;

// GL_ARB_shading_language_100 & GL_ARB_shader_objects
static PFNGLCREATEPROGRAMPROC           glCreateProgram         = NULL;
static PFNGLCREATESHADERPROC            glCreateShader          = NULL;
static PFNGLLINKPROGRAMPROC             glLinkProgram           = NULL;
static PFNGLDELETEPROGRAMPROC           glDeleteProgram         = NULL;
static PFNGLDELETESHADERPROC            glDeleteShader          = NULL;
static PFNGLATTACHSHADERPROC            glAttachShader          = NULL;
static PFNGLCOMPILESHADERPROC           glCompileShader         = NULL;
static PFNGLSHADERSOURCEPROC            glShaderSource          = NULL;
static PFNGLUSEPROGRAMPROC              glUseProgram            = NULL;
static PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog      = NULL;
static PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog     = NULL;
static PFNGLGETSHADERIVPROC             glGetShaderiv           = NULL;
static PFNGLGETPROGRAMIVPROC            glGetProgramiv          = NULL;
static PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocation    = NULL;
static PFNGLUNIFORM4FARBPROC            glUniform4f             = NULL;
static PFNGLUNIFORM3FARBPROC            glUniform3f             = NULL;
static PFNGLUNIFORM2FARBPROC            glUniform2f             = NULL;
static PFNGLUNIFORM1IARBPROC            glUniform1i             = NULL;
static PFNGLUNIFORM1FARBPROC            glUniform1f             = NULL;
static PFNGLUNIFORM1FVARBPROC           glUniform1fv            = NULL;
static PFNGLUNIFORM2FVARBPROC           glUniform2fv            = NULL;
static PFNGLUNIFORM3FVARBPROC           glUniform3fv            = NULL;
static PFNGLUNIFORM4FVARBPROC           glUniform4fv            = NULL;
static PFNGLUNIFORMMATRIX4FVARBPROC     glUniformMatrix4fv      = NULL;
#elif defined(DC_PLATFORM_MACOS)
    #define GL_RGBA16F  GL_RGBA16F_ARB
    #define GL_RGBA32F  GL_RGBA32F_ARB
    #define GL_RGB32F   GL_RGB32F_ARB
#endif  //  #if defined(DC_PLATFORM_WINDOWS)
    
// ----------------------------------------------------------- OpenGL2::Buffer ----------------------------------------------------------- //

// ** OpenGL2::Buffer::create
GLuint OpenGL2::Buffer::create(GLenum type, const void* data, s32 size, GLenum usage)
{
    DC_CHECK_GL;
    
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(type, id);
    glBufferData(type, size, data, usage);
    glBindBuffer(type, 0);
    return id;
}
    
// ** OpenGL2::Buffer::subData
void OpenGL2::Buffer::subData(GLenum target, GLuint id, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    DC_CHECK_GL;
    glBindBuffer(target, id);
    glBufferSubData(target, offset, size, data);
    glBindBuffer(target, 0);
}

// ** OpenGL2::Buffer::bind
void OpenGL2::Buffer::bind(GLenum type, GLuint id)
{
    DC_CHECK_GL;
    glBindBuffer(type, id);
}
    
// ----------------------------------------------------------- OpenGL2::Program ---------------------------------------------------------- //

// ** OpenGL2::Program::deleteProgram
void OpenGL2::Program::deleteProgram(GLuint id)
{
    DC_CHECK_GL;
    glDeleteProgram(id);
}

// ** OpenGL2::Program::deleteShader
void OpenGL2::Program::deleteShader(GLuint id)
{
    DC_CHECK_GL;
    glDeleteShader(id);
}

// ** OpenGL2::Program::compileShader
GLuint OpenGL2::Program::compileShader(GLenum type, CString source, s8* error, s32 maxErrorSize)
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
        
        for (size_t i = 0; i < messages.size(); i++)
        {
            if( result == GL_FALSE )
            {
                LogError("opengl2", "%s\n", messages[i].c_str());
            }
            else
            {
                LogWarning("opengl2", "%s\n", messages[i].c_str());
            }
        }
    }
    
    return id;
}

// ** OpenGL2::Program::createProgram
GLuint OpenGL2::Program::createProgram(const GLuint* shaders, s32 count, s8* error, s32 maxErrorSize)
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
        
        for (size_t i = 0; i < messages.size(); i++)
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
    
// ** OpenGL2::Program::uniformLocation
GLint OpenGL2::Program::uniformLocation(GLuint program, CString name)
{
    return uniformLocation(program, FixedString(name));
}

// ** OpenGL2::Program::uniformLocation
GLint OpenGL2::Program::uniformLocation(GLuint program, const FixedString& name)
{
    DC_CHECK_GL;
    return glGetUniformLocation(program, name) + 1;
}
    
// ** OpenGL2::Program::uniformMatrix4
void OpenGL2::Program::uniformMatrix4(GLint location, const f32 value[16], GLboolean transpose)
{
    DC_CHECK_GL;
    glUniformMatrix4fv(location - 1, 1, transpose, value);
}

// ** OpenGL2::Program::uniform1i
void OpenGL2::Program::uniform1i(GLint location, s32 value)
{
    DC_CHECK_GL;
    glUniform1i(location - 1, value);
}

// ** OpenGL2::Program::uniform1f
void OpenGL2::Program::uniform1f(GLint location, f32 value)
{
    DC_CHECK_GL;
    glUniform1f(location - 1, value);
}

// ** OpenGL2::Program::uniform2f
void OpenGL2::Program::uniform2f(GLint location, const f32 value[2])
{
    DC_CHECK_GL;
    glUniform2fv(location - 1, 1, value);
}

// ** OpenGL2::Program::uniform3f
void OpenGL2::Program::uniform3f(GLint location, const f32 value[3])
{
    DC_CHECK_GL;
    glUniform3fv(location - 1, 1, value);
}

// ** OpenGL2::Program::uniform4f
void OpenGL2::Program::uniform4f(GLint location, const f32 value[4])
{
    DC_CHECK_GL;
    glUniform4fv(location - 1, 1, value);
}

// ** OpenGL2::Program::use
void OpenGL2::Program::use(GLuint program)
{
    glUseProgram(program);
}
    
// ----------------------------------------------------------- OpenGL2::Texture ---------------------------------------------------------- //
    
// ** OpenGL2::Texture::create
GLuint OpenGL2::Texture::create(GLenum target, const void* data, u16 width, u16 height, PixelFormat pixelFormat)
{
    DC_CHECK_GL;
    GLuint id;
    GLenum internalFormat = textureInternalFormat(pixelFormat);
    GLenum format         = textureFormat(pixelFormat);
    GLenum type           = textureType(pixelFormat);
    GLint  align          = textureAlign(pixelFormat);
    
    glGenTextures(1, &id);
    glBindTexture(target, id);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glCompressedTexImage2D( GL_TEXTURE_2D, level, internalFormat, width, height, 0, bytesPerMip( width, height ), data );
    glPixelStorei(GL_UNPACK_ALIGNMENT, align);
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
    glBindTexture(target, 0);

    return id;
}

// ** OpenGL2::Texture::bind
void OpenGL2::Texture::bind(GLenum target, GLuint id, GLuint sampler)
{
    DC_CHECK_GL;
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(target, id);
}

// ** OpenGL2::Texture::destroy
void OpenGL2::Texture::destroy(GLuint id)
{
    DC_CHECK_GL;
    glDeleteTextures(1, &id);
}

// --------------------------------------------------------------- OpenGL2 --------------------------------------------------------------- //

// ** OpenGL2::initialize
bool OpenGL2::initialize()
{
#ifdef DC_PLATFORM_WINDOWS
    glActiveTexture         = ( PFNGLACTIVETEXTUREARBPROC )         wglGetProcAddress( "glActiveTextureARB" );
    glClientActiveTexture   = ( PFNGLCLIENTACTIVETEXTUREARBPROC )   wglGetProcAddress( "glClientActiveTextureARB" );

    glBindBuffer            = ( PFNGLBINDBUFFERARBPROC )            wglGetProcAddress( "glBindBufferARB" );
    glDeleteBuffers         = ( PFNGLDELETEBUFFERSARBPROC )         wglGetProcAddress( "glDeleteBuffersARB" );
    glGenBuffers            = ( PFNGLGENBUFFERSARBPROC )            wglGetProcAddress( "glGenBuffersARB" );
    glBufferData            = ( PFNGLBUFFERDATAARBPROC )            wglGetProcAddress( "glBufferDataARB" );
    glBufferSubData         = ( PFNGLBUFFERSUBDATAARBPROC )         wglGetProcAddress( "glBufferSubDataARB" );

    glCreateProgram         =  (PFNGLCREATEPROGRAMPROC )            wglGetProcAddress( "glCreateProgram" );
    glCreateShader          = ( PFNGLCREATESHADERPROC )             wglGetProcAddress( "glCreateShader" );
    glLinkProgram           = ( PFNGLLINKPROGRAMPROC )              wglGetProcAddress( "glLinkProgram" );
    glDeleteProgram         = ( PFNGLDELETEPROGRAMPROC )            wglGetProcAddress( "glDeleteProgram" );
    glDeleteShader          = ( PFNGLDELETESHADERPROC )             wglGetProcAddress( "glDeleteShader" );
    glAttachShader          = ( PFNGLATTACHSHADERPROC)              wglGetProcAddress( "glAttachShader" );
    glCompileShader         = ( PFNGLCOMPILESHADERPROC )            wglGetProcAddress( "glCompileShader" );
    glShaderSource          = ( PFNGLSHADERSOURCEPROC )             wglGetProcAddress( "glShaderSource" );
    glUseProgram            = ( PFNGLUSEPROGRAMPROC )               wglGetProcAddress( "glUseProgram" );
    glGetShaderInfoLog      = ( PFNGLGETSHADERINFOLOGPROC )         wglGetProcAddress( "glGetShaderInfoLog" );
    glGetProgramInfoLog     = ( PFNGLGETPROGRAMINFOLOGPROC )        wglGetProcAddress( "glGetProgramInfoLog" );
    glGetProgramiv          = ( PFNGLGETPROGRAMIVPROC )             wglGetProcAddress( "glGetProgramiv" );
    glGetShaderiv           = ( PFNGLGETSHADERIVPROC )              wglGetProcAddress( "glGetShaderiv" );
    glGetUniformLocation    = ( PFNGLGETUNIFORMLOCATIONARBPROC )    wglGetProcAddress( "glGetUniformLocationARB" );
    glUniform4f             = ( PFNGLUNIFORM4FARBPROC )             wglGetProcAddress( "glUniform4fARB" );
    glUniform3f             = ( PFNGLUNIFORM3FARBPROC )             wglGetProcAddress( "glUniform3fARB" );
    glUniform2f             = ( PFNGLUNIFORM2FARBPROC )             wglGetProcAddress( "glUniform2fARB" );
    glUniform1i             = ( PFNGLUNIFORM1IARBPROC )             wglGetProcAddress( "glUniform1iARB" );
    glUniform1f             = ( PFNGLUNIFORM1FARBPROC )             wglGetProcAddress( "glUniform1fARB" );
    glUniform1fv            = ( PFNGLUNIFORM1FVARBPROC )            wglGetProcAddress( "glUniform1fvARB" );
    glUniform2fv            = ( PFNGLUNIFORM2FVARBPROC )            wglGetProcAddress( "glUniform2fvARB" );
    glUniform3fv            = ( PFNGLUNIFORM3FVARBPROC )            wglGetProcAddress( "glUniform3fvARB" );
    glUniform4fv            = ( PFNGLUNIFORM4FVARBPROC )            wglGetProcAddress( "glUniform4fvARB" );
    glUniformMatrix4fv      = ( PFNGLUNIFORMMATRIX4FVARBPROC )      wglGetProcAddress( "glUniformMatrix4fvARB" );
#endif  //  #ifdef DC_PLATFORM_WINDOWS
    return true;
}

// ** OpenGL2::clear
void OpenGL2::clear(const GLclampf* color, u8 mask, GLclampd depth, GLint stencil)
{
    DC_CHECK_GL;
    
    GLbitfield flags = 0;
    GLboolean  activeDepthMask;
    bool       shouldResetDepthMask = false;
    
    if(mask & ClearColor)
    {
        flags |= GL_COLOR_BUFFER_BIT;
    }
    if(mask & ClearDepth)
    {
        flags |= GL_DEPTH_BUFFER_BIT;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &activeDepthMask);
        
        // This forces OpenGL to clear a depth buffer even if a depth mask is FALSE
        if (activeDepthMask != GL_TRUE)
        {
            glDepthMask(GL_TRUE);
            shouldResetDepthMask = true;
        }
    }
    if(mask & ClearStencil)
    {
        flags |= GL_STENCIL_BUFFER_BIT;
    }
    
    glClearColor(color[0], color[1], color[2], color[3]);
    glClearDepth(depth);
    glClearStencil(stencil);
    glClear(flags);
    
    if (shouldResetDepthMask)
    {
        glDepthMask(activeDepthMask);
    }
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
        , GL_QUADS
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
        , GL_QUADS
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
    
// ** OpenGL2::convertStencilAction
GLenum OpenGL2::convertStencilAction(StencilAction value)
{
    switch (value)
    {
        case StencilDecSaturate:    return GL_DECR_WRAP;
        case StencilDecWrap:        return GL_DECR;
        case StencilIncSaturate:    return GL_INCR_WRAP;
        case StencilIncWrap:        return GL_INCR_WRAP;
        case StencilInvert:         return GL_INVERT;
        case StencilKeep:           return GL_KEEP;
        case StencilReplace:        return GL_REPLACE;
        case StencilZero:           return GL_ZERO;
        default:                    NIMBLE_NOT_IMPLEMENTED;
    }

    return 0;
}

// ** OpenGL2::textureInternalFormat
GLenum OpenGL2::textureInternalFormat(PixelFormat pixelFormat)
{
    switch (pixelFormat)
    {
        case PixelLuminance8:   return GL_LUMINANCE8;
        case PixelRgb8:         return GL_RGB;  // ** OpenGL ES doesnt accept GL_RGB8
        case PixelRgba8:        return GL_RGBA; // ** OpenGL ES doesnt accept GL_RGBA8
    #if !defined( DC_PLATFORM_IOS ) && !defined( DC_PLATFORM_ANDROID ) && !defined( DC_PLATFORM_HTML5 )
        case PixelDxtc1:        return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case PixelDxtc3:        return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case PixelDxtc5:        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    #endif
    #if defined( DC_PLATFORM_IOS ) || defined( DC_PLATFORM_ANDROID )
        case PixelPvrtc2:       return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
        case PixelPvrtc4:       return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
    #endif
    #if !defined( DC_PLATFORM_ANDROID ) && !defined( DC_PLATFORM_HTML5 )
        case PixelR16F:         return GL_R16F;
        case PixelR32F:         return GL_R32F;
        case PixelRgba16F:      return GL_RGBA16F;
        case PixelRgba32F:      return GL_RGBA32F;
        case PixelRgb32F:       return GL_RGB32F;
    #endif
        case PixelD24X8:        return GL_DEPTH_COMPONENT24;
        default:                NIMBLE_BREAK_IF( "Image format not implemented" );
    }
    
    return 0;
}

// ** OpenGL2::textureType
GLenum OpenGL2::textureType(PixelFormat pixelFormat)
{
    switch (pixelFormat)
    {
        case PixelLuminance8:   return GL_UNSIGNED_BYTE;
        case PixelRgb8:         return GL_UNSIGNED_BYTE;
        case PixelRgba8:        return GL_UNSIGNED_BYTE;
        case PixelDxtc1:        return 0;
        case PixelDxtc3:        return 0;
        case PixelDxtc5:        return 0;
        case PixelPvrtc2:       return 0;
        case PixelPvrtc4:       return 0;
        case PixelR16F:
        case PixelRgba16F:      return GL_HALF_FLOAT;
        case PixelR32F:
        case PixelRgb32F:
        case PixelRgba32F:      return GL_FLOAT;
        case PixelD24S8:
        case PixelD24X8:        return GL_FLOAT;
        default:                NIMBLE_BREAK_IF( "Image format not implemented" );
    }
    
    return 0;
}

// ** OpenGL2::textureFormat
GLenum OpenGL2::textureFormat(PixelFormat pixelFormat)
{
    switch (pixelFormat)
    {
        case PixelLuminance8:   return GL_LUMINANCE;
        case PixelRgb8:         return GL_RGB;
        case PixelRgba8:        return GL_RGBA;
        case PixelDxtc1:        return GL_RGB;
        case PixelDxtc3:        return GL_RGBA;
        case PixelDxtc5:        return GL_RGBA;
        case PixelPvrtc2:       return GL_RGBA;
        case PixelPvrtc4:       return GL_RGBA;
        case PixelRgb32F:       return GL_RGB;
        case PixelRgba16F:      return GL_RGBA;
        case PixelRgba32F:      return GL_RGBA;
        case PixelR32F:
        case PixelR16F:         return GL_RED;
        case PixelD24S8:
        case PixelD24X8:        return GL_DEPTH_COMPONENT;
        default:                NIMBLE_BREAK_IF( "Image format not implemented" );
    }
    
    return 0;
}
    
// ** OpenGL2::textureAlign
GLenum OpenGL2::textureAlign(PixelFormat pixelFormat)
{
    switch (pixelFormat)
    {
        case PixelRgb8: return 1;
        default:        return 4;
    }
    
    return 4;
}

} // namespace Renderer

DC_END_DREEMCHEST
