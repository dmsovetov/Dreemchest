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

namespace Project {

	//! Represents active project.
	class Project : public RefCounted {
	public:

		//! Built-in project paths.
		enum Path {
			  RootPath		//!< Project root location.
			, AssetsPath	//!< Project assets path.
			, CachePath		//!< Project cache path.

			, TotalPaths	//!< The total number of project paths.
		};

		//! Fills an asset menu.
		void				fillAssetMenu( Ui::IMenuWPtr menu, Ui::IAssetTreeWPtr assetTree = Ui::IAssetTreeWPtr() );

		//! Returns project absoulte path.
		String				absolutePath( s32 index ) const;

		//! Returns project assets absolute path.
		String				assetsAbsolutePath( void ) const;

		//! Creates new Project instance.
		static ProjectPtr	create( Ui::IMainWindowWPtr mainWindow, const io::Path& path );

	private:

							//! Constructs Project instance.
							Project( Ui::IMainWindowWPtr mainWindow, const io::Path& path );

		//! Creates new asset with specified name at selected folder.
		void				createAsset( const String& name, const String& ext );

		//! Imports assets to project.
		void				menuImportAssets( Ui::IActionWPtr action );

		//! Creates new folder.
		void				menuCreateFolder( Ui::IActionWPtr action );

		//! Creates new scene.
		void				menuCreateScene( Ui::IActionWPtr action );

		//! Opens an asset for editing.
		void				menuOpenAsset( Ui::IActionWPtr action );

		//! Deletes the selected asset(s).
		void				menuDeleteAsset( Ui::IActionWPtr action );

		//! Browses the selected asset.
		void				menuBrowseAssets( Ui::IActionWPtr action );

		//! Shows the selected asset in explorer.
		void				menuShowInExplorer( Ui::IActionWPtr action );

	private:

		Ui::IMainWindowWPtr m_mainWindow;			//!< Main window instance.
		io::Path			m_paths[TotalPaths];	//!< Project path.
	};

} // namespace Project

#endif	/*	!__DC_Composer_Project_H__	*/