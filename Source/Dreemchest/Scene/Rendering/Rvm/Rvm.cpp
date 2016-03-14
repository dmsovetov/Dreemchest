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

#include "Rvm.h"
#include "../RenderTarget.h"
#include "../../Assets/Renderable.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Rvm::Rvm
Rvm::Rvm( RenderingContextWPtr context, const Array<RenderableHandle>& renderables, const Array<TechniqueHandle>& techniques, Renderer::HalWPtr hal )
    : m_context( context )
    , m_hal( hal )
    , m_renderables( renderables )
    , m_techniques( techniques )
    , m_operationCount( 0 )
    , m_instanceCount( 0 )
    , m_constantColor( Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) )
    , m_sequence( 0 )
{
    m_operations.resize( 16384 );
    m_userData.resize( 16384 );
}

// ** Rvm::setTechnique
void Rvm::setTechnique( s32 value )
{
    // Read-lock material technique
    const Technique& technique = m_techniques[value].readLock();

    if( !technique.program().isValid() ) {
        return;
    }

    // Save current technique
    m_activeState.technique = value;

    // Set the technique shader
    const Program& program = technique.program().readLock();
    Renderer::ShaderWPtr shader = program.shader();

    if( !shader.valid() ) {
        return;
    }

    if( shader != m_activeState.shader ) {
        setShader( shader );
    }

    u32 location;

    // Bind texture samplers used by technique
    for( s32 i = 0, n = technique.textureCount(); i < n; i++ ) {
        if( location = program.inputLocation( static_cast<Program::Input>( Program::Texture0 + i ) ) ) {
        #if !DEV_DISABLE_DRAW_CALLS
            shader->setInt( location, i );
        #endif
        }
    }

    // Set colors exposed by a material
    for( s32 i = 0, n = technique.colorCount(); i < n; i++ ) {
        if( location = program.inputLocation( static_cast<Program::Input>( Program::Color0 + i ) ) ) {
            const Rgba& color = technique.color( i );
        #if !DEV_DISABLE_DRAW_CALLS
            shader->setVec4( location, Vec4( color.r, color.g, color.b, color.a ) );
        #endif
        }
    }
}

// ** Rvm::setRenderable
void Rvm::setRenderable( s32 value )
{
    // Read-lock renderable asset
    const Renderable& renderable = m_renderables[value].readLock();

    // Save current renderable
    m_activeState.renderable = value;
}

// ** Rvm::setShader
void Rvm::setShader( Renderer::ShaderWPtr shader )
{
    if( !shader.valid() ) {
        return;
    }

    u32 location = 0;

    // Read-lock active material technique
    const Technique& technique = m_techniques[m_activeState.technique].readLock();

    // Read-lock active program
    const Program& program = technique.program().readLock();

    // Bind the shader instance
#if !DEV_DISABLE_DRAW_CALLS
    m_hal->setShader( shader );
#endif

    // Set the view-projection matrix input
    if( location = program.inputLocation( Program::ViewProjection ) ) {
    #if !DEV_DISABLE_DRAW_CALLS
        shader->setMatrix( location, m_renderTarget.top().vp );
    #endif
    }

    // Set the constant color input
    if( location = program.inputLocation( Program::Color ) ) {
    #if !DEV_DISABLE_DRAW_CALLS
        shader->setVec4( location, m_constantColor );
    #endif
    }

    // Save active shader
    m_activeState.shader = shader;
}

// ** Rvm::allocateUserData
Rvm::UserData* Rvm::allocateUserData( Rop* rop )
{
    UserData* instance = &m_userData[m_instanceCount++];
    rop->bits.userData = m_instanceCount - 1;
    return instance;
}

// ** Rvm::allocateRop
Rvm::Rop* Rvm::allocateRop( void )
{
    Rop* rop = &m_operations[m_operationCount++];
    rop->bits.sequence = m_sequence;
    return rop;
}

// ** Rvm::setInstance
void Rvm::setInstance( s32 value )
{
    // Get an active shader instance
    Renderer::ShaderWPtr& shader = m_activeState.shader;

    // No active shader - skip
    if( !shader.valid() ) {
        return;
    }

    // Read-lock active material technique
    const Technique& technique = m_techniques[m_activeState.technique].readLock();

    // Read-lock active program
    const Program& program = technique.program().readLock();

    // Get the instance data
    const UserData& userData = m_userData[value];

	// Set the transformation matrix
    u32 location = 0;

	if( location = program.inputLocation( Program::Transform ) ) {
    #if !DEV_DISABLE_DRAW_CALLS
		shader->setMatrix( location, *userData.instance.transform );
    #endif
	}
}

