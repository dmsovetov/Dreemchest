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

#include <json/json.h>

DC_BEGIN_DREEMCHEST

namespace scene {

// ------------------------------------------- Asset ------------------------------------------- //

// ** Asset::Asset
Asset::Asset( AssetBundle* bundle, Type type, const String& name )
	: m_bundle( bundle ), m_type( type ), m_name( name )
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

// ** Asset::load
bool Asset::load( void )
{
	io::DiskFileSystem fileSystem;
	io::StreamPtr      stream = fileSystem.openFile( m_bundle->assetPathByName( name() ) );

	if( stream == io::StreamPtr() ) {
		log::warn( "Asset::load : failed to open file for '%s.%s'\n", m_bundle->name().c_str(), name().c_str() );
		return false;
	}

	log::msg( "Loading asset '%s.%s'...\n", m_bundle->name().c_str(), name().c_str() );

	return loadFromStream( stream );
}

// ** Asset::loadFromStream
bool Asset::loadFromStream( const io::StreamPtr& stream )
{
	log::warn( "Asset::loadFromStream : do not know how to load asset '%s' of type %d\n", m_name.c_str(), m_type );
	return false;
}

// ** Asset::unload
void Asset::unload( void )
{
}

// ---------------------------------------- AssetBundle ------------------------------------------- //

// ** AssetBundle::AssetBundle
AssetBundle::AssetBundle( const String& name, const io::Path& path ) : m_path( path ), m_name( name )
{
}

// ** AssetBundle::create
AssetBundlePtr AssetBundle::create( const String& name, const io::Path& path, const String& json )
{
	AssetBundlePtr assetBundle( DC_NEW AssetBundle( name, path ) );

	if( !assetBundle->loadFromJson( json ) ) {
		return AssetBundlePtr();
	}

	return assetBundle;
}

// ** AssetBundle::name
const String& AssetBundle::name( void ) const
{
	return m_name;
}

// ** AssetBundle::assetPathByName
io::Path AssetBundle::assetPathByName( const String& name ) const
{
	return m_path + name;
}

// ** AssetBundle::findAsset
AssetPtr AssetBundle::findAsset( const String& name, u32 expectedType )
{
	Assets::iterator i = m_assets.find( StringHash( name.c_str() ) );

	if( i == m_assets.end() ) {
		return AssetPtr();
	}

	if( (i->second->type() & expectedType) == 0 ) {
		return AssetPtr();
	}

	return i->second;
}

// ** AssetBundle::findImage
ImagePtr AssetBundle::findImage( const String& name )
{
	AssetPtr asset = findAsset( name, Asset::Image );

	if( asset == AssetPtr() ) {
		return ImagePtr();
	}

	Image* image = static_cast<Image*>( asset.get() );

	return ImagePtr( image );
}

// ** AssetBundle::findMaterial
MaterialPtr AssetBundle::findMaterial( const String& name )
{
	AssetPtr asset = findAsset( name, Asset::Material );

	if( asset == AssetPtr() ) {
		return MaterialPtr();
	}

	Material* material = static_cast<Material*>( asset.get() );

	return MaterialPtr( material );
}

// ** AssetBundle::loadFromJson
bool AssetBundle::loadFromJson( const String& json )
{
	Json::Value root;
	Json::Reader reader;

	if( !reader.parse( json, root ) ) {
		return false;
	}

	for( u32 i = 0, n = root.size(); i < n; i++ ) {
		const Json::Value& item = root[i];

		String identifier = item["identifier"].asString(); 
		String type		  = item["type"].asString();

		if( type == "image" ) {
			addImage( identifier, ImageFormat::ImageRaw, item["width"].asInt(), item["height"].asInt() ).get();
		}
		else {
			log::warn( "AssetBundle::loadFromJson : unknown asset type '%s'\n", type.c_str() );
		}
	}

	return true;
}

// ** AssetBundle::addImage
ImagePtr AssetBundle::addImage( const String& name, ImageFormat format, u32 width, u32 height )
{
	log::msg( "Adding image '%s' to bundle '%s'...\n", name.c_str(), m_name.c_str() );

	ImagePtr image( DC_NEW Image( this, name, format, width, height ) );
	m_assets[StringHash( name.c_str() )] = image;

	image->load();

	return image;
}

// ** AssetBundle::addMaterial
MaterialPtr AssetBundle::addMaterial( const String& name )
{
	log::msg( "Adding material '%s' to bundle '%s'...\n", name.c_str(), m_name.c_str() );

	MaterialPtr material( DC_NEW Material( this, name ) );
	m_assets[StringHash( name.c_str() )] = material;

	return material;
}

} // namespace scene

DC_END_DREEMCHEST