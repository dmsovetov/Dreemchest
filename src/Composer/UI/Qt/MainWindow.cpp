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

#include "MainWindow.h"
#include "Menu.h"

#include "RenderingFrame.h"
#include "FileSystem.h"
#include "AssetTree.h"

#include "../../Project/Project.h"

namespace Ui {

// ** createMainWindow
IMainWindowPtr createMainWindow( const String& title )
{
	return IMainWindowPtr( new MainWindow( title ) );
}

// ** MainWindow::MainWindow
MainWindow::MainWindow( const String& title ) : UserInterface( new QMainWindow )
{
#ifdef NDEBUG
	m_private->setWindowState( Qt::WindowMaximized );
#else
	m_private->resize( 1024, 768 );
#endif

    m_private->setUnifiedTitleAndToolBarOnMac( true );
	m_private->show();
	m_private->setWindowTitle( title.c_str() );
}

// ** MainWindow::initialize
bool MainWindow::initialize( ComposerWPtr composer )
{
	// Create the file system
	m_fileSystem = new FileSystem( m_private.get() );

	// Listen for events
	composer->subscribe<Composer::ProjectOpened>( dcThisMethod( MainWindow::onProjectOpened ) );
	composer->subscribe<Composer::ProjectClosed>( dcThisMethod( MainWindow::onProjectClosed ) );

	return true;
}

// ** MainWindow::message
void MainWindow::message( const String& text, MessageStatus status )
{
	switch( status ) {
	case MessageInfo:		QMessageBox::information( m_private.get(), m_private->windowTitle(), text.c_str() );	break;
	case MessageWarning:	QMessageBox::warning( m_private.get(), m_private->windowTitle(), text.c_str() );		break;
	case MessageError:		QMessageBox::critical( m_private.get(), m_private->windowTitle(), text.c_str() );		break;
	default:				DC_BREAK;
	}
}

// ** MainWindow::fileSystem
IFileSystemWPtr MainWindow::fileSystem( void ) const
{
	return m_fileSystem;
}

// ** MainWindow::assetTree
IAssetTreeWPtr MainWindow::assetTree( void ) const
{
	return m_assetTree;
}

// ** MainWindow::addMenu
IMenuWPtr MainWindow::addMenu( const String& title )
{
	QMenuBar* bar  = m_private->menuBar();
	Menu*	  menu = new Menu( bar );

	menu->setTitle( title );
	bar->addMenu( menu->privateInterface<QMenu>() );
	m_menues.append( menu );

	return menu;
}

// ** MainWindow::addToolBar
IToolBarWPtr MainWindow::addToolBar( void )
{
	ToolBar *toolBar = new ToolBar( m_private.get() );
	m_private->addToolBar( toolBar->privateInterface<QToolBar>() );

	return toolBar;
}

// ** MainWindow::addDock
QDockWidget* MainWindow::addDock( const QString& name, QWidget* widget, Qt::DockWidgetArea initialDockArea, Qt::DockWidgetArea allowedDockAreas )
{
	QDockWidget* dock = new QDockWidget( name, m_private.get() );
	dock->setAllowedAreas( allowedDockAreas );
	dock->setWidget( widget );
	m_private->addDockWidget( initialDockArea, dock );
	return dock;
}

// ** MainWindow::removeToolBar
void MainWindow::removeToolBar( IToolBarWPtr toolBar )
{
	m_private->removeToolBar( toolBar->privateInterface<QToolBar>() );
}

// ** MainWindow::removeMenu
void MainWindow::removeMenu( IMenuWPtr menu )
{
	s32 index = m_menues.indexOf( menu );

	if( index == -1 ) {
		return;
	}

	m_menues.remove( index );
}

// ** MainWindow::onProjectOpened
void MainWindow::onProjectOpened( const Composer::ProjectOpened& e )
{
	// Get the project from event
	Project::ProjectWPtr project = e.project;

	// Create the file system model used by project
	m_fileSystemModel = new QFileSystemModel( m_private.get() );
	m_fileSystemModel->setRootPath( project->assetsAbsolutePath().c_str() );
	m_fileSystemModel->setReadOnly( false );

	// Create the asset tree
	m_assetTree = new AssetTree( project, m_fileSystemModel.get() );

	// Add dock
	addDock( "Assets", m_assetTree->privateInterface<QAssetTree>(), Qt::RightDockWidgetArea );
}

// ** MainWindow::onProjectClosed
void MainWindow::onProjectClosed( const Composer::ProjectClosed& e )
{
	DC_BREAK;
}

} // namespace Ui