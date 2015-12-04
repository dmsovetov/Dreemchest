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

									QRenderingFrame( const QGLWidget* shareWidget, QWidget* parent );

		//! Performs the setup of a shared OpenGL format.
		static void					setupOpenGLFormat( void );

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
	};

	//! RenderingFrame Qt implementation.
	class RenderingFrame : public UserInterface<IRenderingFrame, QRenderingFrame> {
	public:

									RenderingFrame( const QGLWidget* shareWidget, QWidget* parent );

		//! Copy action was triggered.
		//virtual void				handleCopy( void );

		//! Cut action was triggered.
		//virtual void				handleCut( void );

		//! Paste action was triggered.
		//virtual void				handlePaste( void );

		//! Sets the cursor.
		virtual void				setCursor( const String& cursor, bool centered );

		//! Returns the rendering frame width.
		virtual s32					width( void ) const;

		//! Returns the rendering frame height.
		virtual s32					height( void ) const;

		//! Enables or disables the continuous rendering.
		virtual void				setContinuousRendering( bool value );

		//! Returns true if the continuous rendering is enabled.
		virtual bool				isContinuousRendering( void ) const;

		//! Performs the rendering frame update.
		//virtual void				update( void );

		//! Sets the focus to this rendering frame
		virtual void				setFocused( void );

		// ** QRenderingFrame
	//	void				BeginRendering( int interval );
	//	void				EndRendering( void );
	//	bool				IsRendering( void ) const { return timer != -1; }

	//	static void			SetupOpenGLFormat( void );

	private:
		
		bool						m_isContinuousRendering;	//!< Continuous rendering flag.

	//	static QGLFormat	gOpenGLFormat;

	//	int					timer;
	//	bool				isContinuousRendering, updateView, hasLostFocus;
	};

} // namespace Ui

#endif	/*	!__DC_Composer_Qt_RenderingFrame_H__	*/