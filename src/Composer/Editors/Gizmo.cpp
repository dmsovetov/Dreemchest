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

#include "Gizmo.h"

DC_BEGIN_COMPOSER

// ---------------------------------------------------------------- Grid ---------------------------------------------------------------- //

// ** Grid::size
f32 Grid::size( void ) const
{
	return m_size;
}

// ** Grid::cellSize
f32 Grid::cellSize( void ) const
{
	return m_cellSize;
}

// ** Grid::features
const FlagSet8& Grid::features( void ) const
{
	return m_features;
}

// ---------------------------------------------------------- TranslationGizmo ---------------------------------------------------------- //

// ** TranslationGizmo::TranslationGizmo
TranslationGizmo::TranslationGizmo( f32 orthRadius, f32 scalingFactor, f32 selectorScale )
	: m_scalingFactor( scalingFactor ), m_orthRadius( orthRadius ), m_selectorScale( selectorScale )
{
}

// ** TranslationGizmo::scalingFactor
f32 TranslationGizmo::scalingFactor( void ) const
{
	return m_scalingFactor;
}

// ** TranslationGizmo::orthRadius
f32 TranslationGizmo::orthRadius( void ) const
{
	return m_orthRadius;
}

// ** TranslationGizmo::activeTransform
Gizmo::Transform TranslationGizmo::activeTransform( void ) const
{
	return m_gizmo.transform;
}

// ** TranslationGizmo::setActiveTransform
void TranslationGizmo::setActiveTransform( Gizmo::Transform value )
{
	m_gizmo.transform = value;
	m_gizmo.state	  = value == Gizmo::Nothing ? Gizmo::Idle : Gizmo::Active;
}

// ** TranslationGizmo::state
Gizmo::State TranslationGizmo::state( void ) const
{
	return m_gizmo.state;
}

// ** TranslationGizmo::lock
void TranslationGizmo::lock( const Plane& plane, const Vec3& offset )
{
	DC_BREAK_IF( state() != Gizmo::Active );
	m_gizmo.offset = offset;
	m_gizmo.state  = Gizmo::Locked;
	m_gizmo.plane  = plane;
}

// ** TranslationGizmo::unlock
void TranslationGizmo::unlock( void )
{
	DC_BREAK_IF( state() != Gizmo::Locked );
	m_gizmo.state = Gizmo::Idle;
}

// ** TranslationGizmo::offset
const Vec3& TranslationGizmo::offset( void ) const
{
	return m_gizmo.offset;
}

// ** TranslationGizmo::plane
const Plane& TranslationGizmo::plane( void ) const
{
	return m_gizmo.plane;
}

// ** TranslationGizmo::selectorBoundingBox
Bounds TranslationGizmo::selectorBoundingBox( Gizmo::Transform transform ) const
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

// ------------------------------------------------------- TranslationGizmoSystem ------------------------------------------------------- //

// ** TranslationGizmoSystem::TranslationGizmoSystem
TranslationGizmoSystem::TranslationGizmoSystem( Scene::SpectatorCameraWPtr camera ) : GenericEntitySystem( "TranslationGizmoSystem" ), m_camera( camera ), m_buttons( 0 )
{

}

// ** TranslationGizmoSystem::setCursor
void TranslationGizmoSystem::setCursor( u32 x, u32 y, u8 buttons )
{
	m_cursor.x = x;
	m_cursor.y = y;
	m_buttons  = buttons;
}

