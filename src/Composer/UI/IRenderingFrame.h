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
#include "IMenu.h"

namespace Ui {

	//! Rendering frame interface.
	class IRenderingFrame : public IUserInterface {
	public:

		//! Copy action was triggered.
		//virtual void						handleCopy( void )								= 0;

		//! Cut action was triggered.
		//virtual void						handleCut( void )								= 0;

		//! Paste action was triggered.
		//virtual void						handlePaste( void )								= 0;

		//! Sets the rendering frame delegate.
		virtual void						setDelegate( IRenderingFrameDelegateWPtr value )= 0;

		//! Returns the rendering frame delegate.
		virtual IRenderingFrameDelegateWPtr	delegate( void ) const							= 0;

		//! Sets the cursor.
		virtual void						setCursor( const String& cursor, bool centered )= 0;

		//! Sets the rendering interval.
		virtual void						setInterval( s32 value )						= 0;

		//! Returns the rendering frame width.
		virtual s32							width( void ) const								= 0;

		//! Returns the rendering frame height.
		virtual s32							height( void ) const							= 0;

		//! Enables or disables the continuous rendering.
		virtual void						setContinuousRendering( bool value )			= 0;

		//! Returns true if the continuous rendering is enabled.
		virtual bool						isContinuousRendering( void ) const				= 0;

		//! Performs the rendering frame update.
		//virtual void						update( void )									= 0;

		//! Sets the focus to this rendering frame
		virtual void						setFocused( void )								= 0;
	};

	//! Rendering frame delegate to process events.
	class IRenderingFrameDelegate : public RefCounted {
	public:

		//! Handles the update event.
		virtual void				handleUpdate( void ) {}

		//! Handles the resize event.
		virtual void				handleResize( s32 width, s32 height ) {}

		//! Handles the mouse pressed event.
		virtual void				handleMousePress( s32 x, s32 y, MouseButton button ) {}

		//! Handles the mouse released event.
		virtual void				handleMouseRelease( s32 x, s32 y, MouseButton button ) {}

		//! Handles the mouse moved event.
		virtual void				handleMouseMove( s32 x, s32 y, s32 globalX, s32 globalY ) {}

		//! Handles the focus in event.
		virtual void				handleFocusIn( void ) {}

		//! Handles the focus out event.
		virtual void				handleFocusOut( void ) {}

		//! Handles the key pressed event.
		virtual void				handleKeyPress( Platform::Key key ) {}

		//! Handles the key released event.
		virtual void				handleKeyRelease( Platform::Key key ) {}

		//! Handles the context menu event.
		virtual void				handleContextMenu( IMenuWPtr menu ) {}

		//! Handles the mouse wheel event.
		virtual void				handleMouseWheel( s32 delta ) {}

		//! Handles the drag enter event.
		virtual bool				handleDragEnter( void ) { return false; }

		//! Handles the drag move event.
		virtual void				handleDragMove( s32 x, s32 y ) {}

		//! Handles the drop move event.
		virtual void				handleDrop( s32 x, s32 y ) {}
	};

} // namespace Ui

#endif	/*	!__DC_Composer_MainWindow_H__	*/