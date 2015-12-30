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

#include "AssetTree.h"
#include "Document.h"
#include "SceneTree.h"
#include "Inspector.h"
#include "Menu.h"
#include "RenderingFrame.h"

#include "../FileSystem.h"
#include "../Project/Project.h"
#include "../Editors/AssetEditor.h"

#if DEV_USE_DOCK_INDICATOR
	#include "DockIndicator.h"
#endif	/*	DEV_USE_DOCK_INDICATOR	*/

DC_BEGIN_COMPOSER

namespace Ui {

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
MainWindow::MainWindow( const String& title ) : m_assetTree( NULL ), m_activeDocument( NULL ), m_sceneTree( NULL ), m_inspector( NULL )
{
#if DEV_USE_DOCK_INDICATOR
	// Add the dock indicator widget
	new DockIndicator( this );
#endif	/*	DEV_USE_DOCK_INDICATOR	*/

	// Setup the share OpenGL format
	RenderingFrame::setupOpenGLFormat();

	// Create the shared OpenGL context
	m_sharedRenderingContext = new RenderingFrame( NULL, this );
	m_sharedRenderingContext->hide();
	//setCentralWidget( m_sharedRenderingContext->privateInterface<QRenderingFrame>() );

#ifdef NDEBUG
	//setWindowState( Qt::WindowMaximized );
	resize( 1024, 768 );
#else
	resize( 1024, 768 );
#endif

    setUnifiedTitleAndToolBarOnMac( true );
	show();
	setWindowTitle( title.c_str() );
}

// ** MainWindow::initialize
bool MainWindow::initialize( ComposerWPtr composer )
{
	// Create the file system
	m_fileSystem = new FileSystem( this );

	// Listen for events
	composer->subscribe<Composer::ProjectOpened>( dcThisMethod( MainWindow::onProjectOpened ) );
	composer->subscribe<Composer::ProjectClosed>( dcThisMethod( MainWindow::onProjectClosed ) );

	return true;
}

// ** MainWindow::message
void MainWindow::message( const String& text, MessageStatus status ) const
{
	switch( status ) {
	case MessageInfo:		QMessageBox::information( const_cast<MainWindow*>( this ), windowTitle(), text.c_str() );	break;
	case MessageWarning:	QMessageBox::warning( const_cast<MainWindow*>( this ), windowTitle(), text.c_str() );		break;
	case MessageError:		QMessageBox::critical( const_cast<MainWindow*>( this ), windowTitle(), text.c_str() );		break;
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
RenderingFrameQPtr MainWindow::sharedRenderingContext( void ) const
{
	return m_sharedRenderingContext;
}

// ** MainWindow::fileSystem
FileSystemQPtr MainWindow::fileSystem( void ) const
{
	return m_fileSystem;
}

// ** MainWindow::assetTree
AssetTreeQPtr MainWindow::assetTree( void ) const
{
	return m_assetTree;
}

// ** MainWindow::sceneTree
SceneTreeQPtr MainWindow::sceneTree( void ) const
{
	return m_sceneTree;
}

// ** MainWindow::inspector
InspectorQPtr MainWindow::inspector( void ) const
{
	return m_inspector;
}

// ** MainWindow::addMenu
MenuQPtr MainWindow::addMenu( const String& title )
{
	QMenuBar* bar  = menuBar();
	Menu*	  menu = new Menu( bar );

	menu->setTitle( QString::fromStdString( title ) );
	bar->addMenu( menu );

	return menu;
}

// ** MainWindow::addToolBar
ToolBarQPtr MainWindow::addToolBar( void )
{
	ToolBar* toolBar = new ToolBar( this );
	QMainWindow::addToolBar( toolBar );

	return toolBar;
}

// ** MainWindow::addDock
QDockWidget* MainWindow::addDock( const QString& name, QWidget* widget, Qt::DockWidgetArea initialDockArea, Qt::DockWidgetAreas allowedDockAreas, QDockWidget* destination )
{
	QDockWidget* dock = new QDockWidget( name );
	dock->setAllowedAreas( allowedDockAreas );
	dock->setWidget( widget );
	addDockWidget( initialDockArea, dock );

	if( destination ) {
		tabifyDockWidget( dock, destination );
	}

	return dock;
}

// ** MainWindow::editDocument
DocumentQPtr MainWindow::editDocument( Editors::AssetEditorWPtr assetEditor, const Scene::AssetPtr& asset )
{
	DC_BREAK_IF( !assetEditor.valid() );

	// First lookup the exising document
	DocumentQPtr existing = findDocument( asset );

	if( existing ) {
		existing->raise();
		return existing;
	}

	// Create the document instance
	DocumentQPtr document = new Document( this, assetEditor, QString::fromStdString( asset->name() ) );

	// Initialize the asset editor with a document
	if( !assetEditor->initialize( m_project, asset, document ) ) {
        delete document;
		return NULL;
	}

	addDockWidget( Qt::LeftDockWidgetArea, document );

	// Find opened documents of a same type
	QVector<DocumentQPtr> documents = findDocuments( asset );

	if( documents.size() ) {
		QDockWidget* tabifyTo = documents[0];
		tabifyDockWidget( tabifyTo, document );
	}

	// Set the document as active
	setActiveDocument( document );

	// Save created document
	m_documents.append( document );

	return document;
}

// ** MainWindow::closeDocument
bool MainWindow::closeDocument( DocumentQPtr document )
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
	setActiveDocument( (m_documents.size() - 1) ? m_documents[index - 1] : NULL );

	// Remove the document dock widget
	removeDockWidget( document );

	// Remove from documents
	m_documents.remove( index );

    // Delete document
    delete document;

	return true;
}

// ** MainWindow::findDocument
DocumentQPtr MainWindow::findDocument( const Scene::AssetWPtr& asset ) const
{
	foreach( DocumentQPtr document, m_documents ) {
		if( document->assetEditor()->asset() == asset ) {
			return document;
		}
	}

	return NULL;
}

// ** MainWindow::findDocuments
QVector<DocumentQPtr> MainWindow::findDocuments( const Scene::AssetWPtr& asset ) const
{
	QVector<DocumentQPtr> documents;

	foreach( DocumentQPtr document, m_documents ) {
		if( document->assetEditor()->asset()->type() == asset->type() ) {
			documents.append( document );
		}
	}

	return documents;
}

// ** MainWindow::setActiveDocument
void MainWindow::setActiveDocument( DocumentQPtr document )
{
	// This document is already set as active
	if( m_activeDocument == document ) {
		return;
	}

	// Notify the active document about moving to background.
	if( m_activeDocument ) {
		m_activeDocument->assetEditor()->notifyEnterBackground( this );
	}

	// Set new active document
	m_activeDocument = document;

	// Notify active document about moving to foreground.
	if( m_activeDocument ) {
		// Raise the document dock widget
		m_activeDocument->raise();

		// Notify the asset editor
		m_activeDocument->assetEditor()->notifyEnterForeground( this );
	}
}

// ** MainWindow::ensureSaved
bool MainWindow::ensureSaved( DocumentQPtr document ) const
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
    DC_BREAK_IF( m_assetTree );
    DC_BREAK_IF( m_sceneTree );
    DC_BREAK_IF( m_inspector );

	// Get the project from event
	m_project = e.project;

	// Create the asset tree
	m_assetTree = new AssetTree( m_project, this );
	m_assetTree->setModel( m_project->assetsModel() );

	// Create the scene tree
	m_sceneTree = new SceneTree( this );

	// Create the object inspector
	m_inspector = new Inspector( this );

	// Setup status bar
	statusBar()->show();

	// Add dock windows
	addDock( "Inspector", m_inspector, Qt::RightDockWidgetArea );
	addDock( "Assets", m_assetTree, Qt::RightDockWidgetArea );
	addDock( "Hierarchy", m_sceneTree, Qt::LeftDockWidgetArea );
	addDock( "Output", new QTreeView, Qt::LeftDockWidgetArea );

	// Update window caption
	setWindowTitle( m_project->name().c_str() + QString( " - Dreemchest Composer" ) );
}

// ** MainWindow::onProjectClosed
void MainWindow::onProjectClosed( const Composer::ProjectClosed& e )
{
	m_project = Project::ProjectWPtr();
	DC_BREAK;
}

} // namespace Ui

DC_END_COMPOSER