// ** Rvm::setRenderingMode
void Rvm::setRenderingMode( u8 value )
{
	DC_ABORT_IF( value >= TotalRenderModes, "invalid rendering mode" )

    // Get the rendering mode by index
	const RasterizationOptions& options = m_rasterization[value];

    // Setup rasterization states
	m_hal->setBlendFactors( options.blend.src, options.blend.dst );
	m_hal->setAlphaTest( options.alpha.function, options.alpha.reference );
	m_hal->setDepthTest( options.depth.write, options.depth.function );

    // Save current rendering mode
    m_activeState.renderingMode = value;
}

// ** Rvm::setColor
void Rvm::setColor( const Rgba& value )
{
    m_constantColor = Vec4( value.r, value.g, value.b, value.a );
}

// ** Rvm::executeCommand
void Rvm::executeCommand( const Rop& rop, const UserData& userData )
{
    switch( rop.bits.mode ) {
    case Rop::PushRenderTarget: {
                                    // Get the render target state from command user data
                                    const RenderTargetState& rt = userData.rt;

                                    // Push this render target to a stack
                                    m_renderTarget.push( rt );

                                    // Begin rendering
                                    userData.rt.instance->begin( m_context );

                                    // Setup the viewport for this target
                                    m_hal->setViewport( rt.viewport[0], rt.viewport[1], rt.viewport[2], rt.viewport[3] );
                                }
                                break;

    case Rop::PopRenderTarget:  {
                                    DC_ABORT_IF( m_renderTarget.empty(), "render target stack underflow" );

                                    // End rendering
                                    m_renderTarget.top().instance->end( m_context );

                                    // Pop render target from a stack
                                    m_renderTarget.pop();

                                    // Rollback to the previous render target
                                    if( m_renderTarget.size() ) {
                                        const RenderTargetState& rt = m_renderTarget.top();
                                        m_hal->setViewport( rt.viewport[0], rt.viewport[1], rt.viewport[2], rt.viewport[3] );
                                    }
                                }
                                break;

    case Rop::RasterOptions:    {
                                    // Get the rasterization options from command user data
                                    const RasterizationOptions& ro = userData.rasterization;

                                    for( s32 i = 0; i < TotalRenderModes; i++ ) {
                                        if( BIT( i ) & ro.modes ) {
                                            m_rasterization[i] = ro;
                                        }
                                    }
                                }
                                break;

    default:                    DC_NOT_IMPLEMENTED;
    }
}

// ** Rvm::flush
void Rvm::flush( void )
{
    // Sort accumulated commands
    std::sort( m_operations.begin(), m_operations.begin() + m_operationCount );

#if 0
    //
    CString commands[] = {
          "pushRenderTarget"
        , "popRenderTarget"
        , "rasterOptions"  
    };
    CString modes[] = {
		  "opaque"
		, "cutout"
		, "translucent"
		, "additive"
    };

    for( s32 i = 0; i < m_operationCount; i++ ) {
        const Rop& rop = m_operations[i];

        if( rop.bits.command ) {
            LogVerbose( "rvm", "%d: %s\n", rop.bits.sequence, commands[rop.bits.mode] );
        }
        else {
            LogVerbose( "rvm", "%d: drawCall : %s\n", rop.bits.sequence, modes[rop.bits.mode] );
        }
    }
#endif

    // Process all commands
    for( s32 i = 0, n = m_operationCount; i < n; i++ ) {
        // Decode the command
        const Rop& rop = m_operations[i];

        // Is this a command?
        if( rop.bits.command ) {
            executeCommand( rop, m_userData[rop.bits.userData] );
            continue;
        }

        // Set the rendering technique
        if( m_activeState.technique != rop.bits.technique ) {
            setTechnique( rop.bits.technique );
        }

        // Set the rendering mode
        if( m_activeState.renderingMode != rop.bits.mode ) {
            setRenderingMode( rop.bits.mode );
        }

        // Set renderable
        if( m_activeState.renderable != rop.bits.renderable ) {
            setRenderable( rop.bits.renderable );
        }

        // Set instance data
        setInstance( rop.bits.userData );

        // Render all chunks
        const Renderable& renderable = m_renderables[rop.bits.renderable].readLock();

        for( s32 j = 0; j < renderable.chunkCount(); j++ ) {
            Renderer::VertexBufferWPtr vertexBuffer = renderable.vertexBuffer( j );

            if( vertexBuffer != m_activeState.vertexBuffer ) {
            #if !DEV_DISABLE_DRAW_CALLS
                m_hal->setVertexBuffer( vertexBuffer );
            #endif
                m_activeState.vertexBuffer = vertexBuffer;
            }
        #if !DEV_DISABLE_DRAW_CALLS
            m_hal->renderIndexed( renderable.primitiveType(), renderable.indexBuffer( j ), 0, renderable.indexBuffer( j )->size() );
        #endif
        }        
    }

    // Clear processed commands
    m_operationCount = 0;
    m_instanceCount = 0;

    memset( &m_operations[0], 0, sizeof( Rop ) * m_operations.size() );
    memset( &m_userData[0], 0, sizeof( UserData ) * m_userData.size() );

    // Reset the sequence number
    m_sequence = 0;

    // Reset active state
    m_activeState = ActiveState();

    // Reset rendering states
    reset();
}

