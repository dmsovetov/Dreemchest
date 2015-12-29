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
Bounds TranslationTool::selectorBoundingBox( u8 idx ) const
{
	// Get the axes vectors
	Vec3 x = Vec3::axisX();
	Vec3 y = Vec3::axisY();
	Vec3 z = Vec3::axisZ();

	// Store the scaling factors
	f32 r = m_orthRadius;
	f32 s = m_selectorScale;

	// Construct selector bounding boxes
	switch( idx ) {
	case X:		return Bounds() << x * s << x + (y + z) * r << x - (y + z) * r;
	case Y:		return Bounds() << y * s << y + (x + z) * r << y - (x + z) * r;
	case Z:		return Bounds() << z * s << z + (y + x) * r << z - (y + x) * r;
	case XY:	return Bounds() << x * s << (x + y) * s << y * s;
	case YZ:	return Bounds() << y * s << (y + z) * s << z * s;
	case XZ:	return Bounds() << x * s << (x + z) * s << z * s;
	default:	DC_BREAK;
	}

	return Bounds();
}

// ------------------------------------------------------- TranslationToolSystem ------------------------------------------------------- //

// ** TranslationToolSystem::TranslationToolSystem
TranslationToolSystem::TranslationToolSystem( Scene::ViewportWPtr viewport ) : GenericTouchSystem( viewport )
{

}

// ** TranslationToolSystem::touchMovedEvent
void TranslationToolSystem::touchMovedEvent( Scene::Viewport::TouchMoved& e, Ecs::Entity& entity, TranslationTool& tool, Scene::Transform& transform )
{
	// Get gizmo data
	Gizmo& gizmo = tool.gizmo();

	// Get the world space gizmo position.
	Vec3 position = transform.worldSpacePosition();

	// Calculate the scale factor of a gizmo.
	f32 scale = (m_viewport->eye() - position).length() * tool.scalingFactor();

	// Get the camera ray.
	const Ray& ray = m_viewport->ray();

	// Update the gizmo according to it's state
	switch( gizmo.state() ) {
	case Gizmo::Active:
	case Gizmo::Idle:	{
							u8 type = mapRayToAxis( tool, scale, ray * transform.matrix().inversed() );
							
							if( type != TranslationTool::Null ) {
								gizmo.activate( type );
							} else {
								gizmo.deactivate();
							}
						}
						break;
	case Gizmo::Locked:	{
							// Find initial intersection point.
							Vec3 start = mapRayToPoint( tool, transform, scale, gizmo.ray() );

							// Find current intersection point.
							Vec3 end = mapRayToPoint( tool, transform, scale, m_viewport->ray() );

							// Caclulate translation delta.
							Vec3 delta = end - start;

							// Calculate final offset.
							Vec3 offset;

							switch( gizmo.type() ) {
							case TranslationTool::X:	offset.x = delta.x;
														break;
							case TranslationTool::Y:	offset.y = delta.y;
														break;
							case TranslationTool::Z:	offset.z = delta.z;
														break;
							case TranslationTool::XY:	offset.x = delta.x;
														offset.y = delta.y;
														break;
							case TranslationTool::YZ:	offset.y = delta.y;
														offset.z = delta.z;
														break;
							case TranslationTool::XZ:	offset.x = delta.x;
														offset.z = delta.z;
														break;
							}

							// Apply translation
							transform.setPosition( gizmo.transform().position() + offset );
						}
						break;
	}
}

// ** TranslationToolSystem::touchBeganEvent
void TranslationToolSystem::touchBeganEvent( Scene::Viewport::TouchBegan& e, Ecs::Entity& entity, TranslationTool& tool, Scene::Transform& transform )
{
	// Process only left mouse button clicks
	if( !e.buttons.is( Ui::LeftMouseButton ) ) {
		return;
	}

	// Get the camera ray.
	const Ray& ray = m_viewport->ray();

	// Get the world space gizmo position.
	Vec3 position = transform.worldSpacePosition();

	// Calculate the scale factor of a gizmo.
	f32 scale = (m_viewport->eye() - position).length();

	// Map current view ray to an axis selector.
	u8 axis = mapRayToAxis( tool, scale, m_viewport->ray() * transform.matrix().inversed() );

	// Lock the gizmo until mouse is released
	if( axis != TranslationTool::Null ) {
		tool.gizmo().lock( transform, m_viewport->pos(), m_viewport->ray() );
	}
}

