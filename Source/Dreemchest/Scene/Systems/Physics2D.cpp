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

#include "Physics2D.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------------ Physics2D ------------------------------------------------ //

// ** Physics2D::Physics2D
Physics2D::Physics2D( const String& name, f32 timeStep ) : EntitySystem( name, Ecs::Aspect::all<RigidBody2D, Shape2D, Transform>() ), m_timeStep( timeStep ), m_maxSimulationSteps( 5 ), m_accumulator( 0.0f )
{

}

// ** Physics2D::setTimeStep
void Physics2D::setTimeStep( f32 value )
{
	m_timeStep = value;
}

// ** Physics2D::simulatePhysics
void Physics2D::simulatePhysics( f32 dt )
{
    // Increase the time accumulator
    m_accumulator += dt;

    // Calculate the total number of simulation steps to perform
    s32 steps = static_cast<s32>( floor( m_accumulator / m_timeStep ) );
    
    // Clamp step count
    steps = min2( steps, m_maxSimulationSteps );

    // Simulate physics
    while( steps-- ) {
        simulate( m_timeStep );
        m_accumulator -= m_timeStep;
    }
}

// ** Physics2D::updateLinearVelocity
void Physics2D::updateLinearVelocity( RigidBody2D& rigidBody, const Vec2& value )
{
    rigidBody.setLinearVelocity( value );
}

// ** Physics2D::updateMass
void Physics2D::updateMass( RigidBody2D& rigidBody, f32 value )
{
    rigidBody.updateMass( value );
}

// ** Physics2D::clearState
void Physics2D::clearState( RigidBody2D& rigidBody )
{
	// Clear all forces now
	rigidBody.clear();

    // Clear all queued events from last simulation step
    rigidBody.clearEvents();
}

// ** Physics2D::queueCollisionEvent
void Physics2D::queueCollisionEvent( RigidBody2D& rigidBody, const RigidBody2D::CollisionEvent& e )
{
    rigidBody.queueCollisionEvent( e );
}

#ifdef DC_BOX2D_ENABLED

// ------------------------------------------------ Box2DPhysics::Collisions ------------------------------------------------ //

// ** Box2DPhysics::Collisions
class Box2DPhysics::Collisions : public b2ContactListener {
public:

    //! Collision event data.
    struct Event {
        //! Collision event type.
        enum Type {
              Begin //!< Contact begin.
            , End   //!< Contact end.
        };

                        //! Constructs empty Event instance.
                        Event( void )
                            {}
                        //! Constructs Event instance.
                        Event( Type type, b2Fixture* first, b2Fixture* second, const Array<b2Vec2>& points = Array<b2Vec2>() )
                            : type( type ), first( first ), second( second ), points( points ) {}

        b2Fixture*      first;      //!< First contact body.
        b2Fixture*      second;     //!< Second contact body.
        Type            type;       //!< Collision type.
        bool            isSensor;   //!< Do we touch the sensor?
        Array<b2Vec2>   points;     //!< Contact points.
    };

    //! Clears recorded events.
    void            clear( void );

    //! Returns total number of recorded events.
    s32             eventCount( void ) const;

    //! Returns recorded event by index.
    const Event&    event( s32 index ) const;

protected:

    //! Called by Box2D when two fixtures begin to touch.
    virtual void    BeginContact( b2Contact* contact ) NIMBLE_OVERRIDE;

    //! Called by Box2D when two fixtures cease to touch.
    virtual void    EndContact( b2Contact* contact ) NIMBLE_OVERRIDE;

private:

    Array<Event>    m_events;   //!< Recorded events.
};

// ** Box2DPhysics::Collisions::clear
void Box2DPhysics::Collisions::clear( void )
{
    m_events.clear();
}

// ** Box2DPhysics::Collisions::eventCount
s32 Box2DPhysics::Collisions::eventCount( void ) const
{
    return ( s32 )m_events.size();
}

// ** Box2DPhysics::Collisions::event
const Box2DPhysics::Collisions::Event& Box2DPhysics::Collisions::event( s32 index ) const
{
    NIMBLE_ABORT_IF( index < 0 || index >= eventCount(), "index is out of range" );
    return m_events[index];
}

