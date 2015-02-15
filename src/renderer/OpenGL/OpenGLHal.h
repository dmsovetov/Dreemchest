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

#ifndef		__DC_R_OpenGLHal_H__
#define		__DC_R_OpenGLHal_H__

#include	"../Hal.h"

#ifdef DC_DEBUG
    #ifdef DC_THREADS_ENABLED
        #define CHECK_THREAD DC_BREAK_IF( m_renderThread != thread::Thread::currentThread() )
    #else
        #define CHECK_THREAD
    #endif

    #define		DC_CHECK_GL_CONTEXT	DC_BREAK_IF( glGetString( GL_EXTENSIONS ) == NULL )
    #define     DC_CHECK_GL         sOpenGLErrorCheck __gl_check; CHECK_THREAD
    #define     DC_CHECK_GL_ERROR   DC_BREAK_IF( glGetError() != GL_NO_ERROR )
#else
    #define		DC_CHECK_GL_CONTEXT
    #define     DC_CHECK_GL
    #define     DC_CHECK_GL_ERROR
#endif

// ** OpenGL headers and libraries
#if defined( DC_PLATFORM_WINDOWS )
	#include	<windows.h>

    #include	<gl/gl.h>
    #include	<gl/glu.h>
    #include	<gl/glext.h>
    #include	<gl/wglext.h>

    #pragma comment( lib, "opengl32.lib" )
    #pragma comment( lib, "glu32.lib" )
#endif

#if defined( DC_PLATFORM_ANDROID )
    #define GL_GLEXT_PROTOTYPES

    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif

#if defined( DC_PLATFORM_HTML5 )
    #define GL_GLEXT_PROTOTYPES

    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif

#if defined( DC_PLATFORM_IOS )
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#endif

#if defined( DC_PLATFORM_MACOS )
    #include <OpenGL/gl.h>
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/glext.h>
#endif

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

DC_BEGIN_DREEMCHEST

namespace renderer {

    // ** struct sOpenGLErrorCheck
    struct sOpenGLErrorCheck {
        sOpenGLErrorCheck( void ) { DC_BREAK_IF( DumpErrors() ); }
        ~sOpenGLErrorCheck( void ) { DC_BREAK_IF( DumpErrors() ); }

        bool DumpErrors( void ) {
            bool hasErrors = false;

            do {
                GLenum error = glGetError();
                if( error == GL_NO_ERROR ) {
                    break;
                }

                switch( error ) {
                case GL_INVALID_ENUM:       log::error( "GL_INVALID_ENUM\n" );      break;
                case GL_INVALID_VALUE:      log::error( "GL_INVALID_VALUE\n" );     break;
                case GL_INVALID_OPERATION:  log::error( "GL_INVALID_OPERATION\n" ); break;
                case GL_OUT_OF_MEMORY:      log::error( "GL_OUT_OF_MEMORY\n" );     break;
                case GL_STACK_OVERFLOW:     log::error( "GL_STACK_OVERFLOW\n" );    break;
                case GL_STACK_UNDERFLOW:    log::error( "GL_STACK_UNDERFLOW\n" );   break;
                default:                    log::error( "Unknown OpenGL error\n" );
                                            continue;
                }

                hasErrors = true;
            } while( /*true*/false );
            
            return hasErrors;
        }
    };

    // ** class OpenGLView
    class OpenGLView : public RenderView {
    public:

    };

    // ** class OpenGLHal
    class OpenGLHal : public Hal {
    public:

                                    OpenGLHal( RenderView* view );

        // ** Renderer
        virtual void				clear( const Rgba& clearColor, f32 depth, u32 stencil, u32 mask );
        virtual void				present( void );
        virtual void                renderPrimitives( PrimitiveType primType, u32 offset, u32 count );
        virtual void				renderIndexed( PrimitiveType primType, IndexBuffer *indexBuffer, u32 firstIndex, u32 count );
        virtual Texture2D*			createTexture2D( u32 width, u32 height, PixelFormat format );
        virtual TextureCube*        createTextureCube( u32 size, PixelFormat format );
        virtual RenderTarget*       createRenderTarget( u32 width, u32 height, PixelFormat format );
        virtual Shader*             createShader( const char *vertex, const char *fragment );
        virtual IndexBuffer*        createIndexBuffer( u32 count, bool GPU = true );
        virtual VertexBuffer*		createVertexBuffer( VertexDeclaration *declaration, u32 count, bool GPU = true );
        virtual void				setShader( Shader *shader );
        virtual void				setRenderTarget( RenderTarget *renderTarget );
        virtual void				setTexture( u32 sampler, Texture *texture );
        virtual void                setSamplerState( u32 sampler, TextureWrap wrap, TextureFilter filter );
        virtual void                setVertexBuffer( VertexBuffer *vertexBuffer, VertexDeclaration *vertexDeclaration );
        virtual void                setViewport( u32 x, u32 y, u32 width, u32 height );
        virtual void				setColorMask( u32 value );
        virtual void				setBlendFactors( BlendFactor source, BlendFactor destination );
        virtual void				setCulling( TriangleFace value );
        virtual void				setScissorTest( bool enabled, u32 x, u32 y, u32 width, u32 height );
        virtual void				setDepthTest( bool mask, Compare compare );
        virtual void				setStencilAction( TriangleFace face, Compare compare, StencilAction pass, StencilAction depthFail, StencilAction stencilFail );
        virtual void				setStencilValue( u32 value, u32 mask );
        virtual void                setBlendState( dcBlendState state );
        virtual void                setDepthStencilState( dcDepthStencilState state );
        virtual void                setRasterizerState( dcRasterizerState state );
    //    virtual void                setTransform( Transform transform, const mat4& T );
        virtual void                setColorModulation( f32 r, f32 g, f32 b, f32 a );

