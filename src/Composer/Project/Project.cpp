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

#include "Project.h"
#include "Cache.h"

#include "../UI/IMainWindow.h"
#include "../UI/IMenu.h"
#include "../UI/IFileSystem.h"
#include "../UI/IAssetTree.h"
#include "../UI/IDocument.h"

#ifdef emit
	#undef emit
#endif

namespace Project {

// --------------------------------------------------------- Project --------------------------------------------------------- //

// ** Project::Project
Project::Project( Ui::IMainWindowWPtr mainWindow, const io::Path& path ) : m_mainWindow( mainWindow )
{
	// Save project name
	m_name = path.last();

	// Construct the built-in paths.
	m_paths[RootPath]	= path;
	m_paths[AssetsPath]	= path + "Assets";
	m_paths[CachePath]	= path + "Cache";

	// Declare asset editors.
	m_assetEditors.declare<Editors::AssetEditor>( "scene" );

	// Create delegate
	m_delegate = new AssetsModelDelegate( this );
	mainWindow->assetsModel()->setDelegate( m_delegate );

	// Create project cache
	m_cache = new Cache( mainWindow->fileSystem(), this );
}

// ** Project::create
ProjectPtr Project::create( Ui::IMainWindowWPtr mainWindow, const io::Path& path )
{
	return ProjectPtr( new Project( mainWindow, path ) );
}

// ** Project::name
const String& Project::name( void ) const
{
	return m_name;
}

// ** Project::absolutePath
String Project::absolutePath( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= TotalPaths );
	return m_paths[index].c_str();
}

// ** Project::assetsAbsolutePath
String Project::assetsAbsolutePath( void ) const
{
	return absolutePath( AssetsPath );
}

// ** Project::fillAssetMenu
void Project::fillAssetMenu( Ui::IMenuWPtr menu, Ui::IAssetTreeWPtr assetTree )
{
	// Clear the menu
	menu->clear();

	Ui::IMenuWPtr create = menu->addMenu( "Create" );
	create->addAction( "Folder", BindAction( Project::menuCreateFolder ) );
	create->addAction( "Scene", BindAction( Project::menuCreateScene ) );

	menu->addAction( "Import Assets", BindAction( Project::menuImportAssets ) );
	menu->addSeparator();

	if( assetTree.valid() ) {
		bool singleSelection = assetTree->selection().size() == 1;

		menu->addAction( "Open", BindAction( Project::menuOpenAsset ) )->setDisabled( !singleSelection );
		menu->addAction( "Delete", BindAction( Project::menuDeleteAsset ) )->setDisabled( !singleSelection );
		menu->addSeparator();
		menu->addAction( "Show in Explorer", BindAction( Project::menuShowInExplorer ) )->setDisabled( !singleSelection && !assetTree->selection().empty() );
	} else {
		menu->addSeparator();
		menu->addAction( "Browse...", BindAction( Project::menuBrowseAssets ) );
	}
}

// ** Project::createAsset
void Project::createAsset( const String& name, const String& ext )
{
	// Get the file system instance
	Ui::IFileSystemWPtr fs = m_mainWindow->fileSystem();

	// Get the parent folder
	StringArray selected = m_mainWindow->assetTree()->selection();
	String		path	 = selected.empty() ? assetsAbsolutePath() : fs->extractFileInfo( selected[0] ).path;

	// Generate the file name
	String fileName = fs->generateFileName( path, name, ext );

	// Create an asset
	if( ext == "" ) {
		fs->createDirectory( path + "/" + fileName );
	} else {
		fs->createFile( path + "/" + fileName );
	}

	// Expand parent item
	m_mainWindow->assetTree()->expandSelectedItems();
}

// ** Project::editAsset
Ui::IDocumentWPtr Project::editAsset( const Ui::FileInfo& fileInfo )
{
	// Construct the asset editor by file extension
	AssetEditorFactory::Ptr assetEditor = m_assetEditors.construct( fileInfo.ext );

	// No asset editor found - open the asset file with standard editor
	if( !assetEditor.valid() ) {
		m_mainWindow->fileSystem()->browse( fileInfo.path );
		return Ui::IDocumentWPtr();
	}

	// Dock the editor to main window
	Ui::IDocumentWPtr result = m_mainWindow->editDocument( assetEditor, fileInfo );
	
	return result;
}

// ** Project::menuImportAssets
void Project::menuImportAssets( Ui::IActionWPtr action )
{
	// Get the file system instance
	Ui::IFileSystemWPtr fs = m_mainWindow->fileSystem();

	// Select files
	StringArray files = fs->selectExistingFiles( "Import Assets" );

	if( files.empty() ) {
		return;
	}

	// Copy selected files
	for( s32 i = 0, n = ( s32 )files.size(); i < n; i++ ) {
		// Get the file info
		Ui::FileInfo info = fs->extractFileInfo( files[i] );

		// Copy the file
		fs->copyFile( files[i], assetsAbsolutePath() + "/" + info.fileName );
	}
}

// ** Project::menuCreateFolder
void Project::menuCreateFolder( Ui::IActionWPtr action )
{
	createAsset( "Folder", "" );
}

// ** Project::menuCreateScene
void Project::menuCreateScene( Ui::IActionWPtr action )
{
	createAsset( "Scene", "scene" );
}

// ** Project::menuOpenAsset
void Project::menuOpenAsset( Ui::IActionWPtr action )
{
	m_mainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Project::menuDeleteAsset
void Project::menuDeleteAsset( Ui::IActionWPtr action )
{
	m_mainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Project::menuBrowseAssets
void Project::menuBrowseAssets( Ui::IActionWPtr action )
{
	m_mainWindow->fileSystem()->browse( assetsAbsolutePath() );
}

// ** Project::menuShowInExplorer
void Project::menuShowInExplorer( Ui::IActionWPtr action )
{
	// Get the file system instance
	Ui::IFileSystemWPtr fs = m_mainWindow->fileSystem();

	// Get the selected items
	StringArray selected = m_mainWindow->assetTree()->selection();

	// Get the file info
	Ui::FileInfo fileInfo = fs->extractFileInfo( selected.empty() ? assetsAbsolutePath() : selected[0] );

	// Browse to asset
	m_mainWindow->fileSystem()->browse( fileInfo.ext == "" ? fileInfo.path : fileInfo.directory );
}

// ----------------------------------------------------- AssetsModelDelegate ------------------------------------------------------ //

// ** AssetsModelDelegate::AssetsModelDelegate
AssetsModelDelegate::AssetsModelDelegate( ProjectWPtr project ) : m_project( project )
{
}

// ** AssetsModelDelegate::AssetsModelDelegate
void AssetsModelDelegate::handleAssetAdded( const Ui::Asset& asset )
{
	m_project->m_events.emit<Project::AssetAdded>( asset );
}

// ** AssetsModelDelegate::AssetsModelDelegate
void AssetsModelDelegate::handleAssetRemoved( const Ui::Asset& asset )
{
	m_project->m_events.emit<Project::AssetRemoved>( asset );
}

// ** AssetsModelDelegate::AssetsModelDelegate
void AssetsModelDelegate::handleAssetChanged( const Ui::Asset& asset )
{
	m_project->m_events.emit<Project::AssetChanged>( asset );
}

} // namespace Project