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
#include "Project.h"

#include "../Importers/ImageImporter.h"
#include "../Importers/FileImporter.h"
#include "../Importers/MeshImporter.h"

DC_BEGIN_COMPOSER

namespace Project {

// ** Assets::Assets
Assets::Assets( FileSystemWPtr fileSystem, const Io::Path& path, AssetsModelWPtr assetsModel ) : m_fileSystem( fileSystem ), m_path( path ), m_assetsModel( assetsModel )
{
	// Create an asset bundle
	m_bundle = Scene::AssetBundle::create( "Assets", path );

	// Declare asset importers.
#ifdef HAVE_TIFF
	m_assetImporters.declare<Importers::ImageImporterTIF>( "tif" );
#endif	/*	HAVE_TIFF	*/

#ifdef HAVE_FBX
	m_assetImporters.declare<Importers::MeshImporterFBX>( "fbx" );
#endif	/*	HAVE_FBX	*/

	m_assetImporters.declare<Importers::ImageImporterTGA>( "tga" );
	m_assetImporters.declare<Importers::FileImporter>( "material" );

	// Subscribe for assets model event
	m_assetsModel.lock()->subscribe<AssetsModel::Added>( dcThisMethod( Assets::handleAssetAdded ) );
	m_assetsModel.lock()->subscribe<AssetsModel::Removed>( dcThisMethod( Assets::handleAssetRemoved ) );
	m_assetsModel.lock()->subscribe<AssetsModel::Changed>( dcThisMethod( Assets::handleAssetChanged ) );
}

Assets::~Assets( void )
{
	m_assetsModel.lock()->unsubscribe<AssetsModel::Added>( dcThisMethod( Assets::handleAssetAdded ) );
	m_assetsModel.lock()->unsubscribe<AssetsModel::Removed>( dcThisMethod( Assets::handleAssetRemoved ) );
	m_assetsModel.lock()->unsubscribe<AssetsModel::Changed>( dcThisMethod( Assets::handleAssetChanged ) );
}

// ** Assets::bundle
Scene::AssetBundleWPtr Assets::bundle( void ) const
{
	return m_bundle;
}

// ** Assets::createAssetForFile
Scene::AssetPtr Assets::createAssetForFile( const FileInfoWPtr& fileInfo )
{
	// Get the asset type by extension
	Scene::Asset::Type type = assetTypeFromExtension( fileInfo->extension() );

	// Create asset by type
	Scene::AssetPtr asset = m_bundle->createAssetByType( type );

	if( !asset.valid() ) {
		return Scene::AssetPtr();
	}

	// Set meta file
	m_assetsModel.lock()->setMetaData( fileInfo, asset->keyValue() );

	return asset;
}

// ** Assets::registerExtension
void Assets::registerExtension( const String& extension, Scene::Asset::Type type )
{
	m_assetTypes[extension] = type;
}

// ** Assets::assetTypeFromExtension
Scene::Asset::Type Assets::assetTypeFromExtension( const String& extension ) const
{
	AssetTypes::const_iterator i = m_assetTypes.find( extension );
	return i != m_assetTypes.end() ? i->second : Scene::Asset::TotalAssetTypes;
}

// ** Assets::handleAssetAdded
void Assets::handleAssetAdded( const AssetsModel::Added& e )
{
	addAssetFile( e.file );
}

// ** Assets::handleAssetRemoved
void Assets::handleAssetRemoved( const AssetsModel::Removed& e )
{
	// Get the asset UUID.
	String uuid = m_assetsModel.lock()->uuid( e.file );

	// Remove asset from bundle
	m_bundle->removeAsset( uuid );

	// Remove asset from cache
	removeFromCache( uuid );
}

// ** Assets::handleAssetChanged
void Assets::handleAssetChanged( const AssetsModel::Changed& e )
{
	// Get the asset UUID.
	String uuid = m_assetsModel.lock()->uuid( e.file );

	// Update asset's cache
	putToCache( e.file, uuid );
}

// ** Assets::addAssetFile
void Assets::addAssetFile( const FileInfoWPtr& fileInfo )
{
	// Read the meta data
	Io::KeyValue meta = m_assetsModel.lock()->metaData( fileInfo );

	// Added asset
	Scene::AssetPtr asset;

	// Create asset from data or create the new one
	if( !meta.isNull() ) {
		asset = m_bundle->createAssetFromData( meta );
	} else {
		asset = createAssetForFile( fileInfo );
	}

	if( !asset.valid() ) {
		return;
	}

	// Set asset name
	String name = fileInfo->relativePath( m_assetsModel.lock()->rootPath() );
	bool result = m_bundle->setAssetName( asset, name );
	DC_BREAK_IF( !result );

	// Add asset to bundle
	qDebug() << "Added" << asset->name().c_str();
	m_bundle->addAsset( asset );

	// Put asset to cache
	putToCache( fileInfo, asset->uuid() );
}

// ** Assets::putToCache
bool Assets::putToCache( const FileInfoWPtr& fileInfo, const String& uuid )
{
	// Create an asset importer for
	Importers::AssetImporterPtr importer = m_assetImporters.construct( fileInfo->extension() );

	if( !importer.valid() ) {
		return false;
	}

	// Create the Assets folder
	Io::Path AssetsFolderPath = cacheFolderFromUuid( uuid );

	if( !m_fileSystem->fileExists( AssetsFolderPath.c_str() ) ) {
		m_fileSystem->createDirectory( AssetsFolderPath.c_str() );
	}

	// Get Assets file path
	Io::Path AssetsFilePath = cacheFileFromUuid( uuid );

	// Don't Assets twice
	if( m_fileSystem->fileExists( AssetsFilePath.c_str() ) ) {
		return true;
	}

	// Perform asset caching.
	bool result = importer->import( m_fileSystem, fileInfo->absolutePath(), AssetsFilePath );
	DC_BREAK_IF( !result );

	qDebug() << "Cached" << fileInfo->fileName().c_str();

	return result;
}

// ** Assets::removeFromCache
void Assets::removeFromCache( const String& uuid )
{
	m_fileSystem->removeFile( cacheFileFromUuid( uuid ).c_str() );
}

// ** Assets::cacheFolderFromUuid
Io::Path Assets::cacheFileFromUuid( const String& uuid ) const
{
	return cacheFolderFromUuid( uuid ) + uuid;
}

// ** Assets::cacheFolderFromUuid
Io::Path Assets::cacheFolderFromUuid( const String& uuid ) const
{
	String folder = String() + uuid[0] + uuid[1];
	return m_path + folder;
}

} // namespace Project

DC_END_COMPOSER