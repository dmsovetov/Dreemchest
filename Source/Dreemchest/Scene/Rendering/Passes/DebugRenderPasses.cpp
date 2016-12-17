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

// ** DebugStaticMeshPass::emitRenderOperations
void DebugStaticMeshPass::emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const StaticMesh& staticMesh, const Transform& transform )
{
    emitWireBounds( frame, commands, stateStack, staticMesh.worldSpaceBounds() );
}

// ------------------------------------------------------------------ DebugLightPass ------------------------------------------------------------------ //

// ** DebugLightPass::DebugLightPass
DebugLightPass::DebugLightPass( RenderingContext& context, RenderScene& renderScene )
    : StreamedRenderPass( context, renderScene, 96 )
{
}

// ** DebugLightPass::emitRenderOperations
void DebugLightPass::emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Light& light, const Transform& transform )
{
    // Calculate a light's local space bounding box
    f32    range  = light.range();
    Bounds bounds = Bounds( Vec3( -range, -range, -range ), Vec3( range, range, range ) );

    // Emit a bounding box to an output stream
    switch( light.type() ) {
    case LightType::Point:  emitWireBounds( frame, commands, stateStack, bounds * transform.matrix() );
                            break;
    case LightType::Spot:   emitFrustum( frame, commands, stateStack, light.cutoff() * 2.0f, 1.0f, 0.1f, light.range() * 2.0f, transform.matrix() );
                            break;
    }
}

// ------------------------------------------------------------------ DebugCameraPass ------------------------------------------------------------------ //

// ** DebugCameraPass::DebugCameraPass
DebugCameraPass::DebugCameraPass( RenderingContext& context, RenderScene& renderScene )
    : StreamedRenderPass( context, renderScene, 96 )
{
}

// ** DebugCameraPass::emitRenderOperations
void DebugCameraPass::emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
{
    // By default a frustum aspect is 1.0f
    f32 aspect = 1.0f;

    // Inherit a camera frustum from a viewport
    if( const Viewport* viewport = entity.has<Viewport>() ) {
        aspect = viewport->aspect();
    }

    switch( camera.projection() ) {
    case Projection::Perspective:   emitFrustum( frame, commands, stateStack, camera.fov(), aspect, camera.near(), camera.far(), transform.matrix() );
                                    break;
    }
}

// ------------------------------------------------------------------ DebugRenderTarget ------------------------------------------------------------------ //

// ** DebugRenderTarget::DebugRenderTarget
DebugRenderTarget::DebugRenderTarget( RenderingContext& context, RenderScene& renderScene )
    : StreamedRenderPassBase( context, renderScene, 6 )
{
    // Create a view constant buffer
    m_cbuffer = m_context.deprecatedRequestConstantBuffer( NULL, sizeof( RenderScene::CBuffer::View ), RenderScene::CBuffer::View::Layout );

    // Create a shader
    m_shader = m_context.deprecatedRequestShader( "../../Source/Dreemchest/Scene/Rendering/Shaders/Default.shader" );
}
    
// ** DebugRenderTarget::render
void DebugRenderTarget::render( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Viewport& viewport, TransientRenderTarget slot, Renderer::RenderTargetAttachment attachment, s32 size, s32 x, s32 y )
{
    // Begin a render pass
    begin( frame, commands, stateStack );

    // Upload a view constant buffer
    RenderScene::CBuffer::View view = orthoView( viewport );
    commands.uploadConstantBuffer( m_cbuffer, frame.internBuffer( &view, sizeof view ), sizeof view );

    // Push a rendering state
    StateScope pass = stateStack.newScope();
    pass->bindProgram( m_shader );
    pass->bindRenderedTexture( slot, TextureSampler::Diffuse, attachment );
    pass->bindConstantBuffer( m_cbuffer, Constants::Pass );
    pass->setCullFace( Renderer::TriangleFaceBack );

    // Emit a single rectangle
    Rgba color( 0.0f, 1.0f, 1.0f );
    emitRect( frame, commands, stateStack, Vec3( x + size * 0.5f, y + size * 0.5f, 0.0f ), Vec3( size * 0.5f , 0.0f, 0.0f ), Vec3( 0.0f, size * 0.5f , 0.0f ), &color );

    // End render pass
    end( frame, commands, stateStack );
}

// ------------------------------------------------------------------ DebugCascadedShadows ------------------------------------------------------------------ //

// ** DebugCascadedShadows::DebugCascadedShadows
DebugCascadedShadows::DebugCascadedShadows( RenderingContext& context, RenderScene& renderScene )
    : StreamedRenderPass( context, renderScene, 96 )
{
    // Create a shader
    m_shader = m_context.deprecatedRequestShader( "../../Source/Dreemchest/Scene/Rendering/Shaders/Default.shader" );
}

// ** DebugCascadedShadows::render
void DebugCascadedShadows::render( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const CascadedShadowMaps& csm, const Rgba colors[] )
{
    // Default split colors
    static Rgba kSplitColors[] = {
          { 1.0f, 0.0f, 0.0f }
        , { 0.0f, 1.0f, 0.0f }
        , { 0.0f, 0.0f, 1.0f }
        , { 0.0f, 1.0f, 1.0f }
    };

    // Save cms instance and split colors before rendering
    m_csm    = csm;
    m_colors = colors ? colors : kSplitColors;

    // Begin a render pass
    begin( frame, commands, stateStack );

    // Push a pass state
    StateScope pass = stateStack.newScope();
    pass->bindProgram( m_shader );
    pass->setBlend( Renderer::BlendSrcAlpha, Renderer::BlendInvSrcAlpha );

    // Emit all required render operations
    StreamedRenderPass::emitRenderOperations( frame, commands, stateStack );

    // End a render pass
    end( frame, commands, stateStack );
}

// ** DebugCascadedShadows::emitRenderOperations
void DebugCascadedShadows::emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
{
    if( camera.projection() != Projection::Perspective ) {
        return;
    }

    // By default a frustum aspect is 1.0f
    f32 aspect = 1.0f;

    // Inherit a camera frustum from a viewport
    if( const Viewport* viewport = entity.has<Viewport>() ) {
        aspect = viewport->aspect();
    }

    // Caclulate CSM splits
    s32 cascades = m_csm.cascadeCount();
    m_csm = CascadedShadowMaps( transform.matrix(), m_csm.light(), m_csm.textureSize() );
    m_csm.calculate( camera.fov(), camera.near(), camera.far(), aspect, 0.3f, cascades );

    // Visualize camera frustum splits
    for( s32 i = 0, n = m_csm.cascadeCount(); i < n; i++ ) {
        // Get a cascade at specified index
        const CascadedShadowMaps::Cascade& cascade = m_csm.cascadeAt( i );
        
        // Construct a transparent color
        Rgba color = m_colors[i].transparent( 0.5f );

        // Render a world space cascade bounding box
        emitWireBounds( frame, commands, stateStack, Bounds::fromSphere( cascade.worldSpaceBounds.center(), cascade.worldSpaceBounds.radius() ), &color );

        // Render a split projection box
        emitWireBounds( frame, commands, stateStack, cascade.lightSpaceVertices, &color );
    }

    emitBasis( frame, commands, stateStack, Matrix4() );
}

} // namespace Scene

DC_END_DREEMCHEST
