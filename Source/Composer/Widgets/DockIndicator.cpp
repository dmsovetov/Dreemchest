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

#include "DockIndicator.h"
#include "DockIndicatorImage.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ** DockIndicator::DockIndicator
DockIndicator::DockIndicator( QMainWindow* window, const QColor& color, int transparency )
	: QWidget( window, Qt::Widget ), m_color( color ), m_transparency( transparency ), m_window( window ), m_grabbed( NULL ), m_underlying( NULL )
{
	// Install the event filter
	m_window->installEventFilter( this );

	// Create indicator image
	m_indicator = QImage( IndicatorImage );
}

// ** DockIndicator::indicatorCenter
QPoint DockIndicator::indicatorCenter( void ) const
{
	Q_ASSERT( m_underlying != NULL );
	return m_underlying->pos() + QPoint( m_underlying->width() / 2, m_underlying->height() / 2 );
}

// ** DockIndicator::begin
void DockIndicator::begin( QDockWidget* dock )
{
	Q_ASSERT( dock );
	Q_ASSERT( m_grabbed == NULL );

	// Set the widget
	m_grabbed = dock;

	// Show an resize the indicator
	show();
	resize( m_window->size() );

	// Render the dock widget to image
	m_dock = widgetToImage( m_grabbed, QPainter::CompositionMode_DestinationOut ); 
}

// ** DockIndicator::end
void DockIndicator::end( QDockWidget* dock )
{
	Q_ASSERT( m_grabbed != NULL );
	Q_ASSERT( m_grabbed == dock );

	// Place the dock widget to an underlying dock.
	if( m_underlying ) {
		int pane = activePane();

		if( pane ) {
			placeToPane( dock, m_underlying, activePane() );
		}
	}

	// Hide the indicator
	hide();

	// Destroy the dock image
	m_dock = QImage();

	// Set grabbed widget to NULL
	m_grabbed = NULL;
}

// ** DockIndicator::widgetToImage
QImage DockIndicator::widgetToImage( QWidget* widget, QPainter::CompositionMode mode ) const
{
	// Render the dock widget to image
#ifdef DC_QT4_ENABLED
	QImage im = QPixmap::grabWidget( widget ).toImage();
#elif DC_QT5_ENABLED
	QImage im = widget->grab().toImage();
#endif	/*	DC_QT4_ENABLED	*/

	// Construct the destination image
	QImage result = QImage( im.size(), QImage::Format_ARGB32_Premultiplied );

	// Pain dock widget image with blending.
	QPainter p( &result );
	p.drawImage( QPoint(), im );
	p.setBrush( QColor( 0, 0, 0, 100 ) );
	p.setCompositionMode( mode );
	p.drawRect( im.rect() );
	p.end();

	return result;
}

// ** DockIndicator::activePaneRect
QRect DockIndicator::activePaneRect( void ) const
{
	Q_ASSERT( m_underlying != NULL );

	QRect rect = m_underlying->rect();
	rect.moveTo( m_underlying->pos() );

	int pane = activePane();

	switch( pane ) {
	case Qt::AllDockWidgetAreas:	return rect;
	case Qt::TopDockWidgetArea:		return rect.adjusted( 0, 0, 0, -rect.height() / 2 );
	case Qt::BottomDockWidgetArea:	return rect.adjusted( 0, rect.height() / 2, 0, 0 );
	case Qt::LeftDockWidgetArea:	return rect.adjusted( 0, 0, -rect.width() / 2, 0 );
	case Qt::RightDockWidgetArea:	return rect.adjusted( rect.width() / 2, 0, 0, 0 );
	}

	return QRect();
}

// ** DockIndicator::activePane
int DockIndicator::activePane( void ) const
{
	if( !m_underlying ) {
		return 0;
	}

	QPoint offset = m_cursor - indicatorCenter();
	
	if ( QRect( -Radius, -Radius, Radius*2, Radius*2 ).contains( offset ) )
		return Qt::AllDockWidgetAreas;
	else if ( QRect( -Radius, -Radius*3, Radius*2, Radius*2 ).contains( offset ) )
		return Qt::TopDockWidgetArea;
	else if ( QRect( -Radius, Radius, Radius*2, Radius*2 ).contains( offset ) )
		return Qt::BottomDockWidgetArea;
	else if ( QRect( -Radius*3, -Radius, Radius*2, Radius*2 ).contains( offset ) )
		return Qt::LeftDockWidgetArea;
	else if ( QRect( Radius, -Radius, Radius*2, Radius*2 ).contains( offset ) )
		return Qt::RightDockWidgetArea;

	return 0;
}

class QRenderingFrame;

// ** DockIndicator::update
void DockIndicator::update( const QPoint& cursor )
{
	if( !m_grabbed ) {
		return;
	}

	m_underlying	= findDockAtPoint( cursor );
	m_cursor		= m_window->mapFromGlobal( cursor );

	QWidget::update();
}

