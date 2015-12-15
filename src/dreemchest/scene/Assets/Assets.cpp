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

#include "Assets.h"

#include "Image.h"
#include "Material.h"
#include "Mesh.h"
#include "Terrain.h"
#include "Loaders.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------- Asset ------------------------------------------- //

// ** Asset::Asset
Asset::Asset( AssetBundle* bundle, Type type, const String& uuid, const String& name )
	: m_bundle( bundle ), m_type( type ), m_uuid( uuid ), m_name( name ), m_state( Unknown )
{
	DC_BREAK_IF( bundle == NULL )
}

// ** Asset::type
Asset::Type Asset::type( void ) const
{
	return m_type;
}

// ** Asset::name
const String& Asset::name( void ) const
{
	return m_name;
}

// ** Asset::uuid
const String& Asset::uuid( void ) const
{
	return m_uuid;
}

// ** Asset::state
Asset::State Asset::state( void ) const
{
	return m_state;
}

// ** Asset::needsLoading
bool Asset::needsLoading( void ) const
{
	return m_state != Loading && m_state != Loaded;
}

// ** Asset::loader
const AssetLoaderPtr& Asset::loader( void ) const
{
	return m_loader;
}

// ** Asset::setLoader
void Asset::setLoader( const AssetLoaderPtr& value )
{
	m_loader = value;
}

// ** Asset::load
bool Asset::load( const Renderer::HalPtr& hal )
{
	if( m_loader == AssetLoaderPtr() ) {
		log::verbose( "Asset::load : %s, has no asset loader\n", name().c_str() );
		return true;
	}

	DC_BREAK_IF( !needsLoading() );

	m_state = Loading;

	Io::DiskFileSystem fileSystem;
	Io::StreamPtr      stream = fileSystem.openFile( m_bundle->assetPathByIdentifier( name() ) );

	if( stream == Io::StreamPtr() ) {
		log::warn( "Asset::load : failed to open file for '%s.%s'\n", m_bundle->name().c_str(), name().c_str() );
		m_state = LoadingError;
		return false;
	}

	log::msg( "Loading asset '%s.%s'...\n", m_bundle->name().c_str(), name().c_str() );

	if( !m_loader->loadFromStream( m_bundle, hal, stream ) ) {
		log::warn( "Asset::load : do not know how to load asset '%s' of type %d\n", m_name.c_str(), m_type );
		m_state = LoadingError;
		return false;
	}

	m_state = Loaded;

	return true;
}

// ** Asset::unload
void Asset::unload( void )
{
	m_state = Unloaded;
}

// ---------------------------------------- AssetBundle ------------------------------------------- //

// ** AssetBundle::AssetBundle
AssetBundle::AssetBundle( const String& name, const Io::Path& path ) : m_path( path ), m_name( name ), m_uuidFileNames( true )
{
}

// ** AssetBundle::create
AssetBundlePtr AssetBundle::create( const String& name, const Io::Path& path )
{
	return AssetBundlePtr( DC_NEW AssetBundle( name, path ) );
}

// ** AssetBundle::createFromJson
AssetBundlePtr AssetBundle::createFromJson( const String& name, const Io::Path& path, const String& json )
{
	DC_BREAK_IF( json.empty() );

	// Create asset bundle instance
	AssetBundlePtr assetBundle( DC_NEW AssetBundle( name, path ) );

	// Load assets from JSON
	if( !assetBundle->loadFromJson( json ) ) {
		return AssetBundlePtr();
	}

	return assetBundle;
}

// ** AssetBundle::createFromFile
AssetBundlePtr AssetBundle::createFromFile( const String& name, const Io::Path& path, const String& fileName )
{
	// Read the JSON file
	String json = Io::DiskFileSystem::readTextFile( fileName );

	if( json == "" ) {
		log::warn( "AssetBundle::createFromFile : %s, file not found or empty JSON\n", fileName.c_str() );
		return AssetBundlePtr();
	}

	// Create from JSON string.
	return createFromJson( name, path, json );
}

// ** AssetBundle::name
const String& AssetBundle::name( void ) const
{
	return m_name;
}

// ** AssetBundle::uuidFileNames
bool AssetBundle::uuidFileNames( void ) const
{
	return m_uuidFileNames;
}

// ** AssetBundle::setUuidFileNames
void AssetBundle::setUuidFileNames( bool value )
{
	m_uuidFileNames = value;
}

// ** AssetBundle::assetPathByIdentifier
Io::Path AssetBundle::assetPathByIdentifier( const String& name ) const
{
	AssetPtr asset = findAsset( name );

	if( asset == AssetPtr() ) {
		return "";
	}

	String uuid = asset->uuid();

	if( m_uuidFileNames ) {
		uuid = String() + uuid[0] + uuid[1] + "/" + uuid;
	}

	return m_path + uuid;
}

