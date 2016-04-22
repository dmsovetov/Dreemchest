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

#include	"OpenGLHal.h"
#include	"OpenGLExtensions.h"

#include    "../RenderStateDeprecated.h"

DC_BEGIN_DREEMCHEST

namespace Renderer {

// ----------------------------------------------- OpenGLHal ----------------------------------------------- //

// ** OpenGLHal::OpenGLHal
OpenGLHal::OpenGLHal( RenderView* view ) : Hal( view )
{
    if( m_view ) m_view->makeCurrent();

    DC_BREAK_IF( glGetString( GL_EXTENSIONS ) == NULL, "the OpenGL was not properly initialized" )

    LogVerbose( "opengl", "version=%s, renderer=%s, vendor=%s\n", glGetString( GL_VERSION ), glGetString( GL_RENDERER ), glGetString( GL_VENDOR ) );
    LogVerbose( "opengl", "%s\n", glGetString( GL_EXTENSIONS ) );

    m_stencilFunc   = Always;
    m_stencilMask   = ~0;
    m_stencilValue  =  0;

	loadOpenGLExtensions();

	for( u32 i = 0; i < MAX_SAMPLERS; i++ ) {
		m_samplers[i].m_texture = NULL;
		m_samplers[i].m_filter  = GL_LINEAR;
		m_samplers[i].m_wrap	= GL_CLAMP;

        glActiveTexture( GL_TEXTURE0 + i );
        glEnable( GL_TEXTURE_2D );
	}

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
    glEnable( GL_VERTEX_PROGRAM_POINT_SIZE );

    glActiveTexture( GL_TEXTURE0 );
    glCullFace( GL_BACK );

    while( glGetError() != GL_NO_ERROR ) {}
}

// ** OpenGLHal::clear
bool OpenGLHal::clear( const Rgba& clearColor, f32 depth, u32 stencil, u32 mask )
{
    DC_CHECK_GL;
    
    if( !Hal::clear( clearColor, depth, stencil, mask ) ) {
        return false;
    }
    
    GLbitfield flags = 0;

    if( mask & ClearColor ) {
        flags |= GL_COLOR_BUFFER_BIT;
    }
    if( mask & ClearDepth ) {
        flags |= GL_DEPTH_BUFFER_BIT;
    }
    if( mask & ClearStencil ) {
        flags |= GL_STENCIL_BUFFER_BIT;
    }

    glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
    glClearDepth( depth );
    glClearStencil( stencil );
    glClear( flags );

    return true;
}

// ** OpenGLHal::present
void OpenGLHal::present( void )
{
    Hal::present();
}

// ** OpenGLHal::renderPrimitives
void OpenGLHal::renderPrimitives( PrimitiveType primType, u32 offset, u32 count )
{
    DC_CHECK_GL;
    DC_ABORT_IF( !m_activeInputLayout.valid(), "invalid input layout" )

    static GLenum glPrimType[TotalPrimitiveTypes] = { GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_POINTS };

    glDrawArrays( glPrimType[primType], offset, count );
}

// ** OpenGLHal::renderIndexed
void OpenGLHal::renderIndexed( PrimitiveType primType, u32 firstIndex, u32 count )
{
    DC_CHECK_GL;
    DC_ABORT_IF( !m_activeInputLayout.valid(), "invalid input layout" )
    DC_ABORT_IF( !m_activeIndexBuffer.valid(), "invalid index buffer" );
    
    static GLenum glPrimType[TotalPrimitiveTypes + 1] = { GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_POINTS, 0 };
    DC_BREAK_IF( glPrimType[primType] == 0, "invalid primitive type" );

    glDrawElements( glPrimType[primType], count, GL_UNSIGNED_SHORT, m_activeIndexBuffer->pointer() + firstIndex );
}

// ** OpenGLHal::createTexture2D
Texture2DPtr OpenGLHal::createTexture2D( u32 width, u32 height, PixelFormat format )
{
    DC_CHECK_GL;
    
    return Texture2DPtr( DC_NEW OpenGLTexture2D( width, height, format ) );
}

// ** OpenGLHal::createTextureCube
TextureCube* OpenGLHal::createTextureCube( u32 size, PixelFormat format )
{
    DC_CHECK_GL;
    
    return DC_NEW OpenGLTextureCube( size, format );
}

// ** OpenGLHal::createRenderTarget
RenderTargetPtr OpenGLHal::createRenderTarget( u32 width, u32 height )
{
    DC_CHECK_GL;
	DC_CHECK_GL_CONTEXT;

    OpenGLRenderTarget* rt = DC_NEW OpenGLRenderTarget( width, height );
    return RenderTargetPtr( rt );
}

// ** OpenGLHal::createShader
ShaderPtr OpenGLHal::createShader( const char *vertex, const char *fragment )
{
    DC_CHECK_GL;
    
    OpenGLShader* shader = DC_NEW OpenGLShader;
	ShaderPtr	  result( shader );
    char          error[256];

    if( !shader->compile( GL_VERTEX_SHADER, vertex, error, sizeof( error ) ) ) {
        return ShaderPtr();
    }

    if( !shader->compile( GL_FRAGMENT_SHADER, fragment, error, sizeof( error ) ) ) {
        return ShaderPtr();
    }

    if( !shader->link() ) {
        return ShaderPtr();
    }

    return result;
}

// ** OpenGLHal::createIndexBuffer
IndexBufferPtr OpenGLHal::createIndexBuffer( u32 count, bool GPU )
{
	DC_CHECK_GL_CONTEXT
    DC_CHECK_GL;

	if( !GPU ) {
		return Hal::createIndexBuffer( count, GPU );
	}

    return IndexBufferPtr( DC_NEW OpenGLIndexBuffer( count ) );
}

// ** OpenGLHal::createVertexBuffer
VertexBufferPtr OpenGLHal::createVertexBuffer( s32 size, bool GPU )
{
	DC_CHECK_GL_CONTEXT
    DC_CHECK_GL;

	if( !GPU ) {
		return Hal::createVertexBuffer( size, GPU );
	}
    
    return VertexBufferPtr( DC_NEW OpenGLVertexBuffer( size ) );
}

// ** OpenGLHal::createConstantBuffer
ConstantBufferPtr OpenGLHal::createConstantBuffer( u32 size, const ConstantBufferLayout* layout )
{
	DC_CHECK_GL_CONTEXT
    DC_CHECK_GL;

	if( layout == NULL ) {
		LogWarning( "hal", "GPU constant buffers are not supported, creating a CPU-side emulation\n" );
	}

    return Hal::createConstantBuffer( size, layout );
}

// ** OpenGLHal::setPolygonMode
void OpenGLHal::setPolygonMode( PolygonMode mode )
{
#ifndef HAVE_OPENGLES
    static GLenum fill[] = { GL_LINE, GL_FILL };
    glPolygonMode( GL_FRONT_AND_BACK, fill[mode] );
#endif
}

// ** OpenGLHal::setShader
void OpenGLHal::setShader( const ShaderPtr& shader )
{
	DC_CHECK_GL_CONTEXT;
	DC_CHECK_GL;

	glUseProgram( shader.valid() ? static_cast<OpenGLShader*>( shader.get() )->m_program : 0 );

#if DEV_RENDERER_SOFTWARE_CBUFFERS
    // Update shader uniforms from bound constant buffers
    Hal::setShader( shader );
#endif  /*  #if DEV_RENDERER_SOFTWARE_CBUFFERS  */
}

// ** OpenGLHal::setRenderTarget
void OpenGLHal::setRenderTarget( const RenderTargetPtr& renderTarget )
{
	DC_CHECK_GL_CONTEXT;
	DC_CHECK_GL;

    glBindFramebuffer( GL_FRAMEBUFFER, renderTarget.valid() ? static_cast<OpenGLRenderTarget*>( renderTarget.get() )->m_id : 0 );
}

// ** OpenGLHal::setTexture
void OpenGLHal::setTexture( u32 sampler, Texture *texture )
{
    DC_CHECK_GL;
    DC_ABORT_IF( sampler < 0 || sampler >= MAX_SAMPLERS, "sampler index is out of range" );

    if( m_samplers[sampler].m_texture == texture ) {
        return;
    }

    GLenum glType = textureType( texture ? texture : m_samplers[sampler].m_texture );

    glActiveTexture( GL_TEXTURE0 + sampler );
    glBindTexture( glType, textureID( texture ) );

    if( texture ) {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_samplers[sampler].m_wrap );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_samplers[sampler].m_wrap );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_samplers[sampler].m_filter );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_samplers[sampler].m_filter );
    }

    m_samplers[sampler].m_texture = texture;
}