// ** TranslationToolSystem::touchEndedEvent
void TranslationToolSystem::touchEndedEvent( Scene::Viewport::TouchEnded& e, Ecs::Entity& entity, TranslationTool& tool, Scene::Transform& transform )
{
	// Process only left mouse button clicks
	if( !e.buttons.is( Ui::LeftMouseButton ) ) {
		return;
	}

	// If it's locked - unlock it
	if( tool.isLocked() ) {
		tool.gizmo().unlock();
	}
}

// ** TranslationToolSystem::mapRayToPoint
Vec3 TranslationToolSystem::mapRayToPoint( TranslationTool& tool, const Scene::Transform& transform, f32 scale, const Ray& ray )
{
	// Get the world space transform of this gizmo.
	Vec3 position = transform.worldSpacePosition();

	// Select the best plane for selected transform axis.
	Plane a, b, best;

	switch( tool.gizmo().type() ) {
	case TranslationTool::X:	a = Plane::xy( position );
								b = Plane::xz( position );
								break;
	case TranslationTool::Y:	a = Plane::yz( position );
								b = Plane::xy( position );
								break;
	case TranslationTool::Z:	a = Plane::yz( position );
								b = Plane::xz( position );
								break;
	case TranslationTool::XY:	a = b = Plane::xy( position );
								break;
	case TranslationTool::YZ:	a = b = Plane::yz( position );
								break;
	case TranslationTool::XZ:	a = b = Plane::xz( position );
								break;
	}

	best = fabs( a.normal() * ray.direction() ) > fabs( b.normal() * ray.direction() ) ? a : b;

	// Find the view ray and gizmo transform plane intersection
	Vec3 point;
	bool hasIntersection = ray.intersects( best, &point );
	DC_BREAK_IF( !hasIntersection );

	return point;
}

// ** TranslationToolSystem::mapRayToAxis
u8 TranslationToolSystem::mapRayToAxis( TranslationTool& tool, f32 scale, const Ray& ray ) const
{
	// Find the ray with a gizmo intersection.
	for( s32 i = 0; i < TranslationTool::Null; i++ ) {
		// Get the selector bounding box
		Bounds bounds = tool.selectorBoundingBox( i ) * scale;

		// Check for intersection.
		if( !ray.intersects( bounds ) ) {
			continue;
		}

		// Selector found - finish.
		return i;
	}

	return TranslationTool::Null;
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
	Bounds x  = tool.selectorBoundingBox( TranslationTool::X  ) * scale;
	Bounds y  = tool.selectorBoundingBox( TranslationTool::Y  ) * scale;
	Bounds z  = tool.selectorBoundingBox( TranslationTool::Z  ) * scale;
	Bounds xz = tool.selectorBoundingBox( TranslationTool::XZ ) * scale;
	Bounds yz = tool.selectorBoundingBox( TranslationTool::YZ ) * scale;
	Bounds xy = tool.selectorBoundingBox( TranslationTool::XY ) * scale;

	// Get active selector
	u8 type = tool.gizmo().type();

	// Get the transform axes.
	Vec3 ax = Vec3::axisX() * scale;
	Vec3 ay = Vec3::axisY() * scale;
	Vec3 az = Vec3::axisZ() * scale;

	// Construct selecto colors.
	Rgba xColor  = type == TranslationTool::X  ? Gizmo::kActive : Gizmo::kRed;
	Rgba yColor  = type == TranslationTool::Y  ? Gizmo::kActive : Gizmo::kGreen;
	Rgba zColor  = type == TranslationTool::Z  ? Gizmo::kActive : Gizmo::kBlue;
	Rgba xzColor = type == TranslationTool::XZ ? Gizmo::kActive : Gizmo::kGreen;
	Rgba yzColor = type == TranslationTool::YZ ? Gizmo::kActive : Gizmo::kRed;
	Rgba xyColor = type == TranslationTool::XY ? Gizmo::kActive : Gizmo::kBlue;

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