// ** Box2DPhysics::Collisions::BeginContact
void Box2DPhysics::Collisions::BeginContact( b2Contact* contact )
{
    // Get contact bodies
    b2Fixture* first  = contact->GetFixtureA();
    b2Fixture* second = contact->GetFixtureB();

    // Bodies with no user data don't trigger collision events
    if( !first->GetBody()->GetUserData() || !second->GetBody()->GetUserData() ) {
        return;
    }

    // Get contact points
    Array<b2Vec2>     points;
    const b2Manifold* manifold = contact->GetManifold();
    b2WorldManifold   worldManifold;

    contact->GetWorldManifold( &worldManifold );
   
    for( s32 i = 0, n = manifold->pointCount; i < n; i++ ) {
        points.push_back( worldManifold.points[i] );
    }

    // Record an event.
    m_events.push_back( Event( Event::Begin, first, second, points ) );
}

// ** Box2DPhysics::Collisions::EndContact
void Box2DPhysics::Collisions::EndContact( b2Contact* contact )
{
    // Get contact bodies
    b2Fixture* first  = contact->GetFixtureA();
    b2Fixture* second = contact->GetFixtureB();

    // Bodies with no user data don't trigger collision events
    if( !first->GetBody()->GetUserData() || !second->GetBody()->GetUserData() ) {
        return;
    }

    // Record an event.
    m_events.push_back( Event( Event::End, first, second ) );
}

// ------------------------------------------------ Box2DPhysics ------------------------------------------------ //

// ** Box2DPhysics::Box2DPhysics
Box2DPhysics::Box2DPhysics( f32 timeStep, const ScaleFactors& scalingFactors, const Vec2& gravity ) : Physics2D( "Box2DPhysics", timeStep ), m_scalingFactors( scalingFactors )
{
    // Create Box2D world instance
	m_world = DC_NEW b2World( forceToBox2D( gravity ) );

    // Disable automatic force clearing
    m_world->SetAutoClearForces( false );

    // Ensure that continuous physics is enabled.
    m_world->SetContinuousPhysics( true );

    // Create collisions
    m_collisions = DC_NEW Collisions;

    // Set contact listener
    m_world->SetContactListener( m_collisions.get() );
}

// ** Box2DPhysics::queryRect
SceneObjectSet Box2DPhysics::queryRect( const Rect& rect ) const
{
	// Query callback
	struct Callback : public b2QueryCallback {
		virtual bool ReportFixture( b2Fixture* fixture ) NIMBLE_OVERRIDE {
			Ecs::Entity* entity = reinterpret_cast<Ecs::Entity*>( fixture->GetBody()->GetUserData() );
			m_result.insert( entity );
			return true;
		}

		SceneObjectSet m_result;
	};

	// Construct the AABB of a query
	b2AABB aabb;
	aabb.lowerBound = positionToBox2D( rect.min() );
	aabb.upperBound = positionToBox2D( rect.max() );

	// Run the query
	Callback callback;
	m_world->QueryAABB( &callback, aabb );

	return callback.m_result;
}

// ** Box2DPhysics::querySegment
SceneObjectSet Box2DPhysics::querySegment( const Vec2& start, const Vec2& end ) const
{
	NIMBLE_BREAK_IF( (start - end).length() < 1.0f, "the queried segment is too short" );

	// Ray casting callback
	struct Callback : public b2RayCastCallback {
		Callback( void ) {}

		virtual float32 ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction ) NIMBLE_OVERRIDE {
			Ecs::Entity* entity = reinterpret_cast<Ecs::Entity*>( fixture->GetBody()->GetUserData() );
			m_result.insert( entity );
			return 1.0f;
		}

		SceneObjectSet	m_result;
	};

	Callback callback;
	m_world->RayCast( &callback, positionToBox2D( start ), positionToBox2D( end ) );

	return callback.m_result;
}

