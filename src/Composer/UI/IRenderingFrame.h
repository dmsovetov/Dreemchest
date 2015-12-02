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

#ifndef __DC_Composer_IRenderingFrame_H__
#define __DC_Composer_IRenderingFrame_H__

#include "IUserInterface.h"

namespace Ui {

	//! Rendering frame interface.
	class IRenderingFrame : public IUserInterface {
	public:

		//! Copy action was triggered.
		//virtual void				handleCopy( void )								= 0;

		//! Cut action was triggered.
		//virtual void				handleCut( void )								= 0;

		//! Paste action was triggered.
		//virtual void				handlePaste( void )								= 0;

		//! Sets the cursor.
		virtual void				setCursor( const String& cursor, bool centered )= 0;

		//! Returns the rendering frame width.
		virtual s32					width( void ) const								= 0;

		//! Returns the rendering frame height.
		virtual s32					height( void ) const							= 0;

		//! Enables or disables the continuous rendering.
		virtual void				setContinuousRendering( bool value )			= 0;

		//! Returns true if the continuous rendering is enabled.
		virtual bool				isContinuousRendering( void ) const				= 0;

		//! Performs the rendering frame update.
		//virtual void				update( void )									= 0;

		//! Sets the focus to this rendering frame
		virtual void				setFocused( void )								= 0;
	};

} // namespace Ui

#endif	/*	!__DC_Composer_MainWindow_H__	*/