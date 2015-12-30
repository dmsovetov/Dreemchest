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

DC_BEGIN_COMPOSER

namespace Editors {

// ----------------------------------------------- VisualEditor ----------------------------------------------- //

// ** VisualEditor::VisualEditor
VisualEditor::VisualEditor( void )
{
	m_backgroundColor = Rgba( 0.5f, 0.5f, 0.5f );
	m_viewport		  = new Scene::Viewport( Scene::SceneObjectWPtr() );
}

// ** VisualEditor::initialize
bool VisualEditor::initialize( ProjectQPtr project, const Scene::AssetPtr& asset, Ui::DocumentQPtr document )
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

	// Attach the rendering frame delegate.
	m_renderingFrameDelegate = new RenderingFrameDelegate( this );
	frame->setDelegate( m_renderingFrameDelegate );
	frame->makeCurrent();

	// Create the rendering HAL.
	m_hal = Renderer::Hal::create( Renderer::OpenGL );

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
    y = m_document->renderingFrame()->height() - y;
    return m_viewport->constructViewRay( x, y );
}

// ** VisualEditor::handleUpdate
void VisualEditor::handleResize( s32 width, s32 height )
{

}

// ** VisualEditor::handleMousePress
void VisualEditor::handleMousePress( s32 x, s32 y, u8 button )
{
	y = m_document->renderingFrame()->height() - y;
	m_viewport->touchBegan( x, y, m_viewport->flags() | button );
}

// ** VisualEditor::handleMouseRelease
void VisualEditor::handleMouseRelease( s32 x, s32 y, u8 button )
{
	y = m_document->renderingFrame()->height() - y;
	m_viewport->touchEnded( x, y, button );
}

// ** VisualEditor::handleMouseMove
void VisualEditor::handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, u8 buttons )
{
	y = m_document->renderingFrame()->height() - y;
	m_viewport->touchMoved( x, y, buttons );
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

// ------------------------------------------ RenderingFrameDelegate ------------------------------------------ //

// ** RenderingFrameDelegate::RenderingFrameDelegate
RenderingFrameDelegate::RenderingFrameDelegate( VisualEditorWPtr editor ) : m_editor( editor )
{

}

// ** RenderingFrameDelegate::handleUpdate
void RenderingFrameDelegate::handleUpdate( f32 dt )
{
	m_editor->update( dt );
}

// ** RenderingFrameDelegate::handleUpdate
void RenderingFrameDelegate::handleResize( s32 width, s32 height )
{
	m_editor->handleResize( width, height );
}

// ** RenderingFrameDelegate::handleMousePress
void RenderingFrameDelegate::handleMousePress( s32 x, s32 y, u8 button )
{
	m_editor->handleMousePress( x, y, button );
}

// ** RenderingFrameDelegate::handleMouseRelease
void RenderingFrameDelegate::handleMouseRelease( s32 x, s32 y, u8 button )
{
	m_editor->handleMouseRelease( x, y, button );
}

// ** RenderingFrameDelegate::handleMouseMove
void RenderingFrameDelegate::handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, u8 buttons )
{
	m_editor->handleMouseMove( x, y, dx, dy, buttons );
}

// ** RenderingFrameDelegate::handleMouseWheel
void RenderingFrameDelegate::handleMouseWheel( s32 delta )
{
	m_editor->handleMouseWheel( delta );
}

// ** RenderingFrameDelegate::handleKeyPress
void RenderingFrameDelegate::handleKeyPress( Platform::Key key )
{
	m_editor->handleKeyPress( key );
}

// ** RenderingFrameDelegate::handleKeyRelease
void RenderingFrameDelegate::handleKeyRelease( Platform::Key key )
{
	m_editor->handleKeyRelease( key );
}

// ** RenderingFrameDelegate::handleDragEnter
bool RenderingFrameDelegate::handleDragEnter( MimeDataQPtr mime )
{
	return m_editor->handleDragEnter( mime );
}

// ** RenderingFrameDelegate::handleDragMove
void RenderingFrameDelegate::handleDragMove( MimeDataQPtr mime, s32 x, s32 y )
{
	m_editor->handleDragMove( mime, x, y );
}

// ** RenderingFrameDelegate::handleDrop
void RenderingFrameDelegate::handleDrop( MimeDataQPtr mime, s32 x, s32 y )
{
	m_editor->handleDrop( mime, x, y );
}

// ** RenderingFrameDelegate::handleContextMenu
void RenderingFrameDelegate::handleContextMenu( Ui::MenuQPtr menu )
{
	m_editor->handleContextMenu( menu );
}

// ** RenderingFrameDelegate::handleFocusIn
void RenderingFrameDelegate::handleFocusIn( void )
{
	m_editor->handleFocusIn();
}

// ** RenderingFrameDelegate::handleFocusOut
void RenderingFrameDelegate::handleFocusOut( void )
{
	m_editor->handleFocusOut();
}

} // namespace Editors

DC_END_COMPOSER