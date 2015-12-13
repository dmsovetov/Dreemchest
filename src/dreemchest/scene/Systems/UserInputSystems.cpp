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

#include "UserInputSystems.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------------------- MoveAlongAxesSystem ------------------------------------------------------- //

// ** MoveAlongAxesSystem::MoveAlongAxesSystem
MoveAlongAxesSystem::MoveAlongAxesSystem( void ) : GenericEntitySystem( "MoveAlongAxesSystem" )
{

}

// ** MoveAlongAxesSystem::process
void MoveAlongAxesSystem::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, MoveAlongAxes& moveAlongAxes, Transform& transform )
{
	// Get movement values
	Vec3 movement = moveAlongAxes.delta() * moveAlongAxes.speed() * dt;

	// Get the coordinate system flags.
	u8 cs = moveAlongAxes.coordinateSystem();

	// Construct the coordinate system axes
	Vec3 axes[] = {
		  cs & CSLocalX ? transform.axisX() : Vec3( 1.0f, 0.0f, 0.0f )
		, cs & CSLocalY ? transform.axisY() : Vec3( 0.0f, 1.0f, 0.0f )
		, cs & CSLocalZ ? transform.axisZ() : Vec3( 0.0f, 0.0f, 1.0f )
	};

	// Update the position
	transform.setPosition( transform.position() + axes[0] * movement.x );
	transform.setPosition( transform.position() + axes[1] * movement.y );
	transform.setPosition( transform.position() + axes[2] * movement.z );

}

// ------------------------------------------------------- RotateAroundAxesSystem ------------------------------------------------------- //

// ** RotateAroundAxesSystem::RotateAroundAxesSystem
RotateAroundAxesSystem::RotateAroundAxesSystem( void ) : GenericEntitySystem( "RotateAroundAxesSystem" )
{

}

// ** RotateAroundAxesSystem::process
void RotateAroundAxesSystem::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, RotateAroundAxes& rotateAroundAxes, Transform& transform )
{
	// Get rotation values
	Vec3 rotation = rotateAroundAxes.delta() * rotateAroundAxes.speed() * dt;

	// Get the coordinate system flags.
	u8 cs = rotateAroundAxes.coordinateSystem();

	// Construct the coordinate system axes
	Vec3 axes[] = {
		  cs & CSLocalX ? transform.axisX() : Vec3( 1.0f, 0.0f, 0.0f )
		, cs & CSLocalY ? transform.axisY() : Vec3( 0.0f, 1.0f, 0.0f )
		, cs & CSLocalZ ? transform.axisZ() : Vec3( 0.0f, 0.0f, 1.0f )
	};

	// Rotate the transform
	if( fabs( rotation.x ) > 0.001f ) {
		transform.rotate( rotation.x, axes[0].x, axes[0].y, axes[0].z );
	}
	if( fabs( rotation.y ) > 0.001f ) {
		transform.rotate( rotation.y, axes[1].x, axes[1].y, axes[1].z );
	}
	if( fabs( rotation.z ) > 0.001f ) {
		transform.rotate( rotation.z, axes[2].x, axes[2].y, axes[2].z );
	}
}

} // namespace Scene

DC_END_DREEMCHEST