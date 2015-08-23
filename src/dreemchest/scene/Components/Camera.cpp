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

DC_BEGIN_DREEMCHEST

namespace scene {

// ------------------------------------------ BasicCamera ----------------------------------------- //

// ** BasicCamera::viewProj
Matrix4 BasicCamera::viewProj( void ) const
{
	return view() * proj();
}

// ** BasicCamera::view
Matrix4 BasicCamera::view( void ) const
{
	return Matrix4();
}

// ** BasicCamera::proj
Matrix4 BasicCamera::proj( void ) const
{
	return Matrix4();
}

// ------------------------------------------- Camera2D ------------------------------------------- //

// ** Camera2D::width
u32 Camera2D::width( void ) const
{
	return m_width;
}

// ** Camera2D::setWidth
void Camera2D::setWidth( u32 value )
{
	m_width = value;
}

// ** Camera2D::height
u32 Camera2D::height( void ) const
{
	return m_height;
}

// ** Camera2D::setHeight
void Camera2D::setHeight( u32 value )
{
	m_height = value;
}

// ** Camera2D::view
Matrix4 Camera2D::view( void ) const
{
	return Matrix4::translation( m_pan.x, m_pan.y, 0 ) * Matrix4::scale( m_zoom, m_zoom, 1.0f );
}

// ** Camera2D::proj
Matrix4 Camera2D::proj( void ) const
{
	DC_BREAK_IF( m_width == 0 || m_height == 0 )
	return Matrix4::ortho( 0, ( f32 )m_width, 0, ( f32 )m_height, -1, 100000 );
}

// -------------------------------------------- Camera -------------------------------------------- //

// ** Camera::move
void Camera::move( f32 amount )
{
	m_position = m_position + m_direction * amount;
}

// ** Camera::strafe
void Camera::strafe( f32 amount )
{
	m_position = m_position + m_right * amount;
}

// ** Camera::pitch
void Camera::pitch( f32 amount )
{
	m_direction = Quat::rotateAroundAxis( amount, m_right ).rotate( m_direction );
}

// ** Camera::yaw
void Camera::yaw( f32 amount )
{
	// ** Rotate the view direction
	m_direction = Quat::rotateAroundAxis( amount, m_up ).rotate( m_direction );

	// ** Calculate a new side vector.
	m_right = m_direction % m_up;
	m_right.normalize();
}

// ** Camera::view
Matrix4 Camera::view( void ) const
{
	return Matrix4::lookAt( m_position, m_position + m_direction, m_up );
}

// ** Camera::proj
Matrix4 Camera::proj( void ) const
{
	return Matrix4::perspective( m_fov, m_aspect, m_near, m_far );
}

} // namespace scene

DC_END_DREEMCHEST