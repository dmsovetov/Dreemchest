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
#include "../Assets/Loaders.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** AssetSystem::AssetSystem
AssetSystem::AssetSystem( AssetBundleWPtr assets ) : EntitySystem( "AssetSystem", Ecs::Aspect::any<StaticMesh, Sprite, Particles>() ), m_assets( assets )
{
	// Declare available asset loaders
	m_loaders.declare<ImageLoaderRAW>( AssetFormatImageRaw );
	m_loaders.declare<MeshLoaderRAW>( AssetFormatMesh );
	m_loaders.declare<MaterialLoader>( AssetFormatMaterial );
}

// ** AssetSystem::entityAdded
void AssetSystem::entityAdded( const Ecs::Entity& entity )
{
	if( StaticMesh* mesh = entity.has<StaticMesh>() ) {
		queueStaticMesh( mesh );
	}
	if( Sprite* sprite = entity.has<Sprite>() ) {
		queueSprite( sprite );
	}
	if( Particles* particles = entity.has<Particles>() ) {
		if( particles->material().valid() ) {
			queueMaterialAsset( particles->material() );
		}
	}
}

// ** AssetSystem::load
bool AssetSystem::load( AssetWPtr asset ) const
{
	// Asset does not need loading - skip
	if( !asset->needsLoading() ) {
		return true;
	}

	// This asset does not require loading
	if( asset->format() == AssetFormatGenerated ) {
		return true;
	}

	// Switch asset state
	asset->setState( Asset::Loading );

	// Find an asset load for a specified format
	AssetLoaderPtr loader = m_loaders.construct( asset->format() );

	if( !loader.valid() ) {
		DC_BREAK;
		asset->setState( Asset::LoadingError );
		LogError( "AssetSystem::load : asset '%s' has invalid asset format\n", asset->name().c_str() );
		return false;
	}

	// Open the data stream
	Io::StreamPtr stream = openFileStream( asset );

	if( !stream.valid() ) {
		DC_BREAK;
		asset->setState( Asset::LoadingError );
		LogError( "AssetSystem::load : failed to open the file stream for '%s'\n", asset->name().c_str() );
		return false;
	}

	// Load an asset data
	bool result = loader->loadFromStream( m_assets, asset, stream );

	// Switch asset state
	asset->setState( result ? Asset::Loaded : Asset::LoadingError );

	return result;
}

// ** AssetSystem::update
void AssetSystem::update( u32 currentTime, f32 dt )
{
	// Put assets from bundle to a queue
	AssetSet assets = m_assets->waitingForLoading();

	for( AssetSet::const_iterator i = assets.begin(), end = assets.end(); i != end; ++i ) {
		queueAsset( *i );
		m_assets->removeFromLoading( *i );
	}

	if( m_queue.empty() ) {
		return;
	}

	// Get the next asset in queue
	AssetWPtr asset = m_queue.front();

	// Pop it from queue
	m_queue.pop_front();

	// Load an asset
	if( !load( asset ) ) {
		return;
	}

	//! WORKAROUND: queue material textures for loading.
	if( Material* material = castTo<Material>( asset.get() ) ) {
		for( s32 i = 0; i < Material::TotalMaterialLayers; i++ ) {
			ImageWPtr image = material->texture( static_cast<Material::Layer>( i ) );

			if( image.valid() ) {
				queueAsset( image );
			}
		}
	}
}

// ** AssetSystem::openFileStream
Io::StreamPtr AssetSystem::openFileStream( AssetWPtr asset ) const
{
	// Get an asset name
	String name = asset->name();

	// No name - use the UUID
	if( name == "" ) {
		name = asset->uuid();
	}

	// Construct the file path
	Io::Path fileName = m_assets->assetPathByIdentifier( name );

	// Open the file stream
	Io::StreamPtr stream = Io::DiskFileSystem::open( fileName );

	return stream;
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
	queueAsset( material );

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

	struct Compare {
		static bool byType( const AssetWPtr& a, const AssetWPtr& b )
		{
			return a->type() < b->type();
		}
	};

	if( std::find( m_queue.begin(), m_queue.end(), asset ) == m_queue.end() ) {
		m_queue.push_back( asset );
		m_queue.sort( Compare::byType );
	}	
}

} // namespace Scene

DC_END_DREEMCHEST