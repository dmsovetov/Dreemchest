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

#include "TranslationTool.h"

DC_BEGIN_COMPOSER

// ---------------------------------------------------------- TranslationTool ---------------------------------------------------------- //

// ** TranslationTool::TranslationTool
TranslationTool::TranslationTool( f32 orthRadius, f32 scalingFactor, f32 selectorScale )
	: m_scalingFactor( scalingFactor ), m_orthRadius( orthRadius ), m_selectorScale( selectorScale )
{
}

// ** TranslationTool:isActive
bool TranslationTool::isActive( void ) const
{
	return m_gizmo.activeTransform() != Gizmo::Nothing;
}

// ** TranslationTool:isLocked
bool TranslationTool::isLocked( void ) const
{
	return m_gizmo.state() == Gizmo::Locked;
}

// ** TranslationTool::scalingFactor
f32 TranslationTool::scalingFactor( void ) const
{
	return m_scalingFactor;
}

// ** TranslationTool::orthRadius
f32 TranslationTool::orthRadius( void ) const
{
	return m_orthRadius;
}

// ** TranslationTool::gizmo
const Gizmo& TranslationTool::gizmo( void ) const
{
	return m_gizmo;
}

// ** TranslationTool::gizmo
Gizmo& TranslationTool::gizmo( void )
{
	return m_gizmo;
}

// ** TranslationTool::selectorBoundingBox
Bounds TranslationTool::selectorBoundingBox( Gizmo::Transform transform ) const
{
	// Get the axes vectors
	Vec3 x = Vec3::axisX();
	Vec3 y = Vec3::axisY();
	Vec3 z = Vec3::axisZ();

	// Store the scaling factors
	f32 r = m_orthRadius;
	f32 s = m_selectorScale;

	// Construct selector bounding boxes
	switch( transform ) {
	case Gizmo::X:	return Bounds() << x * s << x + (y + z) * r << x - (y + z) * r;
	case Gizmo::Y:	return Bounds() << y * s << y + (x + z) * r << y - (x + z) * r;
	case Gizmo::Z:	return Bounds() << z * s << z + (y + x) * r << z - (y + x) * r;
	case Gizmo::XY:	return Bounds() << x * s << (x + y) * s << y * s;
	case Gizmo::YZ:	return Bounds() << y * s << (y + z) * s << z * s;
	case Gizmo::XZ:	return Bounds() << x * s << (x + z) * s << z * s;
	}

	return Bounds();
}

// ------------------------------------------------------- TranslationToolSystem ------------------------------------------------------- //

// ** TranslationToolSystem::TranslationToolSystem
TranslationToolSystem::TranslationToolSystem( Editors::CursorWPtr cursor, Scene::SpectatorCameraWPtr camera ) : GenericEntitySystem( "TranslationToolSystem" ), m_cursor( cursor ), m_camera( camera )
{
	cursor->subscribe<Editors::Cursor::Pressed>( dcThisMethod( TranslationToolSystem::handleMousePressed ) );
	cursor->subscribe<Editors::Cursor::Released>( dcThisMethod( TranslationToolSystem::handleMouseReleased ) );
}

