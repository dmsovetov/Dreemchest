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

#include "TestRenderSystem.h"
#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** TestRenderSystem::TestRenderSystem
TestRenderSystem::TestRenderSystem( RenderingContext& context, RenderScene& renderScene, Renderer::HalWPtr hal )
    : RenderSystemBase( context, renderScene, renderScene.scene()->ecs()->requestIndex( "", Ecs::Aspect::all<Camera, Transform>() ) )
{
    m_phongShader       = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Phong.shader" );
    m_ambientShader     = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Ambient.shader" );
    m_shadowShader      = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Shadow.shader" );
    m_shadowCBuffer     = m_context.requestConstantBuffer( NULL, sizeof RenderScene::CBuffer::Shadow, RenderScene::CBuffer::Shadow::Layout );
}

// ** TestRenderSystem::emitRenderOperations
void TestRenderSystem::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
{
    // Ambient pass
    {
        StateScope pass = stateStack.newScope();
        pass->bindProgram( m_context.internShader( m_ambientShader ) );
        pass->enableFeatures( ShaderEmissionColor | ShaderAmbientColor );

        emitStaticMeshes( frame, commands, stateStack );
        emitPointClouds( frame, commands, stateStack );
    }

    // Get all light sources
    const RenderScene::Lights& lights = m_renderScene.lights();

    // A light type feature bits
    Ubershader::Bitmask lightType[] = { ShaderPointLight, ShaderSpotLight, ShaderDirectionalLight };

    // Render a scene for each light in scene
    for( s32 i = 0, n = lights.count(); i < n; i++ ) {
        // Get a light by index
        const RenderScene::LightNode& light = lights[i];

        // A shadow map render target handle
        u8 shadows = 0;

        if( light.light->castsShadows() ) {
            shadows = renderShadows( frame, commands, stateStack, light, 1024 );
        }

        // Light state block
        StateScope state = stateStack.newScope();
        state->bindConstantBuffer( light.constantBuffer, RenderState::LightConstants );
        state->enableFeatures( lightType[light.light->type()] | ShaderShadowFiltering3 );
        state->bindProgram( m_context.internShader( m_phongShader ) );
        state->setBlend( Renderer::BlendOne, Renderer::BlendOne );

        // Bind a rendered shadowmap
        if( shadows ) {
            state->bindRenderedTexture( shadows, RenderState::Texture1, Renderer::RenderTarget::Depth );
            state->bindConstantBuffer( m_shadowCBuffer, RenderState::ShadowConstants );
        }

        // Emit render operations
        emitStaticMeshes( frame, commands, stateStack, RenderMaskPhong );
        emitPointClouds( frame, commands, stateStack, RenderMaskPhong );

        // Release an intermediate shadow render target
        if( shadows ) {
            commands.releaseRenderTarget( shadows );
        }
    }
}

// ** TestRenderSystem::renderShadows
u8 TestRenderSystem::renderShadows( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, s32 dimensions )
{
    DC_BREAK_IF( !light.light->castsShadows(), "a light instance does not cast shadows" );

    // Acquire a shadow render target
    u8 renderTarget = commands.acquireRenderTarget( dimensions, dimensions, Renderer::PixelD24X8 );

    // Render scene from a light's point of view
    RenderCommandBuffer& cmd = commands.renderToTarget( renderTarget );
    cmd.clear( Rgba( 1.0f, 1.0f, 1.0f, 1.0f ), ~0 );

    // Update a shadow constant buffer
    m_shadowParameters.transform = Matrix4::perspective( light.light->cutoff() * 2.0f, 1.0f, 0.1f, light.light->range() * 2.0f ) * light.matrix->inversed();
    m_shadowParameters.invSize   = 1.0f / dimensions;
    cmd.uploadConstantBuffer( m_shadowCBuffer, frame.internBuffer( &m_shadowParameters, sizeof m_shadowParameters ), sizeof m_shadowParameters );

    // Push a shadow pass scope
    StateScope state = stateStack.newScope();
    state->bindConstantBuffer( m_shadowCBuffer, RenderState::ShadowConstants );
    state->bindProgram( m_context.internShader( m_shadowShader ) );
    state->setCullFace( Renderer::TriangleFaceFront );

    // Render all static meshes to a target
    emitStaticMeshes( frame, cmd, stateStack );

    return renderTarget;
}

} // namespace Scene

DC_END_DREEMCHEST