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

#include "../UI/IAssetsModel.h"

namespace Project {

	//! Represents active project.
	class Project : public RefCounted {
	friend class AssetsModelDelegate;
	public:

		//! Built-in project paths.
		enum Path {
			  RootPath		//!< Project root location.
			, AssetsPath	//!< Project assets path.
			, CachePath		//!< Project cache path.

			, TotalPaths	//!< The total number of project paths.
		};

		//! Fills an asset menu.
		void						fillAssetMenu( Ui::IMenuWPtr menu, Ui::IAssetTreeWPtr assetTree = Ui::IAssetTreeWPtr() );

		//! Returns project name.
		const String&				name( void ) const;

		//! Returns project absoulte path.
		String						absolutePath( s32 index ) const;

		//! Returns project assets absolute path.
		String						assetsAbsolutePath( void ) const;

		//! Opens the asset editor.
		Ui::IDocumentWPtr			editAsset( const Ui::FileInfo& fileInfo );

		//! Subscribes for a project event.
		template<typename TEvent>
		void						subscribe( const typename event::EventEmitter::Callback<TEvent>::Type& callback ) { m_events.subscribe<TEvent>( callback ); }

		//! Removes an event listener.
		template<typename TEvent>
		void						unsubscribe( const typename event::EventEmitter::Callback<TEvent>::Type& callback ) { m_events.unsubscribe<TEvent>( callback ); }

		//! Creates new Project instance.
		static ProjectPtr			create( Ui::IMainWindowWPtr mainWindow, const io::Path& path );

		//! This event is emitted when a new asset was added to a project.
		struct AssetAdded {
									AssetAdded( const Ui::Asset& asset )
										: asset( asset ) {}
			Ui::Asset				asset;	//!< Asset that was added.
		};

		//! This event is emitted when a new asset was removed from a project.
		struct AssetRemoved {
									AssetRemoved( const Ui::Asset& asset )
										: asset( asset ) {}
			Ui::Asset				asset;	//!< Asset that was removed.
		};

		//! This event is emitted when a new asset was changed.
		struct AssetChanged {
									AssetChanged( const Ui::Asset& asset )
										: asset( asset ) {}
			Ui::Asset				asset;	//!< Asset that was changed.
		};

	private:

									//! Constructs Project instance.
									Project( Ui::IMainWindowWPtr mainWindow, const io::Path& path );

		//! Creates new asset with specified name at selected folder.
		void						createAsset( const String& name, const String& ext );

		//! Imports assets to project.
		void						menuImportAssets( Ui::IActionWPtr action );

		//! Creates new folder.
		void						menuCreateFolder( Ui::IActionWPtr action );

		//! Creates new scene.
		void						menuCreateScene( Ui::IActionWPtr action );

		//! Opens an asset for editing.
		void						menuOpenAsset( Ui::IActionWPtr action );

		//! Deletes the selected asset(s).
		void						menuDeleteAsset( Ui::IActionWPtr action );

		//! Browses the selected asset.
		void						menuBrowseAssets( Ui::IActionWPtr action );

		//! Shows the selected asset in explorer.
		void						menuShowInExplorer( Ui::IActionWPtr action );

	private:

		//! Alias the asset editor factory type.
		typedef AbstractFactory<Editors::AssetEditor, String> AssetEditorFactory;

		String						m_name;					//!< Project name.
		Ui::IMainWindowWPtr			m_mainWindow;			//!< Main window instance.
		Ui::IAssetsModelDelegatePtr	m_delegate;				//!< Assets model delegate.
		io::Path					m_paths[TotalPaths];	//!< Project path.
		AssetEditorFactory			m_assetEditors;			//!< Asset editor factory.
		CachePtr					m_cache;				//!< The project cache.
		event::EventEmitter			m_events;				//!< Project event emitter.
	};

	//! Project assets model delegate.
	class AssetsModelDelegate : public Ui::IAssetsModelDelegate {
	public:

							//! Constructs the AssetsModelDelegate instance.
							AssetsModelDelegate( ProjectWPtr project );

		//! Handles the asset added event.
		virtual void		handleAssetAdded( const Ui::Asset& asset );

		//! Handles the asset removed event.
		virtual void		handleAssetRemoved( const Ui::Asset& asset );

		//! Handles the asset changed event.
		virtual void		handleAssetChanged( const Ui::Asset& asset );

	private:

		ProjectWPtr			m_project;	//!< Parent project.
	};

} // namespace Project

#endif	/*	!__DC_Composer_Project_H__	*/