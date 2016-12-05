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

#include "VisualEditor.h"
#include "../Project/Project.h"
#include "../Widgets/Menu.h"

DC_BEGIN_COMPOSER

namespace Editors {

// ----------------------------------------------- VisualEditor ----------------------------------------------- //

// ** VisualEditor::VisualEditor
VisualEditor::VisualEditor( void ) : m_hasLostFocus( true )
{
	m_backgroundColor = Rgba( 0.5f, 0.5f, 0.5f );
}

// ** VisualEditor::initialize
bool VisualEditor::initialize( ProjectQPtr project, const FileInfo& asset, Ui::DocumentQPtr document )
{
	// Perform basic initialization first.
	if( !AssetEditor::initialize( project, asset, document ) ) {
		return false;
	}

	// Attach the rendering frame to this document.
	Ui::RenderingFrameQPtr frame = document->attachRenderingFrame();

	if( !frame ) {
		return false;
	}

    // Connect to an update signal
    connect( frame, SIGNAL(update(f32)), this, SLOT(renderingFrameUpdate(f32)) );

	// Attach the rendering frame delegate.
    frame->installEventFilter( this );
	frame->makeCurrent();

	// Create the rendering HAL.
	m_hal = Renderer::Hal::create( Renderer::OpenGL );

    // Create the scene viewport instance
    m_viewport = RenderingFrameViewport::create( document->renderingFrame() );

	return true;
}

// ** VisualEditor::hal
Renderer::HalWPtr VisualEditor::hal( void ) const
{
	return m_hal;
}

// ** VisualEditor::viewport
Scene::ViewportWPtr VisualEditor::viewport( void ) const
{
    return m_viewport;
}

// ** VisualEditor::backgroundColor
const Rgba& VisualEditor::backgroundColor( void ) const
{
	return m_backgroundColor;
}

// ** VisualEditor::update
void VisualEditor::update( f32 dt )
{
	beginFrameRendering();
	render( dt );
	endFrameRendering();
}

// ** VisualEditor::beginFrameRendering
void VisualEditor::beginFrameRendering( void )
{
	m_hal->clear( m_backgroundColor );
}

// ** VisualEditor::endFrameRendering
void VisualEditor::endFrameRendering( void )
{
	m_hal->present();
}

// ** VisualEditor::constructViewRay
Ray VisualEditor::constructViewRay( s32 x, s32 y ) const
{
#if DEV_DEPRECATED_SCENE_INPUT
    y = m_document->renderingFrame()->height() - y;
    return m_viewport->constructViewRay( x, y );
#else
    NIMBLE_NOT_IMPLEMENTED;
    return Ray();
#endif  /*  #if DEV_DEPRECATED_SCENE_INPUT  */
}

// ** VisualEditor::handleUpdate
void VisualEditor::handleResize( s32 width, s32 height )
{

}

// ** VisualEditor::handleMousePress
void VisualEditor::handleMousePress( s32 x, s32 y, const Ui::MouseButtons& button )
{
	y = m_document->renderingFrame()->height() - y;
   m_viewport->notify<RenderingFrameViewport::TouchBegan>( m_viewport, 0, x, y );
}

// ** VisualEditor::handleMouseRelease
void VisualEditor::handleMouseRelease( s32 x, s32 y, const Ui::MouseButtons& button )
{
	y = m_document->renderingFrame()->height() - y;
    m_viewport->notify<RenderingFrameViewport::TouchEnded>( m_viewport, 0, x, y );
}

// ** VisualEditor::handleMouseMove
void VisualEditor::handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, const Ui::MouseButtons& buttons )
{
	y = m_document->renderingFrame()->height() - y;
    m_viewport->notify<RenderingFrameViewport::TouchMoved>( m_viewport, 0, x, y, dx, dy );
}

// ** VisualEditor::handleContextMenu
void VisualEditor::handleContextMenu( Ui::MenuQPtr menu )
{

}

// ** VisualEditor::handleFocusIn
void VisualEditor::handleFocusIn( void )
{

}

// ** VisualEditor::handleFocusOut
void VisualEditor::handleFocusOut( void )
{

}

// ** VisualEditor::renderingFrameUpdate
void VisualEditor::renderingFrameUpdate( f32 dt )
{
    update( dt );
}

