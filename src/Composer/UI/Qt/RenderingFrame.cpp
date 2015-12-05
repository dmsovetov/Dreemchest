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
#include "Menu.h"

namespace Ui {

// --------------------------------------------- QRenderingFrame --------------------------------------------- //

// ** QRenderingFrame::kOpenGLFormat;
QGLFormat QRenderingFrame::kOpenGLFormat;

// ** QRenderingFrame::QRenderingFrame
QRenderingFrame::QRenderingFrame( RenderingFrame* parentRenderingFrame, const QGLWidget* sharedWidget, QWidget* parent )
	: QGLWidget( kOpenGLFormat, parent, sharedWidget ), m_parent( parentRenderingFrame ), m_timer( -1 ), m_hasLostFocus( false )
{
	setAcceptDrops( true );
	setMouseTracking( true );

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

// ** QRenderingFrame::setInterval
void QRenderingFrame::setInterval( s32 value )
{
	killTimer( m_timer );
	m_timer = startTimer( value );
}

// ** QRenderingFrame::EndRendering
//void QRenderingFrame::EndRendering( void )
//{
//	killTimer( timer );
//	timer = -1;
//}

// ** QRenderingFrame::mousePressEvent
void QRenderingFrame::mousePressEvent( QMouseEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Set the focus to this widget
	setFocus();

	// Get the mouse button from event
	MouseButton button;

	switch( e->button() ) {
	case Qt::LeftButton:	button = LeftMouseButton;	break;
	case Qt::RightButton:	button = RightMouseButton;	break;
	case Qt::MidButton:		button = MiddleMouseButton;	break;	
	}

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleMousePress( e->pos().x(), e->pos().y(), button );
	}
}

// ** QRenderingFrame::mouseReleaseEvent
void QRenderingFrame::mouseReleaseEvent( QMouseEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Get the mouse button from event
	MouseButton button;

	switch( e->button() ) {
	case Qt::LeftButton:	button = LeftMouseButton;	break;
	case Qt::RightButton:	button = RightMouseButton;	break;
	case Qt::MidButton:		button = MiddleMouseButton;	break;	
	}

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleMouseRelease( e->pos().x(), e->pos().y(), button );
	}
}

// ** QRenderingFrame::wheelEvent
void QRenderingFrame::wheelEvent( QWheelEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleMouseWheel( e->delta() );
	}
}

// ** QRenderingFrame::resizeEvent
void QRenderingFrame::resizeEvent( QResizeEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Get the new size
	s32 w = e->size().width();
	s32 h = e->size().height();

	// Process the resize by QGLWidget
	resizeGL( w, h );

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleResize( w, h );
	}
}

// ** QRenderingFrame::dragEnterEvent
void QRenderingFrame::dragEnterEvent( QDragEnterEvent* e )
{
	//e->mimeData()
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	if( !delegate.valid() ) {
		return;
	}

	if( delegate->handleDragEnter() ) {
		e->acceptProposedAction();
	}
}

// ** QRenderingFrame::dragMoveEvent
void QRenderingFrame::dragMoveEvent( QDragMoveEvent* e )
{
	//e->mimeData()
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	if( delegate.valid() ) {
		delegate->handleDragMove( e->pos().x(), e->pos().y() );
	}
}

// ** QRenderingFrame::dropEvent
void QRenderingFrame::dropEvent( QDropEvent* e )
{
	//e->mimeData()
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	if( delegate.valid() ) {
		delegate->handleDrop( e->pos().x(), e->pos().y() );
	}
}

// ** QRenderingFrame::contextMenuEvent
void QRenderingFrame::contextMenuEvent( QContextMenuEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	if( !delegate.valid() ) {
		return;
	}

	// Populate the menu by delegate
	IMenuPtr menu( new Menu( this ) );
    delegate->handleContextMenu( menu );

	// Skip empty menues
	if( !menu->size() ) {
		return;
	}

	// Show the context menu
    menu->exec( e->globalPos().x(), e->globalPos().y() );
}

// ** QRenderingFrame::mouseMoveEvent
void QRenderingFrame::mouseMoveEvent( QMouseEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleMouseMove( e->pos().x(), e->pos().y(), e->globalPos().x(), e->globalPos().y() );
	}
}

// ** QRenderingFrame::keyPressEvent
void QRenderingFrame::keyPressEvent( QKeyEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleKeyPress( convertKey( e->key() ) );
	}
}

// ** QRenderingFrame::keyReleaseEvent
void QRenderingFrame::keyReleaseEvent( QKeyEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleKeyRelease( convertKey( e->key() ) );
	}
}

// ** QRenderingFrame::timerEvent
void QRenderingFrame::timerEvent( QTimerEvent *event )
{
	bool shouldUpdate = m_parent->isContinuousRendering() || hasFocus();

	if( isVisible() && shouldUpdate ) {
		update();
	}
}

// ** QRenderingFrame::focusInEvent
void QRenderingFrame::focusInEvent( QFocusEvent* e )
{
	QGLWidget::focusInEvent( e );

	if( !m_hasLostFocus ) {
		return;
	}

	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleFocusIn();
	}

	m_hasLostFocus = false;
}

// ** QRenderingFrame::focusOutEvent
void QRenderingFrame::focusOutEvent( QFocusEvent* e )
{
	QGLWidget::focusOutEvent( e );

	if( e->reason() == Qt::PopupFocusReason ) {
		return;
	}

	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleFocusOut();
	}

	m_hasLostFocus = true;
}

// ** QRenderingFrame::initializeGL
void QRenderingFrame::initializeGL( void )
{

}

// ** QRenderingFrame::paintGL
void QRenderingFrame::paintGL( void )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	if( delegate.valid() ) {
		delegate->handleUpdate();
	}
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

// ** RenderingFrame::RenderingFrame
RenderingFrame::RenderingFrame( const QGLWidget* sharedWidget, QWidget* parent ) : UserInterface( new QRenderingFrame( this, sharedWidget, parent ) )
{

}

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

// ** RenderingFrame::setInterval
void RenderingFrame::setInterval( s32 value )
{
	m_private->setInterval( value );
}

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

// ** RenderingFrame::setDelegate
void RenderingFrame::setDelegate( IRenderingFrameDelegateWPtr value )
{
	m_delegate = value;
}

// ** RenderingFrame::delegate
IRenderingFrameDelegateWPtr RenderingFrame::delegate( void ) const
{
	return m_delegate;
}

} // namespace Ui