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
StaticMeshRopEmitter::StaticMeshRopEmitter( Ecs::Entities& entities, u32 features, Material::Model model )
	: RopEmitter( entities, "StaticMeshRopEmitter" ), m_features( features ), m_model( model )
{

}

// ** StaticMeshRopEmitter::emit
void StaticMeshRopEmitter::emit( Rvm& rvm, ShaderCache& shaders, const StaticMesh& staticMesh, const Transform& transform )
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

		if( material.valid() && !rvm.willRender( material->renderingMode() ) ) {
			continue;
		}

		// Emit a new render operation
		Rvm::Command* rop = rvm.emit();

		// Initialize the rendering operation
		rop->transform		= transform.matrix();
		rop->indexBuffer	= meshData->indexBuffers[i].get();
		rop->vertexBuffer	= meshData->vertexBuffers[i].get();
		rop->mode			= RenderOpaque;
		rop->shader			= NULL;
		rop->distance		= 0;

		if( !material.valid() ) {
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
			if( m_features.not( BIT( j ) ) ) {
				continue;
			}

			Material::Layer layer = static_cast<Material::Layer>( j );

			rop->colors[j] = &material->color( layer );

			const ImageWPtr& image = material->texture( layer );

			if( !image.valid() ) {
				continue;
			}

			AssetTexturePtr data = image->data();
			rop->textures[j] = data.valid() ? data->texture.get() : NULL;
		}
		
	}
}

} // namespace Scene

DC_END_DREEMCHEST