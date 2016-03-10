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
#include "../Assets/Renderable.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Rvm::Rvm
Rvm::Rvm( const Array<RenderableHandle>& renderables, const Array<TechniqueHandle>& techniques, Renderer::HalWPtr hal )
    : m_hal( hal )
    , m_renderables( renderables )
    , m_techniques( techniques )
{
}

// ** Rvm::push
void Rvm::push( const Matrix4* transform, s32 renderable, s32 technique )
{
    Instance instance;
    instance.transform = transform;
    m_instances.push_back( instance );

    Command cmd;
    cmd.bits.renderable = renderable;
    cmd.bits.technique  = technique;
    cmd.bits.instance   = m_instances.size() - 1;
    m_commands.push_back( cmd );
}

// ** Rvm::setTechnique
void Rvm::setTechnique( s32 value )
{
    // Read-lock material technique
    const Technique& technique = m_techniques[value].readLock();

    // Save current technique
    m_activeState.technique = value;

    // Set the technique shader
    if( technique.shader() != m_activeState.shader ) {
        setShader( technique.shader() );
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
    u32 location = 0;

    // Read-lock active material technique
    const Technique& technique = m_techniques[m_activeState.technique].readLock();

    // Bind the shader instance
    m_hal->setShader( shader );

    // Set the view-projection matrix input
    if( location = technique.inputLocation( Technique::ViewProjection ) ) {
        shader->setMatrix( location, m_viewProjection );
    }

    // Set the constant color input
    if( location = technique.inputLocation( Technique::Color ) ) {
        shader->setVec4( location, m_constantColor );
    }

    // Bind texture samples
    for( s32 i = 0, n = technique.textureCount(); i < n; i++ ) {
        if( location = technique.inputLocation( static_cast<Technique::Input>( Technique::Texture0 + i ) ) ) {
            shader->setInt( location, i );
        }
    }

    // Save active shader
    m_activeState.shader = shader;

	//// Set the light position
	//if( location = shader->findUniformLocation( "u_lightPosition" ) ) {
	//	shader->setVec4( location, m_registers[LightPosition] );
	//}

	//// Set the light color
	//if( location = shader->findUniformLocation( "u_lightColor" ) ) {
	//	shader->setVec4( location, m_registers[LightColor] );
	//}
}

// ** Rvm::setInstance
void Rvm::setInstance( s32 value )
{
    // Get an active shader instance
    Renderer::ShaderWPtr shader = m_activeState.shader;

    // No active shader - skip
    if( !shader.valid() ) {
        return;
    }

    // Read-lock active material technique
    const Technique& technique = m_techniques[m_activeState.technique].readLock();

    // Get the instance data
    const Instance& instance = m_instances[value];

	// Set the transformation matrix
    u32 location = 0;

	if( location = technique.inputLocation( Technique::Transform ) ) {
		shader->setMatrix( location, *instance.transform );
	}
}

// ** Rvm::setViewProj
void Rvm::setViewProj( const Matrix4& value )
{
    m_viewProjection = value;
}

// ** Rvm::setColor
void Rvm::setColor( const Rgba& value )
{
    m_constantColor = Vec4( value.r, value.g, value.b, value.a );
}

// ** Rvm::flush
void Rvm::flush( void )
{
    // Sort accumulated commands
    std::sort( m_commands.begin(), m_commands.end() );

    // Process all commands
    for( s32 i = 0, n = m_commands.size(); i < n; i++ ) {
        // Decode the command
        const Command& cmd = m_commands[i];

        // Set the rendering technique
        if( m_activeState.technique != cmd.bits.technique ) {
            setTechnique( cmd.bits.technique );
        }

        // Set renderable
        if( m_activeState.renderable != cmd.bits.renderable ) {
            setRenderable( cmd.bits.renderable );
        }

        // Set instance data
        setInstance( cmd.bits.instance );

        // Render all chunks
        const Renderable& renderable = m_renderables[cmd.bits.renderable].readLock();

        for( s32 j = 0; j < renderable.chunkCount(); j++ ) {
            m_hal->setVertexBuffer( renderable.vertexBuffer( j ) );
            m_hal->renderIndexed( renderable.primitiveType(), renderable.indexBuffer( j ), 0, renderable.indexBuffer( j )->size() );
        }        
    }

    // Clear processed commands
    m_commands.clear();
    m_instances.clear();

    // Reset active state
    m_activeState = ActiveState();
}

// ** Rvm::Command::Command
Rvm::Command::Command( void )
    : key( 0 )
{
}

// ** Rvm::Command::operator <
bool Rvm::Command::operator < ( const Command& other ) const
{
    return key < other.key;
}

// ** Rvm::ActiveState::ActiveState
Rvm::ActiveState::ActiveState( void )
    : technique( -1 )
    , renderable( -1 )
    , shader( NULL )
{
}

} // namespace Scene

DC_END_DREEMCHEST