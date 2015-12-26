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

#if DEV_SCREENSPACE_ARCBALL

// ** ArcballRotationTool::lock
void ArcballRotationTool::lock( s32 x, s32 y )
{
	DC_BREAK_IF( m_isLocked );

	m_lastX		= x;
	m_lastY		= y;
	m_isLocked	= true;
}

// ** ArcballRotationTool::lastX
s32 ArcballRotationTool::lastX( void ) const
{
	return m_lastX;
}

// ** ArcballRotationTool::lastY
s32 ArcballRotationTool::lastY( void ) const
{
	return m_lastY;
}

// ** ArcballRotationTool::setLastCursor
void ArcballRotationTool::setLastCursor( s32 x, s32 y )
{
	m_lastX = x;
	m_lastY = y;
}

#else

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

#endif	/*	DEV_SCREENSPACE_ARCBALL	*/

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

#if DEV_SCREENSPACE_ARCBALL
	// Compute the deltas of cursor movement.
	f32 dx = (m_cursor->x() - tool.lastX()) * 0.5f;
	f32 dy = (m_cursor->y() - tool.lastY()) * 0.5f;

	// Construct quaternions.
	Quat x = Quat::rotateAroundAxis(  dx, m_camera->up() );
	Quat y = Quat::rotateAroundAxis( -dy, m_camera->side() );

	// Apply rotations.
	transform.setRotation( x * y * transform.rotation() );

	// Update last used cursor coordinates.
	tool.setLastCursor( m_cursor->x(), m_cursor->y() );
#else
	Vec3 point;

	// Skip if there is no intersection of a view ray & arcball sphere.
	if( !findIntersectionPoint( tool, transform, point ) ) {
		return;
	}

	// Construct the vector to an intersection point.
	Vec3 direction = Vec3::normalize( point - transform.position() );

	// Calculate the cosine of an angle between the initial direction and current vector.
	f32 angle = direction * tool.initialDirection();

	if( angle >= 1.0f ) {
		return;
	}

	// Calculate the rotation axis as a cross product of two vectors.
	Vec3 axis = Vec3::normalize( direction % tool.initialDirection() );

	// Construct the rotation quaternion.
	Quat r = Quat::rotateAroundAxis( -degrees( acos( angle ) ), axis );

	transform.setRotation( r * tool.initialRotation() );
#endif
}

// ** ArcballRotationToolSystem::findIntersectionPoint
bool ArcballRotationToolSystem::findIntersectionPoint( ArcballRotationTool& tool, Scene::Transform& transform, Vec3& point ) const
{
	// Get the view ray from cursor.
	const Ray& ray = m_cursor->ray();

	// Get the scale factor based on distance to camera
	f32 scale = (m_camera->position() - transform.worldSpacePosition()).length();

	// Construct the arcball sphere in world space coordinates.
	Sphere arcball( transform.position(), tool.radius() * scale );

	// Find the intersection point of a view ray & tool sphere
	bool result = ray.intersects( arcball, &point );

	return result;
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

		// Find the intersection point
		Vec3 point;

		if( !findIntersectionPoint( arcball, transform, point ) ) {
			continue;
		}

	#if DEV_SCREENSPACE_ARCBALL
		// Save initial cursor coordinates
		arcball.lock( m_cursor->x(), m_cursor->y() );
	#else
		// Save the initial rotation & vector to intersection point.
		arcball.lock( transform.rotation(), Vec3::normalize( point - transform.position() ) );
	#endif	/*	DEV_SCREENSPACE_ARCBALL	*/
	}
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