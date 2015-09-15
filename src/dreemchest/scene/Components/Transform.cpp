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

namespace Scene {

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
const Quat& Transform::rotation( void ) const
{
	return m_rotation;
}

// ** Transform::setRotation
void Transform::setRotation( const Quat& value )
{
	m_rotation = value;
}

// ** Transform::rotationX
f32 Transform::rotationX( void ) const
{
	return m_rotation.x;
}

// ** Transform::setRotationX
void Transform::setRotationX( f32 value )
{
	m_rotation.x = value;
}

// ** Transform::rotationY
f32 Transform::rotationY( void ) const
{
	return m_rotation.y;
}

// ** Transform::setRotationY
void Transform::setRotationY( f32 value )
{
	m_rotation.y = value;
}

// ** Transform::rotationZ
f32 Transform::rotationZ( void ) const
{
	return m_rotation.z;
}

// ** Transform::setRotationZ
void Transform::setRotationZ( f32 value )
{
	m_rotation.z = value;
}

// ** Transform::setScale
void Transform::setScale( const Vec3& value )
{
	m_scale = value;
}

// ** Transform::scaleX
f32 Transform::scaleX( void ) const
{
	return m_scale.x;
}

// ** Transform::setScaleX
void Transform::setScaleX( f32 value )
{
	m_scale.x = value;
}

// ** Transform::scaleY
f32 Transform::scaleY( void ) const
{
	return m_scale.y;
}

// ** Transform::setScaleY
void Transform::setScaleY( f32 value )
{
	m_scale.y = value;
}

// ** Transform::scaleZ
f32 Transform::scaleZ( void ) const
{
	return m_scale.z;
}

// ** Transform::setScaleZ
void Transform::setScaleZ( f32 value )
{
	m_scale.z = value;
}

// ------------------------------------------ Rotor ------------------------------------------ //

// ** Rotor::x
f32 Rotor::x( void ) const
{
	return m_x;
}

// ** Rotor::y
f32 Rotor::y( void ) const
{
	return m_y;
}

// ** Rotor::z
f32 Rotor::z( void ) const
{
	return m_z;
}

} // namespace Scene

DC_END_DREEMCHEST