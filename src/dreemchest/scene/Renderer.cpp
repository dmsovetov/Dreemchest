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
#include "SceneObject.h"
#include "Component.h"
#include "Mesh.h"

#define GLSL( ... ) #__VA_ARGS__

DC_BEGIN_DREEMCHEST

namespace scene {

// ** Renderer::Renderer
Renderer::Renderer( renderer::Hal* hal ) : m_hal( hal )
{
	// ** Create the colored unlit shader
	m_shaderDefault = m_hal->createShader(
		GLSL(
			uniform mat4 u_mvp;
			varying vec4 v_color;

			void main()
			{
				v_color		= gl_Color;
				gl_Position = u_mvp * gl_Vertex;
			}
		),
		GLSL(
			varying vec4 v_color;

			void main()
			{
				gl_FragColor = v_color;
			}
		) );
}

// ** Renderer::render
void Renderer::render( const Matrix4& view, const Matrix4& proj, const ScenePtr& scene )
{
	const SceneObjects& sceneObjects = scene->sceneObjects();

	m_hal->setShader( m_shaderDefault );

	for( SceneObjects::const_iterator i = sceneObjects.begin(), end = sceneObjects.end(); i != end; ++i ) {
		const SceneObjectPtr& sceneObject = *i;
		
		if( !sceneObject->has<MeshRenderer>() ) {
			continue;
		}

		StrongPtr<MeshRenderer> meshRenderer = sceneObject->get<MeshRenderer>();
		StrongPtr<Transform>	transform	 = sceneObject->get<Transform>();
		const MeshPtr&			mesh		 = meshRenderer->mesh();
		
		m_shaderDefault->setMatrix( m_shaderDefault->findUniformLocation( "u_mvp" ),  proj * view * transform->affine() );

		for( s32 j = 0, n = mesh->chunkCount(); j < n; j++ ) {
			const Mesh::Chunk& chunk = mesh->chunk( j );

			m_hal->setVertexBuffer( chunk.m_vertexBuffer.get() );
			m_hal->renderIndexed( renderer::PrimTriangles, chunk.m_indexBuffer.get(), 0, chunk.m_indexBuffer->size() );
		}
	}

	m_hal->setShader( NULL );
}

} // namespace scene

DC_END_DREEMCHEST