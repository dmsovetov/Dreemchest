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

#include "Document.h"

#include "MainWindow.h"
#include "RenderingFrame.h"
#include "../../Editors/AssetEditor.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ------------------------------------------- QDocumentDock ------------------------------------------- //

// ** QDocumentDock::QDocumentDock
QDocumentDock::QDocumentDock( Document* document, const QString& title, QWidget* parent ) : QDockWidget( title, parent ), m_document( document )
{
	connect( this, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)) );
}

// ** QDocumentDock::visibilityChanged
void QDocumentDock::visibilityChanged( bool visible )
{
	if( visible ) {
		m_document->activate();
	}
}

// ** QDocumentDock::closeEvent
void QDocumentDock::closeEvent( QCloseEvent *e )
{
	// Close the document
	if( !m_document->close() ) {
		e->ignore();
	} else {
		// Process the event
//		QDockWidget::closeEvent( e );	
	}
}

// ---------------------------------------------- Document ---------------------------------------------- //

// ** Document::Document
Document::Document( IMainWindowWPtr mainWindow, Editors::AssetEditorPtr assetEditor, const String& title, QWidget* parent )
	: PrivateInterface( new QDocumentDock( this, title.c_str(), parent ) ), m_mainWindow( mainWindow ), m_assetEditor( assetEditor )
{

}

// ** Document::renderingFrame
IRenderingFrameWPtr Document::renderingFrame( void )
{
	return m_renderingFrame;
}

// ** Document::attachRenderingFrame
IRenderingFrameWPtr Document::attachRenderingFrame( void )
{
	DC_BREAK_IF( m_renderingFrame.valid() );

	// Get the shared rendering context
	IRenderingFrameWPtr sharedContext = m_mainWindow->sharedRenderingContext();

	// Create the rendering frame instance
	m_renderingFrame = new RenderingFrame( sharedContext->privateInterface<QRenderingFrame>(), m_private.get() );
	m_renderingFrame->setInterval( 10 );
	m_renderingFrame->privateInterface<QRenderingFrame>()->makeCurrent();

	// Set the rendering frame as a main widget of a dock widget
	m_private->setWidget( m_renderingFrame->privateInterface<QRenderingFrame>() );

	return m_renderingFrame;
}

// ** Document::assetEditor
Editors::AssetEditorWPtr Document::assetEditor( void ) const
{
	return m_assetEditor;
}

// ** Document::activate
void Document::activate( void )
{
	m_mainWindow->setActiveDocument( this );
}

// ** Document::close
bool Document::close( void )
{
	return m_mainWindow->closeDocument( this );
}

} // namespace Ui

DC_END_COMPOSER