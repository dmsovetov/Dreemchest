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

#include "Camera.h"

#include <platform/Window.h>

DC_BEGIN_DREEMCHEST

namespace scene {

// ------------------------------------------ WindowView ------------------------------------------ //

// ** WindowView::WindowView
WindowView::WindowView( const WindowWPtr& window ) : m_window( window )
{
}

// ** WindowView::width
u32 WindowView::width( void ) const
{
	return m_window->width();
}

// ** WindowView::height
u32 WindowView::height( void ) const
{
	return m_window->height();
}

// ** WindowView::create
ViewPtr WindowView::create( const WindowWPtr& window )
{
	return ViewPtr( DC_NEW WindowView( window ) );
}

// -------------------------------------------- Camera -------------------------------------------- //

// ** Camera::clearColor
const Rgba& Camera::clearColor( void ) const
{
	return m_clearColor;
}

// ** Camera::setClearColor
void Camera::setClearColor( const Rgba& value )
{
	m_clearColor = value;
}

// ** Camera::ndc
const Rect& Camera::ndc( void ) const
{
	return m_ndc;
}

// ** Camera::setNdc
void Camera::setNdc( const Rect& value )
{
	m_ndc = value;
}

// ** Camera::viewport
Rect Camera::viewport( void ) const
{
	DC_BREAK_IF( m_view == NULL )

	u32 w = m_view->width();
	u32 h = m_view->height();

	return Rect( w * m_ndc.min().x, h * m_ndc.min().y, w * m_ndc.max().x, h * m_ndc.max().y );
}

// ** Camera::setView
void Camera::setView( const ViewPtr& value )
{
	m_view = value;
}

// ** Camera::view
const ViewPtr& Camera::view( void ) const
{
	return m_view;
}

// ** Camera::calculateProjectionMatrix
Matrix4 Camera::calculateProjectionMatrix( void ) const
{
	DC_BREAK_IF( m_view == NULL )

	f32 width  = static_cast<f32>( m_view->width() );
	f32 height = static_cast<f32>( m_view->height() );

	switch( m_projection ) {
	case Perspective:	return Matrix4::perspective( m_fov, width / height, m_near, m_far );
	case Ortho:			return Matrix4::ortho( 0, width, 0, height, -10000, 10000 );
	case OrthoCenter:	return Matrix4::ortho( -width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, -10000, 10000 );
	default:			DC_BREAK;
	}

	return Matrix4();
}

} // namespace scene

DC_END_DREEMCHEST