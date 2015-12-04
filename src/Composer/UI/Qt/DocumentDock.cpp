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

#include "DocumentDock.h"

#include "MainWindow.h"
#include "../../Editors/AssetEditor.h"

namespace Ui {

// ------------------------------------------- QDocumentDock ------------------------------------------- //

// ** QDocumentDock::QDocumentDock
QDocumentDock::QDocumentDock( DocumentDock* document, const QString& title, QWidget* parent ) : QDockWidget( title, parent ), m_document( document )
{
	connect( this, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)) );
}

// ** QDocumentDock::visibilityChanged
void QDocumentDock::visibilityChanged( bool visible )
{
	if( visible ) {
		m_document->activateDocument();
	}
}

// ** QDocumentDock::closeEvent
void QDocumentDock::closeEvent( QCloseEvent *e )
{
	// Close the document
	if( !m_document->closeDocument() ) {
		e->ignore();
	} else {
		// Process the event
//		QDockWidget::closeEvent( e );	
	}
}

// ------------------------------------------- DocumentDock ------------------------------------------- //

// ** DocumentDock::DocumentDock
DocumentDock::DocumentDock( IMainWindowWPtr mainWindow, Editors::AssetEditorPtr assetEditor, const String& title, QWidget* parent )
	: UserInterface( new QDocumentDock( this, title.c_str(), parent ) ), m_mainWindow( mainWindow ), m_assetEditor( assetEditor )
{

}

// ** DocumentDock::renderingFrame
IRenderingFrameWPtr DocumentDock::renderingFrame( void )
{
	DC_NOT_IMPLEMENTED
}

// ** DocumentDock::setRenderingFrame
void DocumentDock::setRenderingFrame( IRenderingFramePtr value )
{
	DC_NOT_IMPLEMENTED
}

// ** DocumentDock::assetEditor
Editors::AssetEditorWPtr DocumentDock::assetEditor( void ) const
{
	return m_assetEditor;
}

// ** DocumentDock::activateDocument
void DocumentDock::activateDocument( void )
{
	m_mainWindow->setActiveDocument( this );
}

// ** DocumentDock::closeDocument
bool DocumentDock::closeDocument( void )
{
	return m_mainWindow->closeDocument( this );
}

} // namespace Ui