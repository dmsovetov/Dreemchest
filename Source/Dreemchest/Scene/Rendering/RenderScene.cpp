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
#include "RenderSystem/RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderScene::RenderScene
RenderScene::RenderScene( SceneWPtr scene, Renderer::HalWPtr hal )
    : m_scene( scene )
    , m_hal( hal )
{
    // Create entity caches
    m_pointClouds = scene->ecs()->createDataCache<PointCloudCache>( Ecs::Aspect::all<PointCloud, Transform>(), dcThisMethod( RenderScene::createPointCloudNode ) );
}

// ** RenderScene::create
RenderScenePtr RenderScene::create( SceneWPtr scene, Renderer::HalWPtr hal )
{
    return DC_NEW RenderScene( scene, hal );
}

// ** RenderScene::scene
SceneWPtr RenderScene::scene( void ) const
{
    return m_scene;
}

// ** RenderScene::pointClouds
const RenderScene::PointClouds& RenderScene::pointClouds( void ) const
{
    return m_pointClouds->data();
}

// ** RenderScene::captureFrame
RenderFrameUPtr RenderScene::captureFrame( void )
{
    RenderFrameUPtr frame( DC_NEW RenderFrame );

    // Update instance constant buffers
    updateInstanceConstants();

    // Get a state stack
    RenderStateStack& stateStack = frame->stateStack();

    // Push a default state block
    RenderStateBlock& defaults = stateStack.push();
    defaults.disableAlphaTest();
    defaults.disableBlending();
    defaults.setDepthState( Renderer::LessEqual, true );

    // Process all render systems
    for( s32 i = 0, n = static_cast<s32>( m_renderSystems.size() ); i < n; i++ ) {
        m_renderSystems[i]->render( *frame.get() );
    }

    // Pop a default state block
    stateStack.pop();

    return frame;
}

// ** RenderScene::updateInstanceConstants
void RenderScene::updateInstanceConstants( void )
{
    PointClouds& pointClouds = m_pointClouds->data();

    for( s32 i = 0, n = pointClouds.count(); i < n; i++ ) {
        PointCloudNode& node = pointClouds[i];

        CBuffer::Instance* cbuffer = node.constantBuffer->lock<CBuffer::Instance>();
        cbuffer->transform = node.transform->matrix();
        node.constantBuffer->unlock();
    }
}

// ** RenderScene::createPointCloudNode
RenderScene::PointCloudNode RenderScene::createPointCloudNode( const Ecs::Entity& entity )
{
    const Transform*  transform  = entity.get<Transform>();
    const PointCloud* pointCloud = entity.get<PointCloud>();

    PointCloudNode node;

    struct Vertex {
        Vec3    point;
        u8      color[4];
        Vec3    normal;
    };

    node.transform      = transform;
    node.matrix         = &transform->matrix();
    node.vertexCount    = pointCloud->vertexCount();
    node.inputLayout    = m_hal->createInputLayout( sizeof( Vertex ) );
    node.inputLayout->attributeLocation( Renderer::InputLayout::Position, 3, offsetof( Vertex, point ) );
    node.inputLayout->attributeLocation( Renderer::InputLayout::Color, 4, offsetof( Vertex, color ) );
    node.inputLayout->attributeLocation( Renderer::InputLayout::Normal, 3, offsetof( Vertex, normal ) );
    node.vertexBuffer   = m_hal->createVertexBuffer( pointCloud->vertexCount() * sizeof( Vertex ) );

    const PointCloud::Vertex* pointCloudVertices = pointCloud->vertices<PointCloud::Vertex>();

    Vertex* vertices = node.vertexBuffer->lock<Vertex>();
    for( s32 i = 0, n = pointCloud->vertexCount(); i < n; i++ ) {
        Vertex& v   = vertices[i];
        v.point     = pointCloudVertices[i].position;
        v.normal    = pointCloudVertices[i].normal;
        v.color[0]  = static_cast<u8>( pointCloudVertices[i].color.r * 255 );
        v.color[1]  = static_cast<u8>( pointCloudVertices[i].color.g * 255 );
        v.color[2]  = static_cast<u8>( pointCloudVertices[i].color.b * 255 );
        v.color[3]  = static_cast<u8>( pointCloudVertices[i].color.a * 255 );
    }
    node.vertexBuffer->unlock();

    node.constantBuffer = m_hal->createConstantBuffer( sizeof( CBuffer::Instance ), false );
    node.constantBuffer->addConstant( Renderer::ConstantBuffer::Matrix4, offsetof( CBuffer::Instance, transform ), "Instance.transform" );

    return node;
}

} // namespace Scene

DC_END_DREEMCHEST