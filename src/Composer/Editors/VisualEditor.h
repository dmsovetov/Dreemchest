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

#ifndef __DC_Composer_VisualEditor_H__
#define __DC_Composer_VisualEditor_H__

#include "AssetEditor.h"

namespace Editors {

	//! Base class for all visual asset editors.
	class VisualEditor : public AssetEditor {
	friend class RenderingFrameDelegate;
	public:

										//! Constructs the VisualEditor instance.
										VisualEditor( void );

		//! Performs the visual editor initialization.
		virtual bool					initialize( const Ui::FileInfo& asset, Ui::IDocumentWPtr document );

	private:

		//! Update the editor scene.
		virtual void					update( void );

		//! Handles the viewport resize.
		virtual void					handleResize( s32 width, s32 height );

		//! Handles the mouse press event.
		virtual void					handleMousePress( s32 x, s32 y, Ui::MouseButton button );

		//! Handles the mouse release event.
		virtual void					handleMouseRelease( s32 x, s32 y, Ui::MouseButton button );

		//! Handles the mouse move event.
		virtual void					handleMouseMove( s32 sx, s32 sy, s32 ex, s32 ey, s32 dx, s32 dy );

		//! Handles the mouse wheel event.
		virtual void					handleMouseWheel( s32 delta ) {}

		//! Handles the key press event.
		virtual void					handleKeyPress( Platform::Key key ) {}

		//! Handles the key release event.
		virtual void					handleKeyRelease( Platform::Key key ) {}

		//! Handles the drag enter event.
		virtual bool					handleDragEnter( void ) { return false; }

		//! Handles the drag move event.
		virtual void					handleDragMove( s32 x, s32 y ) {}

		//! Handles the drop event.
		virtual void					handleDrop( s32 x, s32 y ) {}

		//! Handles the context menu event.
		virtual void					handleContextMenu( Ui::IMenuWPtr menu );

		//! Handles the focus in event.
		virtual void					handleFocusIn( void );

		//! Handles the focus out event.
		virtual void					handleFocusOut( void );

		//! Begins the frame rendering.
		virtual void					beginFrameRendering( void );

		//! Performs actual frame rendering.
		virtual void					render( void ) {}

		//! Ends the frame rendering.
		virtual void					endFrameRendering( void );

	private:

		Renderer::HalPtr				m_hal;						//!< Rendering HAL instance.
		Rgba							m_backgroundColor;			//!< The background color.
		Ui::IRenderingFrameDelegatePtr	m_renderingFrameDelegate;	//!< Rendering frame delegate.
	};

	//! Visual editor rendering frame delegate.
	class RenderingFrameDelegate : public Ui::IRenderingFrameDelegate {
	public:

										//! Constructs the RenderingFrameDelegate instance.
										RenderingFrameDelegate( VisualEditorWPtr editor );

		//! Handles the update event.
		virtual void					handleUpdate( void );

		//! Handles the resize event.
		virtual void					handleResize( s32 width, s32 height );

		//! Handles the mouse pressed event.
		virtual void					handleMousePress( s32 x, s32 y, Ui::MouseButton button );

		//! Handles the mouse released event.
		virtual void					handleMouseRelease( s32 x, s32 y, Ui::MouseButton button );

		//! Handles the mouse moved event.
		virtual void					handleMouseMove( s32 x, s32 y, s32 globalX, s32 globalY );

		//! Handles the mouse wheel event.
		virtual void					handleMouseWheel( s32 delta );

		//! Handles the focus in event.
		virtual void					handleFocusIn( void );

		//! Handles the focus out event.
		virtual void					handleFocusOut( void );

		//! Handles the key pressed event.
		virtual void					handleKeyPress( Platform::Key key );

		//! Handles the key released event.
		virtual void					handleKeyRelease( Platform::Key key );

		//! Handles the context menu event.
		virtual void					handleContextMenu( Ui::IMenuWPtr menu );

		//! Handles the drag enter event.
		virtual bool					handleDragEnter( void );

		//! Handles the drag move event.
		virtual void					handleDragMove( s32 x, s32 y );

		//! Handles the drop move event.
		virtual void					handleDrop( s32 x, s32 y );

	private:

		VisualEditorWPtr				m_editor;	//!< Parent visual editor.
	};

} // namespace Editors

#endif	/*	!__DC_Composer_VisualEditor_H__	*/