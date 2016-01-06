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
                    Event( Type type, b2Body* first, b2Body* second )
                        : type( type ), first( first ), second( second ) {}

        b2Body*     first;  //!< First contact body.
        b2Body*     second; //!< Second contact body.
        Type        type;   //!< Collision type.
    };

    //! Clears recorded events.
    void            clear( void );

    //! Returns total number of recorded events.
    s32             eventCount( void ) const;

    //! Returns recorded event by index.
    const Event&    event( s32 index ) const;

protected:

    //! Called by Box2D when two fixtures begin to touch.
    virtual void    BeginContact( b2Contact* contact ) DC_DECL_OVERRIDE;

    //! Called by Box2D when two fixtures cease to touch.
    virtual void    EndContact( b2Contact* contact ) DC_DECL_OVERRIDE;

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
    DC_BREAK_IF( index < 0 || index >= eventCount() );
    return m_events[index];
}

// ** Box2DPhysics::Collisions::BeginContact
void Box2DPhysics::Collisions::BeginContact( b2Contact* contact )
{
    // Get contact bodies
    b2Body* first  = contact->GetFixtureA()->GetBody();
    b2Body* second = contact->GetFixtureB()->GetBody();

    // Bodies with no user data don't trigger collision events
    if( !first->GetUserData() || !second->GetUserData() ) {
        return;
    }

    // Record an event.
    m_events.push_back( Event( Event::Begin, first, second ) );
}

// ** Box2DPhysics::Collisions::EndContact
void Box2DPhysics::Collisions::EndContact( b2Contact* contact )
{
    // Get contact bodies
    b2Body* first  = contact->GetFixtureA()->GetBody();
    b2Body* second = contact->GetFixtureB()->GetBody();

    // Bodies with no user data don't trigger collision events
    if( !first->GetUserData() || !second->GetUserData() ) {
        return;
    }

    // Record an event.
    m_events.push_back( Event( Event::End, first, second ) );
}

// ------------------------------------------------ Box2DPhysics ------------------------------------------------ //

// ** Box2DPhysics::Box2DPhysics
Box2DPhysics::Box2DPhysics( f32 deltaTime, f32 scale, const Vec2& gravity ) : GenericEntitySystem( "Box2DPhysics" ), m_scale( scale ), m_deltaTime( deltaTime )
{
    // Create Box2D world instance
	m_world = DC_NEW b2World( b2Vec2( gravity.x, gravity.y ) );

    // Create collisions
    m_collisions = DC_NEW Collisions;

    // Set contact listener
    m_world->SetContactListener( m_collisions.get() );
}

// ** Box2DPhysics::setDeltaTime
void Box2DPhysics::setDeltaTime( f32 value )
{
	m_deltaTime = value;
}

