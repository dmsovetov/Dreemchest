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

#include "ShaderCache.h"
#include "Rvm.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Rvm::s_samplersUniformNames
CString Rvm::s_samplersUniformNames[] = { "u_tex0" , "u_tex1", "u_tex2", "u_tex3", "u_tex4", "u_tex5", "u_tex6", "u_tex7" };

// ** Rvm::s_colorsUniformNames
CString Rvm::s_colorsUniformNames[]   = { "u_clr0" , "u_clr1", "u_clr2", "u_clr3", "u_clr4", "u_clr5", "u_clr6", "u_clr7" };

// ** Rvm::s_transformUniformName
CString Rvm::s_transformUniformName = "u_transform";

// ** Rvm::s_vpUniformName
CString Rvm::s_vpUniformName = "u_vp";

// ** Rvm::Rvm
Rvm::Rvm( u32 maxCommands ) : m_allocator( maxCommands )
{
	m_defaultSrcBlending	= Renderer::BlendDisabled;
	m_defaultDstBlending	= Renderer::BlendDisabled;
	m_defaultDepthFunction	= Renderer::Less;
}

// ** Rvm::setDefaultShader
void Rvm::setDefaultShader( const Renderer::ShaderPtr& value )
{
	m_defaultShader = value;
}

// ** Rvm::setDefaultBlending
void Rvm::setDefaultBlending( Renderer::BlendFactor src, Renderer::BlendFactor dst )
{
	m_defaultSrcBlending = src;
	m_defaultDstBlending = dst;
}

// ** Rvm::setDefaultDepthFunction
void Rvm::setDefaultDepthFunction( Renderer::Compare value )
{
	m_defaultDepthFunction = value;
}

// ** Rvm::setViewProjection
void Rvm::setViewProjection( const Matrix4& value )
{
	m_viewProjection = value;
}

// ** Rvm::setShader
void Rvm::setShader( u32 id, const Renderer::ShaderPtr& shader )
{
	if( id >= ( u32 )m_shaders.size() ) {
		m_shaders.resize( id + 1 );
	}

	m_shaders[id] = shader;
}

// ** Rvm::viewProjection
const Matrix4& Rvm::viewProjection( void ) const
{
	return m_viewProjection;
}

// ** Rvm::findShaderById
Renderer::Shader* Rvm::findShaderById( u32 idx ) const
{
	return idx < ( u32 )m_shaders.size() ? m_shaders[idx].get() : m_defaultShader.get();
}

// ** Rvm::flush
void Rvm::flush( Renderer::HalPtr hal )
{
	// Sort the command buffer	
	m_commands.sort( sortByShaderTextureMesh );

	// Active texture
	Renderer::Texture* activeTextures[Material::TotalMaterialLayers] = { 0 };

	// Active vertex buffer
	Renderer::VertexBuffer* activeVertexBuffer = NULL;

	//! Active shader
	Renderer::Shader* activeShader   = NULL;
	u32				  activeShaderId = -1;

	// Set the default blending function
	hal->setBlendFactors( m_defaultSrcBlending, m_defaultDstBlending );

	// Set the default depth testing function
	hal->setDepthTest( true, m_defaultDepthFunction );

	// Set culling
	hal->setCulling( Renderer::TriangleFaceBack );

	// Perform all rendering operations
	for( EmittedCommands::const_iterator i = m_commands.begin(), end = m_commands.end(); i != end; ++i ) {
		const Command* cmd = *i;

		// Set the shader
		if( activeShaderId != cmd->shader ) {
			Renderer::Shader* shader = findShaderById( cmd->shader );
			DC_BREAK_IF( shader == NULL )

			if( shader != activeShader ) {
				activeShader = shader;
				setShader( hal, activeShader );
			}

			activeShaderId = cmd->shader;
		}

		// Set the instance uniforms
		if( activeShader ) {
			setShaderUniforms( activeShader, cmd );
		}

		// Set the texture
		for( u32 j = 0; j < Material::TotalMaterialLayers; j++ ) {
			Renderer::Texture* texture = cmd->textures[j];

			if( texture != activeTextures[j] ) {
				hal->setTexture( j, texture );
				activeTextures[j] = texture;
			}
		}

		// Set the vertex buffer
		if( cmd->vertexBuffer != activeVertexBuffer ) {
			hal->setVertexBuffer( cmd->vertexBuffer );
			activeVertexBuffer = cmd->vertexBuffer;
		}

		// Render the mesh
		hal->renderIndexed( Renderer::PrimTriangles, cmd->indexBuffer, 0, cmd->indexBuffer->size() );
	}

	// Set the default shader
	hal->setShader( NULL );

	// Set the default vertex buffer
	hal->setVertexBuffer( NULL );

	// Set default textures
	for( s32 i = 0; i < 8; i++ ) {
		hal->setTexture( i, NULL );
	}

	// Enable the depth test back
	hal->setDepthTest( true, Renderer::Less );
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

	memset( rop, 0, sizeof( Command ) );
	m_commands.push_back( rop );

	return rop;
}

// ** Rvm::setShader
void Rvm::setShader( Renderer::HalPtr hal, Renderer::Shader* shader )
{
	hal->setShader( shader );

	u32 location = 0;

	if( location = shader->findUniformLocation( s_vpUniformName ) ) {
		shader->setMatrix( location, m_viewProjection );
	}

	// Bind texture samplers
	for( u32 i = 0; i < 8; i++ ) {
		if( location = shader->findUniformLocation( s_samplersUniformNames[i] ) ) {
			shader->setInt( location, i );
		}
	}

	// Set the constant color value
	if( location = shader->findUniformLocation( "u_color" ) ) {
		shader->setVec4( location, Vec4( 0.1f, 0.1f, 0.0f, 1.0f ) );
	}
}

// ** Rvm::setShaderUniforms
void Rvm::setShaderUniforms( Renderer::Shader* shader, const Command* cmd )
{
	u32 location = 0;

	// Set colors
	for( u32 i = 0; i < 8; i++ ) {
		if( !cmd->colors[i] ) {
			continue;
		}

		if( location = shader->findUniformLocation( s_colorsUniformNames[i] ) ) {
			shader->setVec4( location, Vec4( cmd->colors[i]->r, cmd->colors[i]->g, cmd->colors[i]->b, cmd->colors[i]->a ) );
		}
	}

	// Set the transformation matrix
	if( location = shader->findUniformLocation( s_transformUniformName ) ) {
		shader->setMatrix( location,  cmd->transform );
	}
}

// ** Rvm::sortByShaderTextureMesh
bool Rvm::sortByShaderTextureMesh( const Command* a, const Command* b )
{
	if( a->shader != b->shader ) return a->shader < b->shader;
	if( a->vertexBuffer != b->vertexBuffer ) return a->vertexBuffer > b->vertexBuffer;

	for( u32 i = 0; i < Material::TotalMaterialLayers; i++ ) {
		if( a->textures[i] != b->textures[i] ) return a->textures[i] > b->textures[i];
	}

	return false;
}

} // namespace Scene

DC_END_DREEMCHEST