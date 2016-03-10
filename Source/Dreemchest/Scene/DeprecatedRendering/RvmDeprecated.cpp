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

#include "RvmDeprecated.h"
#include "RenderingContextDeprecated.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

#if DEV_DEPRECATED_SCENE_RENDERER

// ** Rvm::s_samplersUniformNames
CString Rvm::s_samplersUniformNames[] = { "u_tex0" , "u_tex1", "u_tex2", "u_tex3", "u_tex4", "u_tex5", "u_tex6", "u_tex7" };

// ** Rvm::s_colorsUniformNames
CString Rvm::s_colorsUniformNames[]   = { "u_clr0" , "u_clr1", "u_clr2", "u_clr3", "u_clr4", "u_clr5", "u_clr6", "u_clr7" };

// ** Rvm::s_transformUniformName
CString Rvm::s_transformUniformName = "u_transform";

// ** Rvm::s_vpUniformName
CString Rvm::s_vpUniformName = "u_vp";

// ** Rvm::SkipRasterization
const Rvm::RasterizationOptions Rvm::SkipRasterization = { false };

// ** Rvm::OpaqueRasterization
const Rvm::RasterizationOptions Rvm::OpaqueRasterization = { true, Renderer::BlendDisabled, Renderer::BlendDisabled, Renderer::CompareDisabled, 0.0f, true, Renderer::LessEqual };

// ** Rvm::CutoutRasterization
const Rvm::RasterizationOptions Rvm::CutoutRasterization = { true, Renderer::BlendDisabled, Renderer::BlendDisabled, Renderer::Greater, 0.5f, true, Renderer::LessEqual };

// ** Rvm::TranslucentRasterization
const Rvm::RasterizationOptions Rvm::TranslucentRasterization = { true, Renderer::BlendSrcAlpha, Renderer::BlendInvSrcAlpha, Renderer::CompareDisabled, 0.0f, false, Renderer::LessEqual };

// ** Rvm::AdditiveRasterization
const Rvm::RasterizationOptions Rvm::AdditiveRasterization = { true, Renderer::BlendOne, Renderer::BlendOne, Renderer::CompareDisabled, 0.0f, false, Renderer::LessEqual };

// ** Rvm::Rvm
Rvm::Rvm( Renderer::HalPtr hal, u32 maxCommands ) : m_hal( hal ), m_allocator( maxCommands )
{
	m_defaultCullFace		= Renderer::TriangleFaceBack;
	m_defaultPolygonMode	= Renderer::PolygonFill;

	resetCounters();
}

// ** Rvm::setRenderingContext
void Rvm::setRenderingContext( RenderingContextWPtr value )
{
	m_renderingContext = value;
}

// ** Rvm::increase
void Rvm::increase( Counter counter, u32 amount )
{
	m_counters[counter] += amount;
}

// ** Rvm::counter
s32 Rvm::counter( Counter counter ) const
{
	return m_counters[counter];
}

// ** Rvm::resetCounters
void Rvm::resetCounters( void )
{
	memset( m_counters, 0, sizeof( m_counters ) );
	increase( RopAllocatedMemory, sizeof( Command ) * m_allocator.size() );
}

// ** Rvm::hal
Renderer::HalPtr Rvm::hal( void ) const
{
	return m_hal;
}

// ** Rvm::willRender
bool Rvm::willRender( RenderingMode mode ) const
{
	return m_rasterization[mode];
}

// ** Rvm::setDefaultShader
void Rvm::setDefaultShader( const Renderer::ShaderPtr& value )
{
	m_defaultShader = value;
}

// ** Rvm::setRasterization
void Rvm::setRasterization( RenderingMode mode, const RasterizationOptions& value )
{
	m_rasterization[mode] = value;
}

// ** Rvm::setRasterization
void Rvm::setRasterization( u32 mask, const RasterizationOptions& value )
{
	for( u32 mode = 0; mode < TotalRenderModes; mode++ ) {
		if( mask & BIT( mode ) ) {
			m_rasterization[mode] = value;
		}
	}
}

// ** Rvm::setDefaultPolygonMode
void Rvm::setDefaultPolygonMode( Renderer::PolygonMode value )
{
	m_defaultPolygonMode = value;
}

