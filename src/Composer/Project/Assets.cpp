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

// ** Assets::Assets
Assets::Assets( QObject* parent, const Io::Path& path, AssetFileSystemModelQPtr assetFileSystem ) : QObject( parent ), m_path( path ), m_assetFileSystem( assetFileSystem )
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

	// Connect to asset model signals
    connect( m_assetFileSystem, SIGNAL(fileAdded(const FileInfo&)), this, SLOT(addAssetFile(const FileInfo&)) );
    connect( m_assetFileSystem, SIGNAL(fileRemoved(const QString&, const FileInfo&)), this, SLOT(removeFromCache(const QString&, const FileInfo&)) );
    connect( m_assetFileSystem, SIGNAL(fileChanged(const QString&, const FileInfo&)), this, SLOT(updateAssetCache(const QString&, const FileInfo&)) );
}

// ** Assets::bundle
Scene::AssetBundleWPtr Assets::bundle( void ) const
{
	return m_bundle;
}

// ** Assets::createAssetForFile
Scene::AssetPtr Assets::createAssetForFile( const FileInfo& fileInfo )
{
	// Get the asset type by extension
	Scene::Asset::Type type = assetTypeFromExtension( fileInfo.extension() );

	// Create asset by type
	Scene::AssetPtr asset = m_bundle->createAssetByType( type );

	if( !asset.valid() ) {
		return Scene::AssetPtr();
	}

	// Set meta file
	m_assetFileSystem->setMetaData( fileInfo, asset->keyValue() );

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

// ** Assets::removeAssetFromCache
void Assets::removeAssetFromCache( const QString& uuid, const FileInfo& file )
{
	// Remove asset from bundle
	m_bundle->removeAsset( uuid.toStdString() );

	// Remove asset from cache
    qComposer->fileSystem()->removeFile( cacheFileFromUuid( uuid.toStdString() ).c_str() );
}

// ** Assets::updateAssetCache
bool Assets::updateAssetCache( const QString& uuid, const FileInfo& file )
{
	// Create an asset importer for
	Importers::AssetImporterPtr importer = m_assetImporters.construct( file.extension() );

	if( !importer.valid() ) {
		return false;
	}

    // Get the shared file system instance
    FileSystemQPtr fs = qComposer->fileSystem();

	// Create the Assets folder
	Io::Path AssetsFolderPath = cacheFolderFromUuid( uuid.toStdString() );

	if( !fs->fileExists( AssetsFolderPath.c_str() ) ) {
		fs->createDirectory( AssetsFolderPath.c_str() );
	}

	// Get Assets file path
	Io::Path AssetsFilePath = cacheFileFromUuid( uuid.toStdString() );

	// Don't Assets twice
	if( fs->fileExists( AssetsFilePath.c_str() ) ) {
		return true;
	}

	// Perform asset caching.
	bool result = importer->import( fs, file.absolutePath(), AssetsFilePath );
	DC_BREAK_IF( !result );

	qDebug() << "Cached" << file.fileName().c_str();

	return result;
}

// ** Assets::addAssetFile
void Assets::addAssetFile( const FileInfo& fileInfo )
{
	// Read the meta data
	Archive meta = m_assetFileSystem->metaData( fileInfo );

	// Added asset
	Scene::AssetPtr asset;

	// Create asset from data or create the new one
#if DEV_DEPRECATED_KEYVALUE_TYPE
	if( !meta.isNull() ) {
#else
    if( meta.isValid() ) {
#endif  /*  DEV_DEPRECATED_KEYVALUE_TYPE    */
        asset = m_bundle->createAssetFromData( meta );
	} else {
		asset = createAssetForFile( fileInfo );
	}

	if( !asset.valid() ) {
		return;
	}

	// Set asset name
	String name = fileInfo.relativePath( m_assetFileSystem->rootPath() );
	bool result = m_bundle->setAssetName( asset, name );
	DC_BREAK_IF( !result );

	// Add asset to bundle
	qDebug() << "Added" << asset->name().c_str();
	m_bundle->addAsset( asset );

	// Put asset to cache
	updateAssetCache( QString::fromStdString( asset->uuid() ), fileInfo );
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

DC_END_COMPOSER