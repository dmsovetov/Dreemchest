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
#include "SceneTree.h"
#include "Document.h"

#include "../../Project/Project.h"
#include "../../Editors/AssetEditor.h"

#if DEV_USE_DOCK_INDICATOR
	#include "DockIndicator.h"
#endif	/*	DEV_USE_DOCK_INDICATOR	*/

DC_BEGIN_COMPOSER

namespace Ui {

// ** createMainWindow
IMainWindowPtr createMainWindow( const String& title )
{
	return IMainWindowPtr( new MainWindow( title ) );
}

// ** convertKey
Platform::Key convertKey( s32 key )
{
	switch( key ) {
	case Qt::Key_Back:		return Platform::Key::Back;
	case Qt::Key_Delete:	return Platform::Key::Delete;
	case Qt::Key_Left:		return Platform::Key::Left;
	case Qt::Key_Right:		return Platform::Key::Right;
	case Qt::Key_Up:		return Platform::Key::Up;
	case Qt::Key_Down:		return Platform::Key::Down;
	}

	return Platform::Key::Total;
}

// ** MainWindow::MainWindow
MainWindow::MainWindow( const String& title ) : PrivateInterface( new QMainWindow )
{
#if DEV_USE_DOCK_INDICATOR
	// Add the dock indicator widget
	new DockIndicator( m_private.get() );
#endif	/*	DEV_USE_DOCK_INDICATOR	*/

	// Setup the share OpenGL format
	QRenderingFrame::setupOpenGLFormat();

	// Create the shared OpenGL context
	m_sharedRenderingContext = new RenderingFrame( NULL, m_private.get() );
	m_sharedRenderingContext->privateInterface<QRenderingFrame>()->hide();
	//m_private->setCentralWidget( m_sharedRenderingContext->privateInterface<QRenderingFrame>() );

#ifdef NDEBUG
	//m_private->setWindowState( Qt::WindowMaximized );
	m_private->resize( 1024, 768 );
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
	m_fileSystem = new FileSystemPrivate( m_private.get() );

	// Listen for events
	composer->subscribe<Composer::ProjectOpened>( dcThisMethod( MainWindow::onProjectOpened ) );
	composer->subscribe<Composer::ProjectClosed>( dcThisMethod( MainWindow::onProjectClosed ) );

	return true;
}

// ** MainWindow::message
void MainWindow::message( const String& text, MessageStatus status ) const
{
	switch( status ) {
	case MessageInfo:		QMessageBox::information( m_private.get(), m_private->windowTitle(), text.c_str() );	break;
	case MessageWarning:	QMessageBox::warning( m_private.get(), m_private->windowTitle(), text.c_str() );		break;
	case MessageError:		QMessageBox::critical( m_private.get(), m_private->windowTitle(), text.c_str() );		break;
	default:				DC_BREAK;
	}
}

// ** MainWindow::message
MessageBoxResult MainWindow::messageYesNoCancel( const String& text, const String& info, MessageStatus status ) const
{
	QMessageBox msgBox;
	msgBox.setText( text.c_str() );
	msgBox.setInformativeText( info.c_str() );
	msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No );
	msgBox.setDefaultButton( QMessageBox::Cancel );

	switch( status ) {
	case MessageInfo:		msgBox.setIcon( QMessageBox::Information );		break;
	case MessageWarning:	msgBox.setIcon( QMessageBox::Warning );			break;
	case MessageError:		msgBox.setIcon( QMessageBox::Critical );		break;
	default:				DC_BREAK;
	}
	
	switch( msgBox.exec() ) {
	case QMessageBox::Yes:		return MessageBoxYes;
	case QMessageBox::No:		return MessageBoxNo;
	}

	return MessageBoxCancel;
}

// ** MainWindow::sharedRenderingContext
IRenderingFrameWPtr MainWindow::sharedRenderingContext( void ) const
{
	return m_sharedRenderingContext;
}

// ** MainWindow::fileSystem
FileSystemWPtr MainWindow::fileSystem( void ) const
{
	return m_fileSystem;
}

// ** MainWindow::assetTree
IAssetTreeWPtr MainWindow::assetTree( void ) const
{
	return m_assetTree;
}

