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

// --------------------------------------- BasicTransform --------------------------------------- //

// ** BasicTransform::BasicTransform
BasicTransform::BasicTransform( BasicTransform* parent ) : m_parent( parent )
{
}

// ** BasicTransform::parent
const BasicTransformWPtr& BasicTransform::parent( void ) const
{
	return m_parent;
}

// ** BasicTransform::setParent
void BasicTransform::setParent( const BasicTransformWPtr& value )
{
	m_parent = value;
}

// ** BasicTransform::affine
Matrix4 BasicTransform::affine( void ) const
{
	if( const BasicTransform* parent = m_parent.get() ) {
		return parent->affine() * matrix();
	}

	return matrix();
}

// ** BasicTransform::matrix
Matrix4 BasicTransform::matrix( void ) const
{
	return Matrix4();
}

// ----------------------------------------- Transform2D ----------------------------------------- //

// ** Transform2D::matrix
Matrix4 Transform2D::matrix( void ) const
{
	return Matrix4::translation( Vec3( m_x, m_y, 0.0f ) ) * Quat::rotateAroundAxis( m_rotation, Vec3( 0, 0, -1 ) ) * Matrix4::scale( Vec3( m_scaleX, m_scaleY, 1.0f ) );
}

// ** Transform2D::rotation
f32 Transform2D::rotation( void ) const
{
	return m_rotation;
}

// ** Transform2D::position
Vec2 Transform2D::position( void ) const
{
	return Vec2( m_x, m_y );
}

// ** Transform2D::setPosition
void Transform2D::setPosition( const Vec2& value )
{
	m_x = value.x;
	m_y = value.y;
}

// ** Transform2D::x
f32 Transform2D::x( void ) const
{
	return m_x;
}

// ** Transform2D::setX
void Transform2D::setX( f32 value )
{
	m_x = value;
}

// ** Transform2D::y
f32 Transform2D::y( void ) const
{
	return m_y;
}

// ** Transform2D::setY
void Transform2D::setY( f32 value )
{
	m_y = value;
}

// ** Transform2D::setRotation
void Transform2D::setRotation( f32 value )
{
	m_rotation = value;
}

// ------------------------------------------ Transform ------------------------------------------ //

// ** Transform::matrix
Matrix4 Transform::matrix( void ) const
{
	return Matrix4::translation( m_position ) * m_rotation * Matrix4::scale( m_scale );
}

// ------------------------------------------ Rotator2D ------------------------------------------ //

// ** Rotator2D::speed
f32 Rotator2D::speed( void ) const
{
	return m_speed;
}

} // namespace scene

DC_END_DREEMCHEST