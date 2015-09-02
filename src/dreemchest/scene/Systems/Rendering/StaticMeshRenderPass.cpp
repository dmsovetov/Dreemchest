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

#include "StaticMeshRenderPass.h"

#include "../../Assets/Mesh.h"
#include "../../Assets/Material.h"
#include "../../Assets/Image.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** StaticMeshRenderPass::StaticMeshRenderPass
StaticMeshRenderPass::StaticMeshRenderPass( Ecs::Entities& entities, const Renderers& renderers ) : RenderPass( entities, "StaticMeshRenderPass", renderers ), m_renderOperations( 2000 )
{
	m_shaders[ShaderInvalid] = m_renderers.m_hal->createShader(
		CODE(
			uniform mat4 u_mvp;

			void main()
			{
				gl_Position = u_mvp * gl_Vertex;
			} ),
		CODE(
			uniform sampler2D u_texture;

			void main()
			{
				gl_FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
			} ) );

	m_shaders[ShaderSolid] = m_renderers.m_hal->createShader(
		CODE(
			uniform mat4 u_mvp;

			varying vec2 v_tex0;

			void main()
			{
				v_tex0		= gl_MultiTexCoord0.xy;
				gl_Position = u_mvp * gl_Vertex;
			} ),
		CODE(
			uniform sampler2D u_texture;
			uniform vec4	  u_color;

			varying vec2	  v_tex0;

			void main()
			{
                vec4 color   = texture2D( u_texture, v_tex0 );
				gl_FragColor = color * u_color;
			} ) );
}

// ** StaticMeshRenderPass::begin
bool StaticMeshRenderPass::begin( u32 currentTime )
{
	// Clean the allocated render operations
	m_renderOperations.reset();

	// Disable the depth test for 2d rendering
	m_renderers.m_hal->setDepthTest( true, Renderer::Always );

	return true;
}

// ** StaticMeshRenderPass::end
void StaticMeshRenderPass::end( void )
{
	// Get the HAL reference
	Renderer::HalPtr& hal = m_renderers.m_hal;

	// Sort the emitted render operations
	m_frame.m_renderOperations.sort( sortByShaderTextureMesh );

	// Perform all rendering operations
	for( EmittedRenderOps::const_iterator i = m_frame.m_renderOperations.begin(), end = m_frame.m_renderOperations.end(); i != end; ++i ) {
		const RenderOp* rop = *i;

		// Set the shader
		setShader( rop );

		// Set the texture
		if( rop->texture != m_frame.m_texture ) {
			hal->setTexture( 0, rop->texture );
			m_frame.m_texture = rop->texture;
		}

		// Set the vertex buffer
		if( rop->vertexBuffer != m_frame.m_vertexBuffer ) {
			hal->setVertexBuffer( rop->vertexBuffer );
			m_frame.m_vertexBuffer = rop->vertexBuffer;
		}

		// Render the mesh
		hal->renderIndexed( Renderer::PrimTriangles, rop->indexBuffer, 0, rop->indexBuffer->size() );
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

	// Clean the list of emitted render operations
	m_frame.clear();
}

// ** StaticMeshRenderPass::setShader
void StaticMeshRenderPass::setShader( const RenderOp* rop )
{
	Renderer::HalPtr&   hal    = m_renderers.m_hal;
	Renderer::ShaderPtr shader;

	if( m_frame.m_materialShader != rop->shader ) {
		switch( rop->shader ) {
		case Material::Transparent:	hal->setBlendFactors( Renderer::BlendSrcAlpha, Renderer::BlendInvSrcAlpha );
									break;

		case Material::Additive:	hal->setBlendFactors( Renderer::BlendOne, Renderer::BlendOne );
									break;

		default:					hal->setBlendFactors( Renderer::BlendDisabled, Renderer::BlendDisabled );
		}

		shader = rop->shader != Material::Unknown ? m_shaders[ShaderSolid] : m_shaders[ShaderInvalid];
		hal->setShader( shader );
		m_frame.m_materialShader = rop->shader;
	}

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

// ** StaticMeshRenderPass::process
void StaticMeshRenderPass::process( u32 currentTime, f32 dt, SceneObject& sceneObject, StaticMesh& staticMesh, Transform& transform )
{
	// Calculate the MVP matrix
	Matrix4 mvp = m_viewProj * transform.matrix();

	// Get the rendered mesh
	const MeshPtr& mesh = staticMesh.mesh();

	// Get the mesh data
	AssetMeshPtr meshData = mesh->data();

	if( !meshData.valid() ) {
		return;
	}

	// Emit render operation for each mesh chunk
	for( u32 i = 0, n = ( u32 )meshData->vertexBuffers.size(); i < n; i++ ) {
		// Get the material for chunk
		const MaterialPtr& material = staticMesh.material( i );

		// Emit a new render operation
		RenderOp* rop = emitRenderOp();

		// Get the texture data from an asset
		AssetTexturePtr textueData = material.valid() ? material->texture( Material::Diffuse )->data() : AssetTexturePtr();

		// Initialize the rendering operation
		rop->mvp			= mvp;
		rop->indexBuffer	= meshData->indexBuffers[i].get();
		rop->vertexBuffer	= meshData->vertexBuffers[i].get();
		rop->shader			= material.valid() ? material->shader() : Material::Unknown;
		rop->texture		= textueData.valid() ? textueData->texture.get() : NULL;
		rop->diffuse		= material.valid() ? &material->color( Material::Diffuse ) : NULL;
	}
}

// ** StaticMeshRenderPass::emitRenderOp
StaticMeshRenderPass::RenderOp* StaticMeshRenderPass::emitRenderOp( void )
{
	RenderOp* rop = m_renderOperations.allocate();
	DC_BREAK_IF( rop == NULL )

	m_frame.m_renderOperations.push_back( rop );

	return rop;
}

// ** StaticMeshRenderPass::sortByShaderTextureMesh
bool StaticMeshRenderPass::sortByShaderTextureMesh( const RenderOp* a, const RenderOp* b )
{
	if( a->shader != b->shader ) return a->shader < b->shader;
	if( a->vertexBuffer != b->vertexBuffer ) return a->vertexBuffer > b->vertexBuffer;
	if( a->texture != b->texture ) return a->texture > b->texture;

	return false;
}

} // namespace Scene

DC_END_DREEMCHEST