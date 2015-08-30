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

#include "UnlitMeshRenderer.h"

#include "../Assets/Mesh.h"
#include "../Assets/Material.h"
#include "../Assets/Image.h"

#include "../Components/Transform.h"
#include "../Components/Camera.h"

DC_BEGIN_DREEMCHEST

#define CODE( ... ) #__VA_ARGS__

namespace scene {

// ** MeshRendererPass::MeshRendererPass
MeshRendererPass::MeshRendererPass( ecs::Entities& entities, renderer::Hal* hal ) : RenderPass( entities, "MeshRendererPass", hal ), m_renderOperations( 2000 )
{
	m_shaders[ShaderSolid] = m_hal->createShader(
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

// ** MeshRendererPass::begin
bool MeshRendererPass::begin( u32 currentTime )
{
	// Clean the allocated render operations
	m_renderOperations.reset();

	// Disable the depth test for 2d rendering
	m_hal->setDepthTest( true, renderer::Always );

	return true;
}

// ** MeshRendererPass::end
void MeshRendererPass::end( void )
{
	// Sort the emitted render operations
	m_frame.m_renderOperations.sort( sortByShaderTextureMesh );

	// Perform all rendering operations
	for( EmittedRenderOps::const_iterator i = m_frame.m_renderOperations.begin(), end = m_frame.m_renderOperations.end(); i != end; ++i ) {
		const RenderOp* rop = *i;

		// Set the shader
		setShader( rop );

		// Set the texture
		if( rop->texture != m_frame.m_texture ) {
			m_hal->setTexture( 0, rop->texture );
			m_frame.m_texture = rop->texture;
		}

		// Set the vertex buffer
		if( rop->vertexBuffer != m_frame.m_vertexBuffer ) {
			m_hal->setVertexBuffer( rop->vertexBuffer );
			m_frame.m_vertexBuffer = rop->vertexBuffer;
		}

		// Render the mesh
		m_hal->renderIndexed( renderer::PrimTriangles, rop->indexBuffer, 0, rop->indexBuffer->size() );
	}

	// Set the default shader
	m_hal->setShader( NULL );

	// Set the default vertex buffer
	m_hal->setVertexBuffer( NULL );

	// Set default textures
	for( s32 i = 0; i < 8; i++ ) {
		m_hal->setTexture( i, NULL );
	}

	// Enable the depth test back
	m_hal->setDepthTest( true, renderer::Less );

	// Clean the list of emitted render operations
	m_frame.clear();
}

// ** MeshRendererPass::setShader
void MeshRendererPass::setShader( const RenderOp* rop )
{
	if( m_frame.m_materialShader != rop->shader ) {
		switch( rop->shader ) {
		case Material::Solid:		m_hal->setBlendFactors( renderer::BlendDisabled, renderer::BlendDisabled );
									break;

		case Material::Transparent:	m_hal->setBlendFactors( renderer::BlendSrcAlpha, renderer::BlendInvSrcAlpha );
									break;

		case Material::Additive:	m_hal->setBlendFactors( renderer::BlendOne, renderer::BlendOne );
									break;
		}

		m_hal->setShader( m_shaders[ShaderSolid] );
		m_frame.m_materialShader = rop->shader;
	}

	renderer::Shader* shader = m_shaders[ShaderSolid];
	shader->setMatrix( shader->findUniformLocation( "u_mvp" ),  rop->mvp );
	shader->setInt( shader->findUniformLocation( "u_texture" ), 0 );
	shader->setVec4( shader->findUniformLocation( "u_color" ), Vec4( rop->diffuse->r, rop->diffuse->g, rop->diffuse->b, rop->diffuse->a ) );
}

// ** MeshRendererPass::process
void MeshRendererPass::process( u32 currentTime, f32 dt, SceneObject& sceneObject, MeshRenderer& meshRenderer, Transform& transform )
{
	// Calculate the MVP matrix
	Matrix4 mvp = m_viewProj * transform.matrix();

	// Get the rendered mesh
	const MeshPtr& mesh = meshRenderer.mesh();

	// Emit render operation for each mesh chunk
	for( u32 i = 0, n = mesh->chunkCount(); i < n; i++ ) {
		// Get the mesh chunk by index
		const Mesh::Chunk& chunk = mesh->chunk( i );

		// Get the material for chunk
		const MaterialPtr& material = meshRenderer.material( i );

		// Emit a new render operation
		RenderOp* rop = emitRenderOp();

		// Initialize the rendering operation
		rop->mvp			= mvp;
		rop->indexBuffer	= chunk.m_indexBuffer.get();
		rop->vertexBuffer	= chunk.m_vertexBuffer.get();
		rop->shader			= material->shader();
		rop->texture		= const_cast<ImageWPtr&>( material->texture( Material::Diffuse ) )->requestTexture( m_hal ).get();
		rop->diffuse		= &material->color( Material::Diffuse );
	}
}

// ** MeshRendererPass::emitRenderOp
MeshRendererPass::RenderOp* MeshRendererPass::emitRenderOp( void )
{
	RenderOp* rop = m_renderOperations.allocate();
	DC_BREAK_IF( rop == NULL )

	m_frame.m_renderOperations.push_back( rop );

	return rop;
}

// ** MeshRendererPass::sortByShaderTextureMesh
bool MeshRendererPass::sortByShaderTextureMesh( const RenderOp* a, const RenderOp* b )
{
	if( a->shader != b->shader ) return a->shader < b->shader;
	if( a->vertexBuffer != b->vertexBuffer ) return a->vertexBuffer > b->vertexBuffer;
	if( a->texture != b->texture ) return a->texture > b->texture;

	return false;
}

} // namespace scene

DC_END_DREEMCHEST