// ** Box2DPhysics::queryRect
SceneObjectSet Box2DPhysics::queryRect( const Rect& rect ) const
{
	// Query callback
	struct Callback : public b2QueryCallback {
		virtual bool ReportFixture( b2Fixture* fixture ) DC_DECL_OVERRIDE {
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
	DC_BREAK_IF( (start - end).length() < 1.0f );

	// Ray casting callback
	struct Callback : public b2RayCastCallback {
		Callback( void ) {}

		virtual float32 ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction ) DC_DECL_OVERRIDE {
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

		virtual float32 ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction ) DC_DECL_OVERRIDE {
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

// ** Box2DPhysics::begin
bool Box2DPhysics::begin( u32 currentTime )
{
	m_world->Step( m_deltaTime, 6, 2 );
	return true;
}

// ** Box2DPhysics::end
void Box2DPhysics::end( void )
{
    // Dispatch all recorded events
    for( s32 i = 0, n = m_collisions->eventCount(); i < n; i++ ) {
        // Get collision event by index
        const Collisions::Event& e = m_collisions->event( i );

        // Get scene objects from collision event
        SceneObjectWPtr first  = reinterpret_cast<Ecs::Entity*>( e.first->GetUserData() );
        SceneObjectWPtr second = reinterpret_cast<Ecs::Entity*>( e.second->GetUserData() );

        // Emit an event
        switch( e.type ) {
        case Collisions::Event::Begin:  notify<CollisionBegin>( first, second );    break;
        case Collisions::Event::End:    notify<CollisionEnd>( first, second );      break;
        }
    }

    // Clear recorded collision events
    m_collisions->clear();
}

// ** Box2DPhysics::process
void Box2DPhysics::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, RigidBody2D& rigidBody, Transform& transform )
{
    if( rigidBody.type() == RigidBody2D::Kinematic ) {
        rigidBody.internal<Internal>()->m_body->SetTransform( positionToBox2D( Vec2( transform.x(), transform.y() ) ), rotationToBox2D( transform.rotationZ() ) );
    }

	if( rigidBody.type() != RigidBody2D::Dynamic ) {
		return;
	}

	// Get the Box2D body
	Internal::Ptr physical = rigidBody.internal<Internal>();
	DC_BREAK_IF( physical == NULL );

	b2Body* body = physical->m_body;

	// Get the body transform
	const b2Transform& rigidBodyTransform = body->GetTransform();

	// Update the Transform2D instance
	transform.setPosition( positionFromBox2D( rigidBodyTransform.p ) );
	transform.setRotationZ( rotationFromBox2D( rigidBodyTransform.q.GetAngle() ) );

	// Now apply forces
	f32			mass   = body->GetMass();
	f32			torque = rigidBody.torque();
	const Vec2& force  = rigidBody.force();

	body->SetLinearDamping( rigidBody.linearDamping() );
	body->SetAngularDamping( rigidBody.angularDamping() );
	body->ApplyTorque( -torque * mass, true );
	body->ApplyForceToCenter( b2Vec2( force.x * mass, force.y * mass ), true );

	for( u32 i = 0, n = rigidBody.appliedForceCount(); i < n; i++ ) {
		const RigidBody2D::AppliedForce& appliedForce = rigidBody.appliedForce( i );
		b2Vec2 value = b2Vec2( appliedForce.m_value.x * mass, appliedForce.m_value.y * mass );
		b2Vec2 point = body->GetWorldPoint( positionToBox2D( appliedForce.m_point ) );
		body->ApplyForce( value, point, true );
	}

	// Clear all forces now
	rigidBody.clear();
}

// ** Box2DPhysics::entityAdded
void Box2DPhysics::entityAdded( const Ecs::Entity& entity )
{
	Transform*	 transform = entity.get<Transform>();
	RigidBody2D* rigidBody = entity.get<RigidBody2D>();
	Shape2D*     shape	   = entity.get<Shape2D>();

	DC_BREAK_IF( rigidBody->internal<Internal>() != Internal::Ptr() )
	DC_BREAK_IF( shape == NULL )
	DC_BREAK_IF( shape->partCount() == 0 )

    b2BodyDef def;

	// Initialize body type
	switch( rigidBody->type() ) {
	case RigidBody2D::Static:		def.type = b2_staticBody;		break;
	case RigidBody2D::Dynamic:		def.type = b2_dynamicBody;		break;
	case RigidBody2D::Kinematic:	def.type = b2_kinematicBody;	break;
	default:						DC_BREAK;
	}

	// Set the initial body transform
	def.position = positionToBox2D( transform->position() );
	def.angle = rotationToBox2D( transform->rotationZ() );

	// Construct the Box2D body and attach scene object to it
    b2Body* body = m_world->CreateBody( &def );
	body->SetUserData( const_cast<Ecs::Entity*>( &entity ) );

	// Initialize the body shape
	for( u32 i = 0, n = shape->partCount(); i < n; i++ ) {
		// Get the shape part by index
		const Shape2D::Part& part = shape->part( i );

		// Initialize Box2D fixture
		switch( part.type ) {
		case Shape2D::Circle:	addCircleFixture( body, part );		break;
		case Shape2D::Rect:		addRectFixture( body, part );		break;
		case Shape2D::Polygon:	addPolygonFixture( body, part );	break;
		default:				DC_BREAK;
		}
	}

	// Attach created body to a component
	rigidBody->setInternal<Internal>( DC_NEW Internal( body ) );
}

// ** Box2DPhysics::entityRemoved
void Box2DPhysics::entityRemoved( const Ecs::Entity& entity )
{
	RigidBody2D* rigidBody = entity.get<RigidBody2D>();

	Internal::Ptr physical = rigidBody->internal<Internal>();
	DC_BREAK_IF( physical == NULL )

	b2Body* body = physical->m_body;
	DC_BREAK_IF( body == NULL );

    body->SetUserData( NULL );
    m_world->DestroyBody( body );
}

// ** Box2DPhysics::addCircleFixture
void Box2DPhysics::addCircleFixture( b2Body* body, const Shape2D::Part& shape ) const
{
	b2FixtureDef fixture;
	b2CircleShape circle;

	fixture.density = shape.material.density;
	fixture.friction = shape.material.friction;
	fixture.restitution = shape.material.restitution;

	circle.m_p = positionToBox2D( Vec3( shape.circle.x, shape.circle.y, 0.0f ) );
	circle.m_radius = sizeToBox2D( shape.circle.radius );
	fixture.shape = &circle;

	body->CreateFixture( &fixture );
}

// ** Box2DPhysics::addRectFixture
void Box2DPhysics::addRectFixture( b2Body* body, const Shape2D::Part& shape ) const
{
	b2FixtureDef fixture;
	b2PolygonShape polygon;

	fixture.density = shape.material.density;
	fixture.friction = shape.material.friction;
	fixture.restitution = shape.material.restitution;

	polygon.m_centroid = positionToBox2D( Vec3( shape.rect.x, shape.rect.y, 0.0f ) );
	polygon.SetAsBox( sizeToBox2D( shape.rect.width * 0.5f ), sizeToBox2D( shape.rect.height * 0.5f ) );
	fixture.shape = &polygon;

	body->CreateFixture( &fixture );
}

// ** Box2DPhysics::addPolygonFixture
void Box2DPhysics::addPolygonFixture( b2Body* body, const Shape2D::Part& shape ) const
{
	b2FixtureDef fixture;
	b2PolygonShape polygon;

	fixture.density = shape.material.density;
	fixture.friction = shape.material.friction;
	fixture.restitution = shape.material.restitution;

	Array<b2Vec2> points;

	for( u32 i = 0; i < shape.polygon.count; i++ ) {
		b2Vec2 position = positionToBox2D( Vec3( shape.polygon.vertices[i * 2 + 0], shape.polygon.vertices[i * 2 + 1], 0.0f ) );
		points.push_back( position );
	}

	polygon.Set( &points[0], points.size() );

	fixture.shape = &polygon;

	body->CreateFixture( &fixture );
}

// ** Box2DPhysics::positionFromBox2D
Vec3 Box2DPhysics::positionFromBox2D( const b2Vec2& position ) const
{
	return Vec3( position.x, position.y, 0.0f ) * m_scale;
}

// ** Box2DPhysics::rotationFromBox2D
f32 Box2DPhysics::rotationFromBox2D( f32 angle ) const
{
	return degrees( -angle );
}

// ** Box2DPhysics::positionToBox2D
b2Vec2 Box2DPhysics::positionToBox2D( const Vec3& position ) const
{
	return b2Vec2( position.x / m_scale, position.y / m_scale );
}

// ** Box2DPhysics::positionToBox2D
b2Vec2 Box2DPhysics::positionToBox2D( const Vec2& position ) const
{
	return b2Vec2( position.x / m_scale, position.y / m_scale );
}

// ** Box2DPhysics::rotationToBox2D
f32 Box2DPhysics::rotationToBox2D( f32 angle ) const
{
	return radians( -angle );
}

// ** Box2DPhysics::sizeToBox2D
f32 Box2DPhysics::sizeToBox2D( f32 value ) const
{
	return value / m_scale;
}

#endif	/*	DC_BOX2D_ENABLED	*/

} // namespace Scene

DC_END_DREEMCHEST