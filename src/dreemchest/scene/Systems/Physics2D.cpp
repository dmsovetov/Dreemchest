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

// ------------------------------------------------ Box2DPhysics ------------------------------------------------ //

#ifdef DC_BOX2D_ENABLED

// ** Box2DPhysics::Box2DPhysics
Box2DPhysics::Box2DPhysics( f32 deltaTime, f32 scale ) : GenericEntitySystem( "Box2DPhysics" ), m_scale( scale ), m_deltaTime( deltaTime )
{
	m_world = DC_NEW b2World( b2Vec2( 0, -9.8f ) );
}

// ** Box2DPhysics::queryRect
SceneObjectsList Box2DPhysics::queryRect( const Rect& rect ) const
{
	// Query callback
	struct Callback : public b2QueryCallback {
		virtual bool ReportFixture( b2Fixture* fixture ) {
			Ecs::Entity* entity = reinterpret_cast<Ecs::Entity*>( fixture->GetBody()->GetUserData() );
			m_result.push_back( entity );
			return true;
		}

		SceneObjectsList m_result;
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

// ** Box2DPhysics::rayCast
bool Box2DPhysics::rayCast( const Vec2& start, const Vec2& end, Vec2& intersectionPoint ) const
{
	// Ray casting callback
	struct Callback : public b2RayCastCallback {
		Callback( void ) : m_hasIntersection( false ) {}

		virtual float32 ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction ) {
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

// ** Box2DPhysics::update
bool Box2DPhysics::begin( u32 currentTime )
{
	m_world->Step( m_deltaTime, 6, 2 );
	return true;
}

// ** Box2DPhysics::process
void Box2DPhysics::process( u32 currentTime, f32 dt, Ecs::Entity& sceneObject, RigidBody2D& rigidBody, Transform& transform )
{
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

	body->ApplyTorque( -torque * mass, true );
	body->ApplyForceToCenter( b2Vec2( force.x * mass, force.y * mass ), true );

	// Clear all forces now
	rigidBody.clear();
}

// ** Box2DPhysics::sceneObjectAdded
void Box2DPhysics::sceneObjectAdded( Ecs::Entity& sceneObject, RigidBody2D& rigidBody, Transform& transform )
{
	DC_BREAK_IF( rigidBody.internal<Internal>() != Internal::Ptr() )

	Shape2D* shape = sceneObject.get<Shape2D>();
	DC_BREAK_IF( shape == NULL )
	DC_BREAK_IF( shape->size() == 0 )

    b2BodyDef def;

	// Initialize body type
	switch( rigidBody.type() ) {
	case RigidBody2D::Static:		def.type = b2_staticBody;		break;
	case RigidBody2D::Dynamic:		def.type = b2_dynamicBody;		break;
	case RigidBody2D::Kinematic:	def.type = b2_kinematicBody;	break;
	default:						DC_BREAK;
	}

	// Set the initial body transform
	def.position = positionToBox2D( transform.position() );
	def.angle = rotationToBox2D( transform.rotationZ() );

	// Construct the Box2D body and attach scene object to it
    b2Body* body = m_world->CreateBody( &def );
	body->SetUserData( &sceneObject );

	// Initialize the body shape
	for( u32 i = 0, n = shape->size(); i < n; i++ ) {
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
	rigidBody.setInternal<Internal>( DC_NEW Internal( body ) );
}

// ** Box2DPhysics::sceneObjectRemoved
void Box2DPhysics::sceneObjectRemoved( Ecs::Entity& sceneObject, RigidBody2D& rigidBody, Transform& transform )
{
	Internal::Ptr physical = rigidBody.internal<Internal>();
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

	polygon.m_count = shape.polygon.count;

	for( u32 i = 0; i < shape.polygon.count; i++ ) {
		polygon.m_vertices[i] = positionToBox2D( Vec3( shape.polygon.vertices[i * 2 + 0], shape.polygon.vertices[i * 2 + 1], 0.0f ) );
	}

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