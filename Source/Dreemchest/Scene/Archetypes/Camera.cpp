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

#include "../Components/Rendering.h"
#include "../Components/Transform.h"
#include "../Bindings.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

#if DEV_DEPRECATED_ECS_ARCHETYPES
// --------------------------------------------------------------------- SpectatorCamera --------------------------------------------------------------------- //

// ** SpectatorCamera::construct
void SpectatorCamera::construct( void )
{
	attach<Camera>( Projection::Perspective );
	attach<Transform>();
	attach<RotateAroundAxes>( 10.0f, CSLocalX, DC_NEW Vec3FromMouse )->setRangeForAxis( AxisX, Range( -90.0f, 90.0f ) );
	attach<MoveAlongAxes>( 60.0f, CSLocal, new Vec3FromKeyboard( Platform::Key::A, Platform::Key::D, Platform::Key::W, Platform::Key::S ) );
}

// ** SpectatorCamera::toWorldSpace
Ray SpectatorCamera::toWorldSpace( u32 x, u32 y ) const
{
	// Get components.
	CameraWPtr    camera	= get<Camera>();
	TransformWPtr transform = get<Transform>();

	// Calculate near & far points in world space.
	Vec3 near, far;
	camera->toWorldSpace( Vec3( x, y, 0 ), near, transform->matrix() );
	camera->toWorldSpace( Vec3( x,y, 1 ), far, transform->matrix() );

	// Calculate direction
	Vec3 dir = Vec3::normalize( far - near );

	return Ray( transform->worldSpacePosition(), dir );
}

// ** SpectatorCamera::toScreenSpace
bool SpectatorCamera::toScreenSpace( const Vec3& world, Vec3& screen ) const
{
	return get<Camera>()->pointToScreenSpace( world, screen, get<Transform>()->matrix() );
}

// ** SpectatorCamera::setRotationEnabled
void SpectatorCamera::setRotationEnabled( bool value )
{
	setComponentEnabled<RotateAroundAxes>( value );
}

// ** SpectatorCamera::isRotationEnabled
bool SpectatorCamera::isRotationEnabled( void ) const
{
	return isEnabled<RotateAroundAxes>();
}

// ** SpectatorCamera::setMovementEnabled
void SpectatorCamera::setMovementEnabled( bool value )
{
	setComponentEnabled<MoveAlongAxes>( value );
}

// ** SpectatorCamera::isMovementEnabled
bool SpectatorCamera::isMovementEnabled( void ) const
{
	return isEnabled<MoveAlongAxes>();
}

// ** SpectatorCamera::view
Vec3 SpectatorCamera::view( void ) const
{
	return get<Transform>()->axisZ();
}

// ** SpectatorCamera::side
Vec3 SpectatorCamera::side( void ) const
{
	return get<Transform>()->axisX();
}

// ** SpectatorCamera::up
Vec3 SpectatorCamera::up( void ) const
{
	return get<Transform>()->axisY();
}

// ** SpectatorCamera::position
const Vec3& SpectatorCamera::position( void ) const
{
	return get<Transform>()->position();
}

// ** SpectatorCamera::setPosition
void SpectatorCamera::setPosition( const Vec3& value )
{
	get<Transform>()->setPosition( value );
}

// ** SpectatorCamera::setClearColor
void SpectatorCamera::setClearColor( const Rgba& value )
{
	get<Camera>()->setClearColor( value );
}

// ** SpectatorCamera::clearColor
const Rgba& SpectatorCamera::clearColor( void ) const
{
	return get<Camera>()->clearColor();
}

// ** SpectatorCamera::create
SpectatorCameraPtr SpectatorCamera::create( const RenderTargetPtr& renderTarget, const Vec3BindingPtr& cursor )
{
	SpectatorCameraPtr instance( DC_NEW SpectatorCamera );

	instance->construct();
	instance->get<Camera>()->setTarget( renderTarget );

	if( cursor.valid() ) {
		instance->get<RotateAroundAxes>()->setBinding( cursor );
	}

	return instance;
}

// --------------------------------------------------------------------- Camera2D --------------------------------------------------------------------- //

// ** Camera2D::construct
void Camera2D::construct( void )
{
	attach<Camera>( Projection::OrthoCenter );
	attach<Transform>();
}

// ** Camera2D::zoom
f32 Camera2D::zoom( void ) const
{
	return 1.0f / get<Transform>()->scaleX();
}

// ** Camera2D::setZoom
void Camera2D::setZoom( f32 value )
{
	value = 1.0f / value;
	get<Transform>()->setScale( Vec3( value, value, value ) );
}

// ** Camera2D::pan
Vec2 Camera2D::pan( void ) const
{
	const Vec3& position = get<Transform>()->position();
	return Vec2( position.x, position.y );
}

// ** Camera2D::setPan
void Camera2D::setPan( const Vec2& value )
{
	get<Transform>()->setPosition( Vec3( value.x, value.y, 0.0f ) );
}

// ** Camera2D::create
Camera2DPtr Camera2D::create( const RenderTargetPtr& renderTarget )
{
	Camera2DPtr instance( DC_NEW Camera2D );

	instance->construct();
	instance->get<Camera>()->setTarget( renderTarget );

	return instance;
}
#endif  /*  #if DEV_DEPRECATED_ECS_ARCHETYPES   */

} // namespace Scene

DC_END_DREEMCHEST