// ** TranslationToolSystem::process
void TranslationToolSystem::process( u32 currentTime, f32 dt, Ecs::Entity& entity, TranslationTool& tool, Scene::Transform& transform )
{
	// Get gizmo data
	Gizmo& gizmo = tool.gizmo();

	// Get the world space gizmo position.
	Vec3 position = transform.worldSpacePosition();

	// Calculate the scale factor of a gizmo.
	f32 scale = (m_camera->position() - position).length() * tool.scalingFactor();

	// Get the camera ray.
	const Ray& ray = m_cursor->ray();

	// Update the gizmo according to it's state
	switch( gizmo.state() ) {
	case Gizmo::Active:
	case Gizmo::Idle:	{
							Gizmo::Transform activeTransform = findTransformByRay( tool, scale, ray * transform.matrix().inversed() );
							gizmo.setActiveTransform( activeTransform );
						}
						break;
	case Gizmo::Locked:	{
							// Find the intersection point
							Vec3 intersection;
							bool hasIntersection = ray.intersects( gizmo.plane(), &intersection );
							DC_BREAK_IF( !hasIntersection );

							// Calculate final position value
							Vec3 result = position + gizmo.offset();

							// Cancel some axes
							switch( gizmo.activeTransform() ) {
							case Gizmo::X:  result.x = intersection.x;
											break;
							case Gizmo::Y:  result.y = intersection.y;
											break;
							case Gizmo::Z:  result.z = intersection.z;
											break;
							case Gizmo::XY: result.x = intersection.x;
											result.y = intersection.y;
											break;
							case Gizmo::YZ: result.y = intersection.y;
											result.z = intersection.z;
											break;
							case Gizmo::XZ: result.x = intersection.x;
											result.z = intersection.z;
											break;
							}

							result = result - gizmo.offset();

							// Apply transformation
							transform.setPosition( result );
						}
						break;
	}
}

// ** TranslationToolSystem::handleMousePressed
void TranslationToolSystem::handleMousePressed( const Editors::Cursor::Pressed& e )
{
	// Process only left mouse button clicks
	if( !e.buttons.is( Ui::LeftMouseButton ) ) {
		return;
	}

	// Get the entity set
	Ecs::EntitySet& entities = m_index->entities();

	// Get the camera ray.
	const Ray& ray = m_cursor->ray();

	// Process each gizmo and lock the selected.
	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		// Get the translation tool component.
		TranslationTool* tool = (*i)->get<TranslationTool>();

		// If this gizmo is not active - skip it
		if( !tool->isActive() ) {
			continue;
		}

		// Get the world space transform of this gizmo.
		Vec3 position = (*i)->get<Scene::Transform>()->worldSpacePosition();

		// Find the view ray and gizmo transform plane intersection
		Vec3 intersection;
		Plane plane = findBestPlane( tool->gizmo().activeTransform(), position, ray );
		bool hasIntersection = ray.intersects( plane, &intersection );
		DC_BREAK_IF( !hasIntersection );

		// Lock the gizmo until mouse is released
		tool->gizmo().lock( plane, intersection - position );
	}
}

// ** TranslationToolSystem::handleMouseReleased
void TranslationToolSystem::handleMouseReleased( const Editors::Cursor::Released& e )
{
	// Process only left mouse button clicks
	if( !e.buttons.is( Ui::LeftMouseButton ) ) {
		return;
	}

	// Get the entity set
	Ecs::EntitySet& entities = m_index->entities();

	// Unlock all locked gizmos
	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		// Get the translation tool component.
		TranslationTool* tool = (*i)->get<TranslationTool>();

		// If it's locked - unlock it
		if( tool->isLocked() ) {
			tool->gizmo().unlock();
		}
	}
}

// ** TranslationToolSystem::findBestPlane
Plane TranslationToolSystem::findBestPlane( Gizmo::Transform transform, const Vec3& position, const Ray& ray ) const
{
	// Construct planes
	Plane xy = Plane::calculate( Vec3::axisZ(), position );
	Plane yz = Plane::calculate( Vec3::axisX(), position );
	Plane xz = Plane::calculate( Vec3::axisY(), position );

	// Set of valid planes for each transform type
	Plane planes[][2] = {
		  { xy, xz }
		, { yz, xy }
		, { yz, xz }
		, { xy, xy }
		, { yz, yz }
		, { xz, xz }
	};

	// Select the best plane for active transform
	const Plane& a = planes[transform][0];
	const Plane& b = planes[transform][1];

	if( fabs( a.normal() * ray.direction() ) > fabs( b.normal() * ray.direction() ) ) {
		return a;
	}

	return b;
}

