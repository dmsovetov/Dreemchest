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

namespace Editors {

// ----------------------------------------------- VisualEditor ----------------------------------------------- //

// ** VisualEditor::VisualEditor
VisualEditor::VisualEditor( void )
{
	m_backgroundColor = Rgba( 0.5f, 0.5f, 0.5f );
}

// ** VisualEditor::initialize
bool VisualEditor::initialize( const Ui::FileInfo& asset, Ui::IDocumentWPtr document )
{
	// Perform basic initialization first.
	if( !AssetEditor::initialize( asset, document ) ) {
		return false;
	}

	// Attach the rendering frame to this document.
	Ui::IRenderingFrameWPtr frame = document->attachRenderingFrame();

	if( !frame.valid() ) {
		return false;
	}

	// Attach the rendering frame delegate.
	m_renderingFrameDelegate = new RenderingFrameDelegate( this );
	frame->setDelegate( m_renderingFrameDelegate );

	// Create the rendering HAL.
	m_hal = Renderer::Hal::create( Renderer::OpenGL );

	return true;
}

// ** VisualEditor::update
void VisualEditor::update( void )
{
	beginFrameRendering();
	render();
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

// ** VisualEditor::handleUpdate
void VisualEditor::handleResize( s32 width, s32 height )
{

}

// ** VisualEditor::handleMousePress
void VisualEditor::handleMousePress( s32 x, s32 y, Ui::MouseButton button )
{

}

// ** VisualEditor::handleMouseRelease
void VisualEditor::handleMouseRelease( s32 x, s32 y, Ui::MouseButton button )
{

}

// ** VisualEditor::handleMouseMove
void VisualEditor::handleMouseMove( s32 sx, s32 sy, s32 ex, s32 ey, s32 dx, s32 dy )
{

}

// ** VisualEditor::handleContextMenu
void VisualEditor::handleContextMenu( Ui::IMenuWPtr menu )
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
void RenderingFrameDelegate::handleUpdate( void )
{
	m_editor->update();
}

// ** RenderingFrameDelegate::handleUpdate
void RenderingFrameDelegate::handleResize( s32 width, s32 height )
{
	m_editor->handleResize( width, height );
}

// ** RenderingFrameDelegate::handleMousePress
void RenderingFrameDelegate::handleMousePress( s32 x, s32 y, Ui::MouseButton button )
{
	m_editor->handleMousePress( x, y, button );
}

// ** RenderingFrameDelegate::handleMouseRelease
void RenderingFrameDelegate::handleMouseRelease( s32 x, s32 y, Ui::MouseButton button )
{
	m_editor->handleMouseRelease( x, y, button );
}

// ** RenderingFrameDelegate::handleMouseMove
void RenderingFrameDelegate::handleMouseMove( s32 x, s32 y, s32 globalX, s32 globalY )
{
	m_editor->handleMouseMove( x, y, globalX, globalY, 0, 0 );
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
bool RenderingFrameDelegate::handleDragEnter( Ui::IMimeDataWPtr mime )
{
	return m_editor->handleDragEnter( mime );
}

// ** RenderingFrameDelegate::handleDragMove
void RenderingFrameDelegate::handleDragMove( Ui::IMimeDataWPtr mime, s32 x, s32 y )
{
	m_editor->handleDragMove( mime, x, y );
}

// ** RenderingFrameDelegate::handleDrop
void RenderingFrameDelegate::handleDrop( Ui::IMimeDataWPtr mime, s32 x, s32 y )
{
	m_editor->handleDrop( mime, x, y );
}

// ** RenderingFrameDelegate::handleContextMenu
void RenderingFrameDelegate::handleContextMenu( Ui::IMenuWPtr menu )
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