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

#ifndef __DC_Composer_RenderingFrame_H__
#define __DC_Composer_RenderingFrame_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

namespace Ui {

	//! Extended Qt OpenGL widget implementation.
	class RenderingFrame : public QGLWidget {
	public:

		//! Shared OpenGL format.
		static QGLFormat			kOpenGLFormat;

									//! Constructs RenderingFrame instance.
									RenderingFrame( const QGLWidget* sharedWidget, QWidget* parent );

		//! Performs the setup of a shared OpenGL format.
		static void					setupOpenGLFormat( void );

		//! Sets the rendering interval.
		virtual void				setInterval( s32 value );

		//! Sets the rendering frame delegate.
		void						setDelegate( IRenderingFrameDelegateWPtr value );

		//! Returns the rendering frame delegate.
		IRenderingFrameDelegateWPtr	delegate( void ) const;

		//! Enables or disables the continuous rendering.
		void						setContinuousRendering( bool value );

		//! Returns true if the continuous rendering is enabled.
		bool						isContinuousRendering( void ) const;

		//! Sets the cursor.
		void						setCursor( const String& cursor, bool centered );

	protected:

		//! Handles the widget resizing event.
		virtual void				resizeEvent( QResizeEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget mouse press event.
		virtual void				mousePressEvent( QMouseEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget mouse relase event.
		virtual void				mouseReleaseEvent( QMouseEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget mouse move event.
		virtual void				mouseMoveEvent( QMouseEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget key press event.
		virtual void				keyPressEvent( QKeyEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget key release event.
		virtual void				keyReleaseEvent( QKeyEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget drag enter event.
		virtual void				dragEnterEvent( QDragEnterEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget drag move event.
		virtual void				dragMoveEvent( QDragMoveEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget drop event.
		virtual void				dropEvent( QDropEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget timer event.
		virtual void				timerEvent( QTimerEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget mouse wheel event.
		virtual void				wheelEvent( QWheelEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget context menu event.
		virtual void				contextMenuEvent( QContextMenuEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget focus in event.
		virtual void				focusInEvent( QFocusEvent *e ) Q_DECL_OVERRIDE;

		//! Handles the widget focus out event.
		virtual void				focusOutEvent( QFocusEvent *e ) Q_DECL_OVERRIDE;

		//! Overrides the OpenGL widget paint method.
		virtual void				paintGL( void ) Q_DECL_OVERRIDE;

		//! Overrides the OpenGL widget initialization.
		virtual void				initializeGL( void ) Q_DECL_OVERRIDE;

	private:

		s32							m_timer;			//!< The rendering timer.
		bool						m_hasLostFocus;		//!< Indicates that focus was lost.
		QTime						m_deltaTime;		//!< Tracks the delta time.
		FlagSet8					m_isMouseButtons;	//!< Current mouse state.
		QPoint						m_lastCursorPos;	//!< Last known cursor position.
		IRenderingFrameDelegateWPtr m_delegate;					//!< An attached delegate.
		bool					    m_isContinuousRendering;	//!< Continuous rendering flag.
	};

	//! Rendering frame delegate to process events.
	class IRenderingFrameDelegate : public RefCounted {
	public:

		//! Handles the update event.
		virtual void				handleUpdate( f32 dt ) {}

		//! Handles the resize event.
		virtual void				handleResize( s32 width, s32 height ) {}

		//! Handles the mouse pressed event.
		virtual void				handleMousePress( s32 x, s32 y, u8 button ) {}

		//! Handles the mouse released event.
		virtual void				handleMouseRelease( s32 x, s32 y, u8 button ) {}

		//! Handles the mouse moved event.
		virtual void				handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, u8 buttons ) {}

		//! Handles the focus in event.
		virtual void				handleFocusIn( void ) {}

		//! Handles the focus out event.
		virtual void				handleFocusOut( void ) {}

		//! Handles the key pressed event.
		virtual void				handleKeyPress( Platform::Key key ) {}

		//! Handles the key released event.
		virtual void				handleKeyRelease( Platform::Key key ) {}

		//! Handles the context menu event.
		virtual void				handleContextMenu( MenuQPtr menu ) {}

		//! Handles the mouse wheel event.
		virtual void				handleMouseWheel( s32 delta ) {}

		//! Handles the drag enter event.
		virtual bool				handleDragEnter( MimeDataQPtr mime ) { return false; }

		//! Handles the drag move event.
		virtual void				handleDragMove( MimeDataQPtr mime, s32 x, s32 y ) {}

		//! Handles the drop move event.
		virtual void				handleDrop( MimeDataQPtr mime, s32 x, s32 y ) {}
	};


} // namespace Ui

DC_END_COMPOSER

#endif	/*	!__DC_Composer_RenderingFrame_H__	*/