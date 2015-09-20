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

#ifndef __DC_Scene_Systems_Transofrm_H__
#define __DC_Scene_Systems_Transofrm_H__

#include "../Scene.h"
#include "../Components/Transform.h"
#include "../Components/Rendering.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Affine transform system calculates the transformation matricies for all transform components.
	class AffineTransformSystem : public Ecs::EntityWithComponentsSystem<Transform> {
	public:

							//! Constructs a AffineTransformSystem instance.
							AffineTransformSystem( Ecs::Entities& entities )
								: EntityWithComponentsSystem( entities, "AffineTransformSystem" ) {}

		//! Calculates the affine transform matrix for each transform component.
		virtual void		update( u32 currentTime, f32 dt );

	private:

		//! Called when entity was added.
		virtual void		entityAdded( const Ecs::Entity& entity );

		//! Called when entity was removed.
		virtual void		entityRemoved( const Ecs::Entity& entity );

	private:

		Array<Transform*>	m_transforms;	//!< Active scene transform components.
	};

	//! World space bounding box system calculates bounding volumes for static meshes in scene.
	class WorldSpaceBoundingBoxSystem : public Ecs::EntityWithComponentsSystem2<StaticMesh, Transform> {
	public:

							//! Constructs a WorldSpaceBoundingBoxSystem instance.
							WorldSpaceBoundingBoxSystem( Ecs::Entities& entities )
								: EntityWithComponentsSystem2( entities, "WorldSpaceBoundingBoxSystem" ) {}

		//! Calculates the world space bounds for static mesh.
		virtual void		process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, StaticMesh& staticMesh, Transform& transform );
	};

	//! The RotorSystem system
	class RotorSystem : public Ecs::EntityWithComponentsSystem2<Rotor, Transform> {
	public:

							//! Constructs a RotorSystem instance.
							RotorSystem( Ecs::Entities& entities )
								: EntityWithComponentsSystem2( entities, "RotorSystem" ) {}

		//! Rotates a single entity by a rotor
		virtual void		process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, Rotor& rotator, Transform& transform );
	};

	//! The following system
	class FollowSystem : public Ecs::EntityWithComponentsSystem2<Follow, Transform> {
	public:

							//! Constructs a FollowSystem instance.
							FollowSystem( Ecs::Entities& entities )
								: EntityWithComponentsSystem2( entities, "Follow" ) {}

		//! Follows the target transform
		virtual void		process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, Follow& follow, Transform& transform );

		//! Attaches the internal following data to an added scene object.
		virtual void		sceneObjectAdded( Ecs::Entity& sceneObject, Follow& follow, Transform& transform );

	private:

		//! Holds the accumulated acceleration.
		struct Internal : Ecs::Internal<Internal> {
			Vec3			m_force;		//!< The accumulated force.
		};
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Systems_Transofrm_H__    */