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

// ** Gizmo::activeTransform
Gizmo::Transform Gizmo::activeTransform( void ) const
{
	return m_transform;
}

// ** Gizmo::setActiveTransform
void Gizmo::setActiveTransform( Gizmo::Transform value )
{
	m_transform = value;
	m_state	    = value == Gizmo::Nothing ? Gizmo::Idle : Gizmo::Active;
}

// ** Gizmo::state
Gizmo::State Gizmo::state( void ) const
{
	return m_state;
}

// ** Gizmo::offset
const Vec3& Gizmo::offset( void ) const
{
	return m_offset;
}

// ** Gizmo::plane
const Plane& Gizmo::plane( void ) const
{
	return m_plane;
}

// ** Gizmo::lock
void Gizmo::lock( const Plane& plane, const Vec3& offset )
{
	DC_BREAK_IF( state() != Gizmo::Active );
	m_offset = offset;
	m_state  = Gizmo::Locked;
	m_plane  = plane;
}

// ** Gizmo::unlock
void Gizmo::unlock( void )
{
	DC_BREAK_IF( state() != Gizmo::Locked );
	m_state = Gizmo::Idle;
}

DC_END_COMPOSER