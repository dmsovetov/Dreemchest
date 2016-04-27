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
#include "../RenderSystem/PointCloudEmitter.h"
#include "../RenderSystem/StaticMeshEmitter.h"
#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** TestRenderSystem::TestRenderSystem
TestRenderSystem::TestRenderSystem( RenderingContext& context, RenderScene& renderScene, Renderer::HalWPtr hal )
    : RenderSystemBase( context, renderScene, renderScene.scene()->ecs()->requestIndex( "", Ecs::Aspect::all<Camera, Transform>() ) )
{
    m_pointCloudEmitter = DC_NEW PointCloudEmitter( renderScene );
    m_staticMeshEmitter = DC_NEW StaticMeshEmitter( renderScene );
    m_phongShader   = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Phong.shader" );
    m_ambientShader = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Ambient.shader" );
}

// ** TestRenderSystem::emitRenderOperations
void TestRenderSystem::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
{
    // Ambient pass
    {
        StateScope pass = stateStack.newScope();
        pass->bindProgram( m_context.internShader( m_ambientShader ) );
        pass->enableFeatures( ShaderEmissionColor | ShaderAmbientColor );

        m_staticMeshEmitter->emit( frame, commands, stateStack );
        m_pointCloudEmitter->emit( frame, commands, stateStack );
    }

    // Get all light sources
    const RenderScene::Lights& lights = m_renderScene.lights();

    // Render a scene for each light in scene
    for( s32 i = 0, n = lights.count(); i < n; i++ ) {
        // Get a light by index
        const RenderScene::LightNode& light = lights[i];

        // Light state block
        StateScope state = stateStack.newScope();
        state->bindConstantBuffer( light.constantBuffer, RenderState::LightConstants );
        state->enableFeatures( ShaderPointLight );
        state->bindProgram( m_context.internShader( m_phongShader ) );
        state->setBlend( Renderer::BlendOne, Renderer::BlendOne );

        // Emit render operations
        RopEmitter::Filter filter;
        filter.lightingModels = BIT( LightingModel::Phong );
        m_staticMeshEmitter->emit( frame, commands, stateStack, filter );
        m_pointCloudEmitter->emit( frame, commands, stateStack, filter );
    }
}

} // namespace Scene

DC_END_DREEMCHEST