// ** Rvm::reset
void Rvm::reset( void )
{
	// Reset rasterization options
	m_rasterization[RenderOpaque]       = RasterizationOptions::opaque();
	m_rasterization[RenderCutout]       = RasterizationOptions::cutout();
	m_rasterization[RenderAdditive]     = RasterizationOptions::additive();
	m_rasterization[RenderTranslucent]  = RasterizationOptions::translucent();

	// Reset the face culling
	m_hal->setCulling( Renderer::TriangleFaceBack );

	// Set the default polygon mode
	m_hal->setPolygonMode( Renderer::PolygonFill );

	// Set the default shader
	m_hal->setShader( NULL );

	// Set the default vertex buffer
	m_hal->setVertexBuffer( NULL );

	// Set default textures
	for( s32 i = 0; i < 8; i++ ) {
		m_hal->setTexture( i, NULL );
	}

	// Enable the depth test back
	m_hal->setDepthTest( true, Renderer::LessEqual );
}

// ** Rvm::emitPushRenderTarget
void Rvm::emitPushRenderTarget( RenderTargetWPtr renderTarget, const Matrix4& viewProjection, const Rect& viewport )
{
    Rop* rop = allocateRop();
    rop->setCommand( Rop::PushRenderTarget );
    rop->bits.sequence = m_sequence++;

    UserData* userData = allocateUserData( rop );
    userData->rt.instance = renderTarget.get();
    userData->rt.viewport[0] = static_cast<u32>( viewport.min().x );
    userData->rt.viewport[1] = static_cast<u32>( viewport.min().y );
    userData->rt.viewport[2] = static_cast<u32>( viewport.width() );
    userData->rt.viewport[3] = static_cast<u32>( viewport.height() );

    for( s32 i = 0; i < 16; i++ ) {
        userData->rt.vp[i] = viewProjection[i];
    }
}

// ** Rvm::emitPopRenderTarget
void Rvm::emitPopRenderTarget( void )
{
    Rop* rop = allocateRop();
    rop->setCommand( Rop::PopRenderTarget );
    rop->bits.sequence = ++m_sequence;
}

// ** Rvm::emitRasterOptions
void Rvm::emitRasterOptions( u8 renderingModes, const RasterizationOptions& options )
{
    Rop* rop = allocateRop();
    rop->setCommand( Rop::RasterOptions );
    rop->bits.sequence = m_sequence++;

    UserData* userData = allocateUserData( rop );
    userData->rasterization = options;
    userData->rasterization.modes = renderingModes;
}

// ** Rvm::Rop::Rop
Rvm::Rop::Rop( void )
    : key( 0 )
{
}

// ** Rvm::Rop::operator <
bool Rvm::Rop::operator < ( const Rop& other ) const
{
    return key < other.key;
}

// ** Rvm::ActiveState::ActiveState
Rvm::ActiveState::ActiveState( void )
    : technique( -1 )
    , renderable( -1 )
    , renderingMode( -1 )
    , shader( NULL )
    , vertexBuffer( NULL )
{
}

} // namespace Scene

DC_END_DREEMCHEST