        // ** OpenGLRenderer
        static GLenum               blendFactor( u32 factor );
        static GLenum               compareFunc( u32 compare );
        static GLenum               triangleFace( u32 face );
        static GLenum               stencilOp( u32 action );
        static GLenum               textureWrap( u32 wrap );
        static GLenum               textureFilter( u32 filter );
        static GLenum               textureType( const Texture *texture );
        static GLuint               textureID( const Texture *texture );
        static GLenum               internalImageFormat( u32 pixelFormat );
        static GLenum               imageFormat( u32 pixelFormat );
        static GLenum               imageDataType( u32 pixelFormat );
        static u32                  imageAlign( u32 pixelFormat );

    private:

        void                        enableVertexDeclaration( const u8 *pointer, const VertexDeclaration* vertexDeclaration );
        void                        disableVertexDeclaration( const VertexDeclaration* vertexDeclaration );

    private:

        // ** struct sSampler
        struct sSampler {
            Texture*    m_texture;
            GLenum      m_wrap;
            GLenum      m_filter;
        };

        static const u32            MAX_SAMPLERS = 32;

        u32                         m_stencilValue, m_stencilMask;
        Compare                     m_stencilFunc;
        sSampler                    m_samplers[MAX_SAMPLERS];
    //    cMatrix4                    m_modelTransform, m_viewTransform;
    };

    // ** class OpenGLTexture2D
    class OpenGLTexture2D : public Texture2D {
    friend class OpenGLHal;
    public:

                                    OpenGLTexture2D( u32 width, u32 height, PixelFormat format );
        virtual                     ~OpenGLTexture2D( void );

        // ** Texture2D
        virtual void                setData( u32 level, void *data );
        virtual void*               lock( u32 level, u32& size );
        virtual void                unlock( void );

        // ** OpenGLTexture2D
        GLuint                      id( void ) const;

    private:

        GLuint                      m_id;
    };

    // ** class OpenGLTextureCube
    class OpenGLTextureCube : public TextureCube {
    friend class OpenGLHal;
    public:

                                    OpenGLTextureCube( u32 size, PixelFormat format );

        // ** TextureCube
        virtual void*               lock( u32 face );
        virtual void                unlock( u32 face );

    private:

        GLuint                      m_id;
    };

    // ** class OpenGLRenderTarget
    class OpenGLRenderTarget : public RenderTarget {
    friend class OpenGLHal;
    public:

                                    OpenGLRenderTarget( u32 width, u32 height, PixelFormat format );
        virtual                     ~OpenGLRenderTarget( void );

        // ** OpenGLRenderTarget
        bool                        setColor( OpenGLTexture2D *color );

    private:

        GLuint                      m_id;
        OpenGLTexture2D*            m_color;
    };

    // ** class OpenGLVertexBuffer
    class OpenGLVertexBuffer : public VertexBuffer {
    friend class OpenGLHal;
    public:

                                    OpenGLVertexBuffer( VertexDeclaration *vertexDeclaration, u32 count );
        virtual                     ~OpenGLVertexBuffer( void );

        // ** VertexBuffer
        virtual void*               lock( void );
        virtual void                unlock( void );

    private:

        GLuint                      m_id;
    };

    // ** class OpenGLIndexBuffer
    class OpenGLIndexBuffer : public IndexBuffer {
    friend class OpenGLHal;
    public:

                                    OpenGLIndexBuffer( u32 count );
        virtual                     ~OpenGLIndexBuffer( void );

        // ** IndexBuffer
        virtual u16*                lock( void );
        virtual void                unlock( void );

    private:

        GLuint                      m_id;
    };

    // ** class OpenGLShader
    class OpenGLShader : public Shader {
    friend class OpenGLHal;
    public:

                                    OpenGLShader( void );
        virtual                     ~OpenGLShader( void );

        // ** Shader
        virtual u32                 findUniformLocation( const char * name );
     //   virtual void                setMatrix( u32 location, const mat4& value );
        virtual void                setInt( u32 location, u32 value );
        virtual void                setFloat( u32 location, f32 value );
        virtual void                setVec2( u32 location, const Vec2& value );
     //   virtual void                setVec3( u32 location, const vec3& value );
     //   virtual void                setVec4( u32 location, const vec4& value );

        // ** OpenGLShader
        bool                        compile( GLenum shaderType, const char *data, char *error, u32 errSize );
        bool                        link( void ) const;

    private:

        GLuint                      m_program;
        GLuint                      m_vertex, m_fragment;
    };
    
} // namespace renderer

DC_END_DREEMCHEST

#endif		/*	!__DC_R_OpenGLRenderer_H__	*/