// ** Box2DPhysics::rayCast
bool Box2DPhysics::rayCast( const Vec2& start, const Vec2& end, Vec2& intersectionPoint ) const
{
	// Ray casting callback
	struct Callback : public b2RayCastCallback {
		Callback( void ) : m_hasIntersection( false ) {}

		virtual float32 ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction ) NIMBLE_OVERRIDE {
			m_result = point;
			m_hasIntersection = true;
			return 0.0f;
		}

		bool	m_hasIntersection;
		b2Vec2	m_result;
	};

	// Convert start & end points to Box2D coordinates
	b2Vec2 p1 = positionToBox2D( start );
	b2Vec2 p2 = positionToBox2D( end );

	// Ray cast the physics world.
	Callback callback;
	m_world->RayCast( &callback, p1, p2 );

	// Convert the result to Vec2 and return
	Vec3 result = positionFromBox2D( callback.m_result );
	intersectionPoint = Vec2( result.x, result.y );

	return callback.m_hasIntersection;
}

// ** Box2DPhysics::simulate
void Box2DPhysics::simulate( f32 dt )
{
    m_world->Step( dt, 8, 4 );
}

// ** Box2DPhysics::update
void Box2DPhysics::update( u32 currentTime, f32 dt )
{
    Ecs::EntitySet& entities = m_index->entities();

    // First apply all forces and impulses to each Box2D physical body
    for( Ecs::EntitySet::iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
        RigidBody2D& rigidBody = *(*i)->get<RigidBody2D>();

        // Skip static bodies
        if( rigidBody.type() == RigidBody2D::Static ) {
            continue;
        }

        prepareForSimulation( extractPhysicalBody( rigidBody ), rigidBody, *(*i)->get<Transform>() );
    }

    // Now simulate the physics with a fixed time step
    simulatePhysics( dt );

    // Clear forces
    m_world->ClearForces();

    // Now apply physics transform to a scene transform & dispatch collision events.
    for( Ecs::EntitySet::iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
        RigidBody2D& rigidBody = *(*i)->get<RigidBody2D>();

        // Skip static and kinematic bodies
        if( rigidBody.type() != RigidBody2D::Dynamic ) {
            continue;
        }

        updateTransform( extractPhysicalBody( rigidBody ), rigidBody, *(*i)->get<Transform>() );
    }

    // Finally dispatch collision events
    dispatchCollisionEvents();
}

// ** Box2DPhysics::extractPhysicalBody
b2Body* Box2DPhysics::extractPhysicalBody( const RigidBody2D& rigidBody ) const
{
	Internal::Ptr physical = rigidBody.internal<Internal>();
	NIMBLE_ABORT_IF( physical == NULL, "invalid internal data" );
	return physical->m_body;
}

// ** Box2DPhysics::prepareForSimulation
void Box2DPhysics::prepareForSimulation( b2Body* body, RigidBody2D& rigidBody, Transform& transform )
{
    NIMBLE_BREAK_IF( rigidBody.type() == RigidBody2D::Static, "static objects should not be simulated" );

    // Kinematic bodies inherit scene transform
    if( rigidBody.type() == RigidBody2D::Kinematic ) {
        body->SetTransform( positionToBox2D( Vec2( transform.x(), transform.y() ) ), rotationToBox2D( transform.rotationZ() ) );
    }

	// Get the body transform
	const b2Transform& rigidBodyTransform = body->GetTransform();

    // This rigid body was moved - sync the Box2D body with it
    if( rigidBody.wasMoved() ) {
        body->SetTransform( positionToBox2D( rigidBody.movedTo() ), rigidBodyTransform.q.GetAngle() );
    }

    // This rigid body was rotated - sync the Box2D body with it
    if( rigidBody.wasRotated() ) {
        body->SetTransform( body->GetPosition(), radians( rigidBody.rotatedTo() ) );
    }

    // Rigid body was put to rest - clear velocities
    if( rigidBody.wasPutToRest() ) {
        body->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );
        body->SetAngularVelocity( 0.0f );
    }

	// Now apply forces
	f32			torque = rigidBody.torque();
	const Vec2& force  = rigidBody.force();

	body->SetLinearDamping( rigidBody.linearDamping() );
	body->SetAngularDamping( rigidBody.angularDamping() );
    body->SetLinearVelocity( velocityToBox2D( rigidBody.linearVelocity() ) );
	body->ApplyTorque( -torque, true );
	body->ApplyForceToCenter( forceToBox2D( force ), true );
    body->SetGravityScale( rigidBody.gravityScale() );

	for( u32 i = 0, n = rigidBody.appliedForceCount(); i < n; i++ ) {
		const RigidBody2D::AppliedForce& appliedForce = rigidBody.appliedForce( i );
		b2Vec2 point = body->GetWorldPoint( positionToBox2D( appliedForce.m_point ) );
		body->ApplyForce( forceToBox2D( appliedForce.m_value ), point, true );
	}

    for( u32 i = 0, n = rigidBody.appliedImpulseCount(); i < n; i++ ) {
 		const RigidBody2D::AppliedForce& appliedImpulse = rigidBody.appliedImpulse( i );
		b2Vec2 point = body->GetWorldPoint( positionToBox2D( appliedImpulse.m_point ) );
		body->ApplyLinearImpulse( forceToBox2D( appliedImpulse.m_value ), point, true );   
    }

    // Clear the rigid body state
    clearState( rigidBody );
}