// ** MainWindow::sceneTree
ISceneTreeWPtr MainWindow::sceneTree( void ) const
{
	return m_sceneTree;
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
QDockWidget* MainWindow::addDock( const QString& name, QWidget* widget, Qt::DockWidgetArea initialDockArea, Qt::DockWidgetAreas allowedDockAreas, QDockWidget* destination )
{
	QDockWidget* dock = new QDockWidget( name );
	dock->setAllowedAreas( allowedDockAreas );
	dock->setWidget( widget );
	m_private->addDockWidget( initialDockArea, dock );

	if( destination ) {
		m_private->tabifyDockWidget( dock, destination );
	}

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

// ** MainWindow::editDocument
IDocumentWPtr MainWindow::editDocument( Editors::AssetEditorWPtr assetEditor, const Scene::AssetPtr& asset )
{
	DC_BREAK_IF( !assetEditor.valid() );

	// First lookup the exising document
	IDocumentWPtr existing = findDocument( asset );

	if( existing.valid() ) {
		existing->privateInterface<QDocumentDock>()->raise();
		return existing;
	}

	// Create the document instance
	IDocumentPtr document( new Document( this, assetEditor, asset->name() ) );

	// Initialize the asset editor with a document
	if( !assetEditor->initialize( m_project, asset, document ) ) {
		return IDocumentWPtr();
	}

	m_private->addDockWidget( Qt::LeftDockWidgetArea, document->privateInterface<QDocumentDock>() );

	// Find opened documents of a same type
	DocumentsWeak documents = findDocuments( asset );

	if( documents.size() ) {
		QDockWidget*tabifyTo = documents[0]->privateInterface<QDocumentDock>();
		m_private->tabifyDockWidget( tabifyTo, document->privateInterface<QDocumentDock>() );
	}

	// Set the document as active
	setActiveDocument( document );

	// Save created document
	m_documents.append( document );

	return document;
}

// ** MainWindow::closeDocument
bool MainWindow::closeDocument( IDocumentWPtr document )
{
	// Find the document
	int index = m_documents.indexOf( document );

	if( index == -1 ) {
		DC_BREAK;
		return false;
	}

	// Ensure that document is saved
	if( !ensureSaved( document ) ) {
		return false;
	}

	// Change active document
	setActiveDocument( (m_documents.size() - 1) ? m_documents[index - 1] : IDocumentWPtr() );

	// Remove the document dock widget
	m_private->removeDockWidget( document->privateInterface<QDocumentDock>() );

	// Remove from documents
	m_documents.remove( index );

	return true;
}

// ** MainWindow::findDocument
IDocumentWPtr MainWindow::findDocument( const Scene::AssetWPtr& asset ) const
{
	foreach( IDocumentWPtr document, m_documents ) {
		if( document->assetEditor()->asset() == asset ) {
			return document;
		}
	}

	return IDocumentWPtr();
}

// ** MainWindow::findDocuments
DocumentsWeak MainWindow::findDocuments( const Scene::AssetWPtr& asset ) const
{
	DocumentsWeak documents;

	foreach( IDocumentWPtr document, m_documents ) {
		if( document->assetEditor()->asset()->type() == asset->type() ) {
			documents.push_back( document );
		}
	}

	return documents;
}

// ** MainWindow::setActiveDocument
void MainWindow::setActiveDocument( IDocumentWPtr document )
{
	// This document is already set as active
	if( m_activeDocument == document ) {
		return;
	}

	// Notify the active document about moving to background.
	if( m_activeDocument.valid() ) {
		m_activeDocument->assetEditor()->notifyEnterBackground();
	}

	// Set new active document
	m_activeDocument = document;

	// Notify active document about moving to foreground.
	if( m_activeDocument.valid() ) {
		// Raise the document dock widget
		m_activeDocument->privateInterface<QDocumentDock>()->raise();

		// Notify the asset editor
		m_activeDocument->assetEditor()->notifyEnterForeground();
	}
}

// ** MainWindow::ensureSaved
bool MainWindow::ensureSaved( IDocumentWPtr document ) const
{
	// Get the attached asset editor.
	Editors::AssetEditorWPtr assetEditor = document->assetEditor();

	// The document has no unsaved changes.
	if( !assetEditor->hasChanges() ) {
		return true;
	}

	// Show the message box
	String			 message = "Do you want to save changes to " + assetEditor->asset()->name() + "?";
	String			 info	 = "Your changes will be lost if you don't save them";
	MessageBoxResult result  = messageYesNoCancel( message, info, MessageWarning );

	switch( result ) {
	case MessageBoxCancel:	return false;
	case MessageBoxYes:		assetEditor->save();
	case MessageBoxNo:		break;
	}

	return true;
}

// ** MainWindow::onProjectOpened
void MainWindow::onProjectOpened( const Composer::ProjectOpened& e )
{
	// Get the project from event
	m_project = e.project;

	// Create the asset tree
	m_assetTree = new AssetTree( m_project );
	m_assetTree->setModel( m_project->assetsModel() );

	// Create the scene tree
	m_sceneTree = new SceneTree;

	// Setup status bar
	m_private->statusBar()->show();

	// Add dock windows
	addDock( "Inspector", new QTreeView, Qt::RightDockWidgetArea );
	addDock( "Assets", m_assetTree->privateInterface<QAssetTree>(), Qt::RightDockWidgetArea );
	addDock( "Hierarchy", m_sceneTree->privateInterface<QSceneTree>(), Qt::LeftDockWidgetArea );
	addDock( "Output", new QTreeView, Qt::LeftDockWidgetArea );

	// Update window caption
	m_private->setWindowTitle( m_project->name().c_str() + QString( " - Dreemchest Composer" ) );
}

// ** MainWindow::onProjectClosed
void MainWindow::onProjectClosed( const Composer::ProjectClosed& e )
{
	m_project = Project::ProjectWPtr();
	DC_BREAK;
}

} // namespace Ui

DC_END_COMPOSER