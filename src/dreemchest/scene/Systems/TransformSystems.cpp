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

#include "TransformSystems.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ------------------------------------------------ FollowSystem ------------------------------------------------- //

// ** FollowSystem::process
void FollowSystem::process( u32 currentTime, f32 dt, SceneObject& sceneObject, Follow& follow, Transform& transform )
{
	const TransformWPtr& target = follow.target();
	DC_BREAK_IF( target == NULL )

	switch( follow.type() ) {
	case Follow::Immediate:	transform.setPosition( target->position() );
							break;

	case Follow::Smooth:	{
								Vec3 dir = target->position() - transform.position();
								transform.setPosition( transform.position() + dir * follow.damping() * dt );
							}
							break;

	case Follow::Elastic:	{
								Internal::Ptr internal = follow.internal<Internal>();

								Vec3 direction = target->position() - transform.position();

								internal->m_force += direction * follow.springForce();
								Vec3 velocity = internal->m_force;

								transform.setPosition( transform.position() + velocity * dt );

								internal->m_force *= 1.0f - (follow.damping() * dt);
							}
							break;
	}
	
}

// ** FollowSystem::sceneObjectAdded
void FollowSystem::sceneObjectAdded( SceneObject& sceneObject, Follow& follow, Transform& transform )
{
	follow.setInternal<Internal>( DC_NEW Internal );
}

// ----------------------------------------------- Rotator2DSystem ----------------------------------------------- //

// ** Rotator2DSystem::process
void Rotator2DSystem::process( u32 currentTime, f32 dt, SceneObject& sceneObject, Rotator2D& rotator, Transform& transform )
{
	transform.setRotation( transform.rotation() + rotator.speed() * dt );
}

} // namespace scene

DC_END_DREEMCHEST