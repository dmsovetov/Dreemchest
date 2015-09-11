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

// ** UnlitStaticMeshRenderPass::UnlitStaticMeshRenderPass
UnlitStaticMeshRenderPass::UnlitStaticMeshRenderPass( Ecs::Entities& entities, const Rendering& rendering )
	: StaticMeshRenderPass( entities, rendering, 1 )
{

}

// ** UnlitStaticMeshRenderPass::begin
bool UnlitStaticMeshRenderPass::begin( u32 currentTime )
{
	m_rvm.setDefaultShader( m_rendering.m_shaders->shaderById( ShaderCache::Diffuse ) );
	return StaticMeshRenderPass::begin( currentTime );
}

// ** StaticMeshRenderPass::StaticMeshRenderPass
StaticMeshRenderPass::StaticMeshRenderPass( Ecs::Entities& entities, const Rendering& rendering, u32 materialMask )
	: RenderPass( entities, "StaticMeshRenderPass", rendering, 16000 ), m_materialMask( materialMask )
{

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
	m_rvm.flush( m_rendering.m_hal );
}

// ** StaticMeshRenderPass::process
void StaticMeshRenderPass::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, StaticMesh& staticMesh, Transform& transform )
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
		rop->shader			= material.valid() ? material->shader() : Material::Null;

		if( !material.valid() ) {
			continue;
		}

		for( u32 j = 0; j < Material::TotalMaterialLayers; j++ ) {
			if( (m_materialMask & (1 << j)) == 0 ) {
				continue;
			}

			Material::Layer layer = static_cast<Material::Layer>( j );

			rop->colors[j] = &material->color( layer );

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