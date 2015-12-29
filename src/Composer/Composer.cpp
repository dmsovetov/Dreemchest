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

#include "Composer.h"
#include "IFileSystem.h"

#include "Widgets/IMainWindow.h"
#include "Widgets/IMenu.h"
#include "Widgets/IAssetTree.h"

#include "Project/Project.h"

#ifdef emit
	#undef emit
#endif

DC_BEGIN_COMPOSER

namespace Editors {

// --------------------------------------------------------------- SceneEditorInternal -------------------------------------------------------------- //

// ** SceneEditorInternal::SceneEditorInternal
SceneEditorInternal::SceneEditorInternal( Scene::SceneObjectWPtr parent, u8 flags ) : m_parent( parent ), m_flags( flags )
{

}

// ** SceneEditorInternal::parent
Scene::SceneObjectWPtr SceneEditorInternal::parent( void ) const
{
	return m_parent;
}

// ** SceneEditorInternal::isPrivate
bool SceneEditorInternal::isPrivate( void ) const
{
	return m_flags.is( Private );
}

// ** SceneEditorInternal::isHighlighted
bool SceneEditorInternal::isHighlighted( void ) const
{
	return m_flags.is( Highlighted );
}

// ** SceneEditorInternal::setHighlighted
void SceneEditorInternal::setHighlighted( bool value )
{
	m_flags.set( Highlighted, value );
}

// ** SceneEditorInternal::isSelected
bool SceneEditorInternal::isSelected( void ) const
{
	return m_flags.is( Selected );
}

// ** SceneEditorInternal::setSelected
void SceneEditorInternal::setSelected( bool value )
{
	m_flags.set( Selected, value );
}


} // namespace Editors

// -------------------------------------------------------------------- Composer -------------------------------------------------------------------- //

// ** Composer::s_instance
ComposerWPtr Composer::s_instance = ComposerWPtr();

// ** Composer::kAssetMime
const String Composer::kAssetMime = "text/uri-list";

// ** Composer::Composer
Composer::Composer( Ui::IMainWindowPtr mainWindow ) : m_mainWindow( mainWindow )
{
	s_instance = this;

	Renderer::log::setStandardHandler();
	Scene::log::setStandardHandler();
}

// ** Composer::instance
ComposerWPtr Composer::instance( void )
{
	return s_instance;
}

// ** Composer::project
Project::ProjectWPtr Composer::project( void ) const
{
	return m_project;
}

// ** Composer::window
Ui::IMainWindowWPtr Composer::window( void ) const
{
	return m_mainWindow;
}

// ** Composer::create
ComposerPtr Composer::create( void )
{
	// Create the main window
	Ui::IMainWindowPtr mainWindow = Ui::createMainWindow( "Dreemchest Composer" );
	
	// Create the composer instance
	ComposerPtr composer = new Composer( mainWindow );

	// Initialize the composer
	if( !composer->initialize() ) {
		return ComposerPtr();
	}

	return composer;
}

// ** Composer::initialize
bool Composer::initialize( void )
{
	// Initialize the main window
	if( !m_mainWindow->initialize( this ) ) {
		return false;
	}

	// Add default menues
	m_menues[FileMenu] = m_mainWindow->addMenu( "&File" );
	m_menues[EditMenu] = m_mainWindow->addMenu( "&Edit" );

	// Add actions
	Ui::IActionWPtr create	= m_menues[FileMenu]->addAction( "&Create Project", BindAction( Composer::menuCreateProject ), "Ctrl+N", ":Common/Common/new.png" );
	Ui::IActionWPtr open	= m_menues[FileMenu]->addAction( "&Open Project", BindAction( Composer::menuOpenProject ), "Ctrl+O", ":Common/Common/open.png" );
	Ui::IActionWPtr save	= m_menues[FileMenu]->addAction( "&Save Project", BindAction( Composer::menuSaveProject ), "Ctrl+S", ":Common/Common/save.png" );

	// Add actions
	Ui::IActionWPtr undo	= m_menues[EditMenu]->addAction( "&Undo", BindAction( Composer::menuUndo ), "Ctrl+Z", ":Common/Common/undo.png" );
	Ui::IActionWPtr redo	= m_menues[EditMenu]->addAction( "&Redo", BindAction( Composer::menuRedo ), "Ctrl+Y", ":Common/Common/redo.png" );

	// Create tool bars
	Ui::IToolBarWPtr tools = m_mainWindow->addToolBar();
	tools->addAction( create );
	tools->addAction( open );
	tools->addAction( save );
	tools->addSeparator();
	tools->addAction( undo );
	tools->addAction( redo );

	return true;
}

