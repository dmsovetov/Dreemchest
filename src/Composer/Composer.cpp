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

#include "FileSystem.h"

#include "Widgets/Menu.h"
#include "Widgets/MainWindow.h"
#include "Widgets/AssetTree.h"

#include "Project/Project.h"

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

// ** Composer::kAssetMime
const String Composer::kAssetMime = "text/uri-list";

// ** Composer::Composer
Composer::Composer( int argc, char ** argv ) : QApplication( argc, argv ), m_project( NULL )
{
    // Set application name
    setApplicationName( "Dreemchest Composer" );

    // Create the main window
    m_mainWindow = new Ui::MainWindow( applicationName() );

    // Create the file system
	m_fileSystem = new FileSystem( this );

    // Setup default log handlers
	Renderer::log::setStandardHandler();
	Scene::log::setStandardHandler();
}

// ** Composer::project
ProjectQPtr Composer::project( void ) const
{
	return m_project;
}

// ** Composer::window
Ui::MainWindowQPtr Composer::window( void ) const
{
	return m_mainWindow;
}

// ** Composer::fileSystem
FileSystemQPtr Composer::fileSystem( void ) const
{
	return m_fileSystem;
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
	Ui::ActionQPtr create	= m_menues[FileMenu]->addAction( "&Create Project", BindAction( Composer::menuCreateProject ), "Ctrl+N", ":Common/Common/new.png" );
	Ui::ActionQPtr open	= m_menues[FileMenu]->addAction( "&Open Project", BindAction( Composer::menuOpenProject ), "Ctrl+O", ":Common/Common/open.png" );
	Ui::ActionQPtr save	= m_menues[FileMenu]->addAction( "&Save Project", BindAction( Composer::menuSaveProject ), "Ctrl+S", ":Common/Common/save.png" );

	// Add actions
	Ui::ActionQPtr undo	= m_menues[EditMenu]->addAction( "&Undo", BindAction( Composer::menuUndo ), "Ctrl+Z", ":Common/Common/undo.png" );
	Ui::ActionQPtr redo	= m_menues[EditMenu]->addAction( "&Redo", BindAction( Composer::menuRedo ), "Ctrl+Y", ":Common/Common/redo.png" );

	// Create tool bars
	Ui::ToolBarQPtr tools = m_mainWindow->addToolBar();
	tools->addAction( create );
	tools->addAction( open );
	tools->addAction( save );
	tools->addSeparator();
	tools->addAction( undo );
	tools->addAction( redo );

	return true;
}

// ** Composer::menuCreateProject
void Composer::menuCreateProject( Ui::ActionQPtr action )
{
	// Select the directory
	String path = m_fileSystem->selectExistingDirectory( "Create Project" );

	if( path == "" ) {
		return;
	}

	createProject( path );
}

// ** Composer::menuOpenProject
void Composer::menuOpenProject( Ui::ActionQPtr action )
{
	// Select the directory
	String path = m_fileSystem->selectExistingDirectory( "Open Project" );

	if( path == "" ) {
		return;
	}

	openProject( path );
}

// ** Composer::menuSaveProject
void Composer::menuSaveProject( Ui::ActionQPtr action )
{
	m_mainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Composer::menuUndo
void Composer::menuUndo( Ui::ActionQPtr action )
{
	m_mainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Composer::menuRedo
void Composer::menuRedo( Ui::ActionQPtr action )
{
	m_mainWindow->message( "Not implemented yet.", Ui::MessageWarning );
}

// ** Composer::createProject
void Composer::createProject( const String& path )
{
    // Close active project
    closeProject();

	// Create project instance
	m_project = new Project( this, path );

	// Create all project folders
	for( s32 i = 0; i < Project::TotalPaths; i++ ) {
		m_fileSystem->createDirectory( m_project->absolutePath( i ) );
	}

	// Emit the signal
    emit projectCreated( m_project );

	// Open created project
	openProject( path );
}

// ** Composer::closeProject
void Composer::closeProject( void )
{
    // No project opened
    if( !m_project ) {
        return;
    }

    // Emit the signal
    emit projectClosed( m_project );

    // Destroy project
    delete m_project;
    m_project = NULL;
}

// ** Composer::assetFromMime
Scene::AssetPtr Composer::assetFromMime( MimeDataQPtr mime ) const
{
	Scene::AssetSet assets = assetsFromMime( mime );

	if( assets.empty() ) {
		return Scene::AssetPtr();
	}

	return *assets.begin();
}

// ** Composer::assetsFromMime
Scene::AssetSet Composer::assetsFromMime( MimeDataQPtr mime ) const
{
	// Resulting asset set
	Scene::AssetSet result;

	// Get an array of attached assets
	QList<QUrl> assets = mime->urls();

	// Add assets to scene
	foreach( QUrl url, assets ) {
		// Read an attached meta data
		Io::KeyValue meta = m_project->assetsModel()->metaData( url.toLocalFile().toStdString() );

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
    // Close active project
    closeProject();

	// Create project instance
	m_project = new Project( this, path );

	// Emit the signal
    emit projectOpened( m_project );

	// Setup menues
	//m_menues[EditMenu]	 = m_mainWindow->addMenu( "&Edit" );
	m_menues[ViewMenu]	 = m_mainWindow->addMenu( "&View" );
	m_menues[AssetsMenu] = m_mainWindow->addMenu( "&Assets" );

	// Fill assets menu
	m_project->fillAssetMenu( m_menues[AssetsMenu] );
}

DC_END_COMPOSER

// ** main
int main(int argc, char *argv[])
{
	QCoreApplication::setLibraryPaths( QCoreApplication::libraryPaths() << "." << "imageformats" << "platforms" );

    Composer app( argc, argv );

    if( !app.initialize() ) {
        return -1;
    }
    
    // Open the project
	app.openProject( "~TestProject" );

    return app.exec();
}