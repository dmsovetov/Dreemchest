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

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Rvm::Rvm
Rvm::Rvm( u32 maxCommands ) : m_allocator( maxCommands )
{
}

// ** Rvm::flush
void Rvm::flush( Renderer::HalPtr hal )
{
	// Sort the command buffer	
	m_commands.sort( sortByShaderTextureMesh );

	// Active texture
	Renderer::Texture* activeTexture = NULL;

	// Active vertex buffer
	Renderer::VertexBuffer* activeVertexBuffer = NULL;

	//! Active shader
	Renderer::Shader* activeShader = NULL;

	// Perform all rendering operations
	for( EmittedCommands::const_iterator i = m_commands.begin(), end = m_commands.end(); i != end; ++i ) {
		const Command* cmd = *i;

		// Set the shader
		if( cmd->shader != activeShader ) {
			setShader( hal, cmd );
			activeShader = cmd->shader;
		}

		// Set the texture
		if( cmd->texture != activeTexture ) {
			hal->setTexture( 0, cmd->texture );
			activeTexture = cmd->texture;
		}

		// Set the vertex buffer
		if( cmd->vertexBuffer != activeVertexBuffer ) {
			hal->setVertexBuffer( cmd->vertexBuffer );
			activeVertexBuffer = cmd->vertexBuffer;
		}

		// Render the mesh
		hal->renderIndexed( Renderer::PrimTriangles, cmd->indexBuffer, 0, cmd->indexBuffer->size() );
	}
}

// ** Rvm::clear
void Rvm::clear( void )
{
	m_commands.clear();
	m_allocator.reset();
}

// ** Rvm::emit
Rvm::Command* Rvm::emit( void )
{
	Command* rop = m_allocator.allocate();
	DC_BREAK_IF( rop == NULL )

	m_commands.push_back( rop );

	return rop;
}

// ** Rvm::setShader
void Rvm::setShader( Renderer::HalPtr hal, const Command* rop )
{
	Renderer::ShaderPtr shader = rop->shader;

	hal->setShader( shader );

	u32 location = 0;

	if( location = shader->findUniformLocation( "u_mvp" ) ) {
		shader->setMatrix( location,  rop->mvp );
	}

	if( location = shader->findUniformLocation( "u_texture" ) ) {
		shader->setInt( location, 0 );
	}
	
	if( location = shader->findUniformLocation( "u_color" ) ) {
		shader->setVec4( location, Vec4( rop->diffuse->r, rop->diffuse->g, rop->diffuse->b, rop->diffuse->a ) );
	}
}

// ** Rvm::sortByShaderTextureMesh
bool Rvm::sortByShaderTextureMesh( const Command* a, const Command* b )
{
	if( a->shader != b->shader ) return a->shader < b->shader;
	if( a->vertexBuffer != b->vertexBuffer ) return a->vertexBuffer > b->vertexBuffer;
	if( a->texture != b->texture ) return a->texture > b->texture;

	return false;
}

} // namespace Scene

DC_END_DREEMCHEST