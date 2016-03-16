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
#include "../Assets/Renderable.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Rvm::Rvm
Rvm::Rvm( RenderingContext& context, Renderer::HalWPtr hal )
    : m_context( context )
    , m_hal( hal )
    , m_constantColor( 1.0f, 1.0f, 1.0f, 1.0f )
{
    reset();
}

// ** Rvm::setTechnique
void Rvm::setTechnique( s32 value )
{
    // Read-lock the technique
    const Technique& technique = *m_context.techniqueByIndex( value );
    DC_BREAK_IF( !m_shaders[technique.lightingModel()], "no shader set for this lighting model" );

    // Save current technique
    m_activeState.technique = value;

    // Request the shader permutation
    const Program& program = m_context.requestProgram( m_shaders[technique.lightingModel()], technique.features() ).readLock();

    // Set the permutation shader.
    setProgram( program );

    if( !program.shader().valid() ) {
        return;
    }

    u32 location;

    // Bind texture samplers used by technique
    struct {
        Program::Input  input;  //!< Program input index.
        Material::Layer layer;  //!< Material layer index.
    } samplerInputs[] = {
          { Program::DiffuseTexture,            Material::Diffuse }
        , { Program::SpecularTexture,           Material::Specular }
        , { Program::NormalTexture,             Material::Normal }
        , { Program::HeightmapTexture,          Material::Heightmap }
        , { Program::AmbientOcclusionTexture,   Material::AmbientOcclusion }
        , { Program::EmissionTexture,           Material::Emission }
        , { Program::DiffuseDetailTexture,      Material::DiffuseDetail }
        , { Program::NormalDetailTexture,       Material::NormalDetail }
        , { Program::TintTexture,               Material::Tint }
    };

    s32 samplersUsed = 0;

    for( s32 i = 0, n = technique.textureCount(); i < sizeof( samplerInputs ) / sizeof( samplerInputs[0] ); i++ ) {
        if( location = program.inputLocation( samplerInputs[i].input ) ) {
            const Texture& texture = *technique.texture( samplerInputs[i].layer );
        #if !DEV_DISABLE_DRAW_CALLS
            program.shader()->setInt( location, samplersUsed );
            m_hal->setTexture( samplersUsed, texture.texture().get() );
            samplersUsed++;
        #endif
        }
    }

    // Set colors exposed by a material
    struct {
        Program::Input  input;  //!< Program input index.
        Material::Layer layer;  //!< Material layer index.
    } colorInputs[] = {
          { Program::DiffuseColor,  Material::Diffuse }
        , { Program::SpecularColor, Material::Specular }
        , { Program::EmissionColor, Material::Emission }
        , { Program::TintColor,     Material::Tint }
    };

    for( s32 i = 0, n = technique.colorCount(); i < sizeof( colorInputs ) / sizeof( colorInputs[0] ); i++ ) {
        if( location = program.inputLocation( colorInputs[i].input ) ) {
            const Rgba& color = technique.color( colorInputs[i].layer );
        #if !DEV_DISABLE_DRAW_CALLS
            program.shader()->setVec4( location, Vec4( color.r, color.g, color.b, color.a ) );
        #endif
        }
    }

#if !DEV_DISABLE_DRAW_CALL
    program.shader()->setVec4( program.inputLocation( Program::AmbientColor ), Vec4( 0.4f, 0.4f, 0.4f ) );
#endif
}

// ** Rvm::setRenderable
void Rvm::setRenderable( s32 value )
{
    // Read-lock renderable asset
    const Renderable& renderable = *m_context.renderableByIndex( value );

    // Save current renderable
    m_activeState.renderable = value;
}

// ** Rvm::setProgram
void Rvm::setProgram( const Program& value )
{
    // This program is already set
    if( &value == m_activeState.program ) {
        return;
    }

    // Get the shader from a program
    Renderer::ShaderWPtr shader = value.shader();

    if( !shader.valid() ) {
        return;
    }

    // Bind the shader instance
#if !DEV_DISABLE_DRAW_CALLS
    m_hal->setShader( shader );
#endif

    u32 location = 0;

    // Set the view-projection matrix input
    if( location = value.inputLocation( Program::ViewProjection ) ) {
    #if !DEV_DISABLE_DRAW_CALLS
        shader->setMatrix( location, m_renderTarget.top().vp );
    #endif
    }

    // Set the constant color input
    if( location = value.inputLocation( Program::Color ) ) {
    #if !DEV_DISABLE_DRAW_CALLS
        shader->setVec4( location, m_constantColor );
    #endif
    }

    // Save active program
    m_activeState.program = &value;
}

// ** Rvm::setInstance
void Rvm::setInstance( const Commands::InstanceData& instance )
{
    // Get an active program
    const Program* program = m_activeState.program;

    // Switch the culling mode
    if( m_activeState.culling != instance.culling ) {
    #if !DEV_DISABLE_DRAW_CALLS
        m_hal->setCulling( instance.culling );
    #endif
        m_activeState.culling = instance.culling;
    }

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

#if !DEV_DISABLE_DRAW_CALLS
    // Setup rasterization states
	m_hal->setBlendFactors( options.blend.src, options.blend.dst );
	m_hal->setAlphaTest( options.alpha.function, options.alpha.reference );
	m_hal->setDepthTest( options.depth.write, options.depth.function );
#endif

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
        #if !DEV_DISABLE_DRAW_CALLS
            program->shader()->setVec4( location, m_constantColor );
        #endif
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

                                            #if !DEV_DISABLE_DRAW_CALLS
                                                // Setup the viewport for this target
                                                m_hal->setViewport( rt.viewport[0], rt.viewport[1], rt.viewport[2], rt.viewport[3] );
                                            #endif
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
                                                #if !DEV_DISABLE_DRAW_CALLS
                                                    m_hal->setViewport( rt.viewport[0], rt.viewport[1], rt.viewport[2], rt.viewport[3] );
                                                #endif
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

    case Commands::Rop::ConstantColor:      setConstantColor( userData.color );
                                            break;

    case Commands::Rop::Shader:             {
                                                // Get the shader from command user data
                                                const Commands::LightingModelShader& lightingModelShader = userData.shader;

                                                for( s32 i = 0; i < TotalLightingModels; i++ ) {
                                                    if( BIT( i ) & lightingModelShader.models ) {
                                                        m_shaders[i] = lightingModelShader.shader;
                                                    }
                                                }
                                            }
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
        const Renderable& renderable = *m_context.renderableByIndex( rop.bits.renderable );

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
    // Reset lighting shaders
    memset( m_shaders, 0, sizeof( m_shaders ) );

	// Reset rasterization options
	m_rasterization[RenderOpaque]       = RasterizationOptions::opaque();
	m_rasterization[RenderCutout]       = RasterizationOptions::cutout();
	m_rasterization[RenderAdditive]     = RasterizationOptions::additive();
	m_rasterization[RenderTranslucent]  = RasterizationOptions::translucent();

#if !DEV_DISABLE_DRAW_CALLS
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

    // Disable the alpha test
    m_hal->setAlphaTest( Renderer::CompareDisabled );
#endif
}

// ** Rvm::ActiveState::ActiveState
Rvm::ActiveState::ActiveState( void )
    : technique( -1 )
    , renderable( -1 )
    , renderingMode( -1 )
    , culling( Renderer::TriangleFaceBack )
    , program( NULL )
    , vertexBuffer( NULL )
{
}

} // namespace Scene

DC_END_DREEMCHEST