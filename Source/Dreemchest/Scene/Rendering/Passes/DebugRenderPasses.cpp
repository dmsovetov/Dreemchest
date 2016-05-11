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

#include "DebugRenderPasses.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------------------------------ DebugStaticMeshPass ------------------------------------------------------------------ //

// ** DebugStaticMeshPass::DebugStaticMeshPass
DebugStaticMeshPass::DebugStaticMeshPass( RenderingContext& context, RenderScene& renderScene )
    : StreamedRenderPass( context, renderScene, 96 )
{
}

// ** DebugStaticMeshPass::color
const Rgba& DebugStaticMeshPass::color( void ) const
{
    return m_color;
}

// ** DebugStaticMeshPass::setColor
void DebugStaticMeshPass::setColor( const Rgba& value )
{
    m_color = value;
}

// ** DebugStaticMeshPass::emitRenderOperations
void DebugStaticMeshPass::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const StaticMesh& staticMesh, const Transform& transform )
{
    emitWireBounds( frame, commands, stateStack, staticMesh.worldSpaceBounds(), m_color );
}

// ------------------------------------------------------------------ DebugLightPass ------------------------------------------------------------------ //

// ** DebugLightPass::DebugLightPass
DebugLightPass::DebugLightPass( RenderingContext& context, RenderScene& renderScene )
    : StreamedRenderPass( context, renderScene, 96 )
{
}

// ** DebugLightPass::color
const Rgba& DebugLightPass::color( void ) const
{
    return m_color;
}

// ** DebugLightPass::setColor
void DebugLightPass::setColor( const Rgba& value )
{
    m_color = value;
}

// ** DebugLightPass::emitRenderOperations
void DebugLightPass::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Light& light, const Transform& transform )
{
    // Calculate a light's local space bounding box
    f32    range  = light.range();
    Bounds bounds = Bounds( Vec3( -range, -range, -range ), Vec3( range, range, range ) );

    // Emit a bounding box to an output stream
    switch( light.type() ) {
    case LightType::Point:  emitWireBounds( frame, commands, stateStack, bounds * transform.matrix(), m_color );
                            break;
    case LightType::Spot:   emitFrustum( frame, commands, stateStack, light.cutoff() * 2.0f, 1.0f, 0.1f, light.range() * 2.0f, transform.matrix(), m_color );
                            break;
    }
}

// ------------------------------------------------------------------ DebugCameraPass ------------------------------------------------------------------ //

// ** DebugCameraPass::DebugCameraPass
DebugCameraPass::DebugCameraPass( RenderingContext& context, RenderScene& renderScene )
    : StreamedRenderPass( context, renderScene, 96 )
{
}

// ** DebugCameraPass::color
const Rgba& DebugCameraPass::color( void ) const
{
    return m_color;
}

// ** DebugCameraPass::setColor
void DebugCameraPass::setColor( const Rgba& value )
{
    m_color = value;
}

// ** DebugCameraPass::emitRenderOperations
void DebugCameraPass::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
{
    // By default a frustum aspect is 1.0f
    f32 aspect = 1.0f;

    // Inherit a camera frustum from a viewport
    if( const Viewport* viewport = entity.has<Viewport>() ) {
        aspect = viewport->aspect();
    }

    switch( camera.projection() ) {
    case Projection::Perspective:   emitFrustum( frame, commands, stateStack, camera.fov(), aspect, camera.near(), camera.far(), transform.matrix(), m_color );
                                    break;
    }
}

} // namespace Scene

DC_END_DREEMCHEST