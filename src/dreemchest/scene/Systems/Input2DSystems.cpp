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

#include "Input2DSystems.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ** WasdInput2DSystem::WasdInput2DSystem
WasdInput2DSystem::WasdInput2DSystem( Ecs::Entities& entities ) : SceneSystem2( entities, "WasdInput2D" )
{
	m_input = platform::Input::sharedInstance();
}

// ** WasdInput2DSystem::process
void WasdInput2DSystem::process( u32 currentTime, f32 dt, SceneObject& sceneObject, WasdInput2D& input, Transform& transform )
{
	// Movement direction
	Vec3 direction( 0.0f, 0.0f, 0.0f );

	// Get the movement direction
		 if( m_input->keyDown( input.left() ) )  direction.x = -1.0f;
	else if( m_input->keyDown( input.right() ) ) direction.x =  1.0f;

		 if( m_input->keyDown( input.up() ) )    direction.y =  1.0f;
	else if( m_input->keyDown( input.down() ) )  direction.y = -1.0f;

	// Normalize the direction
	direction.normalize();

	// Scale direction by speed
	direction = direction * input.speed();

	// Update the transform
	transform.setPosition( transform.position() + direction * dt );
}

} // namespace scene

DC_END_DREEMCHEST