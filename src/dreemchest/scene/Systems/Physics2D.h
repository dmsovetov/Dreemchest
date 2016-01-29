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

    //! Base class for all 2D physics systems.
    class Physics2D : public Ecs::EntitySystem {
    public:

		//! Sets the physics fixed time step.
		void					setTimeStep( f32 value );

    protected:

                                //! Constructs Physics2D instance.
                                Physics2D( const String& name, f32 timeStep = 1.0f / 120.0f, f32 scale = 1.0f );

        //! Updates the rigid body linear velocity.
        void                    updateLinearVelocity( RigidBody2D& rigidBody, const Vec2& value );

        //! Updates the rigid body mass.
        void                    updateMass( RigidBody2D& rigidBody, f32 value );

        //! Clears the previous state of a rigid body.
        void                    clearState( RigidBody2D& rigidBody );

        //! Queues the collision event for a rigid body.
        void                    queueCollisionEvent( RigidBody2D& rigidBody, const RigidBody2D::CollisionEvent& e );

        //! Simulates the physics with a fixed time step.
        void                    simulatePhysics( f32 dt );

        //! Performs a single physics simulation step.
        virtual void            simulate( f32 dt ) = 0;

    private:

		f32						m_timeStep;		        //!< Physics fixed time step.
        s32                     m_maxSimulationSteps;   //!< Maximum number of simulation steps to stop the spiral of death on low frame rates.
        f32                     m_accumulator;          //!< Accumulated time that is split into fixed steps.
    };

#ifdef DC_BOX2D_ENABLED

	//! The Box2D physics system
	class Box2DPhysics : public Physics2D {
	public:

								//! Constructs the Box2DPhysics instance.
								Box2DPhysics( f32 timeStep = 1.0f / 120.0f, f32 scale = 1.0f, const Vec2& gravity = Vec2( 0.0f, -9.8f ) );

		//! Performs the ray casting and returns the closes point to the starting one.
		bool					rayCast( const Vec2& start, const Vec2& end, Vec2& intersectionPoint ) const;

		//! Returns all scene objects inside the rect.
		SceneObjectSet			queryRect( const Rect& rect ) const;

		//! Returns all scene objects that are intersected by a ray.
		SceneObjectSet			querySegment( const Vec2& start, const Vec2& end ) const;

        //! Performs a single physics simulation step of a Box2D world.
        virtual void            simulate( f32 dt ) DC_DECL_OVERRIDE;

        //! Updates the physics state.
        virtual void	        update( u32 currentTime, f32 dt ) DC_DECL_OVERRIDE;

		//! Creates the Box2D rigid body for an added scene object.
		virtual void			entityAdded( const Ecs::Entity& sceneObject ) DC_DECL_OVERRIDE;

		//! Destroys the Box2D rigid body of a removed scene object.
		virtual void			entityRemoved( const Ecs::Entity& sceneObject ) DC_DECL_OVERRIDE;

        //! Emitted when two bodies begin to touch.
        struct CollisionBegin {
                                //! Constructs CollisionBegin instance.
                                CollisionBegin( SceneObjectWPtr first, SceneObjectWPtr second )
                                    : first( first ), second( second ) {}
            SceneObjectWPtr     first;  //!< First collision object.
            SceneObjectWPtr     second; //!< Second collision object.
        };

        //! Emitted when two bodies cease to touch.
        struct CollisionEnd {
                                //! Constructs CollisionEnd instance.
                                CollisionEnd( SceneObjectWPtr first, SceneObjectWPtr second )
                                    : first( first ), second( second ) {}
            SceneObjectWPtr     first;  //!< First collision object.
            SceneObjectWPtr     second; //!< Second collision object.
        };

	private:

		//! Adds a circle fixture to a body.
		b2Fixture*				addCircleFixture( b2Body* body, b2Filter filter, const Shape2D::Part& shape, bool isSensor ) const;

		//! Adds a rectangular fixture to a body.
		b2Fixture*				addRectFixture( b2Body* body, b2Filter filter, const Shape2D::Part& shape, bool isSensor ) const;

		//! Adds a polygonal fixture to a body.
		b2Fixture*				addPolygonFixture( b2Body* body, b2Filter filter, const Shape2D::Part& shape, bool isSensor ) const;

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

        //! Prepares the Box2D body for simulation by applying forces, impulses, etc.
        void                    prepareForSimulation( b2Body* body, RigidBody2D& rigidBody, Transform& transform );

        //! Updates the scene transform by a Box2D body transform.
        void                    updateTransform( b2Body* body, RigidBody2D& rigidBody, Transform& transform );

        //! Returns the Box2D body attached to this rigid body.
        b2Body*                 extractPhysicalBody( const RigidBody2D& rigidBody ) const;

        //! Pushes the collision events to a respective bodies.
        void                    dispatchCollisionEvents( void );

	private:

        class Collisions;

		//! Holds the per-instance internal physics data inside the component.
		struct Internal : public Ecs::Internal<Internal> {
								//! Constructs the Internal instance.
								Internal( b2Body* body )
									: m_body( body ) {}

			b2Body*				m_body;				//!< The attached Box2D body.
		};

		AutoPtr<b2World>		m_world;			    //!< The Box2D physics world.
        AutoPtr<Collisions>     m_collisions;           //!< Collision listener interface.
		f32						m_scale;			    //!< Physics world scale.
	};

#endif	/*	DC_BOX2D_ENABLED	*/

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Systems_Physics2D_H__    */