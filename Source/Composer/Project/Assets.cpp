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

// ** AssetManager::AssetManager
AssetManager::AssetManager( QObject* parent, const Io::Path& path, AssetFileSystemModelQPtr assetFileSystem ) : QObject( parent ), m_path( path ), m_assetFileSystem( assetFileSystem )
{
    // Start asset update timer
    startTimer( 16 );

    // Set the randomization seed to generate asset identifiers
    srand( time( NULL ) );

	// Create an asset bundle
	//m_bundle = Scene::AssetBundle::create( "Assets", path );
    LogWarning( "assets", "%s", "no assets path set\n" );

	// Declare asset importers.
#ifdef TIFF_FOUND
	m_assetImporters.declare<Importers::ImageImporterTIF>( "tif" );
#endif	/*	#ifdef TIFF_FOUND	*/

#ifdef FBX_FOUND
	m_assetImporters.declare<Importers::MeshImporterFBX>( "fbx" );
#endif	/*	#ifdef FBX_FOUND	*/

	m_assetImporters.declare<Importers::ImageImporterTGA>( "tga" );
	m_assetImporters.declare<Importers::FileImporter>( "material" );

    // Declare default asset formats
    m_assetFormats.declare<Scene::ImageFormatRaw>( Assets::Asset::typeId<Scene::Image>() );
    m_assetFormats.declare<Scene::MeshFormatRaw>( Assets::Asset::typeId<Scene::Mesh>() );
    m_assetFormats.declare<Scene::MaterialSourceKeyValue>( Assets::Asset::typeId<Scene::Material>() );

	// Connect to asset model signals
    connect( m_assetFileSystem, SIGNAL(fileAdded(const FileInfo&)), this, SLOT(addAssetFile(const FileInfo&)) );
    connect( m_assetFileSystem, SIGNAL(fileRemoved(const QString&, const FileInfo&)), this, SLOT(removeAssetFromCache(const QString&, const FileInfo&)) );
    connect( m_assetFileSystem, SIGNAL(fileChanged(const QString&, const FileInfo&)), this, SLOT(updateAssetCache(const QString&, const FileInfo&)) );
}

// ** AssetManager::assets
const Assets::Assets& AssetManager::assets( void ) const
{
	return m_assets;
}

// ** AssetManager::assets
Assets::Assets& AssetManager::assets( void )
{
	return m_assets;
}

// ** AssetManager::createAssetForFile
Assets::Handle AssetManager::createAssetForFile( const FileInfo& fileInfo )
{
	// Get the asset type by extension
	Assets::TypeId type = assetTypeFromExtension( fileInfo.extension() );

    if( type == 0 ) {
        return Assets::Handle();
    }

    // Create an asset
    Assets::Handle asset = createAsset( type, Guid::generate().toString() );

    // Set meta file
    m_assetFileSystem->setMetaData( fileInfo, KvBuilder() << "uuid" << asset->uniqueId() << "type" << m_assets.assetTypeName( type ) );

    return asset;
}

// ** AssetManager::parseAssetFromData
Assets::Handle AssetManager::parseAssetFromData( const KeyValue& kv )
{
	// Get asset type by name.
	Assets::TypeId type = m_assets.typeFromName( kv.get<String>( "type" ) );
    NIMBLE_BREAK_IF( type == 0 );

    // Read the unique asset identifier.
    Assets::AssetId uid = kv.get<String>( "uuid" );

    // Create an asset
    return createAsset( type, uid );
}

// ** AssetManager::createAsset
Assets::Handle AssetManager::createAsset( const Assets::TypeId& type, const Assets::AssetId& id )
{
    // Create asset source by extension
    Assets::AbstractFileSource* source = m_assetFormats.construct( type );

    // Set the source file name
    if( source ) {
        source->setFileName( cacheFileFromUuid( id ).c_str() );
    }

    // Create asset instance
    Assets::Handle asset = m_assets.addAsset( type, id, source );
    NIMBLE_BREAK_IF( !asset.isValid() );

    // Register this asset file source
    m_files[id] = source;

    return asset;
}

