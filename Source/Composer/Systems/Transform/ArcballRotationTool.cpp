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
    NIMBLE_BREAK_IF( m_isLocked );

    m_initialRotation    = rotation;
    m_initialDirection    = direction;
    m_isLocked            = true;
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
    NIMBLE_BREAK_IF( !m_isLocked );
    m_isLocked = false;
}

#if DEV_DEPRECATED_SCENE_INPUT
// ------------------------------------------------------- ArcballRotationToolSystem ------------------------------------------------------- //

// ** ArcballRotationToolSystem::ArcballRotationToolSystem
ArcballRotationToolSystem::ArcballRotationToolSystem( Scene::ViewportWPtr viewport ) : GenericTouchSystem( viewport )
{

}

// ** ArcballRotationToolSystem::touchMovedEvent
void ArcballRotationToolSystem::touchMovedEvent( Scene::Viewport::TouchMoved& e, Ecs::Entity& entity, ArcballRotationTool& tool, Scene::Transform& transform )
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
    mapToVector( tool, transform, m_viewport->pos(), direction );

    Vec3 v0 = m_viewport->camera()->get<Scene::Transform>()->rotation().rotate( tool.initialDirection() );
    Vec3 v1 = m_viewport->camera()->get<Scene::Transform>()->rotation().rotate( direction );

    Quat drag = Quat( v0 % v1, v0 * v1 );
    transform.setRotation( drag * tool.initialRotation() );
}

// ** ArcballRotationToolSystem::touchBeganEvent
void ArcballRotationToolSystem::touchBeganEvent( Scene::Viewport::TouchBegan& e, Ecs::Entity& entity, ArcballRotationTool& arcball, Scene::Transform& transform )
{
    // Process only left mouse button clicks
    if( !e.buttons.is( Ui::MouseButtons::Left ) ) {
        return;
    }

    // Map cursor coordinates to a direction on an arcball sphere
    Vec3 direction;
    if( !mapToVector( arcball, transform, m_viewport->pos(), direction ) ) {
        return;
    }

    // Lock an arcball
    arcball.lock( transform.rotation(), direction );
}

// ** ArcballRotationToolSystem::mapToVector
bool ArcballRotationToolSystem::mapToVector( const ArcballRotationTool& arcball, const Scene::Transform& transform, const Vec2& cursor, Vec3& direction ) const
{
    // Calculate arcball radius based on distance to camera.
    f32 radius = arcball.radius() * (m_viewport->eye() - transform.worldSpacePosition()).length();

    // Construct the arcball sphere.
    Sphere sphere = Sphere( transform.worldSpacePosition(), radius );

    // Project it onto the screen plane.
    Circle circle = m_viewport->camera()->get<Scene::Camera>()->sphereToScreenSpace( sphere, m_viewport->camera()->get<Scene::Transform>() );

    // Map the cursor point to a vector on a sphere.
    return circle.mapToSphere( cursor, direction );
}

// ** ArcballRotationToolSystem::touchEndedEvent
void ArcballRotationToolSystem::touchEndedEvent( Scene::Viewport::TouchEnded& e, Ecs::Entity& entity, ArcballRotationTool& arcball, Scene::Transform& transform )
{
    // Process only left mouse button clicks
    if( !e.buttons.is( Ui::MouseButtons::Left ) ) {
        return;
    }

    // If it's locked - unlock it
    if( arcball.isLocked() ) {
        arcball.unlock();
    }
}
#endif  /*  #if DEV_DEPRECATED_SCENE_INPUT  */

#if DEV_DEPRECATED_SCENE_RENDERER

// -------------------------------------------------------------- ArcballRotationToolPass -------------------------------------------------------------- //

// ** ArcballRotationToolPass::render
void ArcballRotationToolPass::render( Scene::RenderingContextPtr context, Scene::RenderingContext& rvm, Scene::ShaderCache& shaders, const ArcballRotationTool& tool, const Scene::Transform& transform )
{
    // Get batched renderer interface
    Renderer::Renderer2DWPtr renderer = context->renderer();

    // Get the scale factor based on distance to camera
    f32 scale = (m_transform->position() - transform.worldSpacePosition()).length();
    
    // Get the camera axes to align the arcball circle
    Vec3 side = m_transform->axisX();
    Vec3 up      = m_transform->axisY();

    // Render the circle
    renderer->wireCircle( transform.position(), side, up, tool.radius() * scale, 32, Rgba( 0.6f, 0.6f, 0.6f ) );
}

#endif  /*  DEV_DEPRECATED_SCENE_RENDERER   */

DC_END_COMPOSER
