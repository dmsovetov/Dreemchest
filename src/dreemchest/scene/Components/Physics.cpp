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

#include "Physics.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------- Shape2D ------------------------------------------- //

// ** Shape2D::clear
void Shape2D::clear( void )
{
	m_parts.clear();
}

// ** Shape2D::size
u32 Shape2D::size( void ) const
{
	return ( u32 )m_parts.size();
}

// ** Shape2D::part
const Shape2D::Part& Shape2D::part( u32 index ) const
{
	DC_BREAK_IF( index >= size() )
	return m_parts[index];
}

// ** Shape2D::addCircle
void Shape2D::addCircle( f32 radius, f32 x, f32 y, const Material& material )
{
	Part part;
	part.type = Circle;
	part.material = material;
	part.circle.radius = radius;
	part.circle.x = x;
	part.circle.y = y;
	m_parts.push_back( part );
}

// ** Shape2D::addRect
void Shape2D::addRect( f32 width, f32 height, f32 x, f32 y, const Material& material )
{
	Part part;
	part.type = Rect;
	part.material = material;
	part.rect.width = width;
	part.rect.height = height;
	part.rect.x = x;
	part.rect.y = y;
	m_parts.push_back( part );
}

// ----------------------------------------- RigidBody2D ----------------------------------------- //

// ** RigidBody2D::mass
f32 RigidBody2D::mass( void ) const
{
	return m_mass;
}

// ** RigidBody2D::type
RigidBody2D::Type RigidBody2D::type( void ) const
{
	return m_type;
}

// ** RigidBody2D::linearDamping
f32 RigidBody2D::linearDamping( void ) const
{
	return m_linearDamping;
}

// ** RigidBody2D::setLinearDamping
void RigidBody2D::setLinearDamping( f32 value )
{
	m_linearDamping = value;
}

// ** RigidBody2D::angularDamping
f32 RigidBody2D::angularDamping( void ) const
{
	return m_angularDamping;
}

// ** RigidBody2D::setAngularDamping
void RigidBody2D::setAngularDamping( f32 value )
{
	m_angularDamping = value;
}

// ** RigidBody2D::torque
f32 RigidBody2D::torque( void ) const
{
	return m_torque;
}

// ** RigidBody2D::applyTorque
void RigidBody2D::applyTorque( f32 value )
{
	m_torque += value;
}

// ** RigidBody2D::force
const Vec2& RigidBody2D::force( void ) const
{
	return m_force;
}

// ** RigidBody2D::applyForc
void RigidBody2D::applyForce( const Vec2& value )
{
	m_force += value;
}

// ** RigidBody2D::applyForceToPoint
void RigidBody2D::applyForceToPoint( const Vec2& value, const Vec2& point )
{
	m_forces.push_back( AppliedForce( value, point ) );
}

// ** RigidBody2D::appliedForceCount
u32 RigidBody2D::appliedForceCount( void ) const
{
	return ( u32 )m_forces.size();
}

// ** RigidBody2D::appliedForce
const 	RigidBody2D::AppliedForce&	RigidBody2D::appliedForce( u32 index ) const
{
	DC_BREAK_IF( index >= appliedForceCount() );
	return m_forces[index];
}

// ** RigidBody2D::clear
void RigidBody2D::clear( void )
{
	m_torque = 0.0f;
	m_force  = Vec2( 0.0f, 0.0f );
	m_forces.clear();
}

} // namespace Scene

DC_END_DREEMCHEST