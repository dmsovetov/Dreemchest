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
	// ** Create the solid lightmap shader
	m_shaders[Material::Solid] = m_hal->createShader(
		GLSL(
			uniform mat4 u_mvp;

			varying vec2 v_tex0;
			varying vec2 v_tex1;

			void main()
			{
				v_tex0		= gl_MultiTexCoord0.xy;
				v_tex1		= gl_MultiTexCoord1.xy;
				gl_Position = u_mvp * gl_Vertex;
			} ),
		GLSL(
			uniform sampler2D u_diffuse;
			uniform sampler2D u_lightmap;
			uniform vec4	  u_diffuseColor;

			varying vec2	  v_tex0;
			varying vec2	  v_tex1;

			void main()
			{
                vec4 light   = texture2D( u_lightmap, v_tex1 );
				gl_FragColor = texture2D( u_diffuse, v_tex0 ) * (light * 2.0) * u_diffuseColor + vec4( 0.5254902, 0.80392157, 0.91764706 );
			} ) );

	// ** Create the colored & textured shader
	m_shaders[Material::Transparent] = m_hal->createShader(
		GLSL(
			uniform mat4 u_mvp;

			varying vec2 v_tex0;
			varying vec2 v_tex1;

			void main()
			{
				v_tex0		= gl_MultiTexCoord0.xy;
				v_tex1		= gl_MultiTexCoord1.xy;
				gl_Position = u_mvp * gl_Vertex;
			} ),
		GLSL(
			uniform sampler2D u_diffuse;
			uniform sampler2D u_lightmap;
			uniform vec4	  u_diffuseColor;

			varying vec2	  v_tex0;
			varying vec2	  v_tex1;

			void main()
			{
				gl_FragColor = texture2D( u_diffuse, v_tex0 ) * texture2D( u_lightmap, v_tex1 ) * u_diffuseColor;
			} ) );

	// ** Create the colored & textured shader
	m_shaders[Material::Additive] = m_hal->createShader(
		GLSL(
			uniform mat4 u_mvp;

			varying vec2 v_tex0;

			void main()
			{
				v_tex0		= gl_MultiTexCoord0.xy;
				gl_Position = u_mvp * gl_Vertex;
			} ),
		GLSL(
			uniform sampler2D u_diffuse;
			uniform vec4	  u_diffuseColor;
			uniform vec4	  u_tintColor;

			varying vec2	  v_tex0;

			void main()
			{
				gl_FragColor = texture2D( u_diffuse, v_tex0 ) * u_diffuseColor * u_tintColor * u_tintColor.a;
			} ) );
}

// ** Renderer::render
void Renderer::render( const Matrix4& view, const Matrix4& proj, renderer::Shader* shader, const SceneObjectPtr& sceneObject )
{
	StrongPtr<MeshRenderer> meshRenderer = sceneObject->get<MeshRenderer>();
	StrongPtr<Transform>	transform	 = sceneObject->get<Transform>();
	const MeshPtr&			mesh		 = meshRenderer->mesh();

	m_hal->setShader( shader );

	shader->setMatrix( shader->findUniformLocation( "u_mvp" ),  proj * view * transform->affine() );

	for( s32 j = 0, n = mesh->chunkCount(); j < n; j++ ) {
		const Mesh::Chunk& chunk    = mesh->chunk( j );
		MaterialPtr		   material = meshRenderer->material( j );

		if( material != MaterialPtr() ) {
			Rgba diffuseColor = material->color( Material::Diffuse );
			Rgba tintColor	  = material->color( Material::Tint );

			shader->setInt( shader->findUniformLocation( "u_diffuse" ), 0 );
			shader->setInt( shader->findUniformLocation( "u_lightmap" ), 1 );
			shader->setVec4( shader->findUniformLocation( "u_diffuseColor" ), Vec4( diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a ) );
			shader->setVec4( shader->findUniformLocation( "u_tintColor" ), Vec4( tintColor.r, tintColor.g, tintColor.b, tintColor.a ) );
			m_hal->setTexture( 0, material->texture( Material::Diffuse ).get() );
			m_hal->setTexture( 1, meshRenderer->lightmap().get() );
		} else {
			m_hal->setTexture( 0, NULL );
			m_hal->setTexture( 1, NULL );
		}

		m_hal->setVertexBuffer( chunk.m_vertexBuffer.get() );
		m_hal->renderIndexed( renderer::PrimTriangles, chunk.m_indexBuffer.get(), 0, chunk.m_indexBuffer->size() );
	}
}

// ** Renderer::render
void Renderer::render( const Matrix4& view, const Matrix4& proj, const ScenePtr& scene )
{
	const SceneObjects& sceneObjects = scene->sceneObjects();

	SceneObjects objects[Material::TotalMaterialShaders];

	for( SceneObjects::const_iterator i = sceneObjects.begin(), end = sceneObjects.end(); i != end; ++i ) {
		const SceneObjectPtr& sceneObject = *i;
		
		if( !sceneObject->has<MeshRenderer>() ) {
			continue;
		}

		StrongPtr<MeshRenderer> meshRenderer = sceneObject->get<MeshRenderer>();
		MaterialPtr material = meshRenderer->material( 0 );

		if( material == MaterialPtr() ) {
			continue;
		}

		objects[material->shader()].push_back( sceneObject );
	}

    {
		for( SceneObjects::const_iterator i = objects[Material::Solid].begin(), end = objects[Material::Solid].end(); i != end; ++i ) {
			render( view, proj, m_shaders[Material::Solid], *i );
		}
    }

    {
        m_hal->setAlphaTest( renderer::Greater, 0.5f );
		for( SceneObjects::const_iterator i = objects[Material::Transparent].begin(), end = objects[Material::Transparent].end(); i != end; ++i ) {
			render( view, proj, m_shaders[Material::Transparent], *i );
		}
        m_hal->setAlphaTest( renderer::CompareDisabled );
    }

    {
        m_hal->setDepthTest( false, renderer::Less );
        m_hal->setBlendFactors( renderer::BlendOne, renderer::BlendOne );

		for( SceneObjects::const_iterator i = objects[Material::Additive].begin(), end = objects[Material::Additive].end(); i != end; ++i ) {
			render( view, proj, m_shaders[Material::Additive], *i );
		}

        m_hal->setDepthTest( true, renderer::Less );
        m_hal->setBlendFactors( renderer::BlendDisabled, renderer::BlendDisabled );
    }

	m_hal->setShader( NULL );
}

} // namespace scene

DC_END_DREEMCHEST