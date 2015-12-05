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

#ifndef __DC_Composer_Qt_RenderingFrame_H__
#define __DC_Composer_Qt_RenderingFrame_H__

#include "UserInterface.h"

namespace Ui {

	//! Extended QGLWidget implementation.
	class QRenderingFrame : public QGLWidget {
	public:

		//! Shared OpenGL format.
		static QGLFormat			kOpenGLFormat;

									//! Constructs QRenderingFrame instance.
									QRenderingFrame( RenderingFrame* parentRenderingFrame, const QGLWidget* sharedWidget, QWidget* parent );

		//! Performs the setup of a shared OpenGL format.
		static void					setupOpenGLFormat( void );

		//! Sets the rendering interval.
		virtual void				setInterval( s32 value );

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

		RenderingFrame*				m_parent;		//!< Parent rendering frame instance.
		s32							m_timer;		//!< The rendering timer.
		bool						m_hasLostFocus;	//!< Indicates that focus was lost.
	};

	//! RenderingFrame Qt implementation.
	class RenderingFrame : public UserInterface<IRenderingFrame, QRenderingFrame> {
	public:

											RenderingFrame( const QGLWidget* sharedWidget, QWidget* parent );

		//! Sets the rendering frame delegate.
		virtual void						setDelegate( IRenderingFrameDelegateWPtr value );

		//! Returns the rendering frame delegate.
		virtual IRenderingFrameDelegateWPtr	delegate( void ) const;

		//! Sets the cursor.
		virtual void						setCursor( const String& cursor, bool centered );

		//! Returns the rendering frame width.
		virtual s32							width( void ) const;

		//! Returns the rendering frame height.
		virtual s32							height( void ) const;

		//! Sets the rendering interval.
		virtual void						setInterval( s32 value );

		//! Enables or disables the continuous rendering.
		virtual void						setContinuousRendering( bool value );

		//! Returns true if the continuous rendering is enabled.
		virtual bool						isContinuousRendering( void ) const;

		//! Sets the focus to this rendering frame
		virtual void						setFocused( void );

	private:
		
		IRenderingFrameDelegateWPtr			m_delegate;					//!< An attached delegate.
		bool								m_isContinuousRendering;	//!< Continuous rendering flag.
	};

} // namespace Ui

#endif	/*	!__DC_Composer_Qt_RenderingFrame_H__	*/