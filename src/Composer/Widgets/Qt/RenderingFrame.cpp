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
#include "MimeData.h"

DC_BEGIN_COMPOSER

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
	setMinimumSize( 1, 1 );
}

// ** QRenderingFrame::SetupOpenGLFormat
void QRenderingFrame::setupOpenGLFormat( void )
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

// ** QRenderingFrame::setInterval
void QRenderingFrame::setInterval( s32 value )
{
	if( m_timer != -1 ) {
		killTimer( m_timer );
	}

	m_timer = startTimer( value );
	m_deltaTime.start();
}

// ** QRenderingFrame::mousePressEvent
void QRenderingFrame::mousePressEvent( QMouseEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

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
	if( delegate.valid() ) {
		delegate->handleMousePress( e->pos().x(), e->pos().y(), button );
	}

	// Save current cursor position
	m_lastCursorPos = e->pos();

	// Enable mouse down flag
	m_isMouseButtons.on( button );
}

// ** QRenderingFrame::mouseReleaseEvent
void QRenderingFrame::mouseReleaseEvent( QMouseEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Get the mouse button from event
	u8 button;

	switch( e->button() ) {
	case Qt::LeftButton:	button = LeftMouseButton;	break;
	case Qt::RightButton:	button = RightMouseButton;	break;
	case Qt::MidButton:		button = MiddleMouseButton;	break;	
	}

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleMouseRelease( e->pos().x(), e->pos().y(), button );
	}

	// Disable mouse down flag
	m_isMouseButtons.off( button );
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
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	if( !delegate.valid() ) {
		return;
	}

	const QComposerMime* mime = qobject_cast<const QComposerMime*>( e->mimeData() );

	if( mime && delegate->handleDragEnter( mime->mime() ) ) {
		e->acceptProposedAction();
	}
}

// ** QRenderingFrame::dragMoveEvent
void QRenderingFrame::dragMoveEvent( QDragMoveEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	if( !delegate.valid() ) {
		return;
	}

	const QComposerMime* mime = qobject_cast<const QComposerMime*>( e->mimeData() );

	if( mime ) {
		delegate->handleDragMove( mime->mime(), e->pos().x(), e->pos().y() );
	}
}

// ** QRenderingFrame::dropEvent
void QRenderingFrame::dropEvent( QDropEvent* e )
{
	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	if( !delegate.valid() ) {
		return;
	}

	const QComposerMime* mime = qobject_cast<const QComposerMime*>( e->mimeData() );

	if( mime ) {
		delegate->handleDrop( mime->mime(), e->pos().x(), e->pos().y() );
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
	// No mouse button pressed - return
	if( !m_isMouseButtons ) {
		return;
	}

	IRenderingFrameDelegateWPtr delegate = m_parent->delegate();

	// Calculate mouse movement delta
	QPoint delta = e->pos() - m_lastCursorPos;
	m_lastCursorPos = e->pos();

	// Notify the delegate
	if( delegate.valid() ) {
		delegate->handleMouseMove( e->pos().x(), e->pos().y(), delta.x(), delta.y(), m_isMouseButtons );
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

	// Get the delta time
	f32 dt = m_deltaTime.elapsed() * 0.001f;
	m_deltaTime.restart();

	if( delegate.valid() ) {
		delegate->handleUpdate( dt );
	}
}

// ----------------------------------------------- RenderingFrame ----------------------------------------------- //

// ** RenderingFrame::RenderingFrame
RenderingFrame::RenderingFrame( const QGLWidget* sharedWidget, QWidget* parent ) : PrivateInterface( new QRenderingFrame( this, sharedWidget, parent ) )
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

// ** RenderingFrame::makeCurrent
void RenderingFrame::makeCurrent( void )
{
	m_private->makeCurrent();
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