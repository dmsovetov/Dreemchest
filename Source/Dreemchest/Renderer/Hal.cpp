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
#include "RenderStateDeprecated.h"
#include "BatchRenderer.h"
#include "VertexBufferLayout.h"

#ifdef DC_THREADS
    #include <threads/Task/TaskManager.h>
    #include <threads/Thread.h>
#endif

#ifdef DC_OPENGL_ENABLED
    #include "OpenGL/OpenGLHal.h"
#endif    /*    DC_OPENGL_ENABLED    */

#undef min
#undef max

DC_BEGIN_DREEMCHEST

namespace Renderer {
    
// ----------------------------------------------- Hal ----------------------------------------------- //

#if DEV_DEPRECATED_HAL
// ** Hal::Hal
Hal::Hal( RenderView* view ) : m_view( view ), m_activeInputLayout( NULL ), m_lastInputLayout( NULL )
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
                    #ifdef DC_OPENGL_ENABLED
                        return DC_NEW OpenGLHal( view );
                    #else
                        LogError( "hal", "%s", "OpenGL renderer is not implemented\n" );
                    #endif
                    break;

    case Direct3D:  LogError( "hal", "%s", "Direct3D renderer is not implemented\n" );
                    break;
    }

    return NULL;
}

// ** Hal::createOpenGLView
RenderView* Hal::createOpenGLView( void* window, PixelFormat depthStencil )
{
#if defined( DC_OPENGL_ENABLED )
    return Renderer::createOpenGLView( window, depthStencil );
#else
    LogError( "hal", "%s", "the target platform doesn't support OpenGL.\n" );
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
void Hal::renderIndexed( PrimitiveType primType, u32 firstIndex, u32 count )
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
VertexBufferLayout* Hal::createInputLayout( s32 vertexSize )
{
    return DC_NEW VertexBufferLayout( vertexSize );
}

// ** Hal::createIndexBuffer
IndexBufferPtr Hal::createIndexBuffer( u32 count, bool GPU )
{
    if( GPU ) {
        LogWarning( "hal", "%s", "GPU index buffers are not supported\n" );
    }

    return IndexBufferPtr( DC_NEW IndexBuffer( count, false ) );
}

// ** Hal::createVertexBuffer
VertexBufferPtr Hal::createVertexBuffer( s32 size, bool GPU )
{
    if( GPU ) {
        LogWarning( "hal", "%s", "GPU vertex buffers are not supported\n" );
    }

    return VertexBufferPtr( DC_NEW VertexBuffer( size, false ) );
}

// ** Hal::createConstantBuffer
ConstantBufferPtr Hal::createConstantBuffer( u32 size, const ConstantBufferLayout* layout )
{
    if( layout == NULL ) {
        LogWarning( "hal", "%s", "GPU constant buffers are not supported\n" );
    }

    return ConstantBufferPtr( DC_NEW ConstantBuffer( size, layout ) );
}

// ** Hal::setPolygonMode
void Hal::setPolygonMode( PolygonMode mode )
{
}

// ** Hal::setPolygonOffset
void Hal::setPolygonOffset( f32 factor, f32 units )
{
}

// ** Hal::setShader
void Hal::setShader( const ShaderPtr& shader )
{
#if DEV_RENDERER_SOFTWARE_CBUFFERS
    if( !shader.valid() ) {
        return;
    }

    // Bind texture samplers
    static FixedString kSamplerNames[] = {
          "Texture0"
        , "Texture1"
        , "Texture2"
        , "Texture3"
        , "Texture4"
        , "Texture5"
        , "Texture6"
        , "Texture7"
        , "Texture8"
    };

    for( s32 i = 0, n = sizeof( kSamplerNames ) / sizeof( kSamplerNames[0] ); i < n; i++ ) {
        u32 location = shader->findUniformLocation( kSamplerNames[i] );
        if( location ) {
            const_cast<ShaderPtr&>( shader )->setInt( location, i );
        }
    }

    // Process each bound constant buffer
    for( s32 i = 0, n = static_cast<s32>( m_constantBuffers.size() ); i < n; i++ ) {
        // Get a constant buffer at index
        ConstantBufferWPtr& constantBuffer = m_constantBuffers[i];

        // Skip invalid or GPU side constant buffers
        if( !constantBuffer.valid() || constantBuffer->layout() == NULL ) {
            continue;
        }

        // Get a constant buffer data.
        const u8* data = constantBuffer->data();

        // Submit all constants to a shader
        for( const ConstantBufferLayout* constant = constantBuffer->layout(); constant->name; constant++ ) {
            // Lookup a uniform location by name
            u32 location = shader->findUniformLocation( constant->name );

            // Not found - skip
            if( location == 0 ) {
                continue;
            }

            // Submit constant to a shader
            switch( constant->type ) {
            case ConstantBufferLayout::Integer: const_cast<ShaderPtr&>( shader )->setInt( location, *reinterpret_cast<const u32*>( data + constant->offset ) );
                                                break;
            case ConstantBufferLayout::Float:   const_cast<ShaderPtr&>( shader )->setFloat( location, *reinterpret_cast<const f32*>( data + constant->offset ) );
                                                break;
            case ConstantBufferLayout::Vec2:    const_cast<ShaderPtr&>( shader )->setVec2( location, *reinterpret_cast<const Vec2*>( data + constant->offset ) );
                                                break;
            case ConstantBufferLayout::Vec3:    const_cast<ShaderPtr&>( shader )->setVec3( location, *reinterpret_cast<const Vec3*>( data + constant->offset ) );
                                                break;
            case ConstantBufferLayout::Vec4:    const_cast<ShaderPtr&>( shader )->setVec4( location, *reinterpret_cast<const Vec4*>( data + constant->offset ) );
                                                break;
            case ConstantBufferLayout::Matrix4: const_cast<ShaderPtr&>( shader )->setMatrix( location, *reinterpret_cast<const Matrix4*>( data + constant->offset ) );
                                                break;
            }
        }
    }
#endif  /*  #if DEV_RENDERER_SOFTWARE_CBUFFERS  */
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
void Hal::setVertexBuffer( const VertexBufferPtr& vertexBuffer )
{

}

void Hal::setIndexBuffer( const IndexBufferPtr& indexBuffer )
{
}

// ** Hal::setInputLayout
void Hal::setInputLayout( const VertexBufferLayout* inputLayout )
{

}

// ** Hal::setConstantBuffer
void Hal::setConstantBuffer( const ConstantBufferPtr& constantBuffer, s32 location )
{
    NIMBLE_ABORT_IF( location < 0, "invalid constant buffer location" );

    if( location >= m_constantBuffers.size() ) {
        m_constantBuffers.resize( location + 1 );
    }

    m_constantBuffers[location] = constantBuffer;
}

// ** Hal::setViewport
void Hal::setViewport( u32 x, u32 y, u32 width, u32 height )
{
    
}

// ** Hal::setViewport
void Hal::setViewport( const Rect& rect )
{
    u32 x       = static_cast<u32>( rect.min().x );
    u32 y       = static_cast<u32>( rect.min().y );
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
    NIMBLE_ABORT_IF( texture == NULL, "invalid texture" );

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
    NIMBLE_ABORT_IF( texture == NULL, "invalid texture" );

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
    NIMBLE_ABORT_IF( texture == NULL, "invalid texture" );

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
    NIMBLE_BREAK_IF( !isCompressed(), "expected to be used only for compressed textures" );

    switch( m_pixelFormat ) {
    case PixelDxtc1:    return 8;
    case PixelDxtc3:    return 16;
    case PixelDxtc5:    return 16;
    case PixelPvrtc2:   return 32;
    case PixelPvrtc4:   return 16;
    default:            NIMBLE_BREAK_IF( "Image format is not implemented" );
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
    default:            NIMBLE_BREAK_IF( "Image format is not implemented" );
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

// ** RenderTarget::setAttachment
bool RenderTarget::setAttachment( PixelFormat format, RenderTargetAttachment attachment )
{
    return false;
}

// ** RenderTarget::setDepth
bool RenderTarget::setDepth( PixelFormat format )
{
    return false;
}

// ** RenderTarget:attachment
Texture2DPtr RenderTarget::attachment( RenderTargetAttachment attachment ) const
{
    return m_attachments[attachment];
}

// ----------------------------------------------- VertexBuffer ------------------------------------------------- //

// ** VertexBuffer::VertexBuffer
VertexBuffer::VertexBuffer( s32 size, bool gpu )
    : m_size( size )
    , m_data( NULL )
    , m_isGpu( gpu )
{
    if( !m_isGpu ) {
        m_data = DC_NEW u8[size];
    }
}

VertexBuffer::~VertexBuffer( void )
{
    delete[]( u8* )m_data;
}

// ** VertexBuffer::size
u32 VertexBuffer::size( void ) const
{
    return m_size;
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

// ** VertexBuffer::setBufferData
void VertexBuffer::setBufferData( const void* source, s32 offset, s32 size )
{
    memcpy( reinterpret_cast<u8*>( m_data ) + offset, source, size );
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

// ----------------------------------------------- ConstantBuffer -------------------------------------------------- //

// ** ConstantBuffer::ConstantBuffer
ConstantBuffer::ConstantBuffer( u32 size, const ConstantBufferLayout* layout )
    : m_size( size )
    , m_data( NULL )
    , m_layout( layout )
{
#if DEV_RENDERER_SOFTWARE_CBUFFERS
    if( layout ) {
        m_data = DC_NEW u8[size];
    }
#endif  /*  #if DEV_RENDERER_SOFTWARE_CBUFFERS  */
}

ConstantBuffer::~ConstantBuffer( void )
{
    delete[]m_data;
}

// ** ConstantBuffer::size
u32 ConstantBuffer::size( void ) const
{
    return m_size;
}

// ** ConstantBuffer::lock
void* ConstantBuffer::lock( void )
{
    return m_data;
}

// ** ConstantBuffer::unlock
void ConstantBuffer::unlock( void )
{

}

#if DEV_RENDERER_SOFTWARE_CBUFFERS
// ** ConstantBuffer::data
const u8* ConstantBuffer::data( void ) const
{
    return m_data;
}

// ** ConstantBuffer::size
const ConstantBufferLayout* ConstantBuffer::layout( void ) const
{
    return m_layout;
}
#endif  /*  #if DEV_RENDERER_SOFTWARE_CBUFFERS  */

// -------------------------------------------------- Shader ---------------------------------------------------- //

// ** Shader::findUniformLocation
u32 Shader::findUniformLocation( const char * name ) const
{
    return -1;
}

// ** Shader::findUniformLocation
u32 Shader::findUniformLocation( const FixedString& name ) const
{
    return -1;
}

// ** Shader::setMatrix
void Shader::setMatrix( u32 location, const Matrix4& value )
{
    setMatrix( location, value.m );
}

// ** Shader::setMatrix
void Shader::setMatrix( u32 location, const f32 value[16] )
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
    
#endif  /*  #if DEV_DEPRECATED_HAL  */

} // namespace Renderer

DC_END_DREEMCHEST
