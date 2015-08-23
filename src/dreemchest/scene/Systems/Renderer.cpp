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

#include "Renderer.h"

DC_BEGIN_DREEMCHEST

#define CODE( ... ) #__VA_ARGS__

namespace scene {

// ** Renderer::Renderer
Renderer::Renderer( ecs::Entities& entities, renderer::Hal* hal ) : SceneSystem2<MeshRenderer, Transform2D>( entities, "Renderer" ), m_hal( hal )
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

// ** Renderer::setViewProjection
void Renderer::setViewProjection( const Matrix4& viewProjection )
{
	m_viewProjection = viewProjection;
}

// ** Renderer::begin
bool Renderer::begin( u32 currentTime )
{
	return true;
}

// ** Renderer::end
void Renderer::end( void )
{
	m_hal->setShader( NULL );
}

// ** Renderer::process
void Renderer::process( u32 currentTime, f32 dt, SceneObject& sceneObject, MeshRenderer& meshRenderer, Transform2D& transform )
{
	const MeshPtr&    mesh   = meshRenderer.mesh();
	renderer::Shader* shader = m_shaders[ShaderSolid];
	
	m_hal->setShader( shader );

	shader->setMatrix( shader->findUniformLocation( "u_mvp" ),  m_viewProjection * transform.affine() );

	for( s32 i = 0, n = mesh->chunkCount(); i < n; i++ ) {
		const Mesh::Chunk& chunk = mesh->chunk( i );

		m_hal->setVertexBuffer( chunk.m_vertexBuffer.get() );
		m_hal->renderIndexed( renderer::PrimTriangles, chunk.m_indexBuffer.get(), 0, chunk.m_indexBuffer->size() );
	}
}

} // namespace scene

DC_END_DREEMCHEST