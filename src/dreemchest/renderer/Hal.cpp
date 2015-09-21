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

#include "Hal.h"
#include "RenderState.h"
#include "BatchRenderer.h"

#ifdef DC_THREADS
    #include <threads/Task/TaskManager.h>
    #include <threads/Thread.h>
#endif

#ifdef DC_RENDERER_OPENGL
	#include "OpenGL/OpenGLHal.h"
#endif

#undef min
#undef max

DC_BEGIN_DREEMCHEST

namespace Renderer {

IMPLEMENT_LOGGER( log )

#ifdef DC_RENDERER_OPENGL
	//! Platform-specific OpenGL view constructor.
	extern OpenGLView* createOpenGLView( void* window, PixelFormat depthStencil );
#endif

// ----------------------------------------------- Hal ----------------------------------------------- //

// ** Hal::Hal
Hal::Hal( RenderView* view ) : m_view( view )
{
    //m_batchRenderer = NULL;

#ifdef DC_THREADS_ENABLED
    m_renderThread  = thread::Thread::currentThread();
#endif

	// ** 2D render state
	//m_rasterState2D = DC_NEW RasterizerState;
	//m_rasterState2D->setCullMode( TriangleFaceNone );
	//m_rasterState2D->setZWrite( false );

	//m_blendState2D = DC_NEW BlendState;
	//m_blendState2D->setBlendEnabled( true );
	//m_blendState2D->setBlendFunc( BlendSrcAlpha, BlendInvSrcAlpha );
}

Hal::~Hal( void )
{
	//delete m_rasterState2D;
	//delete m_blendState2D;
}

// ** Hal::create
Hal* Hal::create( RenderingHal renderer, RenderView* view )
{
    switch( renderer ) {
	case OpenGL:	
					#ifdef DC_RENDERER_OPENGL
						return DC_NEW OpenGLHal( view );
					#else
						log::error( "Hal::create : OpenGL renderer is not implemented\n" );
					#endif
					break;

    case Direct3D:  log::error( "Hal::create : Direct3D renderer is not implemented\n" );
					break;
    }

    return NULL;
}

// ** Hal::createOpenGLView
RenderView* Hal::createOpenGLView( void* window, PixelFormat depthStencil )
{
#if defined( DC_RENDERER_OPENGL )
    return Renderer::createOpenGLView( window, depthStencil );
#else
    log::error( "Hal::createOpenGLView : the target platform doesn't support OpenGL.\n" );
    return NULL;
#endif
}

// ** Hal::clear
bool Hal::clear( const Rgba& clearColor, f32 depth, u32 stencil, u32 mask )
{
    if( m_view ) {
        return m_view->beginFrame();
    }

    return true;
}

// ** Hal::present
void Hal::present( void )
{
    if( m_view ) m_view->endFrame();
}

// ** Hal::renderIndexed
void Hal::renderIndexed( PrimitiveType primType, const IndexBufferPtr& indexBuffer, u32 firstIndex, u32 count )
{
    
}

// ** Hal::renderPrimitives
void Hal::renderPrimitives( PrimitiveType primType, u32 offset, u32 count )
{

}

// ** Hal::createTexture2D
Texture2DPtr Hal::createTexture2D( u32 width, u32 height, PixelFormat format )
{
    return Texture2DPtr( DC_NEW Texture2D( width, height, format ) );
}

// ** Hal::createTextureCube
TextureCube* Hal::createTextureCube( u32 size, PixelFormat format )
{
    return DC_NEW TextureCube( size, format );
}

// ** Hal::createRenderTarget
RenderTargetPtr Hal::createRenderTarget( u32 width, u32 height )
{
    return RenderTargetPtr( DC_NEW RenderTarget( width, height ) );
}

// ** Hal::createShader
ShaderPtr Hal::createShader( const char *vertex, const char *fragment )
{
    return ShaderPtr( DC_NEW Shader );
}

// ** Hal::createVertexDeclaration
VertexDeclarationPtr Hal::createVertexDeclaration( const char *format, u32 vertexSize )
{
    VertexDeclarationPtr decl( DC_NEW VertexDeclaration );

    if( decl->parse( format ) ) {
        if( vertexSize ) decl->m_vertexSize = vertexSize;
        return decl;
    }

    return VertexDeclarationPtr();
}

// ** Hal::createIndexBuffer
IndexBufferPtr Hal::createIndexBuffer( u32 count, bool GPU )
{
	if( GPU ) {
        log::warn( "Renderer::createIndexBuffer : GPU index buffers are not supported\n" );
	}

    return IndexBufferPtr( DC_NEW IndexBuffer( count, false ) );
}

// ** Hal::createVertexBuffer
VertexBufferPtr Hal::createVertexBuffer( const VertexDeclarationPtr& declaration, u32 count, bool GPU )
{
	if( GPU ) {
		log::warn( "Renderer::createVertexBuffer : GPU vertex buffers are not supported\n" );
	}

    return VertexBufferPtr( DC_NEW VertexBuffer( declaration, count, false ) );
}

// ** Hal::setPolygonMode
void Hal::setPolygonMode( PolygonMode mode )
{
}

// ** Hal::setShader
void Hal::setShader( const ShaderPtr& shader )
{
    
}

// ** Hal::setRenderTarget
void Hal::setRenderTarget( const RenderTargetPtr& renderTarget )
{

}

// ** Hal::setTexture
void Hal::setTexture( u32 sampler, Texture *texture )
{

}

// ** Hal::setSamplerState
void Hal::setSamplerState( u32 sampler, TextureWrap wrap, TextureFilter filter )
{

}

// ** Hal::setVertexBuffer
void Hal::setVertexBuffer( const VertexBufferPtr& vertexBuffer, const VertexDeclarationWPtr& vertexDeclaration )
{

}

// ** Hal::setViewport
void Hal::setViewport( u32 x, u32 y, u32 width, u32 height )
{
    
}

// ** Hal::setViewport
void Hal::setViewport( const Rect& rect )
{
	u32 x	   = static_cast<u32>( rect.min().x );
	u32 y	   = static_cast<u32>( rect.min().y );
	u32 width  = static_cast<u32>( rect.width() );
	u32 height = static_cast<u32>( rect.height() );

	setViewport( x, y, width, height );
	setScissorTest( true, x, y, width, height );
}

// ** Hal::setColorMask
void Hal::setColorMask( u32 value )
{

}

// ** Hal::setBlendFactors
void Hal::setBlendFactors( BlendFactor source, BlendFactor destination )
{

}

// ** Hal::setFog
void Hal::setFog( FogMode mode, f32 density, const Rgba& color, f32 linearStart, f32 linearEnd )
{

}

// ** Hal::setAlphaTest
void Hal::setAlphaTest( Compare compare, float ref )
{

}

// ** Hal::setCulling
void Hal::setCulling( TriangleFace value )
{

}

// ** Hal::setScissorTest
void Hal::setScissorTest( bool enabled, u32 x, u32 y, u32 width, u32 height )
{

}

// ** Hal::setBlendState
void Hal::setBlendState( BlendState* state )
{
    
}

// ** Hal::setDepthStencilState
void Hal::setDepthStencilState( DepthStencilState* state )
{

}

// ** Hal::setRasterizerState
void Hal::setRasterizerState( RasterizerState* state )
{

}

// ** Hal::setDepthTest
void Hal::setDepthTest( bool mask, Compare compare )
{

}

// ** Hal::setStencilAction
void Hal::setStencilAction( TriangleFace face, Compare compare, StencilAction pass, StencilAction depthFail, StencilAction stencilFail )
{

}

// ** Hal::setStencilValue
void Hal::setStencilValue( u32 value, u32 mask )
{

}

struct sCreateTextureTask {
    Hal*            hal;
    Texture2DPtr*   texture;
    u32             width, height;
    PixelFormat     format;
                    sCreateTextureTask( Hal* hal, Texture2DPtr *texture, u32 width, u32 height, PixelFormat format )
                        : hal( hal ), texture( texture ), width( width ), height( height ), format( format ) {}

#ifdef DC_THREADS_ENABLED
    void thunk( dcTaskProgress progress, void *userData )
#else
    void thunk()
#endif
    {
        *texture = hal->createTexture2D( width, height, format );
    }
};

struct sLockTextureTask {
    Texture2D*  texture;
    u32*        size;
    void*       locked;

