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

} // namespace Ui