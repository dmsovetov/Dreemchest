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
#include "Prefab.h"
#include "Terrain.h"
#include "Loaders.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------- Asset ------------------------------------------- //

// ** Asset::Asset
Asset::Asset( AssetBundle* bundle, Type type, const String& uuid, const String& name )
	: m_bundle( bundle ), m_type( type ), m_uuid( uuid ), m_name( name ), m_state( Unknown ), m_timestamp( 0 )
{
//	DC_BREAK_IF( bundle == NULL )
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

// ** Asset::setUuid
void Asset::setUuid( const String& value )
{
	m_uuid = value;
}

// ** Asset::setBundle
void Asset::setBundle( AssetBundleWPtr value )
{
	m_bundle = value;
}

// ** Asset::timestamp
u32 Asset::timestamp( void ) const
{
	return m_timestamp;
}

// ** Asset::setTimestamp
void Asset::setTimestamp( u32 value )
{
	m_timestamp = value;
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

// ** Asset::keyValue
Io::KeyValue Asset::keyValue( void ) const
{
	return Io::KeyValue::object() << "uuid" << m_uuid << "timestamp" << m_timestamp << "type" << typeToString( m_type );
}

// ** Asset::setKeyValue
bool Asset::setKeyValue( const Io::KeyValue& value )
{
	DC_BREAK_IF( !value.isObject() );

	m_uuid		= value.get( "uuid", "" ).asString();
	m_timestamp = value.get( "timestamp", 0 ).asUInt();

	return m_uuid != "";
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
	Io::StreamPtr      stream = fileSystem.openFile( m_bundle->assetPathByIdentifier( name() != "" ? name() : uuid() ) );

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

// ** Asset::typeFromString
Asset::Type Asset::typeFromString( const String& type )
{
	if( type == "Mesh" )	 return Asset::Mesh;
	if( type == "Image" )	 return Asset::Image;
	if( type == "Prefab" )	 return Asset::Prefab;
	if( type == "Scene" )	 return Asset::Prefab;
	if( type == "Terrain" )	 return Asset::Terrain;
	if( type == "Material" ) return Asset::Material;

	DC_BREAK;
	return Asset::TotalAssetTypes;
}

// ** Asset::typeToString
String Asset::typeToString( Asset::Type type )
{
	switch( type ) {
	case Asset::Mesh:		return "Mesh";
	case Asset::Image:		return "Image";
	case Asset::Prefab:		return "Prefab";
	case Asset::Terrain:	return "Terrain";
	case Asset::Material:	return "Material";
	}

	DC_BREAK;
	return "";
}

// ---------------------------------------- AssetBundle ------------------------------------------- //

// ** AssetBundle::AssetBundle
AssetBundle::AssetBundle( const String& name, const Io::Path& path ) : m_path( path ), m_name( name ), m_uuidFileNames( true )
{
	// Declare asset types
	m_factory.declare<Mesh>( Asset::Mesh );
	m_factory.declare<Material>( Asset::Material );
	m_factory.declare<Image>( Asset::Image );
	m_factory.declare<Prefab>( Asset::Prefab );
	m_factory.declare<Terrain>( Asset::Terrain );
}

// ** AssetBundle::create
AssetBundlePtr AssetBundle::create( const String& name, const Io::Path& path )
{
	return AssetBundlePtr( DC_NEW AssetBundle( name, path ) );
}

// ** AssetBundle::createFromString
AssetBundlePtr AssetBundle::createFromString( const String& name, const Io::Path& path, const String& text )
{
	DC_BREAK_IF( text.empty() );

	// Create asset bundle instance
	AssetBundlePtr assetBundle( DC_NEW AssetBundle( name, path ) );

	// Load assets from key-value
	if( !assetBundle->loadFromString( text ) ) {
		return AssetBundlePtr();
	}

	return assetBundle;
}

// ** AssetBundle::createFromFile
AssetBundlePtr AssetBundle::createFromFile( const String& name, const Io::Path& path, const String& fileName )
{
	// Read the key-value file
	String text = Io::DiskFileSystem::readTextFile( fileName );

	if( text == "" ) {
		log::warn( "AssetBundle::createFromFile : %s, file does not exist or empty\n", fileName.c_str() );
		return AssetBundlePtr();
	}

	// Create from key-value string.
	return createFromString( name, path, text );
}

// ** AssetBundle::createAssetByType
AssetPtr AssetBundle::createAssetByType( Asset::Type type ) const
{
	AssetPtr asset = m_factory.construct( type );

	if( asset.valid() ) {
		asset->setUuid( Guid::generate().toString() );
	}

	return asset;
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

// ** AssetBundle::loadFromString
bool AssetBundle::loadFromString( const String& text )
{
	// Parse key-value from string
	Io::KeyValue kv = Io::KeyValue::parse( text );

	// Failed to parse
	if( kv.isNull() ) {
		return false;
	}

	// Add all assets to bundle
	for( s32 i = 0, n = kv.size(); i < n; i++ ) {
		// Create asset instance from data
		AssetPtr asset = createAssetFromData( kv[i] );

		// Failed to constuct asset
		if( !asset.valid() ) {
			continue;
		}

		addAsset( asset );
	}

	return true;
}

// ** AssetBundle::addAsset
void AssetBundle::addAsset( AssetPtr asset )
{
	log::msg( "Adding asset '%s' to bundle '%s'...\n", asset->name().c_str(), m_name.c_str() );

	// Register an asset by it's UUID
	const String& uuid = asset->uuid();
	DC_BREAK_IF( uuid.empty() );

	m_assets[StringHash( uuid.c_str() )] = asset;

	// Register an asset by it's name
	const String& name = asset->name();

	if( !name.empty() ) {
		m_assets[StringHash( asset->name().c_str() )] = asset;
	}

	//! WORKAROUND: add asset loaders
	switch( asset->type() ) {
	case Asset::Material:	JsonMaterialLoader::attachTo( asset );	break;
	case Asset::Image:		RawImageLoader::attachTo( asset );		break;
	case Asset::Mesh:		RawMeshLoader::attachTo( asset );		break;
	}

	// Set parent bundle
	asset->setBundle( this );
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

// ** AssetBundle::createAssetFromData
AssetPtr AssetBundle::createAssetFromData( const Io::KeyValue& kv ) const
{
	DC_BREAK_IF( !kv.isObject() );

	// Get asset type by name.
	Asset::Type type = Asset::typeFromString( kv.get( "type", "" ).asString() );

	// Create asset instance by type name
	AssetPtr asset = createAssetByType( type );

	// Unknown asset type - return null
	if( !asset.valid() ) {
		return AssetPtr();
	}

	// Load asset from key-value data
	if( !asset->setKeyValue( kv ) ) {
		return AssetPtr();
	}

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