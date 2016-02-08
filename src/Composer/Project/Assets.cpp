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
    // Set the randomization seed to generate asset identifiers
    srand( time( NULL ) );

	// Create an asset bundle
	//m_bundle = Scene::AssetBundle::create( "Assets", path );
    qWarning() << "Assets::Assets : no assets path set.";

	// Declare asset importers.
#ifdef HAVE_TIFF
	m_assetImporters.declare<Importers::ImageImporterTIF>( "tif" );
#endif	/*	HAVE_TIFF	*/

#ifdef HAVE_FBX
	m_assetImporters.declare<Importers::MeshImporterFBX>( "fbx" );
#endif	/*	HAVE_FBX	*/

	m_assetImporters.declare<Importers::ImageImporterTGA>( "tga" );
	m_assetImporters.declare<Importers::FileImporter>( "material" );

    // Declare default asset formats
    m_assetFormats.declare<Scene::ImageFormatRaw>( Assets::Type::fromClass<Scene::Image>() );
    m_assetFormats.declare<Scene::MeshFormatRaw>( Assets::Type::fromClass<Scene::Mesh>() );
    m_assetFormats.declare<Scene::MaterialFormatKeyValue>( Assets::Type::fromClass<Scene::Material>() );

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
Assets::AssetHandle AssetManager::createAssetForFile( const FileInfo& fileInfo )
{
	// Get the asset type by extension
	Assets::Type type = assetTypeFromExtension( fileInfo.extension() );

    if( !type.isValid() ) {
        return Assets::AssetHandle();
    }

    // Create an asset
    Assets::AssetHandle asset = createAsset( type, Guid::generate().toString() );

    // Set meta file
    m_assetFileSystem->setMetaData( fileInfo, Io::KeyValue::object() << "uuid" << asset->uniqueId() << "type" << type.toString() );

    return asset;
}

// ** AssetManager::parseAssetFromData
Assets::AssetHandle AssetManager::parseAssetFromData( const Io::KeyValue& kv )
{
	DC_BREAK_IF( !kv.isObject() );

	// Get asset type by name.
	Assets::Type type = Assets::Type::fromString( kv.get( "type", "" ).asString() );
    DC_BREAK_IF( !type.isValid() );

    // Read the unique asset identifier.
    Assets::AssetId uid = kv.get( "uuid" ).asString();

    // Create an asset
    return createAsset( type, uid );
}

// ** AssetManager::createAsset
Assets::AssetHandle AssetManager::createAsset( Assets::Type type, const Assets::AssetId& id )
{
    // Create asset format by extension
    Assets::AbstractAssetFileFormat* format = m_assetFormats.construct( type );

    // Set the source file name
    if( format ) {
        format->setFileName( cacheFileFromUuid( id ).c_str() );
    }

    // Create asset instance
    Assets::AssetHandle asset = m_assets.addAsset( type, id, format );
    DC_BREAK_IF( !asset.isValid() );

    return asset;
}

// ** AssetManager::registerExtension
void AssetManager::registerExtension( const String& extension, Assets::Type type )
{
	m_assetTypes[extension] = type;
}

// ** AssetManager::assetTypeFromExtension
Assets::Type AssetManager::assetTypeFromExtension( const String& extension ) const
{
	AssetTypes::const_iterator i = m_assetTypes.find( extension );
	return i != m_assetTypes.end() ? i->second : Assets::Type::Invalid;
}

// ** AssetManager::removeAssetFromCache
void AssetManager::removeAssetFromCache( const QString& uuid, const FileInfo& file )
{
	// Remove asset from bundle
	m_assets.removeAsset( uuid.toStdString() );

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

// ** AssetManager::addAssetFile
void AssetManager::addAssetFile( const FileInfo& fileInfo )
{
	// Read the meta data
	Io::KeyValue meta = m_assetFileSystem->metaData( fileInfo );

	// Added asset
	Assets::AssetHandle asset;

	// Create asset from data or create the new one
	if( !meta.isNull() ) {
		asset = parseAssetFromData( meta );
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
	qDebug() << "Added" << asset->name().c_str();
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

DC_END_COMPOSER