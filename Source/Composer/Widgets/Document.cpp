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

#include "../Editors/AssetEditor.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ** Document::Document
Document::Document( MainWindowQPtr parent, Editors::AssetEditorQPtr assetEditor, const QString& title )
    : QDockWidget( title, NULL ), m_mainWindow( parent ), m_assetEditor( assetEditor ), m_renderingFrame( NULL )
{
    assetEditor->setParent( this );
	connect( this, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)) );
}

// ** Document::visibilityChanged
void Document::visibilityChanged( bool visible )
{
	if( visible ) {
		m_mainWindow->setActiveDocument( this );
	}
}

// ** Document::closeEvent
void Document::closeEvent( QCloseEvent *e )
{
	// Close the document
	if( !m_mainWindow->closeDocument( this ) ) {
		e->ignore();
	} else {
		// Process the event
//		QDockWidget::closeEvent( e );	
	}
}

// ** Document::renderingFrame
RenderingFrameQPtr Document::renderingFrame( void ) const
{
	return m_renderingFrame;
}

// ** Document::attachRenderingFrame
RenderingFrameQPtr Document::attachRenderingFrame( void )
{
	NIMBLE_BREAK_IF( m_renderingFrame );

	// Get the shared rendering context
	RenderingFrameQPtr sharedContext = m_mainWindow->sharedRenderingContext();

	// Create the rendering frame instance
	m_renderingFrame = new RenderingFrame( sharedContext, this );
	m_renderingFrame->setInterval( 10 );
	m_renderingFrame->makeCurrent();

	// Set the rendering frame as a main widget of a dock widget
	setWidget( m_renderingFrame );

	return m_renderingFrame;
}

// ** Document::assetEditor
Editors::AssetEditorQPtr Document::assetEditor( void ) const
{
	return m_assetEditor;
}

} // namespace Ui

DC_END_COMPOSER