// ** TranslationGizmoSystem::process
void TranslationGizmoSystem::process( u32 currentTime, f32 dt, Ecs::Entity& entity, TranslationGizmo& gizmo, Scene::Transform& transform )
{
	// Get the world space gizmo position.
	Vec3 position = transform.worldSpacePosition();

	// Calculate the scale factor of a gizmo.
	f32 scale = (m_camera->position() - position).length() * gizmo.scalingFactor();

	// Construct the view ray from cursor coordinates.
	Ray ray = m_camera->toWorldSpace( m_cursor.x, m_cursor.y );

	// Find the highlighted transform.
	Gizmo::Transform activeTransform = findTransformByRay( gizmo, scale, ray * transform.matrix().inversed() );

	// Intersection point
	Vec3 intersection;

	switch( gizmo.state() ) {
	case Gizmo::Idle:	gizmo.setActiveTransform( activeTransform );
						break;

	case Gizmo::Active:	gizmo.setActiveTransform( activeTransform );

						if( m_buttons.is( Ui::LeftMouseButton ) && gizmo.activeTransform() != Gizmo::Nothing ) {
							Plane plane = findBestPlane( gizmo.activeTransform(), position, ray );
							bool hasIntersection = ray.intersects( plane, intersection );
							DC_BREAK_IF( !hasIntersection );

							gizmo.lock( plane, intersection - position );
						}
						break;

	case Gizmo::Locked:	if( m_buttons.is( Ui::LeftMouseButton ) ) {
							// Find the intersection point
							bool hasIntersection = ray.intersects( gizmo.plane(), intersection );
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

							// Apply transformation
							transform.setPosition( result - gizmo.offset() );
						}
						else {
							gizmo.unlock();
						}
						break;
	}
}

