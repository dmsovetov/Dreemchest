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
Rvm::Rvm( RenderingContext& context, Renderer::HalWPtr hal )
    : m_context( context )
    , m_hal( hal )
    , m_constantColor( Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) )
{
}

// ** Rvm::setTechnique
void Rvm::setTechnique( s32 value )
{
    // If the technique is invalid - set the default program
    if( value == 0 ) {
        // Read-lock the default program
        const Program* program = m_programs.top();

        // Set the program shader
        if( m_activeState.program != program ) {
            setProgram( program );
        }

        return;
    }

    // Read-lock material technique
    const Technique& technique = m_context.techniqueByIndex( value ).readLock();

    if( !technique.program().isValid() ) {
        return;
    }

    // Save current technique
    m_activeState.technique = value;

    // Set the technique shader
    const Program* program = &technique.program().readLock();
    Renderer::ShaderWPtr shader = program->shader();

    if( !shader.valid() ) {
        return;
    }

    if( program != m_activeState.program ) {
        setProgram( program );
    }

    u32 location;

    // Bind texture samplers used by technique
    for( s32 i = 0, n = technique.textureCount(); i < n; i++ ) {
        if( location = program->inputLocation( static_cast<Program::Input>( Program::Texture0 + i ) ) ) {
        #if !DEV_DISABLE_DRAW_CALLS
            shader->setInt( location, i );
        #endif
        }
    }

    // Set colors exposed by a material
    for( s32 i = 0, n = technique.colorCount(); i < n; i++ ) {
        if( location = program->inputLocation( static_cast<Program::Input>( Program::Color0 + i ) ) ) {
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
    const Renderable& renderable = m_context.renderableByIndex( value ).readLock();

    // Save current renderable
    m_activeState.renderable = value;
}

// ** Rvm::setProgram
void Rvm::setProgram( const Program* value )
{
    // Get the shader from a program
    Renderer::ShaderWPtr shader = value->shader();

    if( !shader.valid() ) {
        return;
    }

    // Bind the shader instance
#if !DEV_DISABLE_DRAW_CALLS
    m_hal->setShader( shader );
#endif

    u32 location = 0;

    // Set the view-projection matrix input
    if( location = value->inputLocation( Program::ViewProjection ) ) {
    #if !DEV_DISABLE_DRAW_CALLS
        shader->setMatrix( location, m_renderTarget.top().vp );
    #endif
    }

    // Set the constant color input
    if( location = value->inputLocation( Program::Color ) ) {
    #if !DEV_DISABLE_DRAW_CALLS
        shader->setVec4( location, m_constantColor );
    #endif
    }

    // Save active program
    m_activeState.program = value;
}

// ** Rvm::setInstance
void Rvm::setInstance( const Commands::InstanceData& instance )
{
    // Get an active program
    const Program* program = m_activeState.program;

    // No active program - skip
    if( !program ) {
        return;
    }

	// Set the transformation matrix
    u32 location = 0;

	if( location = program->inputLocation( Program::Transform ) ) {
    #if !DEV_DISABLE_DRAW_CALLS
		program->shader()->setMatrix( location, *instance.transform );
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

// ** Rvm::setConstantColor
void Rvm::setConstantColor( const Rgba& value )
{
    // Save this color
    m_constantColor = Vec4( value.r, value.g, value.b, value.a );

    // Update the shader uniform
    if( const Program* program = m_activeState.program ) {
        u32 location = program->inputLocation( Program::Color );
        if( location ) {
            program->shader()->setVec4( location, m_constantColor );
        }
    }
}

// ** Rvm::executeCommand
void Rvm::executeCommand( const Commands::Rop& rop, const Commands::UserData& userData )
{
    switch( rop.bits.mode ) {
    case Commands::Rop::PushRenderTarget:   {
                                                // Get the render target state from command user data
                                                const Commands::RenderTargetState& rt = userData.rt;

                                                // Push this render target to a stack
                                                m_renderTarget.push( rt );

                                                // Begin rendering
                                                userData.rt.instance->begin( &m_context );

                                                // Setup the viewport for this target
                                                m_hal->setViewport( rt.viewport[0], rt.viewport[1], rt.viewport[2], rt.viewport[3] );
                                            }
                                            break;

    case Commands::Rop::PopRenderTarget:    {
                                                DC_ABORT_IF( m_renderTarget.empty(), "render target stack underflow" );

                                                // End rendering
                                                m_renderTarget.top().instance->end( &m_context );

                                                // Pop render target from a stack
                                                m_renderTarget.pop();

                                                // Rollback to the previous render target
                                                if( m_renderTarget.size() ) {
                                                    const Commands::RenderTargetState& rt = m_renderTarget.top();
                                                    m_hal->setViewport( rt.viewport[0], rt.viewport[1], rt.viewport[2], rt.viewport[3] );
                                                }
                                            }
                                            break;

    case Commands::Rop::RasterOptions:      {
                                                // Get the rasterization options from command user data
                                                const RasterizationOptions& ro = userData.rasterization;

                                                for( s32 i = 0; i < TotalRenderModes; i++ ) {
                                                    if( BIT( i ) & ro.modes ) {
                                                        m_rasterization[i] = ro;
                                                    }
                                                }
                                            }
                                            break;

    case Commands::Rop::PushProgram:        m_programs.push( userData.program );
                                            break;

    case Commands::Rop::PopProgram:         m_programs.pop();
                                            break;

    case Commands::Rop::ConstantColor:      setConstantColor( userData.color );
                                            break;

    default:                    DC_NOT_IMPLEMENTED;
    }
}

// ** Rvm::execute
void Rvm::execute( const Commands& commands )
{
#if 0
    commands.dump();
#endif

    // Process all commands
    for( s32 i = 0, n = commands.size(); i < n; i++ ) {
        // Get the command by index
        const Commands::Rop& rop = commands.ropAt( i );

        // Get the user data for current command
        const Commands::UserData& userData = commands.ropUserData( rop );

        // Is this a command?
        if( rop.bits.command ) {
            executeCommand( rop, userData );
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
        setInstance( userData.instance );

        // Render all chunks
        const Renderable& renderable = m_context.renderableByIndex( rop.bits.renderable ).readLock();

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

// ** Rvm::ActiveState::ActiveState
Rvm::ActiveState::ActiveState( void )
    : technique( -1 )
    , renderable( -1 )
    , renderingMode( -1 )
    , program( NULL )
    , vertexBuffer( NULL )
{
}

} // namespace Scene

DC_END_DREEMCHEST