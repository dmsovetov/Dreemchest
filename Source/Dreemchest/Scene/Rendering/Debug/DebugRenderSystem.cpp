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

#include "DebugRenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** DebugRenderSystem::DebugRenderSystem
DebugRenderSystem::DebugRenderSystem( RenderingContext& context, RenderScene& renderScene )
    : RenderSystem( context, renderScene )
	, m_vertexFormat( VertexFormat::Position | VertexFormat::Color )
{
	// Request an input layout for a 2D vertex format
	m_inputLayout  = context.requestInputLayout( m_vertexFormat );

	// Request a vertex buffer used for rendering
	m_vertexBuffer = context.requestVertexBuffer( NULL, MaxVerticesInBatch * m_vertexFormat.vertexSize() );

	// Create a sprite shader
	m_debugShader = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Debug.shader" );
}

// ** DebugRenderSystem::emitRenderOperations
void DebugRenderSystem::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const DebugRenderer& debugRenderer )
{
	// Push a shader sprite rendering state
	StateScope state = stateStack.newScope();
	state->bindVertexBuffer( m_vertexBuffer );
	state->bindInputLayout( m_inputLayout );
	state->bindProgram( m_context.internShader( m_debugShader ) );
    state->setBlend( Renderer::BlendSrcAlpha, Renderer::BlendInvSrcAlpha );

    // Get all static meshes, cameras and lights
    const RenderScene::StaticMeshes& staticMeshes = m_renderScene.staticMeshes();
    const RenderScene::Lights&       lights       = m_renderScene.lights();
    const RenderScene::Cameras&      cameras      = m_renderScene.cameras();

	// Begin batch by allocating a chunk of vertex data
    beginBatch( frame, commands, stateStack, Renderer::PrimLines, min2<s32>( 24 * staticMeshes.count(), MaxVerticesInBatch ) );

    // Emit each static mesh bounding box to an output stream
    for( s32 i = 0, n = staticMeshes.count(); i < staticMeshes.count(); i++ ) {
        emitWireBounds( frame, commands, stateStack, staticMeshes[i].mesh->worldSpaceBounds(), debugRenderer.staticMeshColor() );
    }

    // Emit each light bounding box to an output stream
    for( s32 i = 0, n = lights.count(); i < lights.count(); i++ ) {
        emitLight( frame, commands, stateStack, lights[i], debugRenderer.lightColor() );
    }

    // Emit a frustum for each camera.
    for( s32 i = 0, n = cameras.count(); i < cameras.count(); i++ ) {
        const Camera*   camera   = cameras[i].camera;
        const Viewport* viewport = cameras[i].viewport;

        switch( camera->projection() ) {
        case Projection::Perspective:   emitFrustum( frame, commands, stateStack, camera->fov(), viewport->aspect(), camera->near(), camera->far(), *cameras[i].matrix, debugRenderer.cameraColor() );
                                        break;
        }
    }

    // Flush an active batch
    flush( commands, stateStack );
}

// ** DebugRenderSystem::beginBatch
void DebugRenderSystem::beginBatch( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, Renderer::PrimitiveType primitive, s32 capacity )
{
    // Flush an active batch
    if( m_activeBatch.primitive != primitive ) {
        flush( commands, stateStack );
    }

    // And start a new one
    m_activeBatch.size      = 0;
    m_activeBatch.capacity  = capacity;
    m_activeBatch.primitive = primitive;
    m_activeBatch.stream    = frame.allocate( m_vertexFormat.vertexSize() * capacity );
}

// ** DebugRenderSystem::restartBatch
void DebugRenderSystem::restartBatch( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack )
{
    // Save an active batch state
    ActiveBatch activeBatch = m_activeBatch;

    // Flush an active batch
    flush( commands, stateStack );

    // And start a new one with same parameters
    beginBatch( frame, commands, stateStack, activeBatch.primitive, activeBatch.capacity );
}

// ** DebugRenderSystem::flush
void DebugRenderSystem::flush( RenderCommandBuffer& commands, RenderStateStack& stateStack )
{
    // Nothing to render - just skip
    if( m_activeBatch.size == 0 ) {
        return;
    }

	// Upload vertex data to a GPU buffer and emit a draw primitives command
	commands.uploadVertexBuffer( m_vertexBuffer, m_activeBatch.stream, m_activeBatch.size * m_vertexFormat.vertexSize() );
	commands.drawPrimitives( 0, Renderer::PrimLines, stateStack.states(), 0, m_activeBatch.size );

    // Reset an active batch state
    m_activeBatch = ActiveBatch();
}

// ** DebugRenderSystem::hasEnoughSpace
bool DebugRenderSystem::hasEnoughSpace( s32 additionalVertices ) const
{
    return (m_activeBatch.size + additionalVertices) <= m_activeBatch.capacity;
}

// ** DebugRenderSystem::emitLight
void DebugRenderSystem::emitLight( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, const Rgba& color )
{
    // Calculate a light's local space bounding box
    f32    range  = light.light->range();
    Bounds bounds = Bounds( Vec3( -range, -range, -range ), Vec3( range, range, range ) );

    // Emit a bounding box to an output stream
    switch( light.light->type() ) {
    case LightType::Point:  emitWireBounds( frame, commands, stateStack, bounds * *light.matrix, color );
                            break;
    case LightType::Spot:   emitFrustum( frame, commands, stateStack, light.light->cutoff() * 2.0f, 1.0f, 0.1f, light.light->range() * 2.0f, *light.matrix, color );
                            break;
    }
}

