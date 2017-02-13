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
RotationTool::RotationTool( f32 radius, f32 screenSpaceRadiusScale, f32 width ) : m_radius( radius ), m_screenSpaceRadiusScale( screenSpaceRadiusScale ), m_width( width )
{
}

// ** RotationTool::width
f32 RotationTool::width( void ) const
{
    return m_width;
}

// ** RotationTool::radius
f32 RotationTool::radius( void ) const
{
    return m_radius;
}

// ** RotationTool::screenSpaceRadius
f32 RotationTool::screenSpaceRadius( void ) const
{
    return radius() * m_screenSpaceRadiusScale;
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

// ** RotationTool::isLocked
bool RotationTool::isLocked( void ) const
{
    return m_gizmo.state() == Gizmo::Locked;
}

#if DEV_DEPRECATED_SCENE_INPUT
// ------------------------------------------------------- RotationToolSystem ------------------------------------------------------- //

// ** RotationToolSystem::RotationToolSystem
RotationToolSystem::RotationToolSystem( Scene::ViewportWPtr viewport ) : GenericTouchSystem( viewport )
{

}

// ** RotationToolSystem::touchMovedEvent
void RotationToolSystem::touchMovedEvent( Scene::Viewport::TouchMoved& e, Ecs::Entity& entity, RotationTool& tool, Scene::Transform& transform )
{
    // Get gizmo data
    Gizmo& gizmo = tool.gizmo();

    // Get the world space gizmo position.
    Vec3 position = transform.worldSpacePosition();

    // Calculate the scale factor of a gizmo.
    f32 scale = (m_viewport->eye() - position).length();

    // Get the camera ray.
    const Ray& ray = m_viewport->ray();

    // Update the gizmo according to it's state
    switch( gizmo.state() ) {
    case Gizmo::Active:
    case Gizmo::Idle:    {
                            u8 axis = mapRayToAxis( tool, transform, scale, ray * transform.matrix().inversed(), Vec2() );
                            if( axis != RotationTool::Null ) {
                                gizmo.activate( axis );
                            } else {
                                gizmo.deactivate();
                            }
                        }
                        break;
    case Gizmo::Locked:    {
                            f32  a        = gizmo.tangent() * gizmo.cursor();
                            f32  b        = gizmo.tangent() * m_viewport->pos();
                            f32  angle    = b - a;
                            Quat rotation;

                            // Calculate the rotation quaternion
                            switch( gizmo.type() ) {
                            case RotationTool::Screen:    rotation = Quat::rotateAroundAxis( angle, Vec3::normalize( transform.position() - m_viewport->eye() ) );
                                                        break;
                            case RotationTool::X:       rotation = Quat::rotateAroundAxis( angle, transform.axisX() );
                                                        break;
                            case RotationTool::Y:       rotation = Quat::rotateAroundAxis( angle, transform.axisY() );
                                                        break;
                            case RotationTool::Z:       rotation = Quat::rotateAroundAxis( angle, transform.axisZ() );
                                                        break;
                            }

                            // Apply rotation
                            transform.setRotation( rotation * gizmo.transform()->rotation() );
                        }
                        break;
    }
}

// ** RotationToolSystem::mapRayToAxis
u8 RotationToolSystem::mapRayToAxis( RotationTool& tool, Scene::Transform& transform, f32 scale, const Ray& ray, Vec2& axis ) const
{
    // Construct planes
    Plane planes[] = { Plane::yz(), Plane::xz(), Plane::xy() };

    // Calculate scaled radius
    f32 radius = tool.radius() * scale;

    // Project the center point of a tool to a screen space.
    Vec3 center;
    m_viewport->camera()->get<Scene::Camera>()->pointToScreenSpace( transform.worldSpacePosition(), center, m_viewport->camera()->get<Scene::Transform>()->matrix() );

    // Find the ray with an axis rotation indicators.
    for( s32 i = 0; i < RotationTool::Arcball; i++ ) {
        // Find the intersection point with plane and view ray.
        Vec3 point;

        if( !ray.intersects( planes[i], &point ) ) {
            continue;
        }

        // Calculate distance to a center of a gizmo
        f32 distance = point.length();

        if( fabs( distance - radius ) <= tool.width() * scale * 0.001f ) {
            axis = Vec2::normalized( m_viewport->pos() - ( Vec2 )center ).perp();
            return i;
        }
    }

    // Do we have the intersection with a screen space rotation indicator?
    Circle circle = m_viewport->camera()->get<Scene::Camera>()->sphereToScreenSpace( Sphere( transform.worldSpacePosition(), tool.screenSpaceRadius() * scale ), m_viewport->camera()->get<Scene::Transform>() );

    if( circle.isCircumference( m_viewport->pos(), tool.width() ) ) {
        axis = circle.nearestTangent( m_viewport->pos() );
        return RotationTool::Screen;
    }

    return RotationTool::Null;
}

// ** RotationToolSystem::touchBeganEvent
void RotationToolSystem::touchBeganEvent( Scene::Viewport::TouchBegan& e, Ecs::Entity& entity, RotationTool& tool, Scene::Transform& transform )
{
    // Process only left mouse button clicks
    if( !e.buttons.is( Ui::MouseButtons::Left ) ) {
        return;
    }

    // Get the camera ray.
    const Ray& ray = m_viewport->ray();

    // Get the world space gizmo position.
    Vec3 position = transform.worldSpacePosition();

    // Calculate the scale factor of a gizmo.
    f32 scale = (m_viewport->eye() - position).length();

    // Find the highlighted transform.
    Vec2 axis;
    u8 activeTransform = mapRayToAxis( tool, transform, scale, ray * transform.matrix().inversed(), axis );

    if( activeTransform == RotationTool::Null ) {
        return;
    }

    // Lock the tool.
    tool.gizmo().lock( transform, m_viewport->pos(), m_viewport->ray(), axis );
}

// ** RotationToolSystem::touchEndedEvent
void RotationToolSystem::touchEndedEvent( Scene::Viewport::TouchEnded& e, Ecs::Entity& entity, RotationTool& tool, Scene::Transform& transform )
{
    // Process only left mouse button clicks
    if( !e.buttons.is( Ui::MouseButtons::Left ) ) {
        return;
    }

    // If it's locked - unlock it
    if( tool.isLocked() ) {
        tool.gizmo().unlock();
    }
}
#endif  /*  #if DEV_DEPRECATED_SCENE_INPUT  */

#if DEV_DEPRECATED_SCENE_RENDERER

// -------------------------------------------------------- RotationToolPass -------------------------------------------------------- //

// ** RotationToolPass::render
void RotationToolPass::render( Scene::RenderingContextPtr context, Scene::RenderingContext& rvm, Scene::ShaderCache& shaders, const RotationTool& tool, const Scene::Transform& transform )
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
    u8 type = tool.gizmo().type();

    // Construct selecto colors.
    Rgba xColor  = type == RotationTool::X        ? Gizmo::kActive : Gizmo::kRed;
    Rgba yColor  = type == RotationTool::Y        ? Gizmo::kActive : Gizmo::kGreen;
    Rgba zColor  = type == RotationTool::Z        ? Gizmo::kActive : Gizmo::kBlue;
    Rgba sColor  = type == RotationTool::Screen ? Gizmo::kActive : Gizmo::kScreen;

    // Get the camera view direction
    Vec3 view = m_transform->axisZ();

    // Push gizmo model transform.
    renderer->pushTransform( transform.matrix() );
    {
        // Render circle for X axis.
        renderer->wireCircle( Vec3( 0.0f, 0.0f, 0.0f ), ay, az, tool.radius(), 32, xColor );

        // Render circle for Y axis.
        renderer->wireCircle( Vec3( 0.0f, 0.0f, 0.0f ), ax, az, tool.radius(), 32, yColor );

        // Render circle for Z axis.
        renderer->wireCircle( Vec3( 0.0f, 0.0f, 0.0f ), ax, ay, tool.radius(), 32, zColor );
    }
    // Pop gizmo transform matrix
    renderer->popTransform();

    // Render tilt circle
    {
        // Get the camera axes to align the circle
        Vec3 side = m_transform->axisX();
        Vec3 up      = m_transform->axisY();

        // Render the circle
        renderer->wireCircle( transform.position(), side, up, tool.screenSpaceRadius() * scale, 32, sColor );
    }
}

#endif  /*  DEV_DEPRECATED_SCENE_RENDERER   */

DC_END_COMPOSER
