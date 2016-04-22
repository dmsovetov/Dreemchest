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
#include "Rvm/RenderingContext.h"
#include "RenderSystem/RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

struct Debug_Structure_To_Track_Data_Size_Used_By_Node_Types {
    enum {
          _Light = sizeof( RenderScene::LightNode )
        , _PointCloud = sizeof( RenderScene::PointCloudNode )
        , _Camera = sizeof( RenderScene::CameraNode )
        , _StaticMesh = sizeof( RenderScene::StaticMeshNode )
    };
};

// ** RenderScene::CBuffer::Scene::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::Scene::Layout[] = {
      { "Scene.ambient", Renderer::ConstantBufferLayout::Vec4, offsetof( RenderScene::CBuffer::Scene, ambient ) }
    , { NULL }
};

// ** RenderScene::CBuffer::Light::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::Light::Layout[] = {
      { "Light.position",  Renderer::ConstantBufferLayout::Vec3,  offsetof( CBuffer::Light, position  ), }
    , { "Light.range",     Renderer::ConstantBufferLayout::Float, offsetof( CBuffer::Light, range     ), }
    , { "Light.color",     Renderer::ConstantBufferLayout::Vec3,  offsetof( CBuffer::Light, color     ), }
    , { "Light.intensity", Renderer::ConstantBufferLayout::Float, offsetof( CBuffer::Light, intensity ), }
    , { NULL }
};

// ** RenderScene::CBuffer::View::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::View::Layout[] = {
      { "View.transform", Renderer::ConstantBufferLayout::Matrix4, offsetof( CBuffer::View, transform ), }
    , { NULL }
};

// ** RenderScene::CBuffer::Instance::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::Instance::Layout[] = {
      { "Instance.transform", Renderer::ConstantBufferLayout::Matrix4, offsetof( CBuffer::Instance, transform ), }
    , { NULL }
};

// ** RenderScene::CBuffer::Material::Layout
RenderScene::CBuffer::BufferLayout RenderScene::CBuffer::Material::Layout[] = {
      { "Material.diffuse",  Renderer::ConstantBufferLayout::Vec4, offsetof( CBuffer::Material, diffuse ),  }
    , { "Material.specular", Renderer::ConstantBufferLayout::Vec4, offsetof( CBuffer::Material, specular ), }
    , { "Material.emission", Renderer::ConstantBufferLayout::Vec4, offsetof( CBuffer::Material, emission ), }
    , { NULL }
};

// ** RenderScene::RenderScene
RenderScene::RenderScene( SceneWPtr scene, RenderingContextWPtr context )
    : m_scene( scene )
    , m_context( context )
    , m_renderCache( context )
{
    // Get a parent Ecs instance
    Ecs::EcsWPtr ecs = scene->ecs();

    // Create entity caches
    m_pointClouds   = ecs->createDataCache<PointCloudCache>( Ecs::Aspect::all<PointCloud, Transform>(), dcThisMethod( RenderScene::createPointCloudNode ) );
    m_lights        = ecs->createDataCache<LightCache>( Ecs::Aspect::all<Light, Transform>(), dcThisMethod( RenderScene::createLightNode ) );
    m_cameras       = ecs->createDataCache<CameraCache>( Ecs::Aspect::all<Camera, Transform>(), dcThisMethod( RenderScene::createCameraNode ) );
    m_staticMeshes  = ecs->createDataCache<StaticMeshCache>( Ecs::Aspect::all<StaticMesh, Transform>(), dcThisMethod( RenderScene::createStaticMeshNode ) );

    // Create scene constant buffer
    m_sceneConstants  = m_context->requestConstantBuffer( NULL, sizeof( CBuffer::Scene ), CBuffer::Scene::Layout );
    m_sceneParameters = DC_NEW CBuffer::Scene;
    
    // Create a default shader
    m_defaultShader = m_context->createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Null.shader" );
}

