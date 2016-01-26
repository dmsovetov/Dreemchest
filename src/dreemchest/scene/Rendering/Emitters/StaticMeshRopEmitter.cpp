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

#include "StaticMeshRopEmitter.h"

#include "../../Assets/Mesh.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** StaticMeshRopEmitter::StaticMeshRopEmitter
StaticMeshRopEmitter::StaticMeshRopEmitter( Ecs::EcsWPtr ecs, u32 features, Material::Model model )
	: RopEmitter( ecs, "StaticMeshRopEmitter" ), m_features( features ), m_model( model )
{

}

// ** StaticMeshRopEmitter::emit
void StaticMeshRopEmitter::emit( RenderingContext& ctx, Rvm& rvm, ShaderCache& shaders, const StaticMesh& staticMesh, const Transform& transform )
{
	// Skip if mesh is not visible
	if( !staticMesh.isVisible( m_camera->id() ) ) {
		return;
	}

	// Skip if mesh is not inside the clipper.
	if( !m_clipper.inside( staticMesh.worldSpaceBounds() ) ) {
		return;
	}

	// Get the rendered mesh
	MeshHandle mesh = staticMesh.mesh();
	DC_BREAK_IF( !mesh.isValid() );

	// Emit render operation for each mesh chunk
	for( u32 i = 0, n = mesh->chunkCount(); i < n; i++ ) {
		// Get the material for chunk
		MaterialHandle material = staticMesh.material( i );

		// Get the rendering mode from material or use opaque by default
		::DC_DREEMCHEST_NS Scene::RenderingMode renderingMode = material.isValid() ? material->renderingMode() : RenderOpaque;

		if( !rvm.willRender( renderingMode ) ) {
			continue;
		}

		// Emit a new render operation
		Rvm::Command* rop = rvm.emit();

		// Initialize the rendering operation
		rop->transform		= transform.matrix();
		rop->mesh			= mesh.slot();
		rop->mode			= RenderOpaque;
		rop->shader			= NULL;
		rop->distance		= 0;

		if( !material.isValid() ) {
			continue;
		}

		if( m_features.is( RenderingMode ) ) {
			rop->mode = material->renderingMode();
		}

		if( m_features.is( Shader ) ) {
			Material::Model model = m_features.is( LightModel ) ? material->model() : m_model;
			rop->shader = shaders.materialShader( model, material->features() ).get();
		}
		
		if( m_features.is( Distance ) ) {
			rop->distance = rop->mode == RenderAdditive || rop->mode == RenderTranslucent ? (m_transform->position() - transform.position()).length() : 0;
		}
		
		for( u32 j = 0; j < Material::TotalMaterialLayers; j++ ) {
			if( m_features.disabled( BIT( j ) ) ) {
				continue;
			}

			Material::Layer layer = static_cast<Material::Layer>( j );

			rop->colors[j] = &material->color( layer );

			ImageHandle image = material->texture( layer );

			if( image.isLoaded() ) {
				continue;
			}

			rop->textures[j] = image.slot();
		}	
	}
}

} // namespace Scene

DC_END_DREEMCHEST