// ** OpenGLHal::setSamplerState
void OpenGLHal::setSamplerState( u32 sampler, TextureWrap wrap, TextureFilter filter )
{
    DC_CHECK_GL;
    DC_ABORT_IF( sampler < 0 || sampler >= MAX_SAMPLERS, "sampler index is out of range" );

    m_samplers[sampler].m_wrap      = textureWrap( wrap );
    m_samplers[sampler].m_filter    = textureFilter( filter );
}

// ** OpenGLHal::setVertexBuffer
void OpenGLHal::setVertexBuffer( const VertexBufferPtr& vertexBuffer )
{
	if( !vertexBuffer.valid() ) {
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		return;
	}

	if( vertexBuffer->isGpu() ) {
		static_cast<OpenGLVertexBuffer*>( vertexBuffer.get() )->bind();
	}

    m_activeVertexBuffer = vertexBuffer;

    if( m_lastInputLayout != m_activeInputLayout ) {
	    enableInputLayout( ( const u8* )vertexBuffer->pointer(), m_lastInputLayout );
    }
}

// ** OpenGLHal::setIndexBuffer
void OpenGLHal::setIndexBuffer( const IndexBufferPtr& indexBuffer )
{
    if( m_activeIndexBuffer == indexBuffer ) {
        return;
    }

    // Disable previous index buffer
    if( m_activeIndexBuffer.valid() && m_activeIndexBuffer->isGpu() ) {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    m_activeIndexBuffer = indexBuffer;

    // Bind an active index buffer
	if( m_activeIndexBuffer.valid() && m_activeIndexBuffer->isGpu() ) {
		static_cast<OpenGLIndexBuffer*>( m_activeIndexBuffer.get() )->bind();
	}
}

// ** OpenGLHal::setInputLayout
void OpenGLHal::setInputLayout( const InputLayoutPtr& inputLayout )
{
    if( m_activeInputLayout.valid() ) {
        disableInputLayout( m_activeInputLayout );
    }

    m_lastInputLayout = inputLayout;

    if( m_lastInputLayout != m_activeInputLayout && m_activeVertexBuffer.valid() ) {
	    enableInputLayout( ( const u8* )m_activeVertexBuffer->pointer(), m_lastInputLayout );
    }
}

// ** OpenGLHal::setConstantBuffer
void OpenGLHal::setConstantBuffer( const ConstantBufferPtr& constantBuffer, s32 location )
{
#if DEV_RENDERER_SOFTWARE_CBUFFERS
    Hal::setConstantBuffer( constantBuffer, location );
#endif  /*  #if DEV_RENDERER_SOFTWARE_CBUFFERS  */
}

// ** OpenGLHal::enableInputLayout
void OpenGLHal::enableInputLayout( const u8 *pointer, const InputLayoutWPtr& inputLayout )
{
    DC_CHECK_GL;

    m_activeInputLayout = inputLayout;

    s32 stride = inputLayout->vertexSize();

    const InputLayout::Element& normal     = inputLayout->normal();
    const InputLayout::Element& position   = inputLayout->position();
    const InputLayout::Element& color      = inputLayout->color();
    const InputLayout::Element& pointSize  = inputLayout->pointSize();

    if( normal ) {
        glEnableClientState( GL_NORMAL_ARRAY );
        glNormalPointer( GL_FLOAT, stride, pointer + normal.offset );
    }

    if( color ) {
        glEnableClientState( GL_COLOR_ARRAY );
        glColorPointer( color.count, GL_UNSIGNED_BYTE, stride, pointer + color.offset );
    }

    if( pointSize ) {
	#ifndef DC_PLATFORM_WINDOWS
        glEnableClientState( GL_POINT_SIZE_ARRAY );
        glPointSizePointer( GL_FLOAT, stride, pointer + pointSize.m_offset );
	#endif
    }

    for( u32 i = 0; i < InputLayout::Uv4 - InputLayout::Uv0; i++ ) {
        const InputLayout::Element& uv = inputLayout->uv( i );
        
        if( !uv ) {
            continue;
        }

        glClientActiveTexture( GL_TEXTURE0 + i );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( uv.count, GL_FLOAT, stride, pointer + uv.offset );
    }

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( position.count, GL_FLOAT, stride, pointer + position.offset );
}

// ** OpenGLHal::disableInputLayout
void OpenGLHal::disableInputLayout( const InputLayoutWPtr& inputLayout )
{
    DC_CHECK_GL;

    if( inputLayout->normal() ) {
        glDisableClientState( GL_NORMAL_ARRAY );
    }

    if( inputLayout->color() ) {
        glDisableClientState( GL_COLOR_ARRAY );
    }

    if( inputLayout->pointSize() ) {
	#ifndef DC_PLATFORM_WINDOWS
        glDisableClientState( GL_POINT_SIZE_ARRAY );
	#endif
    }

    for( u32 i = 0; i < InputLayout::Uv4 - InputLayout::Uv0; i++ ) {
        if( inputLayout->uv( i ) ) {
            glClientActiveTexture( GL_TEXTURE0 + i );
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        }
    }

    glDisableClientState( GL_VERTEX_ARRAY );

    m_activeInputLayout = NULL;
}

// ** OpenGLHal::setColorMask
void OpenGLHal::setColorMask( u32 value )
{
    DC_CHECK_GL;
    
    u32 red   = GL_FALSE;
    u32 green = GL_FALSE;
    u32 blue  = GL_FALSE;
    u32 alpha = GL_FALSE;

    if( value & ColorMaskRed ) {
        red = GL_TRUE;
    }
    if( value & ColorMaskGreen ) {
        green = GL_TRUE;
    }
    if( value & ColorMaskBlue ) {
        blue = GL_TRUE;
    }
    if( value & ColorMaskAlpha ) {
        alpha = GL_TRUE;
    }

    glColorMask( red, green, blue, alpha );
}

// ** OpenGLHal::setBlendFactors
void OpenGLHal::setBlendFactors( BlendFactor source, BlendFactor destination )
{
    DC_CHECK_GL;
    
    if( source == BlendDisabled || destination == BlendDisabled ) {
        glDisable( GL_BLEND );
    } else {
        glEnable( GL_BLEND );
        glBlendFunc( blendFactor( source ), blendFactor( destination ) );
    }
}

// ** OpenGLHal::setFog
void OpenGLHal::setFog( FogMode mode, f32 density, const Rgba& color, f32 linearStart, f32 linearEnd )
{
    DC_CHECK_GL;

    if( mode == FogDisabled ) {
        glDisable( GL_FOG );
        return;
    }

    glEnable( GL_FOG );
    glHint( GL_FOG_HINT, GL_NICEST );
    glFogfv( GL_FOG_COLOR, &color.r );

    switch( mode ) {
    case FogLinear: glFogi( GL_FOG_MODE, GL_LINEAR );
                    glFogf( GL_FOG_START, linearStart );
                    glFogf( GL_FOG_END, linearEnd );
                    break;

    case FogExp:    glFogi( GL_FOG_MODE, GL_EXP );
                    glFogf( GL_FOG_DENSITY, density );
                    break;

    case FogExp2:   glFogi( GL_FOG_MODE, GL_EXP2 );
                    glFogf( GL_FOG_DENSITY, density );
                    break;

    default:        break;
    }

    glFogi( GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH );
}

// ** OpenGLHal::setAlphaTest
void OpenGLHal::setAlphaTest( Compare compare, f32 value )
{
    DC_CHECK_GL;

    if( compare == CompareDisabled ) {
        glDisable( GL_ALPHA_TEST );
    } else {
        glEnable( GL_ALPHA_TEST );
        glAlphaFunc( compareFunc( compare ), value );
    }
}

// ** OpenGLHal::setCulling
void OpenGLHal::setCulling( TriangleFace value )
{
    DC_CHECK_GL;
    
    if( value == TriangleFaceNone ) {
        glDisable( GL_CULL_FACE );
    } else {
        glEnable( GL_CULL_FACE );
        glFrontFace( GL_CCW );
        glCullFace( triangleFace( value ) );
    }
}

// ** OpenGLHal::setBlendState
void OpenGLHal::setBlendState( BlendState* state )
{
    DC_CHECK_GL;
    DC_ABORT_IF( state == NULL, "invalid blend state" );

    if( state->m_src != RSValueNotSet || state->m_dst != RSValueNotSet ) {
        glBlendFunc( blendFactor( state->m_src ), blendFactor( state->m_dst ) );
    }

    if( state->m_blend != RSValueNotSet ) {
        state->m_blend == 1 ? glEnable( GL_BLEND ) : glDisable( GL_BLEND );
    }
}

// ** OpenGLHal::setDepthStencilState
void OpenGLHal::setDepthStencilState( DepthStencilState* state )
{
    DC_CHECK_GL;
    DC_ABORT_IF( state == NULL, "invalid depth stencil state" );

    // ** Tencil
    if( state->m_stencilEnable != RSValueNotSet ) {
        state->m_stencilEnable == 0 ? glDisable( GL_STENCIL_TEST ) : glEnable( GL_STENCIL_TEST );
    }

    if( state->m_stencilZFail != RSValueNotSet || state->m_stencilZPass != RSValueNotSet || state->m_stencilFail != RSValueNotSet ) {
        glStencilOp( stencilOp( state->m_stencilFail ), stencilOp( state->m_stencilZFail ), stencilOp( state->m_stencilZPass ) );
    }

    if( state->m_stencilMask != RSValueNotSet ) {
        glStencilMask( state->m_stencilMask );
    }

    if( state->m_stencilFunc != RSValueNotSet ) {
        glStencilFunc( compareFunc( state->m_stencilFunc ), state->m_stencilValue, state->m_stencilMask );
    }

    // ** Depth
    if( state->m_depthFunc != RSValueNotSet ) {
        glDepthFunc( compareFunc( state->m_depthFunc ) );
    }
}

// ** OpenGLHal::setRasterizerState
void OpenGLHal::setRasterizerState( RasterizerState* state )
{
    DC_CHECK_GL;
    
    if( !state ) {
        // ** OpenGL Workaround
        glDepthMask( true );
        return;
    }

#ifndef HAVE_OPENGLES
    // ** Polygon mode
    if( state->m_fillMode != RSValueNotSet ) {
        static GLenum glFill[] = { GL_LINE, GL_FILL };
        glPolygonMode( GL_FRONT_AND_BACK, glFill[state->m_fillMode] );
    }
#endif

    // ** Cull mode
    if( state->m_cullMode != RSValueNotSet ) {
        state->m_cullMode == TriangleFaceNone ? glDisable( GL_CULL_FACE ) : glEnable( GL_CULL_FACE );
        glFrontFace( GL_CCW );
        if( state->m_cullMode != TriangleFaceNone ) {
            glCullFace( triangleFace( state->m_cullMode ) );
        }
    }

    // ** Alpha
    if( state->m_alphaFunc != RSValueNotSet ) {
        state->m_alphaFunc == CompareDisabled ? glDisable( GL_ALPHA_TEST ) : glEnable( GL_ALPHA_TEST );
        glAlphaFunc( compareFunc( state->m_alphaFunc ), state->m_alphaRef );
    }

    // ** Scissor
    if( state->m_scissor != RSValueNotSet ) {
        state->m_scissor == 0 ? glDisable( GL_SCISSOR_TEST ) : glEnable( GL_SCISSOR_TEST );
    }

    // ** Z-write
    if( state->m_depth != RSValueNotSet ) {
        glDepthMask( state->m_depth == 1 );
    }

    // ** Color write
    if( state->m_red != RSValueNotSet || state->m_green != RSValueNotSet || state->m_blue != RSValueNotSet || state->m_alpha != RSValueNotSet ) {
        glColorMask( state->m_red == 1, state->m_green == 1, state->m_blue == 1, state->m_alpha == 1 );
    }
}

// ** OpenGLHal::setViewport
void OpenGLHal::setViewport( u32 x, u32 y, u32 width, u32 height )
{
    DC_CHECK_GL;
    
    glViewport( x, y, width, height );
}

// ** OpenGLHal::setScissorTest
void OpenGLHal::setScissorTest( bool enabled, u32 x, u32 y, u32 width, u32 height )
{
    DC_CHECK_GL;
    
    if( !enabled ) {
        glDisable( GL_SCISSOR_TEST );
    } else {
        glEnable( GL_SCISSOR_TEST );
        glScissor( x, y, width, height );
    }
}

// ** OpenGLHal::setDepthTest
void OpenGLHal::setDepthTest( bool mask, Compare compare )
{
    DC_CHECK_GL;

    glDepthMask( mask ? GL_TRUE : GL_FALSE );
    glDepthFunc( compareFunc( compare ) );
}

// ** OpenGLHal::setStencilAction
void OpenGLHal::setStencilAction( TriangleFace face, Compare compare, StencilAction pass, StencilAction depthFail, StencilAction stencilFail )
{
    DC_CHECK_GL;
    
    if( face == TriangleFaceNone ) {
        glDisable( GL_STENCIL_TEST );
    } else {
        m_stencilFunc = compare;

        glEnable( GL_STENCIL_TEST );
        glStencilOp( stencilOp( stencilFail ), stencilOp( depthFail ), stencilOp( pass ) );
        glStencilFunc( compareFunc( compare ), m_stencilValue, m_stencilMask );
    }
}

// ** OpenGLHal::setStencilValue
void OpenGLHal::setStencilValue( u32 value, u32 mask )
{
    DC_CHECK_GL;
    
    glStencilFunc( compareFunc( m_stencilFunc ), value, mask );
}

// ** OpenGLHal::setTransform
void OpenGLHal::setTransform( Transform transform, const float* matrix )
{
    DC_CHECK_GL;

    switch( transform ) {
    case TransformModel:        //m_modelTransform = T;
                                glMatrixMode( GL_MODELVIEW );
                                break;
            
    case TransformView:         //m_viewTransform = T;
                                glMatrixMode( GL_MODELVIEW );
                                break;

    case TransformProjection:   glMatrixMode( GL_PROJECTION );
                                break;

    default:                    DC_NOT_IMPLEMENTED;
    }

    glLoadMatrixf( matrix );

//    glLoadMatrixf( transform == TransformProjection ? T.m : (m_modelTransform * m_viewTransform).m );
}

// ** OpenGLHal::setColorModulation
void OpenGLHal::setColorModulation( f32 r, f32 g, f32 b, f32 a )
{
    DC_CHECK_GL;
    
    glColor4f( r, g, b, a );
}

// ** OpenGLHal::blendFactor
GLenum OpenGLHal::blendFactor( u32 factor )
{
    switch( factor ) {
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
    }

    DC_BREAK_IF( true );
    return 0;
}

// ** OpenGLHal::triangleFace
GLenum OpenGLHal::triangleFace( u32 face )
{
    switch( face ) {
    case TriangleFaceBack:          return GL_BACK;
    case TriangleFaceFront:         return GL_FRONT;
    case TriangleFaceFrontAndBack:  return GL_FRONT_AND_BACK;
    default:                        DC_BREAK_IF( true );
    }

    DC_BREAK_IF( true );
    return 0;
}

// ** OpenGLHal::compareFunc
GLenum OpenGLHal::compareFunc( u32 compare )
{
    switch( compare ) {
    case Always:        return GL_ALWAYS;
    case Equal:         return GL_EQUAL;
    case Greater:       return GL_GREATER;
    case GreaterEqual:  return GL_GEQUAL;
    case Less:          return GL_LESS;
    case LessEqual:     return GL_LEQUAL;
    case Never:         return GL_NEVER;
    case NotEqual:      return GL_NOTEQUAL;
    }

    DC_BREAK_IF( true );
    return 0;
}

// ** OpenGLHal::stencilOp
GLenum OpenGLHal::stencilOp( u32 action )
{
    switch( action ) {
    case StencilDecSaturate:    return GL_DECR_WRAP;
    case StencilDecWrap:        return GL_DECR;
    case StencilIncSaturate:    return GL_INCR_WRAP;
    case StencilIncWrap:        return GL_INCR_WRAP;
    case StencilInvert:         return GL_INVERT;
    case StencilKeep:           return GL_KEEP;
    case StencilSet:            return GL_SET;
    case StencilZero:           return GL_ZERO;
    }

    DC_BREAK_IF( true );
    return 0;
}

// ** OpenGLHal::textureWrap
GLenum OpenGLHal::textureWrap( u32 wrap )
{
    switch( wrap ) {
    case Clamp:     return GL_CLAMP;
    case Repeat:    return GL_REPEAT;
    }

    DC_BREAK_IF( true );
    return 0;
}

// ** OpenGLHal::textureFilter
GLenum OpenGLHal::textureFilter( u32 filter )
{
    switch( filter ) {
    case FilterNearest:     return GL_NEAREST;                  break;
    case FilterLinear:      return GL_LINEAR;                   break;
    case FilterMipNearest:  return GL_NEAREST_MIPMAP_NEAREST;   break;
    case FilterMipLinear:   return GL_LINEAR_MIPMAP_LINEAR;     break;
    }

    DC_BREAK_IF( true );
    return 0;
}

// ** OpenGLHal::textureType
GLenum OpenGLHal::textureType( const Texture *texture )
{
    DC_ABORT_IF( texture == NULL, "invalid texture" );

	switch( texture->type() ) {
	case Texture::TextureType2D:	return GL_TEXTURE_2D;
	case Texture::TextureTypeCube:	return GL_TEXTURE_CUBE_MAP;
	}

    DC_BREAK_IF( true );
    return 0;
}

// ** OpenGLHal::textureID
GLuint OpenGLHal::textureID( const Texture *texture )
{
    if( !texture ) {
        return 0;
    }

	switch( texture->type() ) {
	case Texture::TextureType2D:	return static_cast<const OpenGLTexture2D*>( texture )->m_id;
    case Texture::TextureTypeCube:	return static_cast<const OpenGLTextureCube*>( texture )->m_id;
	}

    DC_BREAK_IF( true );
    return 0;
}

// ** OpenGLHal::internalImageFormat
GLenum OpenGLHal::internalImageFormat( u32 pixelFormat )
{
    switch( pixelFormat ) {
    case PixelAlpha8:   return GL_ALPHA;
    case PixelRgb8:     return GL_RGB;  // ** OpenGL ES doesnt accept GL_RGB8
    case PixelRgba8:    return GL_RGBA; // ** OpenGL ES doesnt accept GL_RGBA8
#if !defined( DC_PLATFORM_IOS ) && !defined( DC_PLATFORM_ANDROID ) && !defined( DC_PLATFORM_HTML5 )
    case PixelDxtc1:    return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    case PixelDxtc3:    return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case PixelDxtc5:    return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
#endif
#if defined( DC_PLATFORM_IOS ) || defined( DC_PLATFORM_ANDROID )
    case PixelPvrtc2:   return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
    case PixelPvrtc4:   return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
#endif
#if !defined( DC_PLATFORM_ANDROID ) && !defined( DC_PLATFORM_HTML5 )
    case PixelRgba16F:  return GL_RGBA16F;
    case PixelRgba32F:  return GL_RGBA32F;
    case PixelRgb32F:   return GL_RGB32F;
#endif
	case PixelD24X8:	return GL_DEPTH_COMPONENT24;
    default:            DC_BREAK_IF( "Image format not implemented" );
    }

    return 0;
}

// ** OpenGLHal::imageFormat
GLenum OpenGLHal::imageFormat( u32 pixelFormat )
{
    switch( pixelFormat ) {
    case PixelAlpha8:   return GL_ALPHA;
    case PixelRgb8:     return GL_RGB;
    case PixelRgba8:    return GL_RGBA;
    case PixelDxtc1:    return GL_RGB;
    case PixelDxtc3:    return GL_RGBA;
    case PixelDxtc5:    return GL_RGBA;
    case PixelPvrtc2:   return GL_RGBA;
    case PixelPvrtc4:   return GL_RGBA;
    case PixelRgb32F:   return GL_RGB;
    case PixelRgba16F:  return GL_RGBA;
    case PixelRgba32F:  return GL_RGBA;
    default:            DC_BREAK_IF( "Image format not implemented" );
    }
    
    return 0;
}

// ** OpenGLHal::imageDataType
GLenum OpenGLHal::imageDataType( u32 pixelFormat )
{
    switch( pixelFormat ) {
    case PixelAlpha8:   return GL_UNSIGNED_BYTE;
    case PixelRgb8:     return GL_UNSIGNED_BYTE;
    case PixelRgba8:    return GL_UNSIGNED_BYTE;
    case PixelDxtc1:    return 0;
    case PixelDxtc3:    return 0;
    case PixelDxtc5:    return 0;
    case PixelPvrtc2:   return 0;
    case PixelPvrtc4:   return 0;
    case PixelRgba16F:  return GL_HALF_FLOAT;
    case PixelRgb32F:
    case PixelRgba32F:  return GL_FLOAT;
    default:            DC_BREAK_IF( "Image format not implemented" );
    }
    
    return 0;
}

// ** OpenGLHal::imageAlign
u32 OpenGLHal::imageAlign( u32 pixelFormat )
{
    switch( pixelFormat ) {
    case PixelRgb8: return 1;
    default:        return 4;
    }

    return 4;
}

// -------------------------------------------------- OpenGLTexture2D ------------------------------------------------- //

// ** OpenGLTexture2D::OpenGLTexture2D
OpenGLTexture2D::OpenGLTexture2D( u32 width, u32 height, PixelFormat format ) : Texture2D( width, height, format )
{
    DC_CHECK_GL;
    glGenTextures( 1, &m_id );
}

OpenGLTexture2D::~OpenGLTexture2D( void )
{
//    DC_CHECK_GL;
    glDeleteTextures( 1, &m_id );
}

// ** OpenGLTexture2D::id
GLuint OpenGLTexture2D::id( void ) const
{
    return m_id;
}

// ** OpenGLTexture2D::setData
void OpenGLTexture2D::setData( u32 level, const void *data )
{
    DC_CHECK_GL;

    GLenum internalFormat = OpenGLHal::internalImageFormat( m_pixelFormat );
    GLenum format         = OpenGLHal::imageFormat( m_pixelFormat );
    GLenum type           = OpenGLHal::imageDataType( m_pixelFormat );
    u32    width          = m_width >> level;
    u32    height         = m_height >> level;

    glBindTexture( GL_TEXTURE_2D, m_id );
    if( isCompressed() ) {
        glCompressedTexImage2D( GL_TEXTURE_2D, level, internalFormat, width, height, 0, bytesPerMip( width, height ), data );
    } else {
        glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
        glTexImage2D( GL_TEXTURE_2D, level, internalFormat, width, height, 0, format, type, data );
    }
    glBindTexture( GL_TEXTURE_2D, 0 );
}

// ** OpenGLTexture2D::lock
void* OpenGLTexture2D::lock( u32 level, u32& size )
{
    DC_BREAK_IF( isLocked(), "already locked" );

    size            = bytesPerMip( m_width >> level, m_height >> level );
    m_locked        = DC_NEW u8[size];
    m_lockedLevel   = level;

    return m_locked;
}

// ** OpenGLTexture2D::unlock
void OpenGLTexture2D::unlock( void )
{
    DC_CHECK_GL;
    DC_BREAK_IF( !isLocked(), "already unlocked" );
    
    GLenum internalFormat = OpenGLHal::internalImageFormat( m_pixelFormat );
    GLenum format         = OpenGLHal::imageFormat( m_pixelFormat );
    GLenum type           = OpenGLHal::imageDataType( m_pixelFormat );
    u32    align          = OpenGLHal::imageAlign( m_pixelFormat );
    u32    width          = m_width >> m_lockedLevel;
    u32    height         = m_height >> m_lockedLevel;

    glBindTexture( GL_TEXTURE_2D, m_id );
    if( isCompressed() ) {
        glCompressedTexImage2D( GL_TEXTURE_2D, m_lockedLevel, internalFormat, width, height, 0, bytesPerMip( width, height ), m_locked );
    } else {
        glPixelStorei( GL_UNPACK_ALIGNMENT, align );
        glTexImage2D( GL_TEXTURE_2D, m_lockedLevel, internalFormat, width, height, 0, format, type, m_locked );
    }
    glBindTexture( GL_TEXTURE_2D, 0 );

    delete[]( u8* )m_locked;
    m_locked = NULL;
}

// ------------------------------------------------- OpenGLTextureCube ------------------------------------------------ //

// ** OpenGLTextureCube::OpenGLTextureCube
OpenGLTextureCube::OpenGLTextureCube( u32 size, PixelFormat format ) : TextureCube( size, format )
{

}

// ** OpenGLTextureCube::lock
void* OpenGLTextureCube::lock( u32 face )
{
    return NULL;
}

// ** OpenGLTextureCube::unlock
void OpenGLTextureCube::unlock( u32 face )
{

}

// ------------------------------------------------ OpenGLRenderTarget ------------------------------------------------ //

// ** OpenGLRenderTarget::OpenGLRenderTarget
OpenGLRenderTarget::OpenGLRenderTarget( u32 width, u32 height ) : RenderTarget( width, height ), m_depth( 0 )
{
    DC_CHECK_GL;
	DC_CHECK_GL_CONTEXT;

    glGenFramebuffers( 1, &m_id );
}

OpenGLRenderTarget::~OpenGLRenderTarget( void )
{
	if( m_depth ) {
		glDeleteRenderbuffers( 1, &m_depth );
	}

    glDeleteFramebuffers( 1, &m_id );
}

// ** OpenGLRenderTarget::check
bool OpenGLRenderTarget::check( void ) const
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_id );
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return status == GL_FRAMEBUFFER_COMPLETE;
}

