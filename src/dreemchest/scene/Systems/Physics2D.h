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

#ifndef __DC_Scene_Systems_Physics2D_H__
#define __DC_Scene_Systems_Physics2D_H__

#include "../Scene.h"
#include "../Components/Transform.h"
#include "../Components/Physics.h"

#ifdef DC_BOX2D_ENABLED
	#include <Box2D/Box2D.h>
#endif

DC_BEGIN_DREEMCHEST

namespace Scene {

#ifdef DC_BOX2D_ENABLED

	//! The 2D physics system
	class Box2DPhysics : public Ecs::GenericEntitySystem<RigidBody2D, Transform> {
	public:

								//! Constructs the Box2DPhysics instance.
								Box2DPhysics( f32 deltaTime = 0.01f, f32 scale = 1.0f, const Vec2& gravity = Vec2( 0.0f, -9.8f ) );

		//! Performs the ray casting and returns the closes point to the starting one.
		bool					rayCast( const Vec2& start, const Vec2& end, Vec2& intersectionPoint ) const;

		//! Returns all scene objects inside the rect.
		SceneObjectSet			queryRect( const Rect& rect ) const;

		//! Sets the physics update delta time.
		void					setDeltaTime( f32 value );

		//! Updates the physics engine state.
		virtual bool			begin( u32 currentTime );

		//! Synchronizes the scene object transform with a rigid body.
		virtual void			process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, RigidBody2D& rigidBody, Transform& transform );

		//! Creates the Box2D rigid body for an added scene object.
		virtual void			entityAdded( const Ecs::Entity& entity );

		//! Destroys the Box2D rigid body of a removed scene object.
		virtual void			entityRemoved( const Ecs::Entity& entity );

	private:

		//! Adds a circle fixture to a body.
		void					addCircleFixture( b2Body* body, const Shape2D::Part& shape ) const;

		//! Adds a rectangular fixture to a body.
		void					addRectFixture( b2Body* body, const Shape2D::Part& shape ) const;

		//! Adds a polygonal fixture to a body.
		void					addPolygonFixture( b2Body* body, const Shape2D::Part& shape ) const;

		//! Converts position from Box2D space to a scene space.
		Vec3					positionFromBox2D( const b2Vec2& position ) const;

		//! Converts position from scene space to a Box2D space.
		b2Vec2					positionToBox2D( const Vec3& position ) const;

		//! Converts position from scene space to a Box2D space.
		b2Vec2					positionToBox2D( const Vec2& position ) const;

		//! Converts size from scene space to a Box2D space.
		f32						sizeToBox2D( f32 value ) const;

		//! Converts rotation from Box2D space to a scene space.
		f32						rotationFromBox2D( f32 angle ) const;

		//! Converts rotation from scene space to a Box2D space.
		f32						rotationToBox2D( f32 angle ) const;

	private:

		//! Holds the per-instance internal physics data inside the component.
		struct Internal : public Ecs::Internal<Internal> {
								//! Constructs the Internal instance.
								Internal( b2Body* body )
									: m_body( body ) {}

			b2Body*				m_body;				//!< The attached Box2D body.
		};

		AutoPtr<b2World>		m_world;			//!< The Box2D physics world.
		f32						m_scale;			//!< Physics world scale.
		f32						m_deltaTime;		//!< Physics delta time.
	};

#endif	/*	DC_BOX2D_ENABLED	*/

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Systems_Physics2D_H__    */