// ** RenderScene::create
RenderScenePtr RenderScene::create( SceneWPtr scene, RenderingContextWPtr context )
{
    return DC_NEW RenderScene( scene, context );
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

// ** RenderScene::lights
const RenderScene::Lights& RenderScene::lights( void ) const
{
    return m_lights->data();
}

// ** RenderScene::staticMeshes
const RenderScene::StaticMeshes& RenderScene::staticMeshes( void ) const
{
    return m_staticMeshes->data();
}

// ** RenderScene::findCameraNode
const  RenderScene::CameraNode& RenderScene::findCameraNode( Ecs::EntityWPtr camera ) const
{
    return m_cameras->dataFromEntity( camera );
}

// ** RenderScene::captureFrame
RenderFrameUPtr RenderScene::captureFrame( void )
{
    RenderFrameUPtr frame( DC_NEW RenderFrame );

    // Update active static meshes
    updateStaticMeshes();

    // Update active constant buffers
    updateConstantBuffers( *frame.get() );

    // Get a state stack
    RenderStateStack& stateStack = frame->stateStack();

    // Gen a frame entry point command buffer
    RenderCommandBuffer& entryPoint = frame->entryPoint();

    // Push a default state block
    StateScope defaults = stateStack.newScope();
    defaults->disableAlphaTest();
    defaults->disableBlending();
    defaults->setDepthState( Renderer::LessEqual, true );
    defaults->bindProgram( m_context->internShader( m_defaultShader ) );
    defaults->enableFeatures( BIT( ShaderAmbientColor ) );
    defaults->bindConstantBuffer( m_sceneConstants, RenderState::GlobalConstants );

    // Clear all cameras
    const Cameras& cameras = m_cameras->data();

    for( s32 i = 0, n = cameras.count(); i < n; i++ ) {
        const CameraNode& camera = cameras[i];
        entryPoint.clear( m_context->internRenderTarget( camera.camera->target() ), camera.camera->clearColor(), camera.camera->viewport(), camera.camera->clearMask() );
    }

    // Process all render systems
    for( s32 i = 0, n = static_cast<s32>( m_renderSystems.size() ); i < n; i++ ) {
        m_renderSystems[i]->render( *frame.get(), entryPoint );
    }

    return frame;
}

// ** RenderScene::updateConstantBuffers
void RenderScene::updateConstantBuffers( RenderFrame& frame )
{
    // Get a frame entry point command buffer
    RenderCommandBuffer& commands = frame.entryPoint();

    // Update scene constant buffer
    m_sceneParameters->ambient = Rgba( 0.2f, 0.2f, 0.2f, 1.0f );
    commands.uploadConstantBuffer( m_sceneConstants, m_sceneParameters.get(), sizeof( CBuffer::Scene ) );

    // Update camera constant buffers
    Cameras& cameras = m_cameras->data();

    for( s32 i = 0, n = cameras.count(); i < n; i++ ) {
        CameraNode& node = cameras[i];
        node.parameters->transform = node.camera->calculateViewProjection( node.transform->matrix() );
        commands.uploadConstantBuffer( node.constantBuffer, node.parameters.get(), sizeof CBuffer::View );
    }

    // Update light constant buffers
    Lights& lights = m_lights->data();

    for( s32 i = 0, n = lights.count(); i < n; i++ ) {
        LightNode& node = lights[i];
        node.parameters->position  = node.transform->worldSpacePosition();
        node.parameters->intensity = node.light->intensity();
        node.parameters->color     = node.light->color();
        node.parameters->range     = node.light->range();
        commands.uploadConstantBuffer( node.constantBuffer, node.parameters.get(), sizeof CBuffer::Light );
    }

    // Update point cloud constant buffers
    PointClouds& pointClouds = m_pointClouds->data();

    for( s32 i = 0, n = pointClouds.count(); i < n; i++ ) {
        PointCloudNode& node = pointClouds[i];

        node.materialParameters->diffuse  = node.material->color( Material::Diffuse );
        node.materialParameters->specular = node.material->color( Material::Specular );
        node.materialParameters->emission = node.material->color( Material::Emission );
        commands.uploadConstantBuffer( node.materialConstants, node.materialParameters.get(), sizeof CBuffer::Material );

        node.instanceParameters->transform = node.transform->matrix();
        commands.uploadConstantBuffer( node.constantBuffer, node.instanceParameters.get(), sizeof CBuffer::Instance );
    }

    // Update static mesh constant buffers
    StaticMeshes& staticMeshes = m_staticMeshes->data();

    for( s32 i = 0, n = staticMeshes.count(); i < n; i++ ) {
        StaticMeshNode& node = staticMeshes[i];

        node.materialParameters->diffuse  = node.material->color( Material::Diffuse );
        node.materialParameters->specular = node.material->color( Material::Specular );
        node.materialParameters->emission = node.material->color( Material::Emission );
        commands.uploadConstantBuffer( node.materialConstants, node.materialParameters.get(), sizeof CBuffer::Material );

        node.instanceParameters->transform = node.transform->matrix();
        commands.uploadConstantBuffer( node.constantBuffer, node.instanceParameters.get(), sizeof CBuffer::Instance );
    }
}

// ** RenderScene::updateStaticMeshes
void RenderScene::updateStaticMeshes( void )
{
    StaticMeshes& meshes = m_staticMeshes->data();

    for( s32 i = 0, n = meshes.count(); i < n; i++ ) {
        StaticMeshNode& node = meshes[i];

        // Get a static mesh asset
        const MeshHandle& mesh = node.mesh->mesh();

        // Get an asset timestamp
        u32 timestamp = mesh.asset().timestamp().modified;

        // Mesh is up-to-date - skip
        if( node.timestamp == timestamp ) {
            continue;
        }
        node.timestamp = timestamp;

        if( mesh->chunkCount() ) {
            DC_BREAK_IF( node.vertexBuffer != 0 && node.indexBuffer != 0, "a static mesh was already created" );

            VertexFormat vf( VertexFormat::Normal | VertexFormat::Uv0 | VertexFormat::Uv1 );
            node.vertexBuffer = m_renderCache.findVertexBuffer( mesh );
            node.indexBuffer  = m_renderCache.findIndexBuffer( mesh );
            node.inputLayout  = m_renderCache.findInputLayout( vf );
            node.indexCount   = mesh->indexBuffer( 0 ).size();
        }
    }
}

// ** RenderScene::createPointCloudNode
RenderScene::PointCloudNode RenderScene::createPointCloudNode( const Ecs::Entity& entity )
{
    const Transform*  transform  = entity.get<Transform>();
    const PointCloud* pointCloud = entity.get<PointCloud>();

    PointCloudNode node;

    node.vertexCount    = pointCloud->vertexCount();
    node.material       = pointCloud->material();
    node.inputLayout    = m_renderCache.findInputLayout( pointCloud->vertexFormat() );
    node.vertexBuffer   = m_context->requestVertexBuffer( pointCloud->vertices(), pointCloud->vertexCount() * pointCloud->vertexFormat().vertexSize() );

    initializeInstanceNode( entity, node, pointCloud->material() );

    return node;
}

// ** RenderScene::createLightNode
RenderScene::LightNode RenderScene::createLightNode( const Ecs::Entity& entity )
{
    LightNode light;

    light.transform         = entity.get<Transform>();
    light.matrix            = &light.transform->matrix();
    light.light             = entity.get<Light>();
    light.constantBuffer    = m_context->requestConstantBuffer( NULL, sizeof( CBuffer::Light ), CBuffer::Light::Layout );
    light.parameters        = DC_NEW CBuffer::Light;

    return light;
}

// ** RenderScene::createCameraNode
RenderScene::CameraNode RenderScene::createCameraNode( const Ecs::Entity& entity )
{
    CameraNode camera;

    camera.transform        = entity.get<Transform>();
    camera.matrix           = &camera.transform->matrix();
    camera.camera           = entity.get<Camera>();
    camera.constantBuffer   = m_context->requestConstantBuffer( NULL, sizeof( CBuffer::View ), CBuffer::View::Layout );
    camera.parameters       = DC_NEW CBuffer::View;

    return camera;
}

// ** RenderScene::createStaticMeshNode
RenderScene::StaticMeshNode RenderScene::createStaticMeshNode( const Ecs::Entity& entity )
{
    StaticMeshNode mesh;

    mesh.mesh = entity.get<StaticMesh>();
    mesh.material = mesh.mesh->material( 0 );
    mesh.timestamp = -1;
    mesh.vertexBuffer = 0;
    mesh.indexBuffer = 0;

    initializeInstanceNode( entity, mesh, mesh.mesh->material(0) );

    mesh.mesh->mesh().readLock();

    return mesh;
}

// ** RenderScene::initializeInstanceNode
void RenderScene::initializeInstanceNode( const Ecs::Entity& entity, InstanceNode& instance, const MaterialHandle& material )
{
    material.readLock();

    instance.transform          = entity.get<Transform>();
    instance.matrix             = &instance.transform->matrix();
    instance.constantBuffer     = m_context->requestConstantBuffer( NULL, sizeof( CBuffer::Instance ), CBuffer::Instance::Layout );
    instance.instanceParameters = DC_NEW CBuffer::Instance;

    if( material.isValid() ) {
        instance.materialConstants  = m_renderCache.findConstantBuffer( material );
        instance.materialParameters = DC_NEW CBuffer::Material;
    }
}

// -------------------------------------------------------------- RenderCache --------------------------------------------------------------

// ** RenderCache::RenderCache
RenderCache::RenderCache( RenderingContextWPtr context )
    : m_context( context )
{
}

// ** RenderCache::findInputLayout
RenderResource RenderCache::findInputLayout( const VertexFormat& format )
{
    InputLayouts::iterator i = m_inputLayouts.find( format );

    if( i != m_inputLayouts.end() ) {
        return i->second;
    }

    RenderResource id = m_context->requestInputLayout( format );
    m_inputLayouts[format] = id;
    return id;
}

// ** RenderCache::findVertexBuffer
RenderResource RenderCache::findVertexBuffer( const MeshHandle& mesh )
{
    RenderResources::iterator i = m_vertexBuffers.find( mesh.asset().uniqueId() );

    if( i != m_vertexBuffers.end() ) {
        return i->second;
    }

    DC_BREAK_IF( mesh->chunkCount() == 0, "could not cache an empty mesh" );

    const Mesh::VertexBuffer& vertices = mesh->vertexBuffer( 0 );
    VertexFormat vertexFormat( VertexFormat::Normal | VertexFormat::Uv0 | VertexFormat::Uv1 );

    RenderResource id = m_context->requestVertexBuffer( &vertices[0], vertices.size() * vertexFormat.vertexSize() );
    m_vertexBuffers[mesh.asset().uniqueId()] = id;

    LogVerbose( "renderCache", "vertex buffer with %d vertices created\n", vertices.size() );

    return id;
}

// ** RenderCache::findIndexBuffer
RenderResource RenderCache::findIndexBuffer( const MeshHandle& mesh )
{
    RenderResources::iterator i = m_indexBuffers.find( mesh.asset().uniqueId() );

    if( i != m_indexBuffers.end() ) {
        return i->second;
    }

    DC_BREAK_IF( mesh->chunkCount() == 0, "could not cache an empty mesh" );

    const Mesh::IndexBuffer& indices = mesh->indexBuffer( 0 );

    RenderResource id = m_context->requestIndexBuffer( &indices[0], indices.size() * sizeof( u16 ) );
    m_indexBuffers[mesh.asset().uniqueId()] = id;

    LogVerbose( "renderCache", "index buffer with %d indices created\n", indices.size() );

    return id;
}

// ** RenderCache::findConstantBuffer
RenderResource RenderCache::findConstantBuffer( const MaterialHandle& material )
{
    RenderResources::iterator i = m_materialConstantBuffers.find( material.asset().uniqueId() );

    if( i != m_materialConstantBuffers.end() ) {
        return i->second;
    }


    RenderResource id = m_context->requestConstantBuffer( NULL, sizeof RenderScene::CBuffer::Material, RenderScene::CBuffer::Material::Layout );
    m_materialConstantBuffers[material.asset().uniqueId()] = id;

    LogVerbose( "renderCache", "material constant buffer created for '%s'\n", material.asset().name().c_str() );

    return id;
}

} // namespace Scene

DC_END_DREEMCHEST