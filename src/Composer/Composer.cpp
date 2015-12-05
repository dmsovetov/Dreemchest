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

#include "UI/IMainWindow.h"
#include "UI/IMenu.h"
#include "UI/IFileSystem.h"
#include "UI/IAssetTree.h"

#include "Project/Project.h"

#ifdef emit
	#undef emit
#endif

// ** Composer::s_instance
ComposerWPtr Composer::s_instance = ComposerWPtr();

// ** Composer::Composer
Composer::Composer( Ui::IMainWindowPtr mainWindow ) : m_mainWindow( mainWindow )
{
	s_instance = this;

	Renderer::log::setStandardHandler();
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

	// Add actions
	m_menues[FileMenu]->addAction( "Create Project", BindAction( Composer::menuCreateProject ), "Ctrl+N" );
	m_menues[FileMenu]->addAction( "Open Project", BindAction( Composer::menuOpenProject ), "Ctrl+O" );

	return true;
}

// ** Composer::menuCreateProject
void Composer::menuCreateProject( Ui::IActionWPtr action )
{
	// Get the file system interface
	Ui::IFileSystemWPtr fs = m_mainWindow->fileSystem();

	// Select the directory
	String path = fs->selectExistingDirectory( "Create Project" );

	if( path == "" ) {
		return;
	}

	createNewProject( path );
}

// ** Composer::menuOpenProject
void Composer::menuOpenProject( Ui::IActionWPtr action )
{
	// Get the file system interface
	Ui::IFileSystemWPtr fs = m_mainWindow->fileSystem();

	// Select the directory
	String path = fs->selectExistingDirectory( "Open Project" );

	if( path == "" ) {
		return;
	}

	openExistingProject( path );
}

// ** Composer::createNewProject
void Composer::createNewProject( const String& path )
{
	// Get the file system interface
	Ui::IFileSystemWPtr fs = m_mainWindow->fileSystem();

	// Create project instance
	m_project = Project::Project::create( m_mainWindow, path );

	// Create all project folders
	for( s32 i = 0; i < Project::Project::TotalPaths; i++ ) {
		fs->createDirectory( m_project->absolutePath( i ) );
	}

	// Emit the event
	m_event.emit<ProjectCreated>( m_project );

	// Open created project
	openExistingProject( path );
}

// ** Composer::openExistingProject
void Composer::openExistingProject( const String& path )
{
	// Create project instance
	m_project = Project::Project::create( m_mainWindow, path );

	// Emit the event
	m_event.emit<ProjectOpened>( m_project );

	// Setup menues
	m_menues[EditMenu]	 = m_mainWindow->addMenu( "&Edit" );
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

    return app.exec();
}