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

#include "ForwardRenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** ForwardRenderSystem::ForwardRenderSystem
ForwardRenderSystem::ForwardRenderSystem( RenderingContext& context, RenderScene& renderScene )
    : RenderSystem( context, renderScene )
    , m_ambient( context, renderScene )
    , m_shadows( context, renderScene )
    , m_debugCascadedShadows( context, renderScene )
    , m_debugRenderTarget( context, renderScene )
{
    m_phongShader       = m_context.requestShader( "../../Source/Dreemchest/Scene/Rendering/Shaders/Phong.shader" );
    m_clipPlanesCBuffer = m_context.requestConstantBuffer( NULL, sizeof( RenderScene::CBuffer::ClipPlanes ), RenderScene::CBuffer::ClipPlanes::Layout );
}

// ** ForwardRenderSystem::emitRenderOperations
void ForwardRenderSystem::emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const ForwardRenderer& forwardRenderer )
{
    // First perform an ambient render pass
    m_ambient.render( frame, commands, stateStack );

    // Get all light sources
    const RenderScene::Lights& lights = m_renderScene.lights();

    // Render a scene for each light in scene
    for( s32 i = 0, n = lights.count(); i < n; i++ ) {
        // Get a light by index
        const RenderScene::LightNode& light = lights[i];

        // Emit render operations according to a light type
        switch( light.light->type() ) {
        case LightType::Spot:           renderSpotLight( frame, commands, stateStack, forwardRenderer, light );
                                        break;
        case LightType::Directional:    renderDirectionalLight( frame, commands, stateStack, forwardRenderer, camera, transform, *entity.get<Viewport>(), light );
                                        break;
        case LightType::Point:          renderPointLight( frame, commands, stateStack, forwardRenderer, light );
                                        break;
                                    
        }
    }
}

// ** ForwardRenderSystem::renderSpotLight
void ForwardRenderSystem::renderSpotLight( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const ForwardRenderer& forwardRenderer, const RenderScene::LightNode& light )
{
    u8               shadowTexture = 0;
    ShadowParameters shadowParameters;

    // Construct a view-projection matrix for this spot light
    Matrix4 viewProjection = Matrix4::perspective( light.light->cutoff() * 2.0f, 1.0f, 0.1f, light.light->range() * 2.0f ) * light.matrix->inversed();

    // First render a shadowmap for this light instance
    if( light.light->castsShadows() ) {
        shadowParameters.transform = viewProjection;
        shadowParameters.invSize   = 1.0f / forwardRenderer.shadowSize();
        shadowTexture              = m_shadows.render( frame, commands, stateStack, shadowParameters );        
    }

    // Render a light pass
    RenderScene::CBuffer::ClipPlanes clip = RenderScene::CBuffer::ClipPlanes::fromViewProjection( viewProjection );
    renderLight( frame, commands, stateStack, light, &clip, shadowTexture );

    // Release an intermediate shadow render target
    if( shadowTexture ) {
        commands.releaseRenderTarget( shadowTexture );
    }
}

// ** ForwardRenderSystem::renderPointLight
void ForwardRenderSystem::renderPointLight( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const ForwardRenderer& forwardRenderer, const RenderScene::LightNode& light )
{
    // Render a light pass
    RenderScene::CBuffer::ClipPlanes clip = RenderScene::CBuffer::ClipPlanes::fromSphere( *light.matrix * Vec3::zero(), light.light->range() );
    renderLight( frame, commands, stateStack, light, &clip, 0 );
}

