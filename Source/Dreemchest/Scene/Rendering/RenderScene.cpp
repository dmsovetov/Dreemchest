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

        {
            CBuffer::Instance* cbuffer = node.instanceConstants->lock<CBuffer::Instance>();
            cbuffer->transform = node.transform->matrix();
            node.instanceConstants->unlock();
        }
        {
            CBuffer::Material* cbuffer = node.materialConstants->lock<CBuffer::Material>();
            cbuffer->diffuse  = node.material->color( Material::Diffuse );
            cbuffer->specular = node.material->color( Material::Specular );
            cbuffer->emission = node.material->color( Material::Emission );
            node.materialConstants->unlock();
        }
    }
}

// ** RenderScene::createPointCloudNode
RenderScene::PointCloudNode RenderScene::createPointCloudNode( const Ecs::Entity& entity )
{
    const Transform*  transform  = entity.get<Transform>();
    const PointCloud* pointCloud = entity.get<PointCloud>();

    PointCloudNode node;

    node.transform      = transform;
    node.matrix         = &transform->matrix();
    node.vertexCount    = pointCloud->vertexCount();
    node.material       = pointCloud->material();
    node.inputLayout    = createInputLayout( pointCloud->vertexFormat() );
    node.vertexBuffer   = createVertexBuffer( pointCloud->vertices(), pointCloud->vertexCount(), pointCloud->vertexFormat(), pointCloud->vertexFormat() );

    node.instanceConstants = m_hal->createConstantBuffer( sizeof( CBuffer::Instance ), false );
    node.instanceConstants->addConstant( Renderer::ConstantBuffer::Matrix4, offsetof( CBuffer::Instance, transform ), "Instance.transform" );

    node.materialConstants = m_hal->createConstantBuffer( sizeof( CBuffer::Material ), false );
    node.materialConstants->addConstant( Renderer::ConstantBuffer::Vec4, offsetof( CBuffer::Material, diffuse ), "Material.diffuse" );
    node.materialConstants->addConstant( Renderer::ConstantBuffer::Vec4, offsetof( CBuffer::Material, specular ), "Material.specular" );
    node.materialConstants->addConstant( Renderer::ConstantBuffer::Vec4, offsetof( CBuffer::Material, emission ), "Material.emission" );

    return node;
}

// ** RenderScene::createInputLayout
Renderer::InputLayoutPtr RenderScene::createInputLayout( const VertexFormat& format )
{
    // Create an input layout
    Renderer::InputLayoutPtr inputLayout = m_hal->createInputLayout( format.vertexSize() );

    // Add vertex attributes to an input layout
    if( format & VertexFormat::Position ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Position, 3, format.attributeOffset( VertexFormat::Position ) );
    }
    if( format & VertexFormat::Color ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Color, 4, format.attributeOffset( VertexFormat::Color ) );
    }
    if( format & VertexFormat::Normal ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Normal, 3, format.attributeOffset( VertexFormat::Normal ) );
    }
    if( format & VertexFormat::Uv0 ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Uv0, 2, format.attributeOffset( VertexFormat::Uv0 ) );
    }
    if( format & VertexFormat::Uv1 ) {
        inputLayout->attributeLocation( Renderer::InputLayout::Uv1, 2, format.attributeOffset( VertexFormat::Uv1 ) );
    }

    return inputLayout;
}

// ** RenderScene::createVertexBuffer
Renderer::VertexBufferPtr RenderScene::createVertexBuffer( const void* vertices, s32 count, const VertexFormat& dstFormat, const VertexFormat& srcFormat )
{
    // Create a vertex buffer instance
    Renderer::VertexBufferPtr vertexBuffer = m_hal->createVertexBuffer( count * dstFormat.vertexSize() );

    // Lock a vertex buffer
    void* locked = vertexBuffer->lock();

    // Just copy memory if vertex formats match
    if( dstFormat == srcFormat ) {
        memcpy( locked, vertices, count * dstFormat.vertexSize() );
    } else {
        // Copy all vertices to a vertex buffer
        for( s32 i = 0; i < count; i++ ) {
            dstFormat.setVertexAttribute( VertexFormat::Position, srcFormat.vertexAttribute<Vec3>( VertexFormat::Position, vertices, i ), locked, i );
            dstFormat.setVertexAttribute( VertexFormat::Color,    srcFormat.vertexAttribute<u32> ( VertexFormat::Color,    vertices, i ), locked, i );
            dstFormat.setVertexAttribute( VertexFormat::Normal,   srcFormat.vertexAttribute<Vec3>( VertexFormat::Normal,   vertices, i ), locked, i );
            dstFormat.setVertexAttribute( VertexFormat::Uv0,      srcFormat.vertexAttribute<Vec2>( VertexFormat::Uv0,      vertices, i ), locked, i );
            dstFormat.setVertexAttribute( VertexFormat::Uv1,      srcFormat.vertexAttribute<Vec2>( VertexFormat::Uv1,      vertices, i ), locked, i );
        }
    }

    // Unlock a vertex buffer.
    vertexBuffer->unlock();

    return vertexBuffer;
}

} // namespace Scene

DC_END_DREEMCHEST