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

DC_BEGIN_COMPOSER

namespace Ui {

// ** RenderingFrame::kOpenGLFormat;
QGLFormat RenderingFrame::kOpenGLFormat;

// ** RenderingFrame::RenderingFrame
RenderingFrame::RenderingFrame( const QGLWidget* sharedWidget, QWidget* parent )
	: QGLWidget( kOpenGLFormat, parent, sharedWidget ), m_timer( -1 ), m_hasLostFocus( false )
{
	setAcceptDrops( true );
	setMouseTracking( true );
	setMinimumSize( 1, 1 );
}

// ** RenderingFrame::SetupOpenGLFormat
void RenderingFrame::setupOpenGLFormat( void )
{
	// Construct the custom OpenGL format
	kOpenGLFormat.setSwapInterval( 1 );
	kOpenGLFormat.setSamples( 8 );
    
	// Construct the test OpenGL widget
    AutoPtr<QGLWidget> widget = new QGLWidget( kOpenGLFormat );

	// The widget was successfully initialized - use this format
	if( widget->isValid() ) {
		return;
	}

	// Fallback to default one
    kOpenGLFormat = QGLFormat::defaultFormat();
    kOpenGLFormat.setDirectRendering( false );
}

// ** RenderingFrame::setInterval
void RenderingFrame::setInterval( s32 value )
{
	if( m_timer != -1 ) {
		killTimer( m_timer );
	}

	m_timer = startTimer( value );
	m_deltaTime.start();
}

// ** RenderingFrame::mousePressEvent
void RenderingFrame::mousePressEvent( QMouseEvent* e )
{
	// Set the focus to this widget
	setFocus();

	// Get the mouse button from event
	u8 button;

	switch( e->button() ) {
	case Qt::LeftButton:	button = LeftMouseButton;	break;
	case Qt::RightButton:	button = RightMouseButton;	break;
	case Qt::MidButton:		button = MiddleMouseButton;	break;	
	}

	// Notify the delegate
	if( m_delegate.valid() ) {
		m_delegate->handleMousePress( e->pos().x(), e->pos().y(), button );
	}

	// Save current cursor position
	m_lastCursorPos = e->pos();

	// Enable mouse down flag
	m_isMouseButtons.on( button );
}

// ** RenderingFrame::mouseReleaseEvent
void RenderingFrame::mouseReleaseEvent( QMouseEvent* e )
{
	// Get the mouse button from event
	u8 button;

	switch( e->button() ) {
	case Qt::LeftButton:	button = LeftMouseButton;	break;
	case Qt::RightButton:	button = RightMouseButton;	break;
	case Qt::MidButton:		button = MiddleMouseButton;	break;	
	}

	// Notify the delegate
	if( m_delegate.valid() ) {
		m_delegate->handleMouseRelease( e->pos().x(), e->pos().y(), button );
	}

	// Disable mouse down flag
	m_isMouseButtons.off( button );
}

// ** RenderingFrame::wheelEvent
void RenderingFrame::wheelEvent( QWheelEvent* e )
{
	// Notify the delegate
	if( m_delegate.valid() ) {
		m_delegate->handleMouseWheel( e->delta() );
	}
}

// ** RenderingFrame::resizeEvent
void RenderingFrame::resizeEvent( QResizeEvent* e )
{
	// Get the new size
	s32 w = e->size().width();
	s32 h = e->size().height();

	// Process the resize by QGLWidget
	resizeGL( w, h );

	// Notify the delegate
	if( m_delegate.valid() ) {
		m_delegate->handleResize( w, h );
	}
}

// ** RenderingFrame::dragEnterEvent
void RenderingFrame::dragEnterEvent( QDragEnterEvent* e )
{
	if( !m_delegate.valid() ) {
		return;
	}

	if( m_delegate->handleDragEnter( e->mimeData() ) ) {
		e->acceptProposedAction();
	}
}

// ** RenderingFrame::dragMoveEvent
void RenderingFrame::dragMoveEvent( QDragMoveEvent* e )
{
	if( !m_delegate.valid() ) {
		return;
	}

	m_delegate->handleDragMove( e->mimeData(), e->pos().x(), e->pos().y() );
}

// ** RenderingFrame::dropEvent
void RenderingFrame::dropEvent( QDropEvent* e )
{
	if( !m_delegate.valid() ) {
		return;
	}

	m_delegate->handleDrop( e->mimeData(), e->pos().x(), e->pos().y() );
}

// ** RenderingFrame::contextMenuEvent
void RenderingFrame::contextMenuEvent( QContextMenuEvent* e )
{
	if( !m_delegate.valid() ) {
		return;
	}

	// Populate the menu by delegate
	MenuQPtr menu = new Menu( this );
    m_delegate->handleContextMenu( menu );

	// Skip empty menues
	if( !menu->size() ) {
		return;
	}

	// Show the context menu
    menu->exec( e->globalPos() );
}

// ** RenderingFrame::mouseMoveEvent
void RenderingFrame::mouseMoveEvent( QMouseEvent* e )
{
	// Calculate mouse movement delta
	QPoint delta = e->pos() - m_lastCursorPos;
	m_lastCursorPos = e->pos();

	// Notify the delegate
	if( m_delegate.valid() ) {
		m_delegate->handleMouseMove( e->pos().x(), e->pos().y(), delta.x(), delta.y(), m_isMouseButtons );
	}
}

// ** RenderingFrame::keyPressEvent
void RenderingFrame::keyPressEvent( QKeyEvent* e )
{
	// Notify the delegate
	if( m_delegate.valid() ) {
		m_delegate->handleKeyPress( convertKey( e->key() ) );
	}
}

// ** RenderingFrame::keyReleaseEvent
void RenderingFrame::keyReleaseEvent( QKeyEvent* e )
{
	// Notify the delegate
	if( m_delegate.valid() ) {
		m_delegate->handleKeyRelease( convertKey( e->key() ) );
	}
}

// ** RenderingFrame::timerEvent
void RenderingFrame::timerEvent( QTimerEvent *event )
{
	bool shouldUpdate = isContinuousRendering() || hasFocus();

	if( isVisible() && shouldUpdate ) {
		update();
	}
}

// ** RenderingFrame::focusInEvent
void RenderingFrame::focusInEvent( QFocusEvent* e )
{
	QGLWidget::focusInEvent( e );

	if( !m_hasLostFocus ) {
		return;
	}

	// Notify the delegate
	if( m_delegate.valid() ) {
		m_delegate->handleFocusIn();
	}

	m_hasLostFocus = false;
}

// ** RenderingFrame::focusOutEvent
void RenderingFrame::focusOutEvent( QFocusEvent* e )
{
	QGLWidget::focusOutEvent( e );

	if( e->reason() == Qt::PopupFocusReason ) {
		return;
	}

	// Notify the delegate
	if( m_delegate.valid() ) {
		m_delegate->handleFocusOut();
	}

	m_hasLostFocus = true;
}

// ** RenderingFrame::initializeGL
void RenderingFrame::initializeGL( void )
{

}

// ** RenderingFrame::paintGL
void RenderingFrame::paintGL( void )
{
	// Get the delta time
	f32 dt = m_deltaTime.elapsed() * 0.001f;
	m_deltaTime.restart();

	if( m_delegate.valid() ) {
		m_delegate->handleUpdate( dt );
	}
}

// ** RenderingFrame::setCursor
void RenderingFrame::setCursor( const String& cursor, bool centered )
{
	if( cursor == "" ) {
		QGLWidget::setCursor( QCursor( Qt::ArrowCursor ) );
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

	QGLWidget::setCursor( QCursor( pixmap ) );
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

DC_END_COMPOSER