// ** OpenGLRenderTarget::setColor
bool OpenGLRenderTarget::setColor( PixelFormat format, u32 index )
{
    DC_CHECK_GL;
	DC_CHECK_GL_CONTEXT;

	OpenGLTexture2D* texture = DC_NEW OpenGLTexture2D( m_width, m_height, format );
	texture->setData( 0, NULL );
    
	glBindFramebuffer( GL_FRAMEBUFFER, m_id );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id(), 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	if( index >= m_color.size() ) {
		m_color.resize( index + 1 );
	}

    m_color[index] = texture;
	DC_ABORT_IF( !check(), "invalid render target configuration" );

	return check();
}

// ** OpenGLRenderTarget::setDepth
bool OpenGLRenderTarget::setDepth( PixelFormat format )
{
	glGenRenderbuffers( 1, &m_depth );
	glBindRenderbuffer( GL_RENDERBUFFER, m_depth );
	glRenderbufferStorage( GL_RENDERBUFFER, OpenGLHal::internalImageFormat( format ), m_width, m_height );

	glBindFramebuffer( GL_FRAMEBUFFER, m_id );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return check();
}

// ----------------------------------------------- OpenGLVertexBuffer ------------------------------------------------- //

// ** OpenGLVertexBuffer::OpenGLVertexBuffer
OpenGLVertexBuffer::OpenGLVertexBuffer( s32 size )
	: VertexBuffer( size, true )
{
    DC_CHECK_GL;
    
	glGenBuffers( 1, &m_id );
	glBindBuffer( GL_ARRAY_BUFFER, m_id );
	glBufferData( GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

OpenGLVertexBuffer::~OpenGLVertexBuffer( void )
{
    DC_CHECK_GL;
    
    glDeleteBuffers( 1, &m_id );
}

// ** OpenGLVertexBuffer::bind
void OpenGLVertexBuffer::bind( void )
{
	DC_CHECK_GL;
	glBindBuffer( GL_ARRAY_BUFFER, m_id );
}

// ** OpenGLVertexBuffer::lock
void* OpenGLVertexBuffer::lock( void )
{
    DC_CHECK_GL;
    
	glBindBuffer( GL_ARRAY_BUFFER, m_id );
	return glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
}

// ** OpenGLVertexBuffer::unlock
void OpenGLVertexBuffer::unlock( void )
{
    DC_CHECK_GL;
    
    glUnmapBuffer( GL_ARRAY_BUFFER );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

// ----------------------------------------------- OpenGLIndexBuffer -------------------------------------------------- //

// ** OpenGLIndexBuffer::OpenGLIndexBuffer
OpenGLIndexBuffer::OpenGLIndexBuffer( u32 count )
	: IndexBuffer( count, true )
{
    DC_CHECK_GL;
    
    glGenBuffers( 1, &m_id );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_id );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, count * sizeof( u16 ), NULL, GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

OpenGLIndexBuffer::~OpenGLIndexBuffer( void )
{
    glDeleteBuffers( 1, &m_id );
}

// ** OpenGLIndexBuffer::bind
void OpenGLIndexBuffer::bind( void )
{
	DC_CHECK_GL;
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_id );
}

// ** OpenGLIndexBuffer::lock
u16* OpenGLIndexBuffer::lock( void )
{
    DC_CHECK_GL;

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_id );
	return ( u16* )glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY );
}

