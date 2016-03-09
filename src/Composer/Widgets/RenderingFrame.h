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

        Q_OBJECT

    Q_SIGNALS:

        //! Emitted each time the rendering frame is updated.
        void                        update( f32 dt );

	public:

		//! Shared OpenGL format.
		static QGLFormat			kOpenGLFormat;

									//! Constructs RenderingFrame instance.
									RenderingFrame( const QGLWidget* sharedWidget, QWidget* parent );

		//! Performs the setup of a shared OpenGL format.
		static void					setupOpenGLFormat( void );

		//! Sets the rendering interval.
		virtual void				setInterval( s32 value );

		//! Enables or disables the continuous rendering.
		void						setContinuousRendering( bool value );

		//! Returns true if the continuous rendering is enabled.
		bool						isContinuousRendering( void ) const;

		//! Sets the cursor.
		void						setCursor( const String& cursor, bool centered );

	protected:

		//! Overrides the OpenGL widget paint method.
		virtual void				paintGL( void ) Q_DECL_OVERRIDE;

		//! Handles the widget timer event.
		virtual void				timerEvent( QTimerEvent* e ) Q_DECL_OVERRIDE;

	private:

		s32							m_timer;			//!< The rendering timer.
		QTime						m_deltaTime;		//!< Tracks the delta time.
		bool					    m_isContinuousRendering;	//!< Continuous rendering flag.
	};

} // namespace Ui

DC_END_COMPOSER

#endif	/*	!__DC_Composer_RenderingFrame_H__	*/