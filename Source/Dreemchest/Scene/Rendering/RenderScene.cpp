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

#include "RenderScene.h"
#include "Rvm/Commands.h"

#include "../Components/Rendering.h"
#include "../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

Renderer::ShaderPtr pinkShader;
Renderer::ShaderPtr whiteShader;
Renderer::ConstantBufferPtr frameConstantBuffer;
Renderer::ConstantBufferPtr passConstantBuffer;
Renderer::ConstantBufferPtr instanceConstantBuffer;
Renderer::VertexBufferPtr pointCloud;
Renderer::VertexDeclarationPtr vertexDeclaration;

struct RenderFrameConstants {
    Vec4        color;
};
struct RenderPassConstants {
    Matrix4     viewProjection;
};
struct RenderInstanceConstants {
    Matrix4     transform;
};

// ** RenderScene::RenderScene
RenderScene::RenderScene( SceneWPtr scene )
    : m_scene( scene )
{
    // Create camera index
    m_cameras = scene->ecs()->requestIndex( "Cameras", Ecs::Aspect::all<Camera>() );
}

// ** RenderScene::create
RenderScenePtr RenderScene::create( SceneWPtr scene )
{
    return DC_NEW RenderScene( scene );
}

// ** RenderScene::captureFrame
RenderFrame RenderScene::captureFrame( Renderer::HalWPtr hal )
{
    RenderFrame frame;

    if( !pinkShader.valid() ) {
        pinkShader = hal->createShader(
                NIMBLE_STRINGIFY(
                    struct RenderPass {
                        mat4    viewProjection;
                    };
                    struct RenderInstance {
                        mat4    transform;
                    };

                    RenderPass uPass;
                    RenderInstance uInstance;

                    void main()
                    {
                        gl_Position = uPass.viewProjection * uInstance.transform * gl_Vertex;
                    }   
                )
            ,   NIMBLE_STRINGIFY(
                    void main()
                    {
                        gl_FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
                    }
                )
            );

        whiteShader = hal->createShader(
                NIMBLE_STRINGIFY(
                    struct RenderPass {
                        mat4    viewProjection;
                    };
                    struct RenderInstance {
                        mat4    transform;
                    };

                    RenderPass uPass;
                    RenderInstance uInstance;

                    void main()
                    {
                        gl_Position = uPass.viewProjection * uInstance.transform * gl_Vertex;
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

        vertexDeclaration = hal->createVertexDeclaration( "P3:C4" );
        pointCloud = hal->createVertexBuffer( vertexDeclaration, 100 );

        struct Point {
            Vec3 pos;
            u8   color[4];
        };
        Point* pts = reinterpret_cast<Point*>( pointCloud->lock() );
        s32 x = sizeof Point;
        for( s32 i = 0; i < 100; i++ ) {
            Point& p = pts[i];
            p.pos = Vec3::randomInSphere( Vec3::zero(), 1.0f );
            p.color[0] = p.color[1] = p.color[2] = p.color[3] = 255;
        }
        pointCloud->unlock();


        frameConstantBuffer = hal->createConstantBuffer( sizeof( RenderFrameConstants ), false );
        passConstantBuffer = hal->createConstantBuffer( sizeof( RenderPassConstants ), false );
        instanceConstantBuffer = hal->createConstantBuffer( sizeof( RenderInstanceConstants ), false );
    }
    
    // Default state block
    RenderStateBlock* defaults = new RenderStateBlock;
    defaults->disableAlphaTest();
    defaults->disableBlending();
    defaults->setDepthState( Renderer::LessEqual, true );
    defaults->bindProgram( frame.internShader( pinkShader ) );
    defaults->bindConstantBuffer( frame.internConstantBuffer( frameConstantBuffer ), RenderState::GlobalConstants );

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

        // Pass state block
        RenderStateBlock* pass = new RenderStateBlock;
        pass->setRenderTarget( frame.internRenderTarget( target ), camera->calculateViewProjection( transform->matrix() ), camera->viewport() );
        pass->bindConstantBuffer( frame.internConstantBuffer( passConstantBuffer ), RenderState::PassConstants );

        // Instance state block
        RenderStateBlock* instance = new RenderStateBlock;
        instance->bindVertexBuffer( frame.internVertexBuffer( pointCloud ) );
        instance->bindConstantBuffer( frame.internConstantBuffer( instanceConstantBuffer ), RenderState::InstanceConstants );

        const RenderStateBlock* stack[] = { instance, pass, defaults, NULL };

        RenderCommandBuffer& commands = frame.createCommandBuffer();
        commands.drawPrimitives( 0, Renderer::PrimPoints, stack, 0, 100 );
	}

    return frame;
}

} // namespace Scene

DC_END_DREEMCHEST