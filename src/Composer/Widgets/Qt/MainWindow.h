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

#include "Widget.h"

DC_BEGIN_COMPOSER

namespace Ui {

	//! Main application window Qt implementation
	class MainWindow : public PrivateInterface<IMainWindow, QMainWindow> {
	public:

										//! Constructs MainWindow instance.
										MainWindow( const String& title );
									
		//! Adds a new toolbar to window.
		virtual IToolBarWPtr			addToolBar( void ) DC_DECL_OVERRIDE;
	
		//! Removes the toolbar from a window.
		virtual void					removeToolBar( IToolBarWPtr toolBar ) DC_DECL_OVERRIDE;
	
		//! Adds a new menu to window.
		virtual IMenuWPtr				addMenu( const String& text ) DC_DECL_OVERRIDE;
	
		//! Removes the menu from a window.
		virtual void					removeMenu( IMenuWPtr menu ) DC_DECL_OVERRIDE;

		//! Performs the main window initialization.
		virtual bool					initialize( ComposerWPtr composer ) DC_DECL_OVERRIDE;

		//! Constructs a new asset editor dock window or brings to front the existing one.
		virtual IDocumentWPtr			editDocument( Editors::AssetEditorWPtr assetEditor, const Scene::AssetPtr& asset ) DC_DECL_OVERRIDE;

		//! Closes the document.
		virtual bool					closeDocument( IDocumentWPtr document ) DC_DECL_OVERRIDE;

		//! Returns the opened document editor by asset.
		virtual IDocumentWPtr			findDocument( const Scene::AssetWPtr& asset ) const DC_DECL_OVERRIDE;

		//! Returns an array of opened documents with a same type.
		virtual DocumentsWeak			findDocuments( const Scene::AssetWPtr& asset ) const DC_DECL_OVERRIDE;

		//! Shows the message box.
		virtual void					message( const String& text, MessageStatus status = MessageInfo ) const DC_DECL_OVERRIDE;

		//! Shows the message box with yes, no, cancel buttons.
		virtual MessageBoxResult		messageYesNoCancel( const String& text, const String& info, MessageStatus status = MessageInfo ) const DC_DECL_OVERRIDE;

		//! Returns the file system instance.
		virtual FileSystemWPtr			fileSystem( void ) const DC_DECL_OVERRIDE;

		//! Returns the asset tree instance.
		virtual IAssetTreeWPtr			assetTree( void ) const DC_DECL_OVERRIDE;

		//! Returns the scene tree instance.
		virtual ISceneTreeWPtr			sceneTree( void ) const DC_DECL_OVERRIDE;

		//! Sets an active document.
		virtual void					setActiveDocument( IDocumentWPtr dock ) DC_DECL_OVERRIDE;

		//! Returns the shared rendering context.
		virtual IRenderingFrameWPtr		sharedRenderingContext( void ) const DC_DECL_OVERRIDE;

	private:

		//! Ensures that the document was saved before closing.
		bool							ensureSaved( IDocumentWPtr document ) const;

		//! Creates the dock widget.
		QDockWidget*					addDock( const QString& name, QWidget* widget, Qt::DockWidgetArea initialDockArea = Qt::LeftDockWidgetArea, Qt::DockWidgetAreas allowedDockAreas = Qt::AllDockWidgetAreas, QDockWidget* destination = NULL );

		//! Handles the ProjectOpened event.
		void							onProjectOpened( const Composer::ProjectOpened& e );

		//! Handles the ProjectClosed event.
		void							onProjectClosed( const Composer::ProjectClosed& e );
	
	private:

		QVector<IMenuPtr>				m_menues;					//!< All added menues reside here.
		QVector<IToolBarPtr>			m_toolbars;					//!< All added toolbars reside here.
		QVector<IDocumentPtr>			m_documents;				//!< All opened documents reside here.
		IDocumentWPtr					m_activeDocument;			//!< An active document.
		IRenderingFramePtr				m_sharedRenderingContext;	//!< The shared OpenGL context.
		FileSystemPtr					m_fileSystem;				//!< File system interface.
		IAssetTreePtr					m_assetTree;				//!< Asset tree instance.
		ISceneTreePtr					m_sceneTree;				//!< Scene tree instance.
		Project::ProjectWPtr			m_project;					//!< Opened project.
	};

} // namespace Ui

DC_END_COMPOSER

#endif	/*	!__DC_Composer_MainWindow_H__	*/