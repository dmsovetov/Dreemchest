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

#include "Transform.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ------------------------------------------- Follow ------------------------------------------- //

// ** Follow::target
const TransformWPtr& Follow::target( void ) const
{
	return m_target;
}

// ** Follow::type
Follow::Type Follow::type( void ) const
{
	return m_type;
}

// ** Follow::damping
f32 Follow::damping( void ) const
{
	return m_damping;
}

// ** Follow::springForce
f32 Follow::springForce( void ) const
{
	return m_springForce;
}

// ------------------------------------------ Transform ------------------------------------------ //

// ** Transform::matrix
Matrix4 Transform::matrix( void ) const
{
	Matrix4 T = Matrix4::translation( m_position ) * m_rotation * Matrix4::scale( m_scale );

	if( m_parent != NULL ) {
		T = m_parent->matrix() * T;
	}

	return T;
}

// ** Transform::parent
const TransformWPtr& Transform::parent( void ) const
{
	return m_parent;
}

// ** Transform::setParent
void Transform::setParent( const TransformWPtr& value )
{
	m_parent = value;
}

// ** Transform::position
const Vec3& Transform::position( void ) const
{
	return m_position;
}

// ** Transform::setPosition
void Transform::setPosition( const Vec3& value )
{
	m_position = value;
}

// ** Transform::x
f32 Transform::x( void ) const
{
	return m_position.x;
}

// ** Transform::setX
void Transform::setX( f32 value )
{
	m_position.x = value;
}

// ** Transform::y
f32 Transform::y( void ) const
{
	return m_position.y;
}

// ** Transform::setY
void Transform::setY( f32 value )
{
	m_position.y = value;
}

// ** Transform::rotation
f32 Transform::rotation( void ) const
{
	return -m_rotation.euler().z;
}

// ** Transform::setRotation
void Transform::setRotation( f32 value )
{
	m_rotation = Quat::rotateAroundAxis( value, Vec3( 0, 0, -1.0f ) );
}

// ------------------------------------------ Rotator2D ------------------------------------------ //

// ** Rotator2D::speed
f32 Rotator2D::speed( void ) const
{
	return m_speed;
}

} // namespace scene

DC_END_DREEMCHEST