// ** VisualEditor::eventFilter
bool VisualEditor::eventFilter( QObject* sender, QEvent* e )
{
    // Ensure the sender is a rendering frame
    Ui::RenderingFrame* frame = qobject_cast<Ui::RenderingFrame*>( sender );
    NIMBLE_BREAK_IF( !frame );

    // Process an event
    switch( e->type() ) {
    case QEvent::MouseButtonPress:		{
                                            QMouseEvent*     me     = static_cast<QMouseEvent*>( e );
                                            Ui::MouseButtons button = me->button();
                                            handleMousePress( me->pos().x(), me->pos().y(), button );
                                            m_lastCursorPos = me->pos();
                                            m_mouseButtons.on( button );
                                            return true;
                                        }
	case QEvent::MouseButtonRelease:	{
                                            QMouseEvent*     me     = static_cast<QMouseEvent*>( e );
                                            Ui::MouseButtons button = me->button();
                                            handleMouseRelease( me->pos().x(), me->pos().y(), me->button() );
                                            m_mouseButtons.off( button );
                                            return true;
                                        }
    case QEvent::MouseMove:             {
                                            QMouseEvent* me = static_cast<QMouseEvent*>( e );
                                            QPoint delta = me->pos() - m_lastCursorPos;
                                            handleMouseMove( me->pos().x(), me->pos().y(), delta.x(), delta.y(), m_mouseButtons );
                                            m_lastCursorPos = me->pos();
                                            return true;
                                        }
    case QEvent::Wheel:                 {
                                            QWheelEvent* we = static_cast<QWheelEvent*>( e );
                                            handleMouseWheel( we->delta() );
                                            return true;
                                        }
    case QEvent::DragEnter:             {
                                            QDragEnterEvent* de = static_cast<QDragEnterEvent*>( e );
                                            if( handleDragEnter( de->mimeData() ) ) {
                                                de->acceptProposedAction();
                                            }
                                            return true;
                                        }
    case QEvent::DragMove:              {
                                            QDragMoveEvent* de = static_cast<QDragMoveEvent*>( e );
                                            handleDragMove( de->mimeData(), de->pos().x(), de->pos().y() );
                                            return true;
                                        }
    case QEvent::Drop:                  {
                                            QDropEvent* de = static_cast<QDropEvent*>( e );
                                            handleDrop( de->mimeData(), de->pos().x(), de->pos().y() );
                                            return true;
                                        }
    case QEvent::FocusIn:               {
                                            QFocusEvent* fe = static_cast<QFocusEvent*>( e );

                                            if( m_hasLostFocus ) {
                                                handleFocusIn();
                                                m_hasLostFocus = false;
                                            }
                                            return true;
                                        }
    case QEvent::FocusOut:              {
                                            QFocusEvent* fe = static_cast<QFocusEvent*>( e );

                                            if( fe->reason() != Qt::PopupFocusReason && !m_hasLostFocus ) {
                                                handleFocusOut();
                                                m_hasLostFocus = true;
                                            }
                                            return true;
                                        }
    case QEvent::ContextMenu:           {
                                            QContextMenuEvent* ce = static_cast<QContextMenuEvent*>( e );

                                            Ui::MenuQPtr menu = new Ui::Menu( frame );
                                            handleContextMenu( menu );

                                            if( menu->size() ) {
                                                menu->exec( ce->globalPos() );
                                            }
                                            delete menu;
                                            return true;
                                        }
    case QEvent::Resize:                {
                                            QResizeEvent* re = static_cast<QResizeEvent*>( e );
                                            handleResize( re->size().width(), re->size().height() );
                                            return true;
                                        }
    case QEvent::KeyPress:              {
                                            QKeyEvent* ke = static_cast<QKeyEvent*>( e );
                                            handleKeyPress( Ui::convertKey( ke->key() ) );
                                            return true;
                                        }
    case QEvent::KeyRelease:            {
                                            QKeyEvent* ke = static_cast<QKeyEvent*>( e );
                                            handleKeyRelease( Ui::convertKey( ke->key() ) );
                                            return true;
                                        }
    }

    return false;
}

} // namespace Editors

DC_END_COMPOSER