// ** Box2DPhysics::updateTransform
void Box2DPhysics::updateTransform( b2Body* body, RigidBody2D& rigidBody, Transform& transform )
{
	// Get the body transform
	const b2Transform& rigidBodyTransform = body->GetTransform();

	// Update the Transform2D instance
	transform.setPosition( positionFromBox2D( rigidBodyTransform.p ) );
	transform.setRotationZ( rotationFromBox2D( rigidBodyTransform.q.GetAngle() ) );

    // Update the body's linear velocity
    rigidBody.setLinearVelocity( velocityFromBox2D( body->GetLinearVelocity() ) );
}

// ** Box2DPhysics::dispatchCollisionEvents
void Box2DPhysics::dispatchCollisionEvents( void )
{
    // Dispatch all recorded events
    for( s32 i = 0, n = m_collisions->eventCount(); i < n; i++ ) {
        // Get collision event by index
        const Collisions::Event& e = m_collisions->event( i );

        // Get fixtures from an event
        b2Fixture* firstFixture = e.first;
        b2Fixture* secondFixture = e.second;

        // Get scene objects from collision event
        SceneObjectWPtr first  = reinterpret_cast<Ecs::Entity*>( firstFixture->GetBody()->GetUserData() );
        SceneObjectWPtr second = reinterpret_cast<Ecs::Entity*>( secondFixture->GetBody()->GetUserData() );

        // Convert Box2D contact points to world space.
        Array<Vec2> points;

        for( s32 j = 0, m = ( s32 )e.points.size(); j < m; j++ ) {
            points.push_back( positionFromBox2D( e.points[j] ) );
        }

        // Emit an event and push it to component's event queue
        switch( e.type ) {
        case Collisions::Event::Begin:  {
                                            queueCollisionEvent( *first->get<RigidBody2D>(),  RigidBody2D::CollisionEvent( RigidBody2D::CollisionEvent::Begin, second, secondFixture->IsSensor(), secondFixture->GetFilterData().categoryBits, points ) );
                                            queueCollisionEvent( *second->get<RigidBody2D>(), RigidBody2D::CollisionEvent( RigidBody2D::CollisionEvent::Begin, first,  firstFixture->IsSensor(), firstFixture->GetFilterData().categoryBits, points ) );

                                            notify<CollisionBegin>( first, second );
                                        }
                                        break;

        case Collisions::Event::End:    {
                                            queueCollisionEvent( *first->get<RigidBody2D>(),  RigidBody2D::CollisionEvent( RigidBody2D::CollisionEvent::End, second, secondFixture->IsSensor(), secondFixture->GetFilterData().categoryBits, points ) );
                                            queueCollisionEvent( *second->get<RigidBody2D>(), RigidBody2D::CollisionEvent( RigidBody2D::CollisionEvent::End, first,  firstFixture->IsSensor(), firstFixture->GetFilterData().categoryBits, points ) );

                                            notify<CollisionEnd>( first, second );
                                        }
                                        break;
        }
    }

    // Clear recorded collision events
    m_collisions->clear();
}

