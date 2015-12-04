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

#ifndef __DC_Composer_MainWindow_H__
#define __DC_Composer_MainWindow_H__

#include "../IMainWindow.h"
#include "UserInterface.h"

namespace Ui {

	//! Main application window Qt implementation
	class MainWindow : public UserInterface<IMainWindow, QMainWindow> {
	public:

										//! Constructs MainWindow instance.
										MainWindow( const String& title );
									
		//! Adds a new toolbar to window.
		virtual IToolBarWPtr			addToolBar( void );
	
		//! Removes the toolbar from a window.
		virtual void					removeToolBar( IToolBarWPtr toolBar );
	
		//! Adds a new menu to window.
		virtual IMenuWPtr				addMenu( const String& text );
	
		//! Removes the menu from a window.
		virtual void					removeMenu( IMenuWPtr menu );

		//! Performs the main window initialization.
		virtual bool					initialize( ComposerWPtr composer );

		//! Shows the message box.
		virtual void					message( const String& text, MessageStatus status = MessageInfo );

		//! Returns the file system instance.
		virtual IFileSystemWPtr			fileSystem( void ) const;

		//! Returns the asset tree instance.
		virtual IAssetTreeWPtr			assetTree( void ) const;

	private:

		//! Creates the dock widget.
		QDockWidget*					addDock( const QString& name, QWidget* widget, Qt::DockWidgetArea initialDockArea = Qt::LeftDockWidgetArea, Qt::DockWidgetArea allowedDockAreas = Qt::AllDockWidgetAreas );

		//! Handles the ProjectOpened event.
		void							onProjectOpened( const Composer::ProjectOpened& e );

		//! Handles the ProjectClosed event.
		void							onProjectClosed( const Composer::ProjectClosed& e );
	
	private:

		QVector<IMenuPtr>				m_menues;			//!< All added menues reside here.
		QVector<IToolBarPtr>			m_toolbars;			//!< All added toolbars reside here.
		IFileSystemPtr					m_fileSystem;		//!< File system interface.
		IAssetTreePtr					m_assetTree;		//!< Asset tree instance.
		AutoPtr<QFileSystemModel>		m_fileSystemModel;	//!< Shared file system model.
	};

} // namespace Ui

#endif	/*	!__DC_Composer_MainWindow_H__	*/