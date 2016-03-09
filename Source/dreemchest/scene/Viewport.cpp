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

#include "Viewport.h"
#include "Components/Transform.h"
#include "Components/Rendering.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Viewport::Viewport
Viewport::Viewport( SceneObjectWPtr camera ) : m_camera( camera )
{

}

// ** Viewport::x
s32 Viewport::x( void ) const
{
	return m_x;
}

// ** Viewport::y
s32 Viewport::y( void ) const
{
	return m_y;
}

// ** Viewport::pos
Vec2 Viewport::pos( void ) const
{
	return Vec2( x(), y() );
}

// ** Viewport::ray
const Ray& Viewport::ray( void ) const
{
	return m_ray;
}

// ** Viewport::eye
const Vec3& Viewport::eye( void ) const
{
	DC_ABORT_IF( !m_camera.valid(), "viewport has no camera set" );
	return m_camera->get<Transform>()->position();
}

// ** Viewport::flags
const FlagSet8& Viewport::flags( void ) const
{
	return m_flags;
}

// ** Viewport::camera
SceneObjectWPtr Viewport::camera( void ) const
{
	return m_camera;
}

// ** Viewport::setCamera
void Viewport::setCamera( SceneObjectWPtr value )
{
	m_camera = value;
}

// ** Viewport::touchBegan
void Viewport::touchBegan( s32 x, s32 y )
{
	touchBegan( x, y, m_flags );
}

// ** Viewport::touchBegan
void Viewport::touchBegan( s32 x, s32 y, const FlagSet8& flags )
{
	// Update viewport flags.
	m_flags = flags;

	// Set viewport cursor position.
	setCursor( x, y );

	// Emit the event.
	notify<TouchBegan>( TouchBegan( this, m_flags ) );
}

// ** Viewport::touchEnded
void Viewport::touchEnded( s32 x, s32 y )
{
	touchEnded( x, y, m_flags );
}

// ** Viewport::touchEnded
void Viewport::touchEnded( s32 x, s32 y, const FlagSet8& flags )
{
	// Update viewport flags.
	m_flags = flags & ~flags;

	// Set viewport cursor position.
	setCursor( x, y );

	// Emit the event.
	notify<TouchEnded>( TouchEnded( this, flags ) );
}

// ** Viewport::touchMoved
void Viewport::touchMoved( s32 x, s32 y, const FlagSet8& flags )
{
	// Update viewport flags.
	m_flags = flags;

	// Set viewport cursor position.
	setCursor( x, y );

	// Emit the event
	notify<TouchMoved>( TouchMoved( this, x, y ) );
}

// ** Viewport::setCursor
void Viewport::setCursor( s32 x, s32 y )
{
	m_x		= x;
	m_y		= y;
	m_ray	= constructViewRay( m_x, m_y );
}

// ** Viewport::constructViewRay
Ray Viewport::constructViewRay( s32 x, s32 y ) const
{
	// Get components.
	CameraWPtr    camera	= m_camera->get<Camera>();
	TransformWPtr transform = m_camera->get<Transform>();

	// Calculate near & far points in world space.
	Vec3 near, far;
	camera->toWorldSpace( Vec3( x, y, 0 ), near, transform->matrix() );
	camera->toWorldSpace( Vec3( x,y, 1 ), far, transform->matrix() );

	// Calculate direction
	Vec3 dir = Vec3::normalize( far - near );

	return Ray( transform->worldSpacePosition(), dir );
}

} // namespace Scene

DC_END_DREEMCHEST
