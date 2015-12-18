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

// ** SpectatorCamera::construct
void SpectatorCamera::construct( void )
{
	attach<Camera>( Camera::Perspective );
	attach<Transform>();
	attach<RotateAroundAxes>( 10.0f, CSLocalX, DC_NEW Vec3FromMouse )->setRangeForAxis( AxisX, Range( -90.0f, 90.0f ) );
	attach<MoveAlongAxes>( 60.0f, CSLocal, new Vec3FromKeyboard( Platform::Key::A, Platform::Key::D, Platform::Key::W, Platform::Key::S ) );
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

} // namespace Scene

DC_END_DREEMCHEST