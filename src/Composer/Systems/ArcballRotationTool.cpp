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

#include "ArcballRotationTool.h"
#include "RotationTool.h"

DC_BEGIN_COMPOSER

// ---------------------------------------------------------- ArcballRotationTool ---------------------------------------------------------- //

// ** ArcballRotationTool::ArcballRotationTool
ArcballRotationTool::ArcballRotationTool( f32 radius ) : m_radius( radius ), m_isLocked( false )
{
}

// ** ArcballRotationTool::radius
f32 ArcballRotationTool::radius( void ) const
{
	return m_radius;
}

// ** ArcballRotationTool::isLocked
bool ArcballRotationTool::isLocked( void ) const
{
	return m_isLocked;
}

// ** ArcballRotationTool::lock
void ArcballRotationTool::lock( const Quat& rotation, const Vec3& direction )
{
	DC_BREAK_IF( m_isLocked );

	m_initialRotation	= rotation;
	m_initialDirection	= direction;
	m_isLocked			= true;
}

// ** ArcballRotationTool::initialDirection
const Vec3& ArcballRotationTool::initialDirection( void ) const
{
	return m_initialDirection;
}

// ** ArcballRotationTool::initialRotation
const Quat& ArcballRotationTool::initialRotation( void ) const
{
	return m_initialRotation;
}

// ** ArcballRotationTool::unlock
void ArcballRotationTool::unlock( void )
{
	DC_BREAK_IF( !m_isLocked );
	m_isLocked = false;
}

// ------------------------------------------------------- ArcballRotationToolSystem ------------------------------------------------------- //

// ** ArcballRotationToolSystem::ArcballRotationToolSystem
ArcballRotationToolSystem::ArcballRotationToolSystem( Editors::CursorWPtr cursor, Scene::SpectatorCameraWPtr camera ) : GenericEntitySystem( "ArcballRotationToolSystem" ), m_cursor( cursor ), m_camera( camera )
{
	cursor->subscribe<Editors::Cursor::Pressed>( dcThisMethod( ArcballRotationToolSystem::handleMousePressed ) );
	cursor->subscribe<Editors::Cursor::Released>( dcThisMethod( ArcballRotationToolSystem::handleMouseReleased ) );
}

// ** ArcballRotationToolSystem::process
void ArcballRotationToolSystem::process( u32 currentTime, f32 dt, Ecs::Entity& entity, ArcballRotationTool& tool, Scene::Transform& transform )
{
	// This arcball is not locked - skip
	if( !tool.isLocked() ) {
		return;
	}

	// Check for rotation tool lock
	if( RotationTool* rotation = entity.has<RotationTool>() ) {
		if( rotation->gizmo().state() == Gizmo::Locked ) {
			return;
		}
	}

	Vec3 direction;
	mapToVector( tool, transform, m_cursor->pos(), direction );

	Vec3 v0 = m_camera->get<Scene::Transform>()->rotation().rotate( tool.initialDirection() );
	Vec3 v1 = m_camera->get<Scene::Transform>()->rotation().rotate( direction );

	Quat drag = Quat( v0 % v1, v0 * v1 );
	transform.setRotation( drag * tool.initialRotation() );
}

// ** ArcballRotationToolSystem::handleMousePressed
void ArcballRotationToolSystem::handleMousePressed( const Editors::Cursor::Pressed& e )
{
	// Process only left mouse button clicks
	if( !e.buttons.is( Ui::LeftMouseButton ) ) {
		return;
	}

	// Get the entity set
	Ecs::EntitySet& entities = m_index->entities();

	// Process each arcball and lock the selected.
	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		// Get needed components
		ArcballRotationTool& arcball	= *(*i)->get<ArcballRotationTool>();
		Scene::Transform&	 transform	= *(*i)->get<Scene::Transform>();

		// Map cursor coordinates to a direction on an arcball sphere
		Vec3 direction;
		if( !mapToVector( arcball, transform, m_cursor->pos(), direction ) ) {
			continue;
		}

		// Lock an arcball
		arcball.lock( transform.rotation(), direction );
	}
}

// ** ArcballRotationToolSystem::mapToVector
bool ArcballRotationToolSystem::mapToVector( const ArcballRotationTool& arcball, const Scene::Transform& transform, const Vec2& cursor, Vec3& direction ) const
{
	// Calculate arcball radius based on distance to camera.
	f32 radius = arcball.radius() * (m_camera->position() - transform.worldSpacePosition()).length();

	// Construct the arcball sphere.
	Sphere sphere = Sphere( transform.worldSpacePosition(), radius );

	// Project it onto the screen plane.
	Circle circle = m_camera->get<Scene::Camera>()->sphereToScreenSpace( sphere, m_camera->get<Scene::Transform>() );

	// Map the cursor point to a vector on a sphere.
	return circle.mapToSphere( cursor, direction );
}

// ** ArcballRotationToolSystem::handleMouseReleased
void ArcballRotationToolSystem::handleMouseReleased( const Editors::Cursor::Released& e )
{
	// Process only left mouse button clicks
	if( !e.buttons.is( Ui::LeftMouseButton ) ) {
		return;
	}

	// Get the entity set
	Ecs::EntitySet& entities = m_index->entities();

	// Unlock all locked arcballs
	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		// Get the arcball tool component.
		ArcballRotationTool* arcball = (*i)->get<ArcballRotationTool>();

		// If it's locked - unlock it
		if( arcball->isLocked() ) {
			arcball->unlock();
		}
	}
}

// -------------------------------------------------------------- ArcballRotationToolPass -------------------------------------------------------------- //

// ** ArcballRotationToolPass::render
void ArcballRotationToolPass::render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const ArcballRotationTool& tool, const Scene::Transform& transform )
{
	// Get batched renderer interface
	Renderer::Renderer2DWPtr renderer = context->renderer();

	// Get the scale factor based on distance to camera
	f32 scale = (m_transform->position() - transform.worldSpacePosition()).length();
	
	// Get the camera axes to align the arcball circle
	Vec3 side = m_transform->axisX();
	Vec3 up	  = m_transform->axisY();

	// Render the circle
	renderer->wireCircle( transform.position(), side, up, tool.radius() * scale, 32, Rgba( 0.6f, 0.6f, 0.6f ) );
}

DC_END_COMPOSER