// ** DebugRenderSystem::emitFrustum
void DebugRenderSystem::emitFrustum( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, f32 fov, f32 aspect, f32 near, f32 far, const Matrix4& transform, const Rgba& color )
{
    // Calculate a tangent of a fov angle
    f32 fovTan2 = tanf( radians( fov * 0.5f ) );

    // Calculate near and far planes size
    f32 nh = fovTan2 * near;
    f32 nw = nh * aspect;
    f32 fh = fovTan2 * far;
    f32 fw = fh * aspect;

    // Construct a world space frustum vertices
    Vec3 worldSpaceVertices[] = {
          // Near plane
          transform * Vec4( -nw, -nh, -near )
        , transform * Vec4(  nw, -nh, -near )
        , transform * Vec4(  nw,  nh, -near )
        , transform * Vec4( -nw,  nh, -near )

          // Far plane
        , transform * Vec4( -fw, -fh, -far )
        , transform * Vec4(  fw, -fh, -far )
        , transform * Vec4(  fw,  fh, -far )
        , transform * Vec4( -fw,  fh, -far )

          // An origin point
        , transform * Vec4( 0.0f, 0.0f, 0.0f )
    };

    // Emit a near plane
    emitLine( frame, commands, stateStack, worldSpaceVertices[0], worldSpaceVertices[1], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[1], worldSpaceVertices[2], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[2], worldSpaceVertices[3], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[3], worldSpaceVertices[0], color );

    // Emit a far plane
    emitLine( frame, commands, stateStack, worldSpaceVertices[4], worldSpaceVertices[5], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[5], worldSpaceVertices[6], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[6], worldSpaceVertices[7], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[7], worldSpaceVertices[4], color );

    // Emit edges that are behind the near plane
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[0], color.transparent( 0.25f ) );
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[1], color.transparent( 0.25f ) );
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[2], color.transparent( 0.25f ) );
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[3], color.transparent( 0.25f ) );

    // Emit edges that are infront of the near plane
    emitLine( frame, commands, stateStack, worldSpaceVertices[0], worldSpaceVertices[4], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[1], worldSpaceVertices[5], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[2], worldSpaceVertices[6], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[3], worldSpaceVertices[7], color );
}

// ** DebugRenderSystem::emitVertex
void DebugRenderSystem::emitVertex( const Vec3& position, const Rgba& color )
{
    DC_ABORT_IF( m_activeBatch.size >= m_activeBatch.capacity, "batch overflow" );
    m_vertexFormat.setVertexAttribute( VertexFormat::Position, position, m_activeBatch.stream, m_activeBatch.size );
    m_vertexFormat.setVertexAttribute( VertexFormat::Color, color.toInteger(), m_activeBatch.stream, m_activeBatch.size );
    m_activeBatch.size++;
}

// ** DebugRenderSystem::emitLine
void DebugRenderSystem::emitLine( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Vec3& start, const Vec3& end, const Rgba& color )
{
    if( !hasEnoughSpace( 2 ) ) {
        restartBatch( frame, commands, stateStack );
    }

    emitVertex( start, color );
    emitVertex( end, color );
}

// ** DebugRenderSystem::emitWireBounds
void DebugRenderSystem::emitWireBounds( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Bounds& bounds, const Rgba& color )
{
    // Get a bounding box min and max values
    const Vec3& min = bounds.min();
    const Vec3& max = bounds.max();

    // Construct an extents vectors
    Vec3 x = Vec3( bounds.width(), 0.0f, 0.0f );
    Vec3 y = Vec3( 0.0f, bounds.height(), 0.0f );
    Vec3 z = Vec3( 0.0f, 0.0f, bounds.depth() );

    // Emit a bottom side of a bounding box
    emitLine( frame, commands, stateStack, min        , min + x,     color );
    emitLine( frame, commands, stateStack, min + x    , min + x + z, color );
    emitLine( frame, commands, stateStack, min + x + z, min + z,     color );
    emitLine( frame, commands, stateStack, min + z    , min,         color );

    // Emit a top side of a bounding box
    emitLine( frame, commands, stateStack, min + y        , min + x + y,     color );
    emitLine( frame, commands, stateStack, min + x + y    , min + x + z + y, color );
    emitLine( frame, commands, stateStack, min + x + z + y, min + z + y,     color );
    emitLine( frame, commands, stateStack, min + z + y    , min + y,         color );

    // Emit vertical lines
    emitLine( frame, commands, stateStack, min        , min + y,         color );
    emitLine( frame, commands, stateStack, min + x    , min + x + y,     color );
    emitLine( frame, commands, stateStack, min + x + z, min + x + z + y, color );
    emitLine( frame, commands, stateStack, min + z    , min + z + y,     color );
}

// ** DebugRenderSystem::emitBasis
void DebugRenderSystem::emitBasis( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Matrix4& transform )
{
    // Calculate a transformed basis origin
    Vec3 origin = transform * Vec3::zero();

    // Emit a line for each basis vector
    emitLine( frame, commands, stateStack, origin, Vec3( transform * Vec3::axisX() ), Rgba( 1.0f, 0.0f, 0.0f ) );
    emitLine( frame, commands, stateStack, origin, Vec3( transform * Vec3::axisY() ), Rgba( 0.0f, 1.0f, 0.0f ) );
    emitLine( frame, commands, stateStack, origin, Vec3( transform * Vec3::axisZ() ), Rgba( 0.0f, 0.0f, 1.0f ) );
}

} // namespace Scene

DC_END_DREEMCHEST