// ** AssetBundle::findAsset
AssetPtr AssetBundle::findAsset( const String& name, u32 expectedType ) const
{
	Assets::const_iterator i = m_assets.find( StringHash( name.c_str() ) );

	if( i == m_assets.end() ) {
		return AssetPtr();
	}

	if( (i->second->type() & expectedType) == 0 ) {
		return AssetPtr();
	}

	return i->second;
}

// ** AssetBundle::loadFromJson
bool AssetBundle::loadFromJson( const String& json )
{
#ifdef HAVE_JSON
	Json::Value root;
	Json::Reader reader;

	if( !reader.parse( json, root ) ) {
		return false;
	}

	for( u32 i = 0, n = root.size(); i < n; i++ ) {
		const Json::Value& item = root[i];

		String identifier = item["identifier"].asString();
		String uuid		  = item["uuid"].asString();
		String type		  = item["type"].asString();

		if( type == "image" ) {
			ImagePtr image = addImage( uuid, identifier, item["width"].asInt(), item["height"].asInt() );
			RawImageLoader::attachTo( image );
		}
		else if( type == "mesh" ) {
			MeshPtr mesh = addMesh( uuid, identifier );
			RawMeshLoader::attachTo( mesh );
		}
		else if( type == "material" ) {
			MaterialPtr material = addMaterial( uuid, identifier );
			JsonMaterialLoader::attachTo( material );
		}
		else if( type == "scene" ) {
			AssetPtr scene = addAsset( Asset::Scene, uuid, identifier );
		}
		else {
			log::warn( "AssetBundle::loadFromJson : unknown asset type '%s'\n", type.c_str() );
		}
	}

	return true;
#else
	log::warn( "AssetBundle::loadFromJson : built with no JSON support.\n" );
	return false;
#endif	/*	HAVE_JSON	*/
}

// ** AssetBundle::addImage
ImagePtr AssetBundle::addImage( const String& uuid, const String& name, u32 width, u32 height )
{
	log::msg( "Adding image '%s' to bundle '%s'...\n", name.c_str(), m_name.c_str() );

	ImagePtr image( DC_NEW Image( this, uuid, name, width, height ) );
	m_assets[StringHash( uuid.c_str() )] = image;
	m_assets[StringHash( name.c_str() )] = image;

	return image;
}

// ** AssetBundle::addMesh
MeshPtr AssetBundle::addMesh( const String& uuid, const String& name )
{
	log::msg( "Adding mesh '%s' to bundle '%s'...\n", name.c_str(), m_name.c_str() );

	MeshPtr mesh( DC_NEW Mesh( this, uuid, name ) );
	m_assets[StringHash( uuid.c_str() )] = mesh;
	m_assets[StringHash( name.c_str() )] = mesh;

	return mesh;
}

// ** AssetBundle::addTerrain
TerrainPtr AssetBundle::addTerrain( const String& uuid, const String& name, u32 size )
{
	log::msg( "Adding terrain '%s' to bundle '%s'...\n", name.c_str(), m_name.c_str() );

	TerrainPtr terrain( DC_NEW Terrain( this, uuid, name, size ) );
	m_assets[StringHash( uuid.c_str() )] = terrain;
	m_assets[StringHash( name.c_str() )] = terrain;

	return terrain;
}


// ** AssetBundle::addMaterial
MaterialPtr AssetBundle::addMaterial( const String& uuid, const String& name )
{
	log::msg( "Adding material '%s' to bundle '%s'...\n", name.c_str(), m_name.c_str() );

	MaterialPtr material( DC_NEW Material( this, uuid, name ) );
	m_assets[StringHash( uuid.c_str() )] = material;
	m_assets[StringHash( name.c_str() )] = material;

	return material;
}

// ** AssetBundle::addAsset
AssetPtr AssetBundle::addAsset( Asset::Type type, const String& uuid, const String& name )
{
	log::msg( "Adding asset '%s' to bundle '%s'...\n", name.c_str(), m_name.c_str() );

	AssetPtr asset;

	switch( type ) {
	case Asset::Mesh:	asset = DC_NEW Mesh( this, uuid, name );
						RawMeshLoader::attachTo( asset );
						break;
	case Asset::Image:	asset = DC_NEW Image( this, uuid, name, 0, 0 );
						RawImageLoader::attachTo( asset );
						break;
	default:			asset = DC_NEW Asset( this, type, uuid, name );		break;
	}

	m_assets[StringHash( uuid.c_str() )] = asset;
	m_assets[StringHash( name.c_str() )] = asset;

	return asset;
}

// ** AssetBundle::removeAsset
void AssetBundle::removeAsset( const String& uuid )
{
	Assets::iterator i = m_assets.find( StringHash( uuid.c_str() ) );

	if( i == m_assets.end() ) {
		log::warn( "AssetBundle::removeAsset : '%s' is not a valid asset id\n", uuid.c_str() );
		return;
	}

	Assets::iterator j = m_assets.find( StringHash( i->second->name().c_str() ) );

	if( j != m_assets.end() ) {
		m_assets.erase( j );
	}

	m_assets.erase( i );
}

} // namespace Scene

DC_END_DREEMCHEST