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
    m_pinkShader = hal->createShader(
            NIMBLE_STRINGIFY(
                struct CBufferCamera {
                    mat4    viewProjection;
                };
                struct CBufferInstance {
                    mat4    transform;
                };

                uniform CBufferCamera Camera;
                uniform CBufferInstance Instance;

                void main()
                {
                    gl_Position = Camera.viewProjection * Instance.transform * gl_Vertex;
                }   
            )
        ,   NIMBLE_STRINGIFY(
                void main()
                {
                    gl_FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
                }
            )
        );

    m_whiteShader = hal->createShader(
            NIMBLE_STRINGIFY(
                struct CBufferCamera {
                    mat4    viewProjection;
                };
                struct CBufferInstance {
                    mat4    transform;
                };

                uniform CBufferCamera Camera;
                uniform CBufferInstance Instance;

                varying vec4 v_Color;

                void main()
                {
                    gl_Position = Camera.viewProjection * Instance.transform * gl_Vertex;
                    gl_PointSize = 5;
                    v_Color = gl_Color;
                }   
            )
        ,   NIMBLE_STRINGIFY(
                varying vec4 v_Color;

                void main()
                {
                    gl_FragColor = v_Color;
                }
            )
        );

    m_cameraConstants = hal->createConstantBuffer( sizeof( CameraConstants ), false );
    m_cameraConstants->addConstant( Renderer::ConstantBuffer::Matrix4, offsetof( CameraConstants, viewProjection ), "Camera.viewProjection" );
}

// ** TestRenderSystem::emitRenderOperations
void TestRenderSystem::emitRenderOperations( RenderFrame& frame, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
{
    // Update camera constant buffer
    CameraConstants* renderPassConstants = m_cameraConstants->lock<CameraConstants>();
    renderPassConstants->viewProjection = camera.calculateViewProjection( transform.matrix() );
    m_cameraConstants->unlock();

    // Pass state block
    RenderStateBlock& pass = stateStack.push();
    pass.bindProgram( frame.internShader( m_whiteShader ) );
    pass.setRenderTarget( frame.internRenderTarget( camera.target() ), camera.viewport() );
    pass.bindConstantBuffer( frame.internConstantBuffer( m_cameraConstants ), RenderState::PassConstants );

    RenderCommandBuffer& commands = frame.createCommandBuffer();
    const RenderScene::PointClouds& pointClouds = m_renderScene.pointClouds();

    for( s32 i = 0, n = pointClouds.count(); i < n; i++ ) {
        const RenderScene::PointCloudNode& pointCloud = pointClouds[i];

        RenderStateBlock& instance = stateStack.push();
        instance.bindVertexBuffer( frame.internVertexBuffer( pointCloud.vertexBuffer ) );
        instance.bindConstantBuffer( frame.internConstantBuffer( pointCloud.constantBuffer ), RenderState::InstanceConstants );
        commands.drawPrimitives( 0, Renderer::PrimPoints, stateStack.states(), 0, pointCloud.vertexCount );
        stateStack.pop();
    }
}

} // namespace Scene

DC_END_DREEMCHEST