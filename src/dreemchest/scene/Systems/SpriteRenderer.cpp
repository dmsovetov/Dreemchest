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

#include "SpriteRenderer.h"

#include "../Assets/Mesh.h"
#include "../Assets/Material.h"
#include "../Assets/Image.h"

DC_BEGIN_DREEMCHEST

#define CODE( ... ) #__VA_ARGS__

namespace scene {

// ** SpriteRenderer::SpriteRenderer
SpriteRenderer::SpriteRenderer( ecs::Entities& entities, renderer::Hal* hal ) : SceneSystem2<MeshRenderer, Transform2D>( entities, "SpriteRenderer" ), m_hal( hal )
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

// ** SpriteRenderer::setViewProjection
void SpriteRenderer::setViewProjection( const Matrix4& viewProjection )
{
	m_viewProjection = viewProjection;
}

// ** SpriteRenderer::begin
bool SpriteRenderer::begin( u32 currentTime )
{
	return true;
}

// ** SpriteRenderer::end
void SpriteRenderer::end( void )
{
	m_hal->setShader( NULL );
}

// ** SpriteRenderer::process
void SpriteRenderer::process( u32 currentTime, f32 dt, SceneObject& sceneObject, MeshRenderer& meshRenderer, Transform2D& transform )
{
	const MeshPtr&    mesh   = meshRenderer.mesh();
	renderer::Shader* shader = m_shaders[ShaderSolid];
	
	m_hal->setShader( shader );

	shader->setMatrix( shader->findUniformLocation( "u_mvp" ),  m_viewProjection * transform.affine() );

	for( s32 i = 0, n = mesh->chunkCount(); i < n; i++ ) {
		const Mesh::Chunk& chunk = mesh->chunk( i );
		const MaterialPtr& material = meshRenderer.material( i );

		if( material != MaterialPtr() ) {
			const Rgba& diffuse = material->color( Material::Diffuse );
			shader->setInt( shader->findUniformLocation( "u_texture" ), 0 );
			shader->setVec4( shader->findUniformLocation( "u_color" ), Vec4( diffuse.r, diffuse.g, diffuse.b, diffuse.a ) );

			const ImageWPtr& image = material->texture( Material::Diffuse );
			m_hal->setTexture( 0, const_cast<ImageWPtr&>( image )->requestTexture( m_hal ).get() );

			if( material->shader() == Material::Transparent ) {
				m_hal->setBlendFactors( renderer::BlendSrcAlpha, renderer::BlendInvSrcAlpha );
				m_hal->setDepthTest( false, renderer::Less );
			} else {
				m_hal->setBlendFactors( renderer::BlendDisabled, renderer::BlendDisabled );
				m_hal->setDepthTest( true, renderer::Less );
			}
		}

		m_hal->setVertexBuffer( chunk.m_vertexBuffer.get() );
		m_hal->renderIndexed( renderer::PrimTriangles, chunk.m_indexBuffer.get(), 0, chunk.m_indexBuffer->size() );
	}
}

} // namespace scene

DC_END_DREEMCHEST