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

#ifndef __DC_Composer_Cache_H__
#define __DC_Composer_Cache_H__

#include "Project.h"
#include "../Importers/AssetImporter.h"

DC_BEGIN_COMPOSER

namespace Project {

	//! Manages the project cache.
	class Cache : public RefCounted {
	friend class Project;
	private:

									//! Constructs Cache instance.
									Cache( IFileSystemWPtr fileSystem, const io::Path& path, AssetsModelWPtr assetsModel );
		virtual						~Cache( void );

		//! Adds an asset to cache.
		void						handleAssetAdded( const AssetsModel::Added& e );

		//! Removes an asset from cache.
		void						handleAssetRemoved( const AssetsModel::Removed& e );

		//! Performs caching of a changed asset.
		void						handleAssetChanged( const AssetsModel::Changed& e );

	private:

		//! Puts an asset to cache.
		bool						putToCache( const Asset& asset );

		//! Removes an asset from cache.
		void						removeFromCache( const Asset& asset );

		//! Returns asset cache path.
		io::Path					cacheFileFromAsset( const Asset& asset ) const;

		//! Returns asset cache folder.
		io::Path					cacheFolderFromAsset( const Asset& asset ) const;

	private:

		//! Alias the asset importer factory type.
		typedef AbstractFactory<Importers::AssetImporter, String> AssetImporterFactory;

		IFileSystemWPtr				m_fileSystem;		//!< File system to use.
		io::Path					m_path;				//!< Root cache folder path.
		AssetsModelWPtr				m_assetsModel;		//!< Assets model to use.
		AssetImporterFactory		m_assetImporters;	//!< Asset importer factory.
	};

} // namespace Project

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Cache_H__	*/