// ** DockIndicator::paintEvent
void DockIndicator::paintEvent( QPaintEvent* e )
{
	QPainter p( this );

	if( !m_underlying ) {
		return;
	}

	QPoint offset = QPoint( m_indicator.width() / 2, m_indicator.height() / 2 );

	p.save();

	QPen pen( m_color.dark( 120 ) );

	p.setPen( pen );
	p.setBrush( QColor( m_color.red(), m_color.green(), m_color.blue(), m_transparency ) );
	p.drawRect( activePaneRect() );

	p.drawImage( m_cursor, m_dock );
	p.drawImage( indicatorCenter() - offset, m_indicator );
	p.restore();
}

// ** DockIndicator::eventFilter
bool DockIndicator::eventFilter( QObject* sender, QEvent* e )
{
	// Get the event type
	QEvent::Type type = e->type();

	// Process an event from a dock widget
	if( QDockWidget* dock = qobject_cast<QDockWidget*>( sender ) ) {
        // Type cast the mouse event
        QMouseEvent* me = static_cast<QMouseEvent*>( e );

        if( me->button() != Qt::LeftButton ) {
            return false;
        }

        // Process the mouse event
		switch( type ) {
		case QEvent::MouseButtonPress:		begin( dock );	return true;
		case QEvent::MouseButtonRelease:	end( dock );	return false;
		}
	}

	// Process an event from a main window
	if( QMainWindow* window = qobject_cast<QMainWindow*>( sender ) ) {
		switch( type ) {
		case QEvent::ChildAdded:	raise();

									if( QDockWidget* dock = qobject_cast<QDockWidget*>( static_cast<QChildEvent*>( e )->child() ) ) {
										dock->installEventFilter( this );
									}
									break;

		case QEvent::ChildRemoved:	if( QDockWidget* dock = qobject_cast<QDockWidget*>( static_cast<QChildEvent*>( e )->child() ) ) {
										Q_ASSERT( false );
									}

		case QEvent::MouseMove:		update( static_cast<QMouseEvent*>( e )->globalPos() );
									break;
		}
	}

	return false;
}

// ** DockIndicator::findDockAtPoint
QDockWidget* DockIndicator::findDockAtPoint( const QPoint& point ) const
{
	// Get the lost of all dock widgets
	QList<QDockWidget*> dockWidgets = m_window->findChildren<QDockWidget*>();

	// For each dock widget in list
	foreach( QDockWidget* dock, dockWidgets ) {
		// Skip the dock widget being grabbed
		if( dock == m_grabbed ) {
			continue;
		}

		// Check if the point is inside the dock widget
		if( dock->rect().contains( dock->mapFromGlobal( point ) ) ) {
			return dock;
		}
	}

	// Nothing found
	return NULL;
}

// ** DockIndicator::placeToPane
void DockIndicator::placeToPane( QDockWidget* dock, QDockWidget* destination, int pane )
{
	Q_ASSERT( pane );
	Q_ASSERT( dock );
	Q_ASSERT( destination );

	int w = destination->width() / 2;
	int h = destination->height() / 2;

	switch( pane ) {
	case Qt::AllDockWidgetAreas:	m_window->tabifyDockWidget( destination, dock );
									break;

	case Qt::RightDockWidgetArea:	m_window->splitDockWidget( destination, dock, Qt::Horizontal );
									setDockSize( destination, QSize( w, destination->height() ) );
									setDockSize( dock, QSize( w, dock->height() ) );
									break;

	case Qt::LeftDockWidgetArea:	m_window->splitDockWidget( destination, dock, Qt::Horizontal );
									m_window->splitDockWidget( dock, destination, Qt::Horizontal );
									setDockSize( destination, QSize( w, destination->height() ) );
									setDockSize( dock, QSize( w, dock->height() ) );
									break;

	case Qt::BottomDockWidgetArea:	m_window->splitDockWidget( destination, dock, Qt::Vertical );
									setDockSize( destination, QSize( destination->width(), h ) );
									setDockSize( dock, QSize( dock->width(), h ) );
									break;

	case Qt::TopDockWidgetArea:		m_window->splitDockWidget( destination, dock, Qt::Vertical );
									m_window->splitDockWidget( dock, destination, Qt::Vertical );
									setDockSize( destination, QSize( destination->width(), h ) );
									setDockSize( dock, QSize( dock->width(), h ) );
									break;
	}
}

// ** DockIndicator::setDockSize
void DockIndicator::setDockSize( QDockWidget* dock, const QSize& value ) const
{
	Q_ASSERT( dock );

	QSize min = dock->minimumSize();
	QSize max = dock->maximumSize();

	dock->setMinimumSize( value );
	dock->setMaximumSize( value );

	m_window->update();

	qApp->processEvents();

	dock->setMinimumSize( min );
	dock->setMaximumSize( max );
}

} // namespace Ui

DC_END_COMPOSER
