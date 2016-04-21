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
TestRenderSystem::TestRenderSystem( RenderScene& renderScene, Renderer::HalWPtr hal )
    : RenderSystemBase( renderScene, renderScene.scene()->ecs()->requestIndex( "", Ecs::Aspect::all<Camera, Transform>() ) )
{
    m_pointCloudShader = renderScene.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Test.shader" );

    m_cameraConstants = hal->createConstantBuffer( sizeof( RenderScene::CBuffer::View ), false );
    m_cameraConstants->addConstant( Renderer::ConstantBuffer::Matrix4, offsetof( RenderScene::CBuffer::View, transform ), "View.transform" );

    m_lightConstants = hal->createConstantBuffer( sizeof( RenderScene::CBuffer::Light ), false );
    m_lightConstants->addConstant( Renderer::ConstantBuffer::Vec3, offsetof( RenderScene::CBuffer::Light, position ), "Light.position" );
    m_lightConstants->addConstant( Renderer::ConstantBuffer::Float, offsetof( RenderScene::CBuffer::Light, radius ), "Light.radius" );
    m_lightConstants->addConstant( Renderer::ConstantBuffer::Vec3, offsetof( RenderScene::CBuffer::Light, color ), "Light.color" );
    m_lightConstants->addConstant( Renderer::ConstantBuffer::Float, offsetof( RenderScene::CBuffer::Light, intensity ), "Light.intensity" );
}

// ** TestRenderSystem::emitRenderOperations
void TestRenderSystem::emitRenderOperations( RenderFrame& frame, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
{
    // Update camera constant buffer
    RenderScene::CBuffer::View* renderPassConstants = m_cameraConstants->lock<RenderScene::CBuffer::View>();
    renderPassConstants->transform = camera.calculateViewProjection( transform.matrix() );
    m_cameraConstants->unlock();

    // Update light constant buffer
    RenderScene::CBuffer::Light* lightConstants = m_lightConstants->lock<RenderScene::CBuffer::Light>();
    lightConstants->position    = Vec3( 10, 5, 10 );
    lightConstants->radius      = 10;
    lightConstants->color       = Rgb( 1.0f, 0.6f, 1.0f );
    lightConstants->intensity   = 5.0f;
    m_lightConstants->unlock();

    // Pass state block
    RenderStateBlock& pass = stateStack.push();
    pass.bindProgram( frame.internShader( m_pointCloudShader ) );
    pass.setRenderTarget( frame.internRenderTarget( camera.target() ), camera.viewport() );
    pass.bindConstantBuffer( frame.internConstantBuffer( m_cameraConstants ), RenderState::PassConstants );
    pass.bindConstantBuffer( frame.internConstantBuffer( m_lightConstants ), RenderState::LightConstants );
    pass.enableFeatures( BIT( ShaderPointLight ) );
    pass.enableFeatures( BIT( ShaderAmbientColor ) );

    RenderCommandBuffer& commands = frame.createCommandBuffer();
    const RenderScene::PointClouds& pointClouds = m_renderScene.pointClouds();

    for( s32 i = 0, n = pointClouds.count(); i < n; i++ ) {
        const RenderScene::PointCloudNode& pointCloud = pointClouds[i];

        RenderStateBlock& instance = stateStack.push();
        instance.bindVertexBuffer( frame.internVertexBuffer( pointCloud.vertexBuffer ) );
        instance.bindConstantBuffer( frame.internConstantBuffer( pointCloud.instanceConstants ), RenderState::InstanceConstants );
        instance.bindConstantBuffer( frame.internConstantBuffer( pointCloud.materialConstants ), RenderState::MaterialConstants );
        instance.bindInputLayout( frame.internInputLayout( pointCloud.inputLayout ) );

        const Material& material = pointCloud.material.readLock();
        if( material.lightingModel() == LightingModel::Unlit ) {
            instance.disableFeatures( BIT( ShaderAmbientColor ) );
        }

        commands.drawPrimitives( 0, Renderer::PrimPoints, stateStack.states(), 0, pointCloud.vertexCount );
        stateStack.pop();
    }
}

} // namespace Scene

DC_END_DREEMCHEST