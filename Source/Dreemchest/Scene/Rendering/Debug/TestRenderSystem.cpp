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

DC_BEGIN_DREEMCHEST

namespace Scene {

namespace CBuffer {

    struct Frame {
        Rgba        color;
    };
    struct Camera {
        Matrix4     viewProjection;
    };
    struct Instance {
        Matrix4     transform;
    };

}

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

                void main()
                {
                    gl_Position = Camera.viewProjection * Instance.transform * gl_Vertex;
                }   
            )
        ,   NIMBLE_STRINGIFY(
                struct RenderFrame {
                    vec4    color;
                };

                RenderFrame uFrame;

                void main()
                {
                    gl_FragColor = uFrame.color;
                }
            )
        );

    m_vertexDeclaration = hal->createVertexDeclaration( "P3:C4" );
    m_pointCloud = hal->createVertexBuffer( m_vertexDeclaration, 100 );

    struct Point {
        Vec3 pos;
        u8   color[4];
    };
    Point* pts = m_pointCloud->lock<Point>();
    s32 x = sizeof Point;
    for( s32 i = 0; i < 100; i++ ) {
        Point& p = pts[i];
        p.pos = Vec3::randomInSphere( Vec3::zero(), 1.0f );
        p.color[0] = p.color[1] = p.color[2] = p.color[3] = 255;
    }
    m_pointCloud->unlock();


    m_frameConstants = hal->createConstantBuffer( sizeof( CBuffer::Frame ), false );
    m_frameConstants->addConstant( Renderer::ConstantBuffer::Vec4, offsetof( CBuffer::Frame, color ), "Frame.color" );

    m_cameraConstants = hal->createConstantBuffer( sizeof( CBuffer::Camera ), false );
    m_cameraConstants->addConstant( Renderer::ConstantBuffer::Matrix4, offsetof( CBuffer::Camera, viewProjection ), "Camera.viewProjection" );

    m_instanceConstants = hal->createConstantBuffer( sizeof( CBuffer::Instance ), false );
    m_instanceConstants->addConstant( Renderer::ConstantBuffer::Matrix4, offsetof( CBuffer::Instance, transform ), "Instance.transform" );
}

// ** TestRenderSystem::emitRenderOperations
void TestRenderSystem::emitRenderOperations( RenderFrame& frame, const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
{
    // Default state block
    RenderStateBlock* defaults = new RenderStateBlock;
    defaults->disableAlphaTest();
    defaults->disableBlending();
    defaults->setDepthState( Renderer::LessEqual, true );
    defaults->bindProgram( frame.internShader( m_pinkShader ) );
    defaults->bindConstantBuffer( frame.internConstantBuffer( m_frameConstants ), RenderState::GlobalConstants );

    CBuffer::Frame* renderFrameConstants = m_frameConstants->lock<CBuffer::Frame>();
    renderFrameConstants->color = Rgba( 1.0f, 1.0f, 0.0f );
    m_frameConstants->unlock();

    // Get all active scene cameras
    const Ecs::EntitySet& cameras = m_cameras->entities();

    // Clear each render target
    for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; i++ ) {
		Camera*			 camera     = (*i)->get<Camera>();
        Transform*	     transform  = (*i)->get<Transform>();
		RenderTargetWPtr target     = camera->target();

		if( !target.valid() ) {
			continue;
		}

        CBuffer::Camera* renderPassConstants = m_cameraConstants->lock<CBuffer::Camera>();
        renderPassConstants->viewProjection = camera->calculateViewProjection( transform->matrix() );
        m_cameraConstants->unlock();

        // Pass state block
        RenderStateBlock* pass = new RenderStateBlock;
        pass->setRenderTarget( frame.internRenderTarget( target ), camera->calculateViewProjection( transform->matrix() ), camera->viewport() );
        pass->bindConstantBuffer( frame.internConstantBuffer( m_cameraConstants ), RenderState::PassConstants );

        // Instance state block
        RenderStateBlock* instance = new RenderStateBlock;
        instance->bindVertexBuffer( frame.internVertexBuffer( m_pointCloud ) );
        instance->bindConstantBuffer( frame.internConstantBuffer( m_instanceConstants ), RenderState::InstanceConstants );

        CBuffer::Instance* renderInstanceConstants = m_instanceConstants->lock<CBuffer::Instance>();
        renderInstanceConstants->transform = Matrix4();
        m_instanceConstants->unlock();

        const RenderStateBlock* stack[] = { instance, pass, defaults, NULL };

        RenderCommandBuffer& commands = frame.createCommandBuffer();
        commands.drawPrimitives( 0, Renderer::PrimPoints, stack, 0, 100 );
	}
}

} // namespace Scene

DC_END_DREEMCHEST