// ** OpenGLIndexBuffer::unlock
void OpenGLIndexBuffer::unlock( void )
{
    DC_CHECK_GL;
    
    glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

// -------------------------------------------------- OpenGLShader ---------------------------------------------------- //

// ** OpenGLShader::OpenGLShader
OpenGLShader::OpenGLShader( void )
{
    m_program  = glCreateProgram();
    m_vertex   = 0;
    m_fragment = 0;
}

OpenGLShader::~OpenGLShader( void )
{
    glDeleteShader( m_vertex );
    glDeleteShader( m_fragment );
    glDeleteProgram( m_program );
}

// ** OpenGLShader::link
bool OpenGLShader::link( void ) const
{
    DC_CHECK_GL;
    
    GLint result;
    
    glLinkProgram( m_program );
    glGetProgramiv( m_program, GL_LINK_STATUS, &result );

    if( result == GL_FALSE ) {
		s8 error[256];
        glGetProgramInfoLog( m_program, sizeof( error ), NULL, error );
		LogError( "opengl", "failed to link shader, %s\n", error );
        return false;
    }

    return result != GL_FALSE;
}

// ** OpenGLShader::compile
bool OpenGLShader::compile( GLenum shaderType, CString data, char *error, u32 errSize )
{
    DC_CHECK_GL;
    
    GLint result;
    
    GLuint id = glCreateShader( shaderType );
    glShaderSource( id, 1, &data, NULL );
    glCompileShader( id );
    glGetShaderiv( id, GL_COMPILE_STATUS, &result );

    switch( shaderType ) {
    case GL_VERTEX_SHADER:      m_vertex   = id; break;
    case GL_FRAGMENT_SHADER:    m_fragment = id; break;
    }

    if( result == GL_FALSE ) {
        glGetShaderInfoLog( id, errSize, NULL, error );
		LogError( "opengl", "failed to compile shader, %s\n", error );
        return false;
    }

    glAttachShader( m_program, id );
    return true;
}

// ** OpenGLShader::findUniformLocation
u32 OpenGLShader::findUniformLocation( const char *name ) const
{
    DC_CHECK_GL;

    String32 hash( name );

    UniformLocations::const_iterator i = m_uniformLocations.find( hash );

    if( i != m_uniformLocations.end() ) {
        return i->second;
    }
    
	u32 location = glGetUniformLocation( m_program, name ) + 1;
    m_uniformLocations[hash] = location;

    return location;
}

// ** OpenGLShader::setMatrix
void OpenGLShader::setMatrix( u32 location, const f32 value[16] )
{
    DC_CHECK_GL;
    glUniformMatrix4fv( location - 1, 1, false, value );
}

// ** OpenGLShader::setInt
void OpenGLShader::setInt( u32 location, u32 value )
{
    DC_CHECK_GL;
    glUniform1i( location - 1, value );
}

// ** OpenGLShader::setFloat
void OpenGLShader::setFloat( u32 location, f32 value )
{
    DC_CHECK_GL;
    glUniform1f( location - 1, value );
}

// ** OpenGLShader::setVec2
void OpenGLShader::setVec2( u32 location, const Vec2& value )
{
    DC_CHECK_GL;
    glUniform2fv( location - 1, 1, &value.x );
}

// ** OpenGLShader::setVec3
void OpenGLShader::setVec3( u32 location, const Vec3& value )
{
    DC_CHECK_GL;
    glUniform3fv( location - 1, 1, &value.x );
}

// ** OpenGLShader::setVec4
void OpenGLShader::setVec4( u32 location, const Vec4& value )
{
    DC_CHECK_GL;
    glUniform4fv( location - 1, 1, &value.x );
}

} // namespace Renderer

DC_END_DREEMCHEST