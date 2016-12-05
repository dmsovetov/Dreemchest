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

#include "../FileSystem.h"

#include "../Widgets/MainWindow.h"
#include "../Widgets/Document.h"
#include "../Widgets/Menu.h"
#include "../Widgets/AssetTree.h"

#include "../Editors/SceneEditor.h"

#ifdef emit
	#undef emit
#endif

DC_BEGIN_COMPOSER

// ** Project::Project
Project::Project( QObject* parent, const Io::Path& path ) : QObject( parent )
{
	// Save project name
	m_name = path.last();

	// Construct the built-in paths.
	m_paths[RootPath]	= path;
	m_paths[AssetsPath]	= path + "Assets";
	m_paths[CachePath]	= path + "Cache";

	// Declare asset editors.
	m_assetEditors.declare<Editors::SceneEditor>( Assets::Asset::typeId<Scene::Prefab>() );

	// Create assets bundle & model
	m_assetFileSystem = new AssetFileSystemModel( this );

	// Create project cache
	m_assets = new AssetManager( this, absolutePath( CachePath ), m_assetFileSystem );

	// Register asset types
	//m_assets->registerExtension( "", Scene::Asset::Folder );
	m_assets->registerExtension( "tga", Assets::Asset::typeId<Scene::Image>() );
	m_assets->registerExtension( "tif", Assets::Asset::typeId<Scene::Image>() );
	m_assets->registerExtension( "fbx", Assets::Asset::typeId<Scene::Mesh>() );
	m_assets->registerExtension( "scene", Assets::Asset::typeId<Scene::Prefab>() );
	m_assets->registerExtension( "prefab", Assets::Asset::typeId<Scene::Prefab>() );
	m_assets->registerExtension( "material", Assets::Asset::typeId<Scene::Material>() );

	// Setup assets model after creating cache
	m_assetFileSystem->setReadOnly( false );
	m_assetFileSystem->setRootPath( assetsAbsolutePath() );
}

Project::~Project( void )
{

}

// ** Project::name
const String& Project::name( void ) const
{
	return m_name;
}

// ** Project::assetFileSystem
AssetFileSystemModelQPtr Project::assetFileSystem( void ) const
{
	return m_assetFileSystem;
}

// ** Project::assets
Assets::Assets& Project::assets( void ) const
{
	return m_assets->assets();
}

// ** Project::absolutePath
String Project::absolutePath( s32 index ) const
{
	NIMBLE_BREAK_IF( index < 0 || index >= TotalPaths );
	return m_paths[index].c_str();
}

// ** Project::assetsAbsolutePath
String Project::assetsAbsolutePath( void ) const
{
	return absolutePath( AssetsPath );
}

// ** Project::fillAssetMenu
void Project::fillAssetMenu( Ui::MenuQPtr menu, Ui::AssetTree* assetTree )
{
	// Clear the menu
	menu->clear();

	Ui::MenuQPtr create = menu->addMenu( "Create" );
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
	FileSystemQPtr fs = qComposer->fileSystem();

	// Get the parent folder
	FileInfoArray     selected = qMainWindow->assetTree()->selection();
	String		      path	   = selected.empty() ? assetsAbsolutePath() : selected[0].absolutePath();

	// Generate the file name
	String fileName = fs->generateFileName( path, name, ext );

	// Create an asset
	if( ext == "" ) {
		fs->createDirectory( path + "/" + fileName );
	} else {
		fs->createFile( path + "/" + fileName );
	}

	// Expand parent item
	qMainWindow->assetTree()->expandSelectedItems();
}

// ** Project::edit
Ui::DocumentQPtr Project::edit( const String& uuid, const FileInfo& fileInfo )
{
    // Get the asset type by extension
    Assets::TypeId assetType = m_assets->assetTypeFromExtension( fileInfo.extension() );

	// Construct the asset editor by asset type
	Editors::AssetEditorQPtr assetEditor = m_assetEditors.construct( assetType );

	// No asset editor found - open the asset file with standard editor
	if( !assetEditor ) {
		qComposer->fileSystem()->browse( fileInfo.absolutePath() );
		return NULL;
	}

	// Find asset by UUID
	Assets::Handle asset = m_assets->assets().findAsset( uuid );
	NIMBLE_BREAK_IF( !asset.isValid() );

	// Dock the editor to main window
	Ui::DocumentQPtr result = qMainWindow->editDocument( assetEditor, fileInfo );
	
	return result;
}

// ** Project::menuImportAssets
void Project::menuImportAssets( Ui::ActionQPtr action )
{
	// Get the file system instance
	FileSystemQPtr fs = qComposer->fileSystem();

	// Select files
	StringArray files = fs->selectExistingFiles( "Import Assets" );

	if( files.empty() ) {
		return;
	}

	// Copy selected files
	for( s32 i = 0, n = ( s32 )files.size(); i < n; i++ ) {
		// Get the file info
		FileInfo info = fs->extractFileInfo( files[i] );

		// Copy the file
		fs->copyFile( files[i], assetsAbsolutePath() + "/" + info.fileName() );
	}
}

// ** Project::menuCreateFolder
void Project::menuCreateFolder( Ui::ActionQPtr action )
{
	createAsset( "Folder", "" );
}

// ** Project::menuCreateScene
void Project::menuCreateScene( Ui::ActionQPtr action )
{
	createAsset( "Scene", "scene" );
}

// ** Project::menuCreateMaterial
void Project::menuCreateMaterial( Ui::ActionQPtr action )
{
	createAsset( "Material", "material" );
}

// ** Project::menuOpenAsset
void Project::menuOpenAsset( Ui::ActionQPtr action )
{
	qMainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Project::menuDeleteAsset
void Project::menuDeleteAsset( Ui::ActionQPtr action )
{
	qMainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Project::menuBrowseAssets
void Project::menuBrowseAssets( Ui::ActionQPtr action )
{
	qComposer->fileSystem()->browse( assetsAbsolutePath() );
}

// ** Project::menuShowInExplorer
void Project::menuShowInExplorer( Ui::ActionQPtr action )
{
	// Get the file system instance
	FileSystemQPtr fs = qComposer->fileSystem();

	// Get the selected items
	FileInfoArray selected = qMainWindow->assetTree()->selection();

	// Get the file info
	FileInfo fileInfo = selected.empty() ? fs->extractFileInfo( assetsAbsolutePath() ) : selected[0];

	// Browse to asset
	fs->browse( fileInfo.isDir() ? fileInfo.absolutePath() : fileInfo.dir() );
}

DC_END_COMPOSER