// ** AssetManager::registerExtension
void AssetManager::registerExtension( const String& extension, Assets::TypeId type )
{
	m_assetTypes[extension] = type;
}

// ** AssetManager::assetTypeFromExtension
Assets::TypeId AssetManager::assetTypeFromExtension( const String& extension ) const
{
	AssetTypes::const_iterator i = m_assetTypes.find( extension );
	return i != m_assetTypes.end() ? i->second : 0;
}

// ** AssetManager::removeAssetFromCache
void AssetManager::removeAssetFromCache( const QString& uuid, const FileInfo& file )
{
    // Construct an asset id
    Assets::AssetId assetId = uuid.toStdString();

    // Remove asset file source
    m_files.erase( assetId );

	// Remove asset from bundle
	m_assets.removeAsset( assetId );

	// Remove asset from cache
    qComposer->fileSystem()->removeFile( cacheFileFromUuid( uuid.toStdString() ).c_str() );
}

// ** AssetManager::updateAssetCache
bool AssetManager::updateAssetCache( const QString& uuid, const FileInfo& file )
{
	// Create an asset importer for
	Importers::AssetImporterPtr importer = m_assetImporters.construct( file.extension() );

	if( !importer.valid() ) {
		return false;
	}

    // Get the shared file system instance
    FileSystemQPtr fs = qComposer->fileSystem();

	// Create the Assets folder
	Io::Path assetsFolderPath = cacheFolderFromUuid( uuid.toStdString() );

	if( !fs->fileExists( assetsFolderPath.c_str() ) ) {
		fs->createDirectory( assetsFolderPath.c_str() );
	}

	// Get assets file path
	Io::Path assetsFilePath = cacheFileFromUuid( uuid.toStdString() );

	// Don't cache assets twice
	if( fs->fileExists( assetsFilePath.c_str() ) ) {
    //    LogDebug( "assets", "%s is already cached\n", file.fileName().c_str() );
	//	return true;
	}

	// Perform asset caching.
	bool result = importer->import( fs, file.absolutePath(), assetsFilePath );
	NIMBLE_BREAK_IF( !result );

    // Update the asset source timestamp
    AssetFiles::iterator i = m_files.find( uuid.toStdString() );
    i->second->setLastModified( Time::current() );

    LogDebug( "assets", "%s cached\n", file.fileName().c_str() );

	return result;
}

// ** AssetManager::addAssetFile
void AssetManager::addAssetFile( const FileInfo& fileInfo )
{
	// Read the meta data
	Archive meta = m_assetFileSystem->metaData( fileInfo );

	// Added asset
	Assets::Handle asset;

	// Create asset from data or create the new one
    if( meta.isValid() ) {
        asset = parseAssetFromData( meta.as<KeyValue>() );
	} else {
		asset = createAssetForFile( fileInfo );
	}

	if( !asset.isValid() ) {
		return;
	}

	// Set asset name
	String name = fileInfo.relativePath( m_assetFileSystem->rootPath() );
	asset->setName( name );

	// Put asset to cache
	updateAssetCache( QString::fromStdString( asset->uniqueId() ), fileInfo );

	// Write message to a console
    LogDebug( "assets", "added %s\n", asset->name().c_str() );
}

// ** AssetManager::cacheFolderFromUuid
Io::Path AssetManager::cacheFileFromUuid( const String& uuid ) const
{
	return cacheFolderFromUuid( uuid ) + uuid;
}

// ** AssetManager::cacheFolderFromUuid
Io::Path AssetManager::cacheFolderFromUuid( const String& uuid ) const
{
	String folder = String() + uuid[0] + uuid[1];
	return m_path + folder;
}

// ** AssetManager::timerEvent
void AssetManager::timerEvent( QTimerEvent* e )
{
    m_assets.update( 1.0f / 60.0f );
}

DC_END_COMPOSER
