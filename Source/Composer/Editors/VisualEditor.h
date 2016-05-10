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

DC_BEGIN_COMPOSER

namespace Editors {

	//! Base class for all visual asset editors.
	class VisualEditor : public AssetEditor {

        Q_OBJECT

	public:

										//! Constructs the VisualEditor instance.
										VisualEditor( void );

		//! Performs the visual editor initialization.
		virtual bool					initialize( ProjectQPtr project, const FileInfo& asset, Ui::DocumentQPtr document ) DC_DECL_OVERRIDE;

		//! Returns rendering HAL.
		Renderer::HalWPtr				hal( void ) const;

        //! Returns an editor viewport instance.
        Scene::ViewportWPtr             viewport( void ) const;

		//! Returns the background color.
		const Rgba&						backgroundColor( void ) const;

	protected:

        //! Computes the view ray at specified coordinates.
        Ray                             constructViewRay( s32 x, s32 y ) const;

		//! Update the editor scene.
		virtual void					update( f32 dt );

		//! Handles the viewport resize.
		virtual void					handleResize( s32 width, s32 height );

		//! Handles the mouse press event.
		virtual void					handleMousePress( s32 x, s32 y, const Ui::MouseButtons& button );

		//! Handles the mouse release event.
		virtual void					handleMouseRelease( s32 x, s32 y, const Ui::MouseButtons& button );

		//! Handles the mouse move event.
		virtual void					handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, const Ui::MouseButtons& buttons );

		//! Handles the mouse wheel event.
		virtual void					handleMouseWheel( s32 delta ) {}

		//! Handles the key press event.
		virtual void					handleKeyPress( Platform::Key key ) {}

		//! Handles the key release event.
		virtual void					handleKeyRelease( Platform::Key key ) {}

		//! Handles the drag enter event.
		virtual bool					handleDragEnter( MimeDataQPtr mime ) { return false; }

		//! Handles the drag move event.
		virtual void					handleDragMove( MimeDataQPtr mime, s32 x, s32 y ) {}

		//! Handles the drop event.
		virtual void					handleDrop( MimeDataQPtr mime, s32 x, s32 y ) {}

		//! Handles the context menu event.
		virtual void					handleContextMenu( Ui::MenuQPtr menu );

		//! Handles the focus in event.
		virtual void					handleFocusIn( void );

		//! Handles the focus out event.
		virtual void					handleFocusOut( void );

		//! Begins the frame rendering.
		virtual void					beginFrameRendering( void );

		//! Performs actual frame rendering.
		virtual void					render( f32 dt ) {}

		//! Ends the frame rendering.
		virtual void					endFrameRendering( void );

    private:

        //! Event filter to process rendering frame events.
        bool                            eventFilter( QObject* sender, QEvent* e ) Q_DECL_OVERRIDE;

    private slots:

        //! Handles an update signal and renders the frame.
        void                            renderingFrameUpdate( f32 dt );

	private:

		Renderer::HalPtr				m_hal;						//!< Rendering HAL instance.
		Rgba							m_backgroundColor;			//!< The background color.
        QPoint                          m_lastCursorPos;            //!< Last tracked cursor position.
        Ui::MouseButtons                m_mouseButtons;             //!< Bitmask to store pressed mouse buttons.
        bool                            m_hasLostFocus;             //!< Indicates that the focus was lost.
        Scene::ViewportPtr              m_viewport;                 //!< A scene viewport instance.
	};

	//! RenderingFrameViewport is used for rendering the scene to rendering frame.
	class RenderingFrameViewport : public Scene::AbstractViewport {
	public:

		//! Returns the frame width.
		virtual s32						width( void ) const NIMBLE_OVERRIDE { return m_frame->width(); }

		//! Returns the frame height.
		virtual s32						height( void ) const NIMBLE_OVERRIDE { return m_frame->height(); }

		//! Creates the FrameViewport instance.
		static Scene::ViewportPtr	    create( const Ui::RenderingFrameQPtr& frame ) { return new RenderingFrameViewport( frame ); }

	private:

										//! Constructs the RenderingFrameViewport instance.
										RenderingFrameViewport( const Ui::RenderingFrameQPtr& frame )
											: m_frame( frame ) {}

	private:

		Ui::RenderingFrameQPtr			m_frame;	//!< The output frame.
	};

} // namespace Editors

DC_END_COMPOSER

#endif	/*	!__DC_Composer_VisualEditor_H__	*/