// ** Rvm::setDefaultCullFace
void Rvm::setDefaultCullFace( Renderer::TriangleFace value )
{
	m_defaultCullFace = value;
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
void Rvm::flush( void )
{
	// Sort the command buffer	
	m_commands.sort( sortByShaderTextureMesh );

	// Active texture
	Renderer::Texture* activeTextures[Material::TotalMaterialLayers] = { 0 };

	// Active vertex buffer
	Renderer::VertexBuffer* activeVertexBuffer = NULL;

	//! Active shader
	Renderer::Shader* activeShader			= ( Renderer::Shader* )-1;
	u8				  activeRenderingMode	= -1;

	// Set culling
	m_hal->setCulling( m_defaultCullFace );

	// Set polygon mode
	m_hal->setPolygonMode( m_defaultPolygonMode );

	// Perform all rendering operations
	for( EmittedCommands::const_iterator i = m_commands.begin(), end = m_commands.end(); i != end; ++i ) {
		const Command* cmd = *i;

		// Set the shader
		if( activeShader != cmd->shader ) {
			activeShader = cmd->shader ? cmd->shader : m_defaultShader.get();
			setShader( activeShader );
			increase( ShaderSwitches );
		}

		DC_ABORT_IF( activeShader == NULL, "no active shader" );

		// Set rendering mode
		if( activeRenderingMode != cmd->mode ) {
			setRenderingMode( cmd->mode );
			activeRenderingMode = cmd->mode;
		}

		// Set the instance uniforms
		if( activeShader ) {
			setShaderUniforms( activeShader, cmd );
		}

		// Set the texture
		for( u32 j = 0; j < Material::TotalMaterialLayers; j++ ) {
            Renderer::Texture* texture = cmd->textures[j];

			if( texture == activeTextures[j] ) {
                continue;
            }

			m_hal->setTexture( j, texture );
			activeTextures[j] = texture;
			increase( TextureSwitches );
		}

		// Set the vertex buffer
		//const RenderingContext::Renderable& renderable = m_renderingContext->requestRenderable( cmd->mesh, 0 );

		//if( renderable.vertexBuffer != activeVertexBuffer ) {
		//	m_hal->setVertexBuffer( renderable.vertexBuffer );
		//	activeVertexBuffer = renderable.vertexBuffer.get();
		//	increase( VertexBufferSwitches );
		//}

		// Render the mesh
		//m_hal->renderIndexed( renderable.primitiveType, renderable.indexBuffer, 0, renderable.indexBuffer->size() );
		//increase( DrawIndexed );
		//increase( Triangles, renderable.indexBuffer->size() / 3 );
		//increase( Vertices, renderable.indexBuffer->size() );

       // DC_NOT_IMPLEMENTED;
	}

	// Clear the command list
	clear();
}

// ** Rvm::clear
void Rvm::clear( void )
{
	// Clear emitted commands
	m_commands.clear();
	m_allocator.reset();
}

// ** Rvm::reset
void Rvm::reset( void )
{
	// Reset rasterization options
	setRasterization( RenderOpaque, OpaqueRasterization );
	setRasterization( RenderAdditive, AdditiveRasterization );
	setRasterization( RenderTranslucent, TranslucentRasterization );
	setRasterization( RenderCutout, CutoutRasterization );

	// Reset render states
	setDefaultCullFace( Renderer::TriangleFaceBack );
	setDefaultPolygonMode( Renderer::PolygonFill );

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

// ** Rvm::emit
Rvm::Command* Rvm::emit( void )
{
	Command* rop = m_allocator.allocate();
	DC_ABORT_IF( rop == NULL, "failed to allocate render operation" )

	memset( rop, 0, sizeof( Command ) );
	m_commands.push_back( rop );

	return rop;
}

// ** Rvm::setRenderingMode
void Rvm::setRenderingMode( u8 value )
{
	DC_ABORT_IF( value >= TotalRenderModes, "invalid rendering mode" )

	const RasterizationOptions& options = m_rasterization[value];
	DC_BREAK_IF( !options, "invalid rasterization options" )

	m_hal->setBlendFactors( options.m_blend[0], options.m_blend[1] );
	m_hal->setAlphaTest( options.m_alphaTest, options.m_alphaRef );
	m_hal->setDepthTest( options.m_depthMask, options.m_depthTest );
}

// ** Rvm::setShader
void Rvm::setShader( Renderer::Shader* shader )
{
	m_hal->setShader( shader );

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
		shader->setVec4( location, m_registers[ConstantColor] );
	}

	// Set the light position
	if( location = shader->findUniformLocation( "u_lightPosition" ) ) {
		shader->setVec4( location, m_registers[LightPosition] );
	}

	// Set the light color
	if( location = shader->findUniformLocation( "u_lightColor" ) ) {
		shader->setVec4( location, m_registers[LightColor] );
	}
}

// ** Rvm::setRegister
void Rvm::setRegister( Register id, const Vec4& value )
{
	m_registers[id] = value;
}

// ** Rvm::setShaderUniforms
void Rvm::setShaderUniforms( Renderer::Shader* shader, const Command* cmd )
{
	u32 location = 0;

	// Set colors
	if( cmd->colors ) {
		for( u32 i = 0; i < 8; i++ ) {
			if( !cmd->colors[i] ) {
				continue;
			}

			if( location = shader->findUniformLocation( s_colorsUniformNames[i] ) ) {
				shader->setVec4( location, Vec4( cmd->colors[i]->r, cmd->colors[i]->g, cmd->colors[i]->b, cmd->colors[i]->a ) );
			}
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
	if( a->mode != b->mode ) return a->mode < b->mode;
	if( a->distance != b->distance ) return a->distance > b->distance;
	if( a->shader != b->shader ) return a->shader < b->shader;
	if( a->mesh != b->mesh ) return a->mesh > b->mesh;
	//if( a->vertexBuffer != b->vertexBuffer ) return a->vertexBuffer > b->vertexBuffer;

	for( u32 i = 0; i < Material::TotalMaterialLayers; i++ ) {
		if( a->textures[i] != b->textures[i] ) return a->textures[i] > b->textures[i];
	}

	return false;
}

#endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */

} // namespace Scene

DC_END_DREEMCHEST