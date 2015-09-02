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

#include "AssetSystem.h"

#include "../Components/Rendering.h"
#include "../Assets/Image.h"
#include "../Assets/Material.h"
#include "../Assets/Mesh.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ** AssetSystem::AssetSystem
AssetSystem::AssetSystem( Ecs::Entities& entities, Renderer::HalPtr hal ) : EntitySystem( entities, "AssetSystem", Ecs::Aspect::any<StaticMesh, Sprite>() ), m_hal( hal )
{

}

// ** AssetSystem::entityAdded
void AssetSystem::entityAdded( const Ecs::EntityPtr& entity )
{
	if( StaticMesh* mesh = entity->get<StaticMesh>() ) {
		queueStaticMesh( mesh );
	}
	if( Sprite* sprite = entity->get<Sprite>() ) {
		queueSprite( sprite );
	}
}

// ** AssetSystem::update
void AssetSystem::update( u32 currentTime, f32 dt )
{
	if( m_queue.empty() ) {
		return;
	}

	m_queue.front()->load( m_hal );
	m_queue.pop_front();
}

// ** AssetSystem::queueSprite
void AssetSystem::queueSprite( Sprite* sprite )
{
	queueAsset( sprite->image() );
}

// ** AssetSystem::queueStaticMesh
void AssetSystem::queueStaticMesh( StaticMesh* mesh )
{
	queueAsset( mesh->mesh() );
	
	for( u32 i = 0; i < mesh->materialCount(); i++ ) {
		queueMaterialAsset( mesh->material( i ) );
	}
}

// ** AssetSystem::queueMaterialAsset
void AssetSystem::queueMaterialAsset( MaterialPtr material )
{
	for( u32 i = 0; i < Material::TotalMaterialLayers; i++ ) {
		const ImageWPtr& image = material->texture( static_cast<Material::Layer>( i ) );

		if( image == ImageWPtr() ) {
			continue;
		}

		queueAsset( image );
	}
}

// ** AssetSystem::queueAsset
void AssetSystem::queueAsset( AssetWPtr asset )
{
	if( !asset->needsLoading() ) {
		return;
	}

	if( std::find( m_queue.begin(), m_queue.end(), asset ) == m_queue.end() ) {
		m_queue.push_back( asset );
	}	
}

} // namespace scene

DC_END_DREEMCHEST