// ** TranslationGizmoSystem::findBestPlane
Plane TranslationGizmoSystem::findBestPlane( Gizmo::Transform transform, const Vec3& position, const Ray& ray ) const
{
	// Construct planes
	Plane xy = Plane::calculate( Vec3::axisZ(), position );
	Plane yz = Plane::calculate( Vec3::axisX(), position );
	Plane xz = Plane::calculate( Vec3::axisY(), position );

	// Set of valid planes for each transform type
	Plane planes[][2] = {
		  { Plane(), Plane() }
		, { xy, xz }
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

// ** TranslationGizmoSystem::findTransformByRay
Gizmo::Transform TranslationGizmoSystem::findTransformByRay( TranslationGizmo& gizmo, f32 scale, const Ray& ray ) const
{
	// Find the ray with a gizmo intersection.
	for( s32 i = Gizmo::Nothing + 1; i < Gizmo::TotalTransforms; i++ ) {
		// Type cast the counter to transform.
		Gizmo::Transform transform = static_cast<Gizmo::Transform>( i );

		// Get the selector bounding box
		Bounds bounds = gizmo.selectorBoundingBox( transform ) * scale;

		// Check for intersection.
		if( !ray.intersects( bounds ) ) {
			continue;
		}

		// Selector found - finish.
		return transform;
	}

	return Gizmo::Nothing;
}

// -------------------------------------------------------- TranslationGizmoPass -------------------------------------------------------- //

// ** TranslationGizmoPass::render
void TranslationGizmoPass::render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const TranslationGizmo& gizmo, const Scene::Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	// Push gizmo model transform.
	renderer->pushTransform( transform.matrix() );

	// Get the center point of a gizmo.
	Vec3 pos = transform.worldSpacePosition();

	// Calculate the scaling based on distance to camera.
	f32 scale = (m_transform->position() - pos).length() * gizmo.scalingFactor();

	// Get the selector bounding boxes.
	Bounds x  = gizmo.selectorBoundingBox( Gizmo::X  ) * scale;
	Bounds y  = gizmo.selectorBoundingBox( Gizmo::Y  ) * scale;
	Bounds z  = gizmo.selectorBoundingBox( Gizmo::Z  ) * scale;
	Bounds xz = gizmo.selectorBoundingBox( Gizmo::XZ ) * scale;
	Bounds yz = gizmo.selectorBoundingBox( Gizmo::YZ ) * scale;
	Bounds xy = gizmo.selectorBoundingBox( Gizmo::XY ) * scale;

	// Get active selector
	u8 selection = gizmo.activeTransform();

	// Get the transform axes.
	Vec3 ax = Vec3::axisX() * scale;
	Vec3 ay = Vec3::axisY() * scale;
	Vec3 az = Vec3::axisZ() * scale;

	// Construct colors.
	Rgba red    = Rgba::fromBytes( 240, 68, 31 );
	Rgba green  = Rgba::fromBytes( 187, 237, 82 );
	Rgba blue   = Rgba::fromBytes( 61, 133, 236 );
	Rgba active = Rgba::fromBytes( 255, 255, 0 );

	// Construct selecto colors.
	Rgba xColor  = selection == Gizmo::X  ? active : red;
	Rgba yColor  = selection == Gizmo::Y  ? active : green;
	Rgba zColor  = selection == Gizmo::Z  ? active : blue;
	Rgba xzColor = selection == Gizmo::XZ ? active : green;
	Rgba yzColor = selection == Gizmo::YZ ? active : red;
	Rgba xyColor = selection == Gizmo::XY ? active : blue;

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
	renderer->wireCone( x.rightCenter() * 0.7f, x.rightCenter(), ay, az, gizmo.orthRadius(), 6, xColor );
	renderer->wireCone( y.topCenter()   * 0.7f, y.topCenter(),   ax, az, gizmo.orthRadius(), 6, yColor );
	renderer->wireCone( z.farCenter()   * 0.7f, z.farCenter(),   ax, ay, gizmo.orthRadius(), 6, zColor );

	// Pop gizmo transform matrix
	renderer->popTransform();
}

// -------------------------------------------------------- GridPass -------------------------------------------------------- //

// ** GridPass::render
void GridPass::render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const Grid& grid, const Scene::Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	// Get grid features
	const FlagSet8& features = grid.features();

	// Render grid cells
	if( features.is( Grid::Cells ) ) {
		// Render with a cell size
		renderGrid( renderer, grid.cellSize() * 1.0f, grid.size(), Rgba( 0.0f, 0.0f, 0.0f, 0.05f ) );

		// Render with a cell x5 size
		renderGrid( renderer, grid.cellSize() * 5.0f, grid.size(), Rgba( 0.0f, 0.0f, 0.0f, 0.1f ) );
	}

	// Render axes
	if( features.is( Grid::Axes ) ) {
		renderCross( renderer, grid.size(), Rgba( 1.0f, 1.0f, 1.0f, 0.2f ) );
	}

	// Render basis
	if( features.is( Grid::Basis ) ) {
		renderer->line( Vec3( 0.0f, 0.0f, 0.0f ), Vec3( 1.0f, 0.0f, 0.0f ), Rgba::fromBytes( 240,  68,  31 ) );
		renderer->line( Vec3( 0.0f, 0.0f, 0.0f ), Vec3( 0.0f, 1.0f, 0.0f ), Rgba::fromBytes( 187, 237,  82 ) );
		renderer->line( Vec3( 0.0f, 0.0f, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ), Rgba::fromBytes(  61, 133, 236 ) );
	}

	// Render outline
	if( features.is( Grid::Outline ) ) {
		f32 s = grid.size() * 0.5f;
		renderer->wireBox( Bounds( Vec3( -s, 0.0f, -s ), Vec3( s, 0.0f, s ) ), Rgba( 1.0f, 1.0f, 0.0f, 0.2f ) );
	}
}

// ** GridPass::renderGrid
void GridPass::renderGrid( Renderer::Renderer2DWPtr renderer, f32 cellSize, f32 size, const Rgba& color ) const
{
	s32 n = floor( size / cellSize ) / 2;

	for( s32 i = -n; i <= n; i++ ) {
		renderer->line( Vec3( i * cellSize, 0.0f, -cellSize * n ), Vec3( i * cellSize, 0.0f, cellSize * n ), color );
	}

	for( s32 i = -n; i <= n; i++ ) {
		renderer->line( Vec3( -cellSize * n, 0.0f, i * cellSize ), Vec3( cellSize * n, 0.0f, i * cellSize ), color );
	}
}

// ** GridPass::renderCross
void GridPass::renderCross( Renderer::Renderer2DWPtr renderer, f32 size, const Rgba& color ) const
{
	f32 s = size * 0.5f;

	renderer->line( Vec3( -s, 0.0f, 0.0f ), Vec3( s, 0.0f, 0.0f ), color );
	renderer->line( Vec3( 0.0f, 0.0f, -s ), Vec3( 0.0f, 0.0f, s ), color );
}


DC_END_COMPOSER