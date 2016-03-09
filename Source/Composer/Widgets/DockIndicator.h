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

#ifndef __DC_Composer_DockIndicator_H__
#define __DC_Composer_DockIndicator_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

namespace Ui {

	//! Dock indicator widget.
	class DockIndicator : public QWidget {

		Q_OBJECT

	public:

		enum { Radius = 18 };

										//! Constructs DockIndicator instance.
										DockIndicator( QMainWindow* window, const QColor& color = Qt::green, int transparency = 20 );

		//! Begins the dragging process.
		void							begin( QDockWidget* value );

		//! Ends the dragging process.
		void							end( QDockWidget* dock );

		//! Sets the underlying dock widget & current cursor position.
		void							update( const QPoint& cursor );

	protected:

		//! Paints the indicator.
		virtual void					paintEvent( QPaintEvent* e ) Q_DECL_OVERRIDE;

		//! Overridden event filter.
		virtual bool					eventFilter( QObject* sender, QEvent* e ) Q_DECL_OVERRIDE;

		//! Finds the dock widget at specified point in global coordinates.
		QDockWidget*					findDockAtPoint( const QPoint& point ) const;

		//! Places the dock widget to a destination one at specified pane.
		void							placeToPane( QDockWidget* dock, QDockWidget* destination, int pane );

		//! Returns the highlighted pane.
		int								activePane( void ) const;

		//! Returns the area of an active dock pane.
		QRect							activePaneRect( void ) const;

		//! Returns the indicator center.
		QPoint							indicatorCenter( void ) const;

		//! Renders the widget to an image with specified composition mode.
		QImage							widgetToImage( QWidget* widget, QPainter::CompositionMode mode ) const;

		//! Sets the dock widget size.
		void							setDockSize( QDockWidget* dock, const QSize& value ) const;

	private:

		QColor							m_color;			//!< Indication color.
		int								m_transparency;		//!< Indication transparency.
		QMainWindow*					m_window;			//!< Parent window.
		QPoint							m_cursor;			//!< Current cursor position.
		QDockWidget*					m_grabbed;			//!< Dock widget being dragged. 
		QDockWidget*					m_underlying;		//!< The underlying dock widget.
		QImage							m_dock;				//!< The image of a dock being dragged.
		QImage							m_indicator;		//!< The image of a placement indicator.
	};

} // namespace Ui

DC_END_COMPOSER

#endif	/*	!__DC_Composer_DockIndicator_H__	*/