// ** ForwardRenderSystem::renderDirectionalLight
void ForwardRenderSystem::renderDirectionalLight( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const ForwardRenderer& forwardRenderer, const Camera& camera, const Transform& cameraTransform, const Viewport& viewport, const RenderScene::LightNode& light )
{
    // Light does not cast any shadows, so just render it
    if( !light.light->castsShadows() ) {
        renderLight( frame, commands, stateStack, light, NULL, 0 );
        return;
    }

    // Get the cascaded shadow maps settings
    s32 shadowSize   = forwardRenderer.shadowSize();
    s32 cascadeCount = forwardRenderer.shadowCascadeCount();
    f32 lambda       = forwardRenderer.shadowCascadeLambda();

    // Construct a cascaded shadow maps instance
    CascadedShadowMaps csm( cameraTransform.matrix(), *light.matrix, shadowSize );
    csm.calculate( camera.fov(), camera.near(), camera.far(), viewport.aspect(), lambda, cascadeCount );

    // Render each cascade
    for( s32 j = 0; j < cascadeCount; j++ ) {
        const CascadedShadowMaps::Cascade& cascade = csm.cascadeAt( j );

        ShadowParameters parameters;
        parameters.invSize   = 1.0f / shadowSize;
        parameters.transform = cascade.transform;

        u8 shadows = m_shadows.render( frame, commands, stateStack, parameters );

        RenderScene::CBuffer::ClipPlanes clip = RenderScene::CBuffer::ClipPlanes::fromNearAndFar( cameraTransform.axisZ(), cameraTransform.worldSpacePosition(), cascade.near, cascade.far );

        renderLight( frame, commands, stateStack, light, &clip, shadows );

        // Render a debug shadow texture
        if( forwardRenderer.isDebugCascadeShadows() )
        {
        #if DEV_DEPRECATED_HAL
            m_debugRenderTarget.render( frame, commands, stateStack, viewport, shadows, Renderer::RenderTarget::Depth, 128, j * 130, 0 );
        #else
            m_debugRenderTarget.render( frame, commands, stateStack, viewport, shadows, -1, 128, j * 130, 0 );
        #endif  /*  #if DEV_DEPRECATED_HAL  */
        }

        // Release an intermediate shadow render target
        commands.releaseRenderTarget( shadows );
    }

    // Render a debug info for a shadow cascades
    if( forwardRenderer.isDebugCascadeShadows() ) {
        m_debugCascadedShadows.render( frame, commands, stateStack, csm );
    }
}

// ** ForwardRenderSystem::renderLight
void ForwardRenderSystem::renderLight( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const RenderScene::LightNode& light, const RenderScene::CBuffer::ClipPlanes* clip, u8 shadows )
{
    // A light type feature bits
    PipelineFeatures lightType[] = { ShaderPointLight, ShaderSpotLight, ShaderDirectionalLight };

    // Clip state block
    StateScope clipper = stateStack.newScope();

    if( clip ) {
        commands.uploadConstantBuffer( m_clipPlanesCBuffer, frame.internBuffer( clip, sizeof( RenderScene::CBuffer::ClipPlanes ) ), sizeof( RenderScene::CBuffer::ClipPlanes ) );
        clipper->bindConstantBuffer( m_clipPlanesCBuffer, State::ClippingPlanes );
    }

    // Light state block
    StateScope state = stateStack.newScope();
    state->bindConstantBuffer( light.constantBuffer, State::LightConstants );
    state->enableFeatures( lightType[light.light->type()] | ShaderShadowFiltering3 );
    state->bindProgram( m_phongShader );
    state->setBlend( Renderer::BlendOne, Renderer::BlendOne );
    state->setDepthState( Renderer::LessEqual, false );
    state->setPolygonOffset( -1, -1 );

    // Bind a rendered shadowmap
    if( shadows )
    {
    #if DEV_DEPRECATED_HAL
        state->bindRenderedTexture( shadows, State::Texture1, Renderer::RenderTarget::Depth );
    #else
        state->bindRenderedTexture( shadows, State::Texture1, -1 );
    #endif  /*  #if DEV_DEPRECATED_HAL  */
        state->bindConstantBuffer( m_shadows.cbuffer(), State::ShadowConstants );
    }

    // Emit render operations
    RenderPassBase::emitStaticMeshes( m_renderScene.staticMeshes(), frame, commands, stateStack, RenderMaskPhong );
    RenderPassBase::emitPointClouds( m_renderScene.pointClouds(), frame, commands, stateStack, RenderMaskPhong );
}

} // namespace Scene

DC_END_DREEMCHEST
