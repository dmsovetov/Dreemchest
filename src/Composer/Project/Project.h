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

#ifndef __DC_Composer_Project_H__
#define __DC_Composer_Project_H__

#include "../Composer.h"
#include "../Editors/AssetEditor.h"
#include "../Models/AssetsModel.h"

DC_BEGIN_COMPOSER

namespace Project {

	//! Represents active project.
	class Project : public QObject {
    friend class Composer;
	public:

		//! Built-in project paths.
		enum Path {
			  RootPath		//!< Project root location.
			, AssetsPath	//!< Project assets path.
			, CachePath		//!< Project cache path.

			, TotalPaths	//!< The total number of project paths.
		};

									~Project( void );

		//! Fills an asset menu.
		void						fillAssetMenu( Ui::MenuQPtr menu, Ui::AssetTree* assetTree = NULL );

		//! Returns project name.
		const String&				name( void ) const;

		//! Returns project assets model.
		AssetsModelQPtr				assetsModel( void ) const;

		//! Returns an asset bundle.
		Scene::AssetBundleWPtr		assets( void ) const;

		//! Returns project absoulte path.
		String						absolutePath( s32 index ) const;

		//! Returns project assets absolute path.
		String						assetsAbsolutePath( void ) const;

		//! Opens the asset editor.
		Ui::DocumentQPtr			edit( const String& uuid, const FileInfo& fileInfo );

	private:

									//! Constructs Project instance.
									Project( QObject* parent, const Io::Path& path );

		//! Creates new asset with specified name at selected folder.
		void						createAsset( const String& name, const String& ext );

		//! Imports assets to project.
		void						menuImportAssets( Ui::ActionQPtr action );

		//! Creates new folder.
		void						menuCreateFolder( Ui::ActionQPtr action );

		//! Creates new scene.
		void						menuCreateScene( Ui::ActionQPtr action );

		//! Creates new material.
		void						menuCreateMaterial( Ui::ActionQPtr action );

		//! Opens an asset for editing.
		void						menuOpenAsset( Ui::ActionQPtr action );

		//! Deletes the selected asset(s).
		void						menuDeleteAsset( Ui::ActionQPtr action );

		//! Browses the selected asset.
		void						menuBrowseAssets( Ui::ActionQPtr action );

		//! Shows the selected asset in explorer.
		void						menuShowInExplorer( Ui::ActionQPtr action );

	private:

		//! Alias the asset editor factory type.
		typedef AbstractFactory<Editors::AssetEditor, String> AssetEditorFactory;

		String						m_name;					//!< Project name.
		AssetsModelQPtr             m_assetsModel;			//!< Assets model.
		Io::Path					m_paths[TotalPaths];	//!< Project path.
		AssetEditorFactory			m_assetEditors;			//!< Asset editor factory.
		AssetsQPtr					m_assets;				//!< The project assets.
	};

} // namespace Project

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Project_H__	*/