                sLockTextureTask( Texture2D* texture, u32 *size )
                    : texture( texture ), size( size ), locked( NULL ) {}

#ifdef DC_THREADS_ENABLED
    void thunk( dcTaskProgress progress, void *userData )
#else
    void thunk()
#endif
    {
        locked = texture->lock( 0, *size );
    }
};

struct sUnlockTextureTask {
    Texture2D*  texture;
                sUnlockTextureTask( Texture2D* texture )
                    : texture( texture ) {}

#ifdef DC_THREADS_ENABLED
    void thunk( dcTaskProgress progress, void *userData )
#else
    void thunk()
#endif
    {
        texture->unlock();
    }
};

// ** Hal::createTexture2D
void Hal::createTexture2D( u32 width, u32 height, PixelFormat format, Texture2DPtr *texture )
{
	DC_BREAK_IF( texture == NULL );

    sCreateTextureTask task( this, texture, width, height, format );
    *texture = NULL;

#ifdef DC_THREADS_ENABLED
    m_ctx->iTaskManager->runMainThreadTask( dcObjectMethod( &task, sCreateTextureTask::thunk ) );
#else
    task.thunk();
#endif
}

// ** Hal::lockTexture
void* Hal::lockTexture( Texture2D* texture, u32& size )
{
    DC_BREAK_IF( texture == NULL );

    sLockTextureTask task( texture, &size );

#ifdef DC_THREADS_ENABLED
    m_ctx->iTaskManager->runMainThreadTask( dcObjectMethod( &task, sLockTextureTask::thunk ) );
#else
    task.thunk();
#endif

    return task.locked;
}

// ** Hal::unlockTexture
void Hal::unlockTexture( Texture2D* texture )
{
    DC_BREAK_IF( texture == NULL );

    sUnlockTextureTask task( texture );

#ifdef DC_THREADS_ENABLED
    m_ctx->iTaskManager->runMainThreadTask( dcObjectMethod( &task, sUnlockTextureTask::thunk ) );
#else
    task.thunk();
#endif
}

// ** Hal::setTransform
void Hal::setTransform( Transform transform, const float* matrix )
{

}

// ** Hal::setColorModulation
void Hal::setColorModulation( f32 r, f32 g, f32 b, f32 a )
{

}

// ----------------------------------------------- RenderResource ----------------------------------------------- //

// ** RenderResource::release
void RenderResource::release( void )
{
    delete this;
}

// --------------------------------------------------- Texture -------------------------------------------------- //

// ** Texture::Texture
Texture::Texture( PixelFormat format, Type type ) : m_type( type ), m_pixelFormat( format ), m_locked( NULL )
{

}

// ** Texture::type
Texture::Type Texture::type( void ) const
{
	return m_type;
}

// ** Texture::pixelFormat
PixelFormat Texture::pixelFormat( void ) const {
    return m_pixelFormat;
}

// ** Texture::bytesPerMip
u32 Texture::bytesPerMip( u32 width, u32 height ) const
{
    u32 size = 0;

    switch( m_pixelFormat ) {
    case PixelDxtc1:
    case PixelDxtc3:
    case PixelDxtc5:    size = ((width + 3) >> 2) * ((height + 3) >> 2) * bytesPerBlock();
                        break;

    case PixelPvrtc2:   size = (width / 8) * (height / 4) * ((bytesPerBlock() * 4) / 8);
                        break;

    case PixelPvrtc4:   size = (width / 4) * (height / 4) * ((bytesPerBlock() * 4) / 8);
                        break;

    default:            size = width * height * bytesPerPixel();
    }

    return size;
}

// ** Texture::bytesPerBlock
u32 Texture::bytesPerBlock( void ) const
{
    DC_BREAK_IF( !isCompressed() );

    switch( m_pixelFormat ) {
    case PixelDxtc1:    return 8;
    case PixelDxtc3:    return 16;
    case PixelDxtc5:    return 16;
    case PixelPvrtc2:   return 32;
    case PixelPvrtc4:   return 16;
    default:            DC_BREAK_IF( "Image format is not implemented" );
    }

    return 0;
}

// ** Texture::bytesPerPixel
u32 Texture::bytesPerPixel( void ) const
{
    switch( m_pixelFormat ) {
    case PixelAlpha8:   return 1;
    case PixelRgb8:     return 3;
    case PixelRgba8:    return 4;
    case PixelR16F:     return 2;
    case PixelRg16F:    return 4;
    case PixelRgba16F:  return 8;
    case PixelR32F:     return 4;
    case PixelRg32F:    return 8;
    case PixelRgba32F:  return 16;
    default:            DC_BREAK_IF( "Image format is not implemented" );
    }
    
    return 0;
}

// ** Texture::isCompressed
bool Texture::isCompressed( void ) const
{
    switch( m_pixelFormat ) {
    case PixelDxtc1:
    case PixelDxtc3:
    case PixelDxtc5:
    case PixelPvrtc2:
    case PixelPvrtc4:   return true;

    default:            return false;   // ** Compiler warning fix
    }

    return false;
}

// ** Texture::isLocked
bool Texture::isLocked( void ) const
{
    return m_locked != NULL;
}

// ** Texture::locked
void* Texture::locked( void ) const
{
    return m_locked;
}

// -------------------------------------------------- Texture2D ------------------------------------------------- //

// ** Texture2D::Texture2D
Texture2D::Texture2D( u32 width, u32 height, PixelFormat format ) : Texture( format, TextureType2D ), m_width( width ), m_height( height )
{

}

// ** Texture2D::width
u32 Texture2D::width( void ) const
{
    return m_width;
}

// ** Texture2D::height
u32 Texture2D::height( void ) const
{
    return m_height;
}

// ** Texture2D::setData
void Texture2D::setData( u32 level, const void *data )
{

}

// ** Texture2D::lock
void* Texture2D::lock( u32 level, u32& size )
{
    size = 0;
    return NULL;
}

// ** Texture2D::unlock
void Texture2D::unlock( void )
{

}

// ------------------------------------------------- TextureCube ------------------------------------------------ //

// ** TextureCube::TextureCube
TextureCube::TextureCube( u32 size, PixelFormat format ) : Texture( format, TextureTypeCube ), m_size( size )
{
    
}

// ** TextureCube::size
u32 TextureCube::size( void ) const
{
    return m_size;
}

// ** TextureCube::lock
void* TextureCube::lock( u32 face )
{
    return NULL;
}

// ** TextureCube::unlock
void TextureCube::unlock( u32 face )
{
    
}

// ------------------------------------------------ RenderTarget ------------------------------------------------ //

// ** RenderTarget::RenderTarget
RenderTarget::RenderTarget( u32 width, u32 height ) : m_width( width ), m_height( height )
{

}

// ** RenderTarget::width
u32 RenderTarget::width( void ) const
{
	return m_width;
}

// ** RenderTarget::height
u32 RenderTarget::height( void ) const
{
	return m_height;
}

// ** RenderTarget::attach
bool RenderTarget::setColor( PixelFormat format, u32 index )
{
	return false;
}

// ** RenderTarget::setDepth
bool RenderTarget::setDepth( PixelFormat format )
{
	return false;
}

// ** RenderTarget:attachment
Texture2DPtr RenderTarget::color( u32 index ) const
{
	return index < ( u32 )m_color.size() ? m_color[index] : Texture2DPtr();
}

// ---------------------------------------------- VertexDeclaration --------------------------------------------- //

// ** VertexDeclaration::parse
bool VertexDeclaration::parse( const char *format )
{
    char *fmt    = strdup( format );
    char *token  = strtok( fmt, ":" );
    bool success = true;

    m_vertexSize = 0;

    do {
        u32 size = 0;

        switch( token[0] ) {
        case 'p':
        case 'P':   size = token[1] - 48;
					m_vertexElements[VertexPosition].m_offset = m_vertexSize;
                    m_vertexElements[VertexPosition].m_count  = size;
                    m_vertexSize += sizeof( f32 ) * size;
                    break;
        case 'n':
        case 'N':   m_vertexElements[VertexNormal].m_offset = m_vertexSize;
                    m_vertexElements[VertexNormal].m_count  = 3;
                    m_vertexSize += sizeof( f32 ) * 3;
                    break;
        case 'c':
        case 'C':   size = token[1] - 48;
                    m_vertexElements[VertexColor].m_offset = m_vertexSize;
                    m_vertexElements[VertexColor].m_count  = size;
                    m_vertexSize += sizeof( u8 ) * size;
                    break;

        case 's':
        case 'S':   size = token[1] - 48;
                    m_vertexElements[VertexPointSize].m_offset = m_vertexSize;
                    m_vertexElements[VertexPointSize].m_count  = 1;
                    m_vertexSize += sizeof( f32 ) * size;
                    break;

        case 't':
        case 'T':   size = token[1] - 48;
					m_vertexElements[VertexTex0 + size].m_offset = m_vertexSize;
                    m_vertexElements[VertexTex0 + size].m_count  = 2;
                    m_vertexSize += sizeof( f32 ) * 2;
                    break;

        case '*':   size = token[1] - 48;
                    m_vertexSize += sizeof( f32 ) * size;
                    break;
        }

		token = strtok( NULL, ":" );
    } while( token );

    free( ( void* )token );

    DC_BREAK_IF( !position() );

    return success;
}

// ** VertexDeclaration::vertexSize
u32 VertexDeclaration::vertexSize( void ) const
{
    return m_vertexSize;
}

// ** VertexDeclaration::position
const VertexDeclaration::VertexElement& VertexDeclaration::position( void ) const
{
    return m_vertexElements[VertexPosition];
}

// ** VertexDeclaration::color
const VertexDeclaration::VertexElement& VertexDeclaration::color( void ) const
{
    return m_vertexElements[VertexColor];
}

// ** VertexDeclaration::normal
const VertexDeclaration::VertexElement& VertexDeclaration::normal( void ) const
{
    return m_vertexElements[VertexNormal];
}

// ** VertexDeclaration::uv
const VertexDeclaration::VertexElement& VertexDeclaration::uv( u32 sampler ) const
{
    return m_vertexElements[VertexTex0 + sampler];
}

// ** VertexDeclaration::pointSize
const VertexDeclaration::VertexElement& VertexDeclaration::pointSize( void ) const
{
    return m_vertexElements[VertexPointSize];
}

// ----------------------------------------------- VertexBuffer ------------------------------------------------- //

// ** VertexBuffer::VertexBuffer
VertexBuffer::VertexBuffer( const VertexDeclarationPtr& vertexDeclaration, u32 count, bool gpu )
	: m_vertexDeclaration( vertexDeclaration ), m_size( count ), m_data( NULL ), m_isGpu( gpu )
{
    if( !m_isGpu ) {
		m_data = DC_NEW u8[vertexDeclaration->vertexSize() * count];
	}
}

VertexBuffer::~VertexBuffer( void )
{
	delete[]( u8* )m_data;
}

// ** VertexBuffer::vertexDeclaration
const VertexDeclarationPtr& VertexBuffer::vertexDeclaration( void ) const
{
    return m_vertexDeclaration;
}

// ** VertexBuffer::pointer
const void* VertexBuffer::pointer( void ) const
{
    return m_data;
}

// ** VertexBuffer::isGpu
bool VertexBuffer::isGpu( void ) const
{
	return m_isGpu;
}

// ** VertexBuffer::lock
void* VertexBuffer::lock( void )
{
    return m_data;
}

// ** VertexBuffer::unlock
void VertexBuffer::unlock( void )
{
    
}

// ----------------------------------------------- IndexBuffer -------------------------------------------------- //

// ** IndexBuffer::IndexBuffer
IndexBuffer::IndexBuffer( u32 count, bool gpu )
	: m_size( count ), m_data( NULL ), m_isGpu( gpu )
{
	if( !m_isGpu ) {
		m_data = DC_NEW u16[count];
	}
}

IndexBuffer::~IndexBuffer( void )
{
	delete[]m_data;
}

// ** IndexBuffer::size
u32 IndexBuffer::size( void ) const
{
    return m_size;
}

// ** IndexBuffer::pointer
const u16* IndexBuffer::pointer( void ) const
{
    return m_data;
}

// ** IndexBuffer::isGpu
bool IndexBuffer::isGpu( void ) const
{
	return m_isGpu;
}

// ** IndexBuffer::lock
u16* IndexBuffer::lock( void )
{
    return m_data;
}

// ** IndexBuffer::unlock
void IndexBuffer::unlock( void )
{

}

// -------------------------------------------------- Shader ---------------------------------------------------- //

// ** Shader::findUniformLocation
u32 Shader::findUniformLocation( const char * name )
{
    return -1;
}

// ** Shader::setMatrix
void Shader::setMatrix( u32 location, const Matrix4& value )
{

}

// ** Shader::setInt
void Shader::setInt( u32 location, u32 value )
{
    
}

// ** Shader::setFloat
void Shader::setFloat( u32 location, f32 value )
{
    
}

// ** Shader::setVec2
void Shader::setVec2( u32 location, const Vec2& value )
{

}

// ** Shader::setVec3
void Shader::setVec3( u32 location, const Vec3& value )
{

}

// ** Shader::setVec4
void Shader::setVec4( u32 location, const Vec4& value )
{

}

} // namespace Renderer

DC_END_DREEMCHEST