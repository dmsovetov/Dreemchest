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

// ** MainWindow::MainWindow
MainWindow::MainWindow( void )
{
#ifdef NDEBUG
	setWindowState( Qt::WindowMaximized );
#else
	resize( 1024, 768 );
#endif

    setUnifiedTitleAndToolBarOnMac( true );
}

// ** MainWindow::initialize
bool MainWindow::initialize( void )
{
	// Create menues
	m_menues[MenuFile]	  = menuBar()->addMenu( "&File" );
	m_menues[MenuView]	  = menuBar()->addMenu( "&View" );
	m_menues[MenuAssets]  = menuBar()->addMenu( "&Assets" );

	addAction( MenuFile, "New Project", SLOT(newProject()) );
	addAction( MenuFile, "Open Project", SLOT(openProject()) );
	addAction( MenuAssets, "Import", SLOT(importAssets()) );

	return true;
}

// ** MainWindow::addAction
QAction* MainWindow::addAction( s32 menu, const QString& caption, CString slot )
{
	DC_BREAK_IF( !m_menues[menu] );

	QAction* action = m_menues[menu]->addAction( caption );
	connect( action, SIGNAL(triggered()), this, slot );

	return action;
}

// ** MainWindow::addDock
QDockWidget* MainWindow::addDock( const QString& name, QWidget* widget, Qt::DockWidgetArea initialDockArea, Qt::DockWidgetArea allowedDockAreas )
{
	QDockWidget* dock = new QDockWidget( name, this );
	dock->setAllowedAreas( allowedDockAreas );
	dock->setWidget( widget );
	addDockWidget( initialDockArea, dock );
	m_menues[MenuView]->addAction( dock->toggleViewAction() );
	return dock;
}

// ** MainWindow::closeEvent
void MainWindow::closeEvent( QCloseEvent* event )
{

}

// ** MainWindow::importAssets
void MainWindow::importAssets( void )
{
	DC_NOT_IMPLEMENTED
}