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

namespace scene {

// ------------------------------------------------ Box2DPhysics ------------------------------------------------ //

#ifdef DC_PHYSICS2D_BOX2D

// ** Box2DPhysics::Box2DPhysics
Box2DPhysics::Box2DPhysics( ecs::Entities& entities, f32 deltaTime, f32 scale ) : SceneSystem2( entities, "Box2DPhysics" ), m_scale( scale ), m_deltaTime( deltaTime )
{
	m_world = DC_NEW b2World( b2Vec2( 0, -9.8f ) );
}

// ** Box2DPhysics::update
bool Box2DPhysics::begin( u32 currentTime )
{
	m_world->Step( m_deltaTime, 6, 2 );
	return true;
}

// ** Box2DPhysics::process
void Box2DPhysics::process( u32 currentTime, f32 dt, SceneObject& sceneObject, RigidBody2D& rigidBody, Transform2D& transform )
{
	if( rigidBody.type() != RigidBody2D::Dynamic ) {
		return;
	}

	// Get the Box2D body
	b2Body* body = reinterpret_cast<b2Body*>( rigidBody.internal() );
	DC_BREAK_IF( body == NULL );

	// Get the body transform
	const b2Transform& rigidBodyTransform = body->GetTransform();

	// Update the Transform2D instance
	transform.setPosition( positionFromBox2D( rigidBodyTransform.p ) );
	transform.setRotation( rotationFromBox2D( rigidBodyTransform.q.GetAngle() ) );
}

// ** Box2DPhysics::sceneObjectAdded
void Box2DPhysics::sceneObjectAdded( SceneObject& sceneObject, RigidBody2D& rigidBody, Transform2D& transform )
{
	DC_BREAK_IF( rigidBody.internal() )

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
	def.angle = rotationToBox2D( transform.rotation() );

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

	rigidBody.setInternal( body );
}

// ** Box2DPhysics::sceneObjectRemoved
void Box2DPhysics::sceneObjectRemoved( SceneObject& sceneObject, RigidBody2D& rigidBody, Transform2D& transform )
{
	b2Body* body = reinterpret_cast<b2Body*>( rigidBody.internal() );
	DC_BREAK_IF( body == NULL )

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

	circle.m_p = positionToBox2D( Vec2( shape.circle.x, shape.circle.y ) );
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

	polygon.m_centroid = positionToBox2D( Vec2( shape.rect.x, shape.rect.y ) );
	polygon.SetAsBox( sizeToBox2D( shape.rect.width ), sizeToBox2D( shape.rect.height ) );
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
		polygon.m_vertices[i] = positionToBox2D( Vec2( shape.polygon.vertices[i * 2 + 0], shape.polygon.vertices[i * 2 + 1] ) );
	}

	fixture.shape = &polygon;

	body->CreateFixture( &fixture );
}

// ** Box2DPhysics::positionFromBox2D
Vec2 Box2DPhysics::positionFromBox2D( const b2Vec2& position ) const
{
	return Vec2( position.x, position.y ) * m_scale;
}

// ** Box2DPhysics::rotationFromBox2D
f32 Box2DPhysics::rotationFromBox2D( f32 angle ) const
{
	return degrees( -angle );
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

#endif	/*	DC_PHYSICS2D_BOX2D	*/

} // namespace scene

DC_END_DREEMCHEST