// ** Box2DPhysics::entityAdded
void Box2DPhysics::entityAdded( const Ecs::Entity& entity )
{
	Shape2D*     shape      =  entity.get<Shape2D>();
    RigidBody2D& rigidBody  = *entity.get<RigidBody2D>();
    Transform&   transform  = *entity.get<Transform>();

	NIMBLE_ABORT_IF( rigidBody.internal<Internal>().valid(), "internal data should not be valid" )
	NIMBLE_ABORT_IF( shape == NULL, "rigid body should have an attached Shape2D" )
	NIMBLE_ABORT_IF( shape->partCount() == 0, "rigid body shape should not be empty" )

    b2BodyDef def;

	// Initialize body type
	switch( rigidBody.type() ) {
	case RigidBody2D::Static:		def.type = b2_staticBody;	    break;
	case RigidBody2D::Dynamic:		def.type = b2_dynamicBody;      break;
	case RigidBody2D::Kinematic:	def.type = b2_kinematicBody;    break;
	default:						NIMBLE_BREAK;
	}

	// Set the initial body transform
	def.position = positionToBox2D( transform.position() );
	def.angle = rotationToBox2D( transform.rotationZ() );
    def.bullet = rigidBody.isBullet();

	// Construct the Box2D body and attach scene object to it
    b2Body* body = m_world->CreateBody( &def );
	body->SetUserData( const_cast<Ecs::Entity*>( &entity ) );

	// Initialize the body shape
	for( u32 i = 0, n = shape->partCount(); i < n; i++ ) {
		// Get the shape part by index
		const SimpleShape2D& part = shape->part( i );

        // Setup fixture collision filter
        b2Filter filter;
        filter.categoryBits = rigidBody.category();
        filter.maskBits     = rigidBody.collisionMask();

		// Initialize Box2D fixture
		switch( part.type ) {
		case Shape2DType::Circle:	    addCircleFixture( body, filter, part, rigidBody.isSensor() );   break;
		case Shape2DType::Rect:         addRectFixture( body, filter, part, rigidBody.isSensor() );		break;
		case Shape2DType::Polygon:      addPolygonFixture( body, filter, part, rigidBody.isSensor() );	break;
		default:				        NIMBLE_BREAK;
		}
	}

    // Now set the mass
    updateMass( rigidBody, body->GetMass() );

	// Attach created body to a component
	rigidBody.setInternal<Internal>( DC_NEW Internal( body ) );
}

// ** Box2DPhysics::entityRemoved
void Box2DPhysics::entityRemoved( const Ecs::Entity& entity )
{
    RigidBody2D* rigidBody = entity.get<RigidBody2D>();
	Internal::Ptr physical = rigidBody->internal<Internal>();
	NIMBLE_ABORT_IF( physical == NULL, "internal data should be valid" )

	b2Body* body = physical->m_body;
	NIMBLE_ABORT_IF( body == NULL, "Box2D body should be valid" );

    body->SetUserData( NULL );
    m_world->DestroyBody( body );
}

// ** Box2DPhysics::addCircleFixture
b2Fixture* Box2DPhysics::addCircleFixture( b2Body* body, b2Filter filter, const SimpleShape2D& shape, bool isSensor ) const
{
	b2FixtureDef fixture;
	b2CircleShape circle;

	fixture.density = shape.material.density;
	fixture.friction = shape.material.friction;
	fixture.restitution = shape.material.restitution;
    fixture.filter = filter;
    fixture.isSensor = isSensor;

	circle.m_p = positionToBox2D( Vec3( shape.circle.x, shape.circle.y, 0.0f ) );
	circle.m_radius = sizeToBox2D( shape.circle.radius );
	fixture.shape = &circle;

	b2Fixture* result = body->CreateFixture( &fixture );
    return result;
}

