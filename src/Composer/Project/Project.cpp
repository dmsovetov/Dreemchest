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
#include "Assets.h"

#include "../IFileSystem.h"

#include "../Widgets/IMainWindow.h"
#include "../Widgets/IMenu.h"
#include "../Widgets/AssetTree.h"
#include "../Widgets/IDocument.h"

#include "../Editors/SceneEditor.h"

#ifdef emit
	#undef emit
#endif

DC_BEGIN_COMPOSER

namespace Project {

// --------------------------------------------------------- Project --------------------------------------------------------- //

// ** Project::Project
Project::Project( Ui::IMainWindowWPtr mainWindow, const Io::Path& path ) : m_mainWindow( mainWindow )
{
	// Save project name
	m_name = path.last();

	// Construct the built-in paths.
	m_paths[RootPath]	= path;
	m_paths[AssetsPath]	= path + "Assets";
	m_paths[CachePath]	= path + "Cache";

	// Declare asset editors.
	m_assetEditors.declare<Editors::SceneEditor>( "scene" );

	// Create assets bundle & model
	m_assetsModel = createAssetsModel();

	// Create project cache
	m_assets = new Assets( mainWindow->fileSystem(), absolutePath( CachePath ), m_assetsModel );

	// Register asset types
	m_assets->registerExtension( "", Scene::Asset::Folder );
	m_assets->registerExtension( "tga", Scene::Asset::Image );
	m_assets->registerExtension( "tif", Scene::Asset::Image );
	m_assets->registerExtension( "fbx", Scene::Asset::Mesh );
	m_assets->registerExtension( "scene", Scene::Asset::Prefab );
	m_assets->registerExtension( "prefab", Scene::Asset::Prefab );
	m_assets->registerExtension( "material", Scene::Asset::Material );

	// Setup assets model after creating cache
	m_assetsModel->setReadOnly( false );
	m_assetsModel->setRootPath( assetsAbsolutePath() );
}

Project::~Project( void )
{

}

// ** Project::create
ProjectPtr Project::create( Ui::IMainWindowWPtr mainWindow, const Io::Path& path )
{
	return ProjectPtr( new Project( mainWindow, path ) );
}

// ** Project::name
const String& Project::name( void ) const
{
	return m_name;
}

// ** Project::assetsModel
AssetsModelWPtr Project::assetsModel( void ) const
{
	return m_assetsModel;
}

// ** Project::assets
Scene::AssetBundleWPtr Project::assets( void ) const
{
	return m_assets->bundle();
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
void Project::fillAssetMenu( Ui::IMenuWPtr menu, Ui::AssetTree* assetTree )
{
	// Clear the menu
	menu->clear();

	Ui::IMenuWPtr create = menu->addMenu( "Create" );
	create->addAction( "Folder", BindAction( Project::menuCreateFolder ) );
	create->addAction( "Scene", BindAction( Project::menuCreateScene ) );
	create->addAction( "Material", BindAction( Project::menuCreateMaterial ) );

	menu->addAction( "Import Assets", BindAction( Project::menuImportAssets ) );
	menu->addSeparator();

	if( assetTree ) {
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
	FileSystemWPtr fs = m_mainWindow->fileSystem();

	// Get the parent folder
    Ui::AssetTreePtr assetTree = m_mainWindow->assetTree().lock();
	FileInfoArray    selected  = assetTree->selection();
	String		     path	   = selected.empty() ? assetsAbsolutePath() : selected[0]->absolutePath();

	// Generate the file name
	String fileName = fs->generateFileName( path, name, ext );

	// Create an asset
	if( ext == "" ) {
		fs->createDirectory( path + "/" + fileName );
	} else {
		fs->createFile( path + "/" + fileName );
	}

	// Expand parent item
	assetTree->expandSelectedItems();
}

// ** Project::edit
Ui::IDocumentWPtr Project::edit( const String& uuid, const FileInfoWPtr& fileInfo )
{
	// Construct the asset editor by file extension
	AssetEditorFactory::Ptr assetEditor = m_assetEditors.construct( fileInfo->extension() );

	// No asset editor found - open the asset file with standard editor
	if( !assetEditor.valid() ) {
		m_mainWindow->fileSystem()->browse( fileInfo->absolutePath() );
		return Ui::IDocumentWPtr();
	}

	// Find asset by UUID
	Scene::AssetPtr asset = m_assets->bundle()->findAsset( uuid );
	DC_BREAK_IF( !asset.valid() );

	// Dock the editor to main window
	Ui::IDocumentWPtr result = m_mainWindow->editDocument( assetEditor, asset );
	
	return result;
}

// ** Project::menuImportAssets
void Project::menuImportAssets( Ui::IActionWPtr action )
{
	// Get the file system instance
	FileSystemWPtr fs = m_mainWindow->fileSystem();

	// Select files
	StringArray files = fs->selectExistingFiles( "Import Assets" );

	if( files.empty() ) {
		return;
	}

	// Copy selected files
	for( s32 i = 0, n = ( s32 )files.size(); i < n; i++ ) {
		// Get the file info
		FileInfoPtr info = fs->extractFileInfo( files[i] );

		// Copy the file
		fs->copyFile( files[i], assetsAbsolutePath() + "/" + info->fileName() );
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

// ** Project::menuCreateMaterial
void Project::menuCreateMaterial( Ui::IActionWPtr action )
{
	createAsset( "Material", "material" );
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
	FileSystemWPtr fs = m_mainWindow->fileSystem();

	// Get the selected items
    Ui::AssetTreePtr assetTree = m_mainWindow->assetTree();
	FileInfoArray    selected  = assetTree->selection();

	// Get the file info
	FileInfoPtr fileInfo = selected.empty() ? fs->extractFileInfo( assetsAbsolutePath() ) : selected[0];

	// Browse to asset
	m_mainWindow->fileSystem()->browse( fileInfo->isDir() ? fileInfo->absolutePath() : fileInfo->dir() );
}

} // namespace Project

DC_END_COMPOSER