// ** TranslationToolSystem::findTransformByRay
Gizmo::Transform TranslationToolSystem::findTransformByRay( TranslationTool& tool, f32 scale, const Ray& ray ) const
{
	// Find the ray with a gizmo intersection.
	for( s32 i = 0; i < Gizmo::Nothing; i++ ) {
		// Type cast the counter to transform.
		Gizmo::Transform transform = static_cast<Gizmo::Transform>( i );

		// Get the selector bounding box
		Bounds bounds = tool.selectorBoundingBox( transform ) * scale;

		// Check for intersection.
		if( !ray.intersects( bounds ) ) {
			continue;
		}

		// Selector found - finish.
		return transform;
	}

	return Gizmo::Nothing;
}

// -------------------------------------------------------- TranslationToolPass -------------------------------------------------------- //

// ** TranslationToolPass::render
void TranslationToolPass::render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const TranslationTool& tool, const Scene::Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	// Push gizmo model transform.
	renderer->pushTransform( transform.matrix() );

	// Get the center point of a gizmo.
	Vec3 pos = transform.worldSpacePosition();

	// Calculate the scaling based on distance to camera.
	f32 scale = (m_transform->position() - pos).length() * tool.scalingFactor();

	// Get the selector bounding boxes.
	Bounds x  = tool.selectorBoundingBox( Gizmo::X  ) * scale;
	Bounds y  = tool.selectorBoundingBox( Gizmo::Y  ) * scale;
	Bounds z  = tool.selectorBoundingBox( Gizmo::Z  ) * scale;
	Bounds xz = tool.selectorBoundingBox( Gizmo::XZ ) * scale;
	Bounds yz = tool.selectorBoundingBox( Gizmo::YZ ) * scale;
	Bounds xy = tool.selectorBoundingBox( Gizmo::XY ) * scale;

	// Get active selector
	u8 selection = tool.gizmo().activeTransform();

	// Get the transform axes.
	Vec3 ax = Vec3::axisX() * scale;
	Vec3 ay = Vec3::axisY() * scale;
	Vec3 az = Vec3::axisZ() * scale;

	// Construct selecto colors.
	Rgba xColor  = selection == Gizmo::X  ? Gizmo::kActive : Gizmo::kRed;
	Rgba yColor  = selection == Gizmo::Y  ? Gizmo::kActive : Gizmo::kGreen;
	Rgba zColor  = selection == Gizmo::Z  ? Gizmo::kActive : Gizmo::kBlue;
	Rgba xzColor = selection == Gizmo::XZ ? Gizmo::kActive : Gizmo::kGreen;
	Rgba yzColor = selection == Gizmo::YZ ? Gizmo::kActive : Gizmo::kRed;
	Rgba xyColor = selection == Gizmo::XY ? Gizmo::kActive : Gizmo::kBlue;

	// Render XZ selector.
	renderer->box( xz, xzColor.transparent( 0.1f ) );
	renderer->wireBox( xz, xzColor );

	// Render YZ selector.
	renderer->box( yz, yzColor.transparent( 0.1f ) );
	renderer->wireBox( yz, yzColor );

	// Render XY selector.
	renderer->box( xy, xyColor.transparent( 0.1f ) );
	renderer->wireBox( xy, xyColor );

	// Render gizmo orths
	renderer->line( Vec3::zero(), x.rightCenter(), xColor );
	renderer->line( Vec3::zero(), y.topCenter(), yColor );
	renderer->line( Vec3::zero(), z.farCenter(), zColor );

	// Render cones
	renderer->wireCone( x.rightCenter() * 0.7f, x.rightCenter(), ay, az, tool.orthRadius(), 6, xColor );
	renderer->wireCone( y.topCenter()   * 0.7f, y.topCenter(),   ax, az, tool.orthRadius(), 6, yColor );
	renderer->wireCone( z.farCenter()   * 0.7f, z.farCenter(),   ax, ay, tool.orthRadius(), 6, zColor );

	// Pop gizmo transform matrix
	renderer->popTransform();
}

DC_END_COMPOSER