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

// ------------------------------------------- MoveTo ------------------------------------------- //

// ** MoveTo::target
Vec3 MoveTo::target( void ) const
{
	return m_target->get();
}

// ** MoveTo::isContinuous
bool MoveTo::isContinuous( void ) const
{
	return m_isContinuous;
}

// ** MoveTo::type
MoveTo::Type MoveTo::type( void ) const
{
	return m_type;
}

// ** MoveTo::damping
f32 MoveTo::damping( void ) const
{
	return m_damping;
}

// ** MoveTo::springForce
f32 MoveTo::springForce( void ) const
{
	return m_springForce;
}

// ------------------------------------------ Transform ------------------------------------------ //

// ** Transform::matrix
const Matrix4& Transform::matrix( void ) const
{
	return m_transform;
}

// ** Transform::setMatrix
void Transform::setMatrix( const Matrix4& value )
{
	m_transform = value;
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

// ** Transform::worldSpacePosition
Vec3 Transform::worldSpacePosition( void ) const
{
	return matrix() * Vec3( 0.0f, 0.0f, 0.0f );
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

// ** Transform::axisX
Vec3 Transform::axisX( void ) const
{
	return m_rotation.rotate( Vec3( 1.0f, 0.0f, 0.0f ) );
}

// ** Transform::axisY
Vec3 Transform::axisY( void ) const
{
	return m_rotation.rotate( Vec3( 0.0f, 1.0f, 0.0f ) );
}

// ** Transform::axisZ
Vec3 Transform::axisZ( void ) const
{
	return m_rotation.rotate( Vec3( 0.0f, 0.0f, 1.0f ) );
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

// ** Transform::z
f32 Transform::z( void ) const
{
	return m_position.z;
}

// ** Transform::setZ
void Transform::setZ( f32 value )
{
	m_position.z = value;
}

// ** Transform::rotation
const Quat& Transform::rotation( void ) const
{
	return m_rotation;
}

// ** Transform::rotate
void Transform::rotate( f32 angle, f32 x, f32 y, f32 z )
{
	Quat r = Quat::rotateAroundAxis( angle, Vec3( x, y, z ) );
	m_rotation = r * m_rotation;
}

// ** Transform::setRotation
void Transform::setRotation( const Quat& value )
{
	m_rotation = value;
}

// ** Transform::rotationX
f32 Transform::rotationX( void ) const
{
    DC_NOT_IMPLEMENTED;
    return 0.0f;
}

// ** Transform::setRotationX
void Transform::setRotationX( f32 value )
{
	m_rotation = Quat::rotateAroundAxis( value, Vec3( 1.0f, 0.0f, 0.0f ) );
}

// ** Transform::rotationY
f32 Transform::rotationY( void ) const
{
    DC_NOT_IMPLEMENTED;
	return 0.0f;
}

// ** Transform::setRotationY
void Transform::setRotationY( f32 value )
{
	m_rotation = Quat::rotateAroundAxis( value, Vec3( 0.0f, 1.0f, 0.0f ) );
}

// ** Transform::rotationZ
f32 Transform::rotationZ( void ) const
{
    return m_rotation.roll();
}

// ** Transform::setRotationZ
void Transform::setRotationZ( f32 value )
{
	m_rotation = Quat::rotateAroundAxis( value, Vec3( 0.0f, 0.0f, 1.0f ) );
}

// ** Transform::setScale
void Transform::setScale( const Vec3& value )
{
	m_scale = value;
}

// ** Transform::scale
const Vec3& Transform::scale( void ) const
{
	return m_scale;
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

// ** Transform::serialize
void Transform::serialize( Ecs::SerializationContext& ctx, Io::KeyValue& ar ) const
{
    ar = Io::KeyValue::object() << "position" << position() << "rotation" << rotation() << "scale" << scale() << "parent" << m_parent.get();
}

// ** Transform::deserialize
void Transform::deserialize( Ecs::SerializationContext& ctx, const Io::KeyValue& ar )
{
    m_position = ar["position"].asVec3();
    m_rotation = ar["rotation"].asQuat();
    m_scale    = ar["scale"].asVec3();
}

// ----------------------------------------------- Identifier ------------------------------------------------ //

// ** Identifier::name
const String& Identifier::name( void ) const
{
	return m_name;
}

// ** Identifier::setName
void Identifier::setName( const String& value )
{
	m_name = value;
}

// ** Identifier::serialize
void Identifier::serialize( Ecs::SerializationContext& ctx, Io::KeyValue& ar ) const
{
    ar = Io::KeyValue::object() << "value" << name();
}

// ** Identifier::deserialize
void Identifier::deserialize( Ecs::SerializationContext& ctx, const Io::KeyValue& ar )
{
    m_name = ar["value"].asString();
}

// --------------------------------------------- MoveAlongAxes --------------------------------------------- //

// ** MoveAlongAxes::speed
f32 MoveAlongAxes::speed( void ) const
{
	return m_speed;
}

// ** MoveAlongAxes::coordinateSystem
u8 MoveAlongAxes::coordinateSystem( void ) const
{
	return m_coordinateSystem;
}

// ** MoveAlongAxes::delta
Vec3 MoveAlongAxes::delta( void ) const
{
	return m_delta->get();
}

// ** MoveAlongAxes::rangeForAxis
const Range& MoveAlongAxes::rangeForAxis( CoordinateSystemAxis axis ) const
{
	return m_range[axis];
}

// ** MoveAlongAxes::setRangeForAxis
void MoveAlongAxes::setRangeForAxis( CoordinateSystemAxis axis, const Range& value )
{
	m_range[axis] = value;
}

// --------------------------------------------- RotateAroundAxes --------------------------------------------- //

// ** RotateAroundAxes::speed
f32 RotateAroundAxes::speed( void ) const
{
	return m_speed;
}

// ** RotateAroundAxes::setSpeed
void RotateAroundAxes::setSpeed( f32 value )
{
	m_speed = value;
}

// ** RotateAroundAxes::coordinateSystem
u8 RotateAroundAxes::coordinateSystem( void ) const
{
	return m_coordinateSystem;
}

// ** RotateAroundAxes::delta
Vec3 RotateAroundAxes::delta( void ) const
{
	return m_delta->get();
}

// ** RotateAroundAxes::binding
Vec3BindingWPtr RotateAroundAxes::binding( void ) const
{
	return m_delta;
}

// ** RotateAroundAxes::setBinding
void RotateAroundAxes::setBinding( const Vec3BindingPtr& value )
{
	m_delta = value;
}

// ** RotateAroundAxes::rangeForAxis
const Range& RotateAroundAxes::rangeForAxis( CoordinateSystemAxis axis ) const
{
	return m_range[axis];
}

// ** RotateAroundAxes::setRangeForAxis
void RotateAroundAxes::setRangeForAxis( CoordinateSystemAxis axis, const Range& value )
{
	m_range[axis] = value;
}

// ** RotateAroundAxes::rotationForAxis
f32 RotateAroundAxes::rotationForAxis( CoordinateSystemAxis axis ) const
{
	return m_rotation[axis];
}

// ** RotateAroundAxes::setRotationForAxis
void RotateAroundAxes::setRotationForAxis( CoordinateSystemAxis axis, f32 value )
{
	m_rotation[axis] = value;
}

} // namespace Scene

DC_END_DREEMCHEST