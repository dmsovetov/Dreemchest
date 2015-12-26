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

#include "RotationTool.h"

DC_BEGIN_COMPOSER

// ---------------------------------------------------------- RotationTool ---------------------------------------------------------- //

// ** RotationTool::RotationTool
RotationTool::RotationTool( f32 scalingFactor ) : m_scalingFactor( scalingFactor )
{
}

// ** RotationTool::scalingFactor
f32 RotationTool::scalingFactor( void ) const
{
	return m_scalingFactor;
}

// ** RotationTool::gizmo
const Gizmo& RotationTool::gizmo( void ) const
{
	return m_gizmo;
}

// ** RotationTool::gizmo
Gizmo& RotationTool::gizmo( void )
{
	return m_gizmo;
}

// ------------------------------------------------------- RotationToolSystem ------------------------------------------------------- //

// ** RotationToolSystem::RotationToolSystem
RotationToolSystem::RotationToolSystem( Scene::SpectatorCameraWPtr camera ) : GenericEntitySystem( "RotationToolSystem" ), m_camera( camera ), m_buttons( 0 )
{

}

// ** RotationToolSystem::setCursor
void RotationToolSystem::setCursor( u32 x, u32 y, u8 buttons )
{
	m_cursor.x = x;
	m_cursor.y = y;
	m_buttons  = buttons;
}

// ** RotationToolSystem::process
void RotationToolSystem::process( u32 currentTime, f32 dt, Ecs::Entity& entity, RotationTool& tool, Scene::Transform& transform )
{
	// Get the world space gizmo position.
	Vec3 position = transform.worldSpacePosition();

	// Calculate the scale factor of a gizmo.
	f32 scale = (m_camera->position() - position).length() /** tool.scalingFactor()*/;

	// Construct the view ray from cursor coordinates.
	Ray ray = m_camera->toWorldSpace( m_cursor.x, m_cursor.y );

	// Intersection point
	Vec3 intersection;
	Plane plane;

	// Find the highlighted transform.
	Gizmo::Transform activeTransform = findTransformByRay( tool, position, plane, intersection, scale, ray * transform.matrix().inversed() );

	// Get gizmo data
	Gizmo& gizmo = tool.gizmo();

	static s32 lastCursor = 0;

	switch( gizmo.state() ) {
	case Gizmo::Idle:	gizmo.setActiveTransform( activeTransform );
						break;

	case Gizmo::Active:	gizmo.setActiveTransform( activeTransform );

						if( m_buttons.is( Ui::LeftMouseButton ) && gizmo.activeTransform() != Gizmo::Nothing ) {
							gizmo.lock( plane, intersection );
							lastCursor = m_cursor.y;
						}
						break;

	case Gizmo::Locked:	if( m_buttons.is( Ui::LeftMouseButton ) ) {
							// Find the intersection point
							bool hasIntersection = ray.intersects( gizmo.plane(), &intersection );
							DC_BREAK_IF( !hasIntersection );

							// 
							f32 d = 0.0f;
							Vec3 a;

							// Calculate final position value
							switch( gizmo.activeTransform() ) {
							case Gizmo::X:	{
												a = transform.axisX();
												d = (m_cursor.y - lastCursor);
												lastCursor = m_cursor.y;
											}
											break;
							case Gizmo::Y:	{
												a = transform.axisY();
												d = (m_cursor.y - lastCursor);
												lastCursor = m_cursor.y;
											}
											break;
							case Gizmo::Z:	{
												a = transform.axisZ();
												d = (m_cursor.y - lastCursor);
												lastCursor = m_cursor.y;
											}
											break;
							}

							transform.rotate( d, a.x, a.y, a.z );
						}
						else {
							gizmo.unlock();
						}
						break;
	}
}

// ** RotationToolSystem::findTransformByRay
Gizmo::Transform RotationToolSystem::findTransformByRay( RotationTool& tool, const Vec3& position, Plane& plane, Vec3& intersection, f32 scale, const Ray& ray ) const
{
	// Construct planes
	Plane planes[] = {
		  Plane::calculate( Vec3::axisX(), position )
		, Plane::calculate( Vec3::axisY(), position )
		, Plane::calculate( Vec3::axisZ(), position )
	};

	f32  distance = FLT_MAX;
	f32  radius = tool.scalingFactor() * scale;
	Gizmo::Transform transform = Gizmo::Nothing;

	for( s32 i = 0; i < 3; i++ ) {
		if( !ray.intersects( planes[i], &intersection ) ) {
			continue;
		}

		f32 d = (position - intersection).length();

		if( fabs( radius - d ) > 0.01f * scale ) {
			continue;
		}

		plane = planes[i];
		transform = static_cast<Gizmo::Transform>( i );
	}

	return transform;
}

// -------------------------------------------------------- RotationToolPass -------------------------------------------------------- //

// ** RotationToolPass::render
void RotationToolPass::render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const RotationTool& tool, const Scene::Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	// Get the center point of a gizmo.
	Vec3 pos = transform.worldSpacePosition();

	// Calculate the scaling based on distance to camera.
	f32 scale = (m_transform->position() - pos).length();

	// Get the transform axes.
	Vec3 ax = Vec3::axisX() * scale;
	Vec3 ay = Vec3::axisY() * scale;
	Vec3 az = Vec3::axisZ() * scale;

	// Get active selector
	u8 selection = tool.gizmo().activeTransform();

	// Construct selecto colors.
	Rgba xColor  = selection == Gizmo::X  ? Gizmo::kActive : Gizmo::kRed;
	Rgba yColor  = selection == Gizmo::Y  ? Gizmo::kActive : Gizmo::kGreen;
	Rgba zColor  = selection == Gizmo::Z  ? Gizmo::kActive : Gizmo::kBlue;

	// Get the camera view direction
	Vec3 view = m_transform->axisZ();

	// Push gizmo model transform.
	renderer->pushTransform( transform.matrix() );

	// Render circle for X axis.
	renderer->wireHalfCircle( Vec3( 0.0f, 0.0f, 0.0f ), ay, az, Vec3( 0.0f, view.y, view.z ), tool.scalingFactor(), 32, xColor );

	// Render circle for Y axis.
	renderer->wireHalfCircle( Vec3( 0.0f, 0.0f, 0.0f ), ax, az, Vec3( view.x, 0.0f, view.z ), tool.scalingFactor(), 32, yColor );

	// Render circle for Z axis.
	renderer->wireHalfCircle( Vec3( 0.0f, 0.0f, 0.0f ), ax, ay, Vec3( view.x, view.y, 0.0f ), tool.scalingFactor(), 32, zColor );

	// Pop gizmo transform matrix
	renderer->popTransform();

	renderer->box( Bounds( tool.gizmo().offset() - Vec3( 0.01f, 0.01f, 0.01f ), tool.gizmo().offset() + Vec3( 0.01f, 0.01f, 0.01f ) ) );
}

DC_END_COMPOSER