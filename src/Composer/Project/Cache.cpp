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

#include "Cache.h"
#include "Project.h"

#include "../Importers/ImageImporter.h"
#include "../Importers/FileImporter.h"

DC_BEGIN_COMPOSER

namespace Project {

// ** Cache::Cache
Cache::Cache( IFileSystemWPtr fileSystem, const io::Path& path, AssetsModelWPtr assetsModel ) : m_fileSystem( fileSystem ), m_path( path ), m_assetsModel( assetsModel )
{
	// Declare asset importers.
	m_assetImporters.declare<Importers::FileImporter>( "tif" );
	m_assetImporters.declare<Importers::FileImporter>( "fbx" );
	m_assetImporters.declare<Importers::FileImporter>( "prefab" );
	m_assetImporters.declare<Importers::FileImporter>( "mat" );
	m_assetImporters.declare<Importers::FileImporter>( "tga" );

	// Subscribe for project event
	m_assetsModel->subscribe<AssetsModel::Added>( dcThisMethod( Cache::handleAssetAdded ) );
	m_assetsModel->subscribe<AssetsModel::Removed>( dcThisMethod( Cache::handleAssetRemoved ) );
	m_assetsModel->subscribe<AssetsModel::Changed>( dcThisMethod( Cache::handleAssetChanged ) );
}

Cache::~Cache( void )
{
	m_assetsModel->unsubscribe<AssetsModel::Added>( dcThisMethod( Cache::handleAssetAdded ) );
	m_assetsModel->unsubscribe<AssetsModel::Removed>( dcThisMethod( Cache::handleAssetRemoved ) );
	m_assetsModel->unsubscribe<AssetsModel::Changed>( dcThisMethod( Cache::handleAssetChanged ) );
}

// ** Cache::handleAssetAdded
void Cache::handleAssetAdded( const AssetsModel::Added& e )
{
	putToCache( e.asset );
}

// ** Cache::handleAssetRemoved
void Cache::handleAssetRemoved( const AssetsModel::Removed& e )
{
	removeFromCache( e.asset );
}

// ** Cache::handleAssetChanged
void Cache::handleAssetChanged( const AssetsModel::Changed& e )
{
	putToCache( e.asset );
}

// ** Cache::putToCache
bool Cache::putToCache( const Asset& asset )
{
	// Create an asset importer for
	Importers::AssetImporterPtr importer = m_assetImporters.construct( asset.ext );

	if( !importer.valid() ) {
		return false;
	}

	// Create the cache folder
	io::Path cacheFolderPath = cacheFolderFromAsset( asset );

	if( !m_fileSystem->fileExists( cacheFolderPath.c_str() ) ) {
		m_fileSystem->createDirectory( cacheFolderPath.c_str() );
	}

	// Get cache file path
	io::Path cacheFilePath = cacheFileFromAsset( asset );

	// Perform asset caching.
	bool result = importer->import( m_fileSystem, asset, cacheFilePath );
	DC_BREAK_IF( !result );

	qDebug() << "CACHE: added" << asset.absoluteFilePath.c_str();

	return result;
}

// ** Cache::removeFromCache
void Cache::removeFromCache( const Asset& asset )
{
	qDebug() << "CACHE: removed" << asset.absoluteFilePath.c_str();
	DC_BREAK
}

// ** Cache::cacheFolderFromAsset
io::Path Cache::cacheFileFromAsset( const Asset& asset ) const
{
	return cacheFolderFromAsset( asset ) + asset.uuid();
}

// ** Cache::cacheFolderFromAsset
io::Path Cache::cacheFolderFromAsset( const Asset& asset ) const
{
	const String& uuid   = asset.uuid();
	String		  folder = String() + uuid[0] + uuid[1];

	return m_path + folder;
}

} // namespace Project

DC_END_COMPOSER