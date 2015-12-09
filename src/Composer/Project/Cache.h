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

namespace Project {

	//! Manages the project cache.
	class Cache : public RefCounted {
	friend class Project;
	public:

	private:

								//! Constructs Cache instance.
								Cache( Ui::IFileSystemWPtr fileSystem, ProjectWPtr project );
		virtual					~Cache( void );

		//! Adds an asset to cache.
		void					handleAssetAdded( const Project::AssetAdded& e );

		//! Removes an asset from cache.
		void					handleAssetRemoved( const Project::AssetRemoved& e );

		//! Performs caching of a changed asset.
		void					handleAssetChanged( const Project::AssetChanged& e );

	private:

		//! Puts an asset to cache.
		bool					putToCache( const Ui::Asset& asset );

		//! Removes an asset from cache.
		void					removeFromCache( const Ui::Asset& asset );

		//! Returns asset cache path.
		io::Path				cacheFileFromAsset( const Ui::Asset& asset ) const;

		//! Returns asset cache folder.
		io::Path				cacheFolderFromAsset( const Ui::Asset& asset ) const;

	private:

		//! Alias the asset importer factory type.
		typedef AbstractFactory<Importers::AssetImporter, String> AssetImporterFactory;

		ProjectWPtr				m_project;			//!< Parent project instance.
		Ui::IFileSystemWPtr		m_fileSystem;		//!< File system to use.
		AssetImporterFactory	m_assetImporters;	//!< Asset importer factory.
	};

} // namespace Project

#endif	/*	!__DC_Composer_Cache_H__	*/