// ** Box2DPhysics::addRectFixture
b2Fixture* Box2DPhysics::addRectFixture( b2Body* body, b2Filter filter, const SimpleShape2D& shape, bool isSensor ) const
{
	b2FixtureDef fixture;
	b2PolygonShape polygon;

	fixture.density = shape.material.density;
	fixture.friction = shape.material.friction;
	fixture.restitution = shape.material.restitution;
    fixture.filter = filter;
    fixture.isSensor = isSensor;

	polygon.m_centroid = positionToBox2D( Vec3( shape.rect.x, shape.rect.y, 0.0f ) );
	polygon.SetAsBox( sizeToBox2D( shape.rect.width * 0.5f ), sizeToBox2D( shape.rect.height * 0.5f ) );
	fixture.shape = &polygon;

	b2Fixture* result = body->CreateFixture( &fixture );
    return result;
}

// ** Box2DPhysics::addPolygonFixture
b2Fixture* Box2DPhysics::addPolygonFixture( b2Body* body, b2Filter filter, const SimpleShape2D& shape, bool isSensor ) const
{
	b2FixtureDef fixture;
	b2PolygonShape polygon;

	fixture.density = shape.material.density;
	fixture.friction = shape.material.friction;
	fixture.restitution = shape.material.restitution;
    fixture.filter = filter;
    fixture.isSensor = isSensor;

	Array<b2Vec2> points;

	for( u32 i = 0; i < shape.polygon.count; i++ ) {
		b2Vec2 position = positionToBox2D( Vec3( shape.polygon.vertices[i * 2 + 0], shape.polygon.vertices[i * 2 + 1], 0.0f ) );
		points.push_back( position );
	}

	polygon.Set( &points[0], points.size() );

	fixture.shape = &polygon;

	b2Fixture* result = body->CreateFixture( &fixture );
    return result;
}

// ** Box2DPhysics::positionFromBox2D
Vec3 Box2DPhysics::positionFromBox2D( const b2Vec2& position ) const
{
	return Vec3( position.x, position.y, 0.0f ) * m_scalingFactors.distance;
}

// ** Box2DPhysics::rotationFromBox2D
f32 Box2DPhysics::rotationFromBox2D( f32 angle ) const
{
	return degrees( angle );
}

// ** Box2DPhysics::positionToBox2D
b2Vec2 Box2DPhysics::positionToBox2D( const Vec3& position ) const
{
	return b2Vec2( position.x / m_scalingFactors.distance, position.y / m_scalingFactors.distance );
}

// ** Box2DPhysics::positionToBox2D
b2Vec2 Box2DPhysics::positionToBox2D( const Vec2& position ) const
{
	return b2Vec2( position.x / m_scalingFactors.distance, position.y / m_scalingFactors.distance );
}

// ** Box2DPhysics::forceToBox2D
b2Vec2 Box2DPhysics::forceToBox2D( const Vec2& value ) const
{
    return b2Vec2( value.x * m_scalingFactors.force, value.y * m_scalingFactors.force );
}

// ** Box2DPhysics::forceFromBox2D
Vec2 Box2DPhysics::forceFromBox2D( const b2Vec2& value ) const
{
    return Vec2( value.x, value.y ) / m_scalingFactors.force;
}

// ** Box2DPhysics::velocityToBox2D
b2Vec2 Box2DPhysics::velocityToBox2D( const Vec2& value ) const
{
    return b2Vec2( value.x * m_scalingFactors.velocity, value.y * m_scalingFactors.velocity );
}

// ** Box2DPhysics::velocityFromBox2D
Vec2 Box2DPhysics::velocityFromBox2D( const b2Vec2& value ) const
{
    return Vec2( value.x, value.y ) / m_scalingFactors.velocity;
}

// ** Box2DPhysics::rotationToBox2D
f32 Box2DPhysics::rotationToBox2D( f32 angle ) const
{
	return radians( -angle );
}

// ** Box2DPhysics::sizeToBox2D
f32 Box2DPhysics::sizeToBox2D( f32 value ) const
{
	return value / m_scalingFactors.distance;
}

#endif	/*	DC_BOX2D_ENABLED	*/

} // namespace Scene

DC_END_DREEMCHEST