// ** Composer::menuCreateProject
void Composer::menuCreateProject( Ui::IActionWPtr action )
{
	// Get the file system interface
	FileSystemWPtr fs = m_mainWindow->fileSystem();

	// Select the directory
	String path = fs->selectExistingDirectory( "Create Project" );

	if( path == "" ) {
		return;
	}

	createProject( path );
}

// ** Composer::menuOpenProject
void Composer::menuOpenProject( Ui::IActionWPtr action )
{
	// Get the file system interface
	FileSystemWPtr fs = m_mainWindow->fileSystem();

	// Select the directory
	String path = fs->selectExistingDirectory( "Open Project" );

	if( path == "" ) {
		return;
	}

	openProject( path );
}

// ** Composer::menuSaveProject
void Composer::menuSaveProject( Ui::IActionWPtr action )
{
	m_mainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Composer::menuUndo
void Composer::menuUndo( Ui::IActionWPtr action )
{
	m_mainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Composer::menuRedo
void Composer::menuRedo( Ui::IActionWPtr action )
{
	m_mainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Composer::createProject
void Composer::createProject( const String& path )
{
	// Get the file system interface
	FileSystemWPtr fs = m_mainWindow->fileSystem();

	// Create project instance
	m_project = Project::Project::create( m_mainWindow, path );

	// Create all project folders
	for( s32 i = 0; i < Project::Project::TotalPaths; i++ ) {
		fs->createDirectory( m_project->absolutePath( i ) );
	}

	// Emit the event
	notify<ProjectCreated>( m_project );

	// Open created project
	openProject( path );
}

// ** Composer::assetFromMime
Scene::AssetPtr Composer::assetFromMime( IMimeDataWPtr mime ) const
{
	Scene::AssetSet assets = assetsFromMime( mime );

	if( assets.empty() ) {
		return Scene::AssetPtr();
	}

	return *assets.begin();
}

// ** Composer::assetsFromMime
Scene::AssetSet Composer::assetsFromMime( IMimeDataWPtr mime ) const
{
	// Resulting asset set
	Scene::AssetSet result;

	// Get an array of attached assets
	StringArray assets = mime->strings();

	// Add assets to scene
	for( s32 i = 0, n = ( s32 )assets.size(); i < n; i++ ) {
		// Read an attached meta data
		Io::KeyValue meta = m_project->assetsModel()->metaData( assets[i] );

		if( meta.isNull() ) {
			continue;	// Unsupported asset type or just a folder
		}

		DC_BREAK_IF( !meta.isObject() );

		// Find asset by UUID.
		Scene::AssetWPtr asset = m_project->assets()->findAsset( meta.get( "uuid", "" ).asString() );
		DC_BREAK_IF( !asset.valid() );

		// Add to set.
		result.insert( asset );
	}

	return result;
}

// ** Composer::openProject
void Composer::openProject( const String& path )
{
	// Create project instance
	m_project = Project::Project::create( m_mainWindow, path );

	// Emit the event
	notify<ProjectOpened>( m_project );

	// Setup menues
	//m_menues[EditMenu]	 = m_mainWindow->addMenu( "&Edit" );
	m_menues[ViewMenu]	 = m_mainWindow->addMenu( "&View" );
	m_menues[AssetsMenu] = m_mainWindow->addMenu( "&Assets" );

	// Fill assets menu
	m_project->fillAssetMenu( m_menues[AssetsMenu] );
}

// ** Composer::ProjectOpened::ProjectOpened
Composer::ProjectOpened::ProjectOpened( Project::ProjectWPtr project ) : project( project )
{

}

// ** Composer::ProjectClosed::ProjectClosed
Composer::ProjectClosed::ProjectClosed( Project::ProjectWPtr project ) : project( project )
{

}

// ** Composer::ProjectCreated::ProjectCreated
Composer::ProjectCreated::ProjectCreated( Project::ProjectWPtr project ) : project( project )
{

}

DC_END_COMPOSER

// ** main
int main(int argc, char *argv[])
{
	QCoreApplication::setLibraryPaths( QCoreApplication::libraryPaths() << "." << "imageformats" << "platforms" );

    QApplication app( argc, argv );
    app.setApplicationName( "Dreemchest Composer" );

	ComposerPtr composer = Composer::create();

	if( !composer.valid() ) {
		return -1;
	}

	composer->openProject( "~TestProject" );

    return app.exec();
}