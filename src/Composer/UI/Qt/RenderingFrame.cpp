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

#include "RenderingFrame.h"

namespace Ui {

// --------------------------------------------- QRenderingFrame --------------------------------------------- //

// ** QRenderingFrame::kOpenGLFormat;
QGLFormat QRenderingFrame::kOpenGLFormat;

// ** QRenderingFrame::QRenderingFrame
QRenderingFrame::QRenderingFrame( const QGLWidget* shareWidget, QWidget *parent ) : QGLWidget( kOpenGLFormat, parent, shareWidget )
{
	setAcceptDrops( true );
	setMouseTracking( true );

//	timer					= -1;
//	isContinuousRendering	= false;
//	updateView				= false;
//	hasLostFocus			= false;

//	if( parent ) {
//		resize( parent->size() );
//	}
}

// ** QRenderingFrame::SetupOpenGLFormat
void QRenderingFrame::setupOpenGLFormat( void )
{
	// Construct the custom OpenGL format
	kOpenGLFormat.setSwapInterval( 1 );
	kOpenGLFormat.setSampleBuffers( true );
	kOpenGLFormat.setSamples( 8 );
    
	// Construct the test QGLWidget
    AutoPtr<QGLWidget> widget = new QGLWidget( kOpenGLFormat );

	// The QGLWidget was successfully initialized - use this format
	if( widget->isValid() ) {
		return;
	}

	// Fallback to default one
    kOpenGLFormat = QGLFormat::defaultFormat();
    kOpenGLFormat.setDirectRendering( false );
}

// ** QRenderingFrame::BeginRendering
//void QRenderingFrame::BeginRendering( int interval )
//{
//	EndRendering();
//	timer = startTimer( interval );
//}

// ** QRenderingFrame::EndRendering
//void QRenderingFrame::EndRendering( void )
//{
//	killTimer( timer );
//	timer = -1;
//}

// ** QRenderingFrame::mousePressEvent
void QRenderingFrame::mousePressEvent( QMouseEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;

	//setFocus();
	//e->SetMouseEvent( 0, event->pos().x(), event->pos().y() );

	//switch( event->button() ) {
	//case Qt::LeftButton:	e->button = ed::sRenderingFrameEvent::LeftButton;	break;
	//case Qt::RightButton:	e->button = ed::sRenderingFrameEvent::RightButton;	break;
	//case Qt::MidButton:		e->button = ed::sRenderingFrameEvent::MiddleButton;	break;
	//}

	//DispatchEvent( ed::sRenderingFrameEvent::TapDown, e );
}

// ** QRenderingFrame::mouseReleaseEvent
void QRenderingFrame::mouseReleaseEvent( QMouseEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;

	//e->SetMouseEvent( 0, event->pos().x(), event->pos().y() );

	//switch( event->button() ) {
	//case Qt::LeftButton:	e->button = ed::sRenderingFrameEvent::LeftButton;	break;
	//case Qt::RightButton:	e->button = ed::sRenderingFrameEvent::RightButton;	break;
	//case Qt::MidButton:		e->button = ed::sRenderingFrameEvent::MiddleButton;	break;
	//}

	//DispatchEvent( ed::sRenderingFrameEvent::TapUp, e );
}

// ** QRenderingFrame::wheelEvent
void QRenderingFrame::wheelEvent( QWheelEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;

	//e->SetWheel( event->delta() );
	//DispatchEvent( ed::sRenderingFrameEvent::Wheel, e );
}

// ** QRenderingFrame::resizeEvent
void QRenderingFrame::resizeEvent( QResizeEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;

	//int w = event->size().width();
	//int h = event->size().height();

	//resizeGL( w, h );
	//e->SetResize( w, h );
	//DispatchEvent( ed::sRenderingFrameEvent::Resize, e );
}

// ** QRenderingFrame::dragEnterEvent
void QRenderingFrame::dragEnterEvent( QDragEnterEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;

	//e->SetDragEvent( event->mimeData(), 0, 0 );
	//e->Ref();
	//DispatchEvent( ed::sRenderingFrameEvent::DragEnter, e );

	//if( !e->cancel ) {
	//	event->acceptProposedAction();
	//}

	//e->Unref();
}

// ** QRenderingFrame::dragMoveEvent
void QRenderingFrame::dragMoveEvent( QDragMoveEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;

	//e->SetDragEvent( event->mimeData(), event->pos().x(), event->pos().y() );
	//DispatchEvent( ed::sRenderingFrameEvent::DragMove, e );
}

// ** QRenderingFrame::dropEvent
void QRenderingFrame::dropEvent( QDropEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;

	//e->SetDragEvent( event->mimeData(), event->pos().x(), event->pos().y() );
	//DispatchEvent( ed::sRenderingFrameEvent::DragDrop, e );
}

// ** QRenderingFrame::contextMenuEvent
void QRenderingFrame::contextMenuEvent( QContextMenuEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
	//QComposerMenu *contextMenu = new QComposerMenu( this );

	//e->SetMouseEvent( 0, event->globalX(), event->globalY() );
	//e->contextMenu = contextMenu;
	//DispatchEvent( ed::sRenderingFrameEvent::ContextMenu, e );

	//contextMenu->Exec( event->globalX(), event->globalY() );
	//delete contextMenu;
}

// ** QRenderingFrame::mouseMoveEvent
void QRenderingFrame::mouseMoveEvent( QMouseEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;

	//e->SetMouseEvent( 0, event->pos().x(), event->pos().y() );
	//e->globalX = event->globalPos().x();
	//e->globalY = event->globalPos().y();
	//DispatchEvent( ed::sRenderingFrameEvent::TapMove, e );
}

// ** QRenderingFrame::keyPressEvent
void QRenderingFrame::keyPressEvent( QKeyEvent *event )
{
	//int k = event->key();

	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
	//e->key = ConvertKey( event->key() );

	//DispatchEvent( ed::sRenderingFrameEvent::KeyDown, e );
}

// ** QRenderingFrame::keyReleaseEvent
void QRenderingFrame::keyReleaseEvent( QKeyEvent *event )
{
	//ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
	//e->key = ConvertKey( event->key() );

	//DispatchEvent( ed::sRenderingFrameEvent::KeyUp, e );
}

// ** QRenderingFrame::timerEvent
void QRenderingFrame::timerEvent( QTimerEvent *event )
{
	//if( isVisible() && (isContinuousRendering || hasFocus() || updateView) ) {
	//	update();
	//}
}

// ** QRenderingFrame::focusInEvent
void QRenderingFrame::focusInEvent( QFocusEvent *event )
{
	QGLWidget::focusInEvent( event );

	//if( hasLostFocus ) {
	//	ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
	//	DispatchEvent( ed::sRenderingFrameEvent::FocusIn, e );
	//	hasLostFocus = false;
	//}
}

// ** QRenderingFrame::focusOutEvent
void QRenderingFrame::focusOutEvent( QFocusEvent *event )
{
	QGLWidget::focusOutEvent( event );

	//if( event->reason() != Qt::PopupFocusReason ) {
	//	ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
	//	DispatchEvent( ed::sRenderingFrameEvent::FocusOut, e );
	//	hasLostFocus = true;
	//}
}

// ** QRenderingFrame::initializeGL
void QRenderingFrame::initializeGL( void )
{
	//qglClearColor( Qt::white );
}

// ** QRenderingFrame::paintGL
void QRenderingFrame::paintGL( void )
{
 //   ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
	//DispatchEvent( ed::sRenderingFrameEvent::Update, e );

	//// ** Workaround for rendering states
	//glEnable( GL_TEXTURE_2D );
}

//// ** QRenderingFrame::OnCopy
//void QRenderingFrame::OnCopy( void )
//{
//    ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
//	DispatchEvent( ed::sRenderingFrameEvent::Copy, e );
//}
//
//// ** QRenderingFrame::OnCut
//void QRenderingFrame::OnCut( void )
//{
//    ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
//	DispatchEvent( ed::sRenderingFrameEvent::Cut, e );
//}
//
//// ** QRenderingFrame::OnPaste
//void QRenderingFrame::OnPaste( void )
//{
//    ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
//	DispatchEvent( ed::sRenderingFrameEvent::Paste, e );
//}
//
//// ** QRenderingFrame::OnRedo
//void QRenderingFrame::OnRedo( void )
//{
//    ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
//	DispatchEvent( ed::sRenderingFrameEvent::Redo, e );
//}
//
//// ** QRenderingFrame::OnUndo
//void QRenderingFrame::OnUndo( void )
//{
//    ed::sRenderingFrameEvent *e = new ed::sRenderingFrameEvent;
//	DispatchEvent( ed::sRenderingFrameEvent::Undo, e );
//}

// ----------------------------------------------- RenderingFrame ----------------------------------------------- //

// ** RenderingFrame::width
s32 RenderingFrame::width( void ) const
{
	return m_private->width();
}

// ** RenderingFrame::height
s32 RenderingFrame::height( void ) const
{
	return m_private->height();
}

// ** RenderingFrame::Update
//void RenderingFrame::Update( void )
//{
//	updateView = true;
//}

// ** RenderingFrame::setFocused
void RenderingFrame::setFocused( void )
{
	m_private->setFocus();
}

// ** RenderingFrame::setCursor
void RenderingFrame::setCursor( const String& cursor, bool centered )
{
	if( cursor == "" ) {
		m_private->setCursor( QCursor( Qt::ArrowCursor ) );
		return;
	}

	QPixmap pixmap( cursor.c_str() );

	if( !centered ) {
		s32	   width	= pixmap.width();
		s32	   height	= pixmap.height();
		QImage srcImage	= pixmap.toImage();
		QImage dstImage	= QImage( width * 2, height * 2, QImage::Format_ARGB32 );

		dstImage.fill( 0 );
		for( int j = 0; j < height; j++ ) {
			for( int i = 0; i < width; i++ ) {
				dstImage.setPixel( width + i, height + j, srcImage.pixel( i, j ) );
			}
		}

		pixmap = QPixmap::fromImage( dstImage );
	}

	m_private->setCursor( QCursor( pixmap ) );
}

// ** RenderingFrame::setContinuousRendering
void RenderingFrame::setContinuousRendering( bool value )
{
	m_isContinuousRendering = value;
}

// ** RenderingFrame::isContinuousRendering
bool RenderingFrame::isContinuousRendering( void ) const
{
	return m_isContinuousRendering;
}

} // namespace Ui