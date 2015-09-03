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
StaticMeshRenderPass::StaticMeshRenderPass( Ecs::Entities& entities, const Renderers& renderers ) : RenderPass( entities, "StaticMeshRenderPass", renderers, 1000 )
{
	m_shaders[ShaderInvalid] = m_renderers.m_hal->createShader(
		CODE(
			uniform mat4 u_vp, u_transform;

			void main()
			{
				gl_Position = u_vp * u_transform * gl_Vertex;
			} ),
		CODE(
			void main()
			{
				gl_FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
			} ) );

	m_shaders[ShaderSolid] = m_renderers.m_hal->createShader(
		CODE(
			uniform mat4 u_vp, u_transform;

			varying vec2 v_uv0;

			void main()
			{
				v_uv0		= gl_MultiTexCoord0.xy;
				gl_Position = u_vp * u_transform * gl_Vertex;
			} ),
		CODE(
			uniform sampler2D u_tex0;
			uniform vec4	  u_clr0;

			varying vec2	  v_uv0;

			void main()
			{
				gl_FragColor = texture2D( u_tex0, v_uv0 ) * u_clr0;
			} ) );
}

// ** StaticMeshRenderPass::begin
bool StaticMeshRenderPass::begin( u32 currentTime )
{
	// Clean the RVM
	m_rvm.clear();
	return true;
}

// ** StaticMeshRenderPass::end
void StaticMeshRenderPass::end( void )
{
	// Run accumulated commands
	m_rvm.flush( m_renderers.m_hal );
}

// ** StaticMeshRenderPass::process
void StaticMeshRenderPass::process( u32 currentTime, f32 dt, SceneObject& sceneObject, StaticMesh& staticMesh, Transform& transform )
{
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
		Rvm::Command* rop = m_rvm.emit();

		// Initialize the rendering operation
		rop->transform		= transform.matrix();
		rop->indexBuffer	= meshData->indexBuffers[i].get();
		rop->vertexBuffer	= meshData->vertexBuffers[i].get();
		rop->shader			= m_shaders[material.valid() ? material->shader() : Material::Unknown].get();

		if( !material.valid() ) {
			continue;
		}

		for( u32 j = 0; j < Material::TotalMaterialLayers; j++ ) {
			Material::Layer  layer = static_cast<Material::Layer>( j );

			rop->colors[j]   = &material->color( layer );

			const ImageWPtr& image = material->texture( layer );

			if( !image.valid() ) {
				continue;
			}

			AssetTexturePtr  data = image->data();
			rop->textures[j] = data.valid() ? data->texture.get() : NULL;
		}
		
	}
}

} // namespace Scene

DC_END_DREEMCHEST