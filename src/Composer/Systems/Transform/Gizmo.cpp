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

#include "Gizmo.h"

DC_BEGIN_COMPOSER

// ** Gizmo::kRed
Rgba Gizmo::kRed = Rgba::fromBytes( 240, 68, 31 );

// ** Gizmo::kGreen
Rgba Gizmo::kGreen = Rgba::fromBytes( 187, 237, 82 );

// ** Gizmo::kBlue
Rgba Gizmo::kBlue = Rgba::fromBytes( 61, 133, 236 );

// ** Gizmo::kActive
Rgba Gizmo::kActive = Rgba::fromBytes( 255, 255, 0 );

// ** Gizmo::kScreen
Rgba Gizmo::kScreen = Rgba( 0.8f, 0.8f, 0.8f );

// ** Gizmo::type
u8 Gizmo::type( void ) const
{
	return m_type;
}

// ** Gizmo::activate
void Gizmo::activate( u8 value )
{
	m_type  = value;
	m_state = Gizmo::Active;
}

// ** Gizmo::deactivate
void Gizmo::deactivate( void )
{
	m_state = Gizmo::Idle;
	m_type  = ~0;
}

// ** Gizmo::state
Gizmo::State Gizmo::state( void ) const
{
	return m_state;
}

// ** Gizmo::ray
const Ray& Gizmo::ray( void ) const
{
	return m_ray;
}

// ** Gizmo::tangent
const Vec2& Gizmo::tangent( void ) const
{
	return m_tangent;
}

// ** Gizmo::cursor
const Vec2& Gizmo::cursor( void ) const
{
	return m_cursor;
}

// ** Gizmo::transform
Scene::TransformWPtr Gizmo::transform( void ) const
{
	return m_transform;
}

// ** Gizmo::lock
void Gizmo::lock( const Scene::Transform& transform, const Vec2& cursor, const Ray& ray, const Vec2& tangent )
{
	m_state		= Gizmo::Locked;
	m_ray		= ray;
	m_cursor	= cursor;
	m_tangent	= tangent;
	m_transform = new Scene::Transform( transform );
}

// ** Gizmo::unlock
void Gizmo::unlock( void )
{
	DC_BREAK_IF( state() != Gizmo::Locked );
	m_state = Gizmo::Idle;
}

DC_END_COMPOSER