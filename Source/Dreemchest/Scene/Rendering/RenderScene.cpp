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
{
    // Get a parent Ecs instance
    Ecs::EcsWPtr ecs = scene->ecs();

    // Create entity caches
    m_pointClouds   = ecs->createDataCache<PointCloudCache>( Ecs::Aspect::all<PointCloud, Transform>(), dcThisMethod( RenderScene::createPointCloudNode ) );
    m_lights        = ecs->createDataCache<LightCache>( Ecs::Aspect::all<Light, Transform>(), dcThisMethod( RenderScene::createLightNode ) );
    m_cameras       = ecs->createDataCache<CameraCache>( Ecs::Aspect::all<Camera, Transform>(), dcThisMethod( RenderScene::createCameraNode ) );
    m_staticMeshes  = ecs->createDataCache<StaticMeshCache>( Ecs::Aspect::all<StaticMesh, Transform>(), dcThisMethod( RenderScene::createStaticMeshNode ) );

    // Create scene constant buffer
    m_sceneConstants  = m_context->createConstantBuffer( sizeof( CBuffer::Scene ), CBuffer::Scene::Layout );
    m_sceneParameters = DC_NEW CBuffer::Scene;
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
    RenderStateBlock& defaults = stateStack.push();
    defaults.disableAlphaTest();
    defaults.disableBlending();
    defaults.setDepthState( Renderer::LessEqual, true );
    defaults.enableFeatures( BIT( ShaderAmbientColor ) );
    defaults.bindConstantBuffer( m_sceneConstants, RenderState::GlobalConstants );

    // Clear all cameras
    const Cameras& cameras = m_cameras->data();

    for( s32 i = 0, n = cameras.count(); i < n; i++ ) {
        const CameraNode& camera = cameras[i];
        entryPoint.clear( m_context->internRenderTarget( camera.camera->target() ), camera.camera->clearColor(), camera.camera->viewport(), camera.camera->clearMask() );
    }

    // Process all render systems
    for( s32 i = 0, n = static_cast<s32>( m_renderSystems.size() ); i < n; i++ ) {
        m_renderSystems[i]->render( *m_context.get(), *frame.get(), entryPoint );
    }

    // Pop a default state block
    stateStack.pop();

    return frame;
}

// ** RenderScene::createShader
UbershaderPtr RenderScene::createShader( const String& fileName ) const
{
	static CString vertexShaderMarker   = "[VertexShader]";
	static CString fragmentShaderMarker = "[FragmentShader]";
    static CString featuresMarker       = "[Features]";

    Map<String, u64> masks;
    
    masks["inputNormal"]        = BIT( ShaderInputNormal     + InputFeaturesOffset );
    masks["inputColor"]         = BIT( ShaderInputColor      + InputFeaturesOffset );
    masks["inputUv0"]           = BIT( ShaderInputUv0        + InputFeaturesOffset );
    masks["inputUv1"]           = BIT( ShaderInputUv1        + InputFeaturesOffset );
    masks["inputUv2"]           = BIT( ShaderInputUv2        + InputFeaturesOffset );
    masks["inputUv3"]           = BIT( ShaderInputUv3        + InputFeaturesOffset );
    masks["inputUv4"]           = BIT( ShaderInputUv4        + InputFeaturesOffset );
    masks["texture0"]           = BIT( ShaderTexture0        + ResourceFeaturesOffset );
    masks["texture1"]           = BIT( ShaderTexture1        + ResourceFeaturesOffset );
    masks["texture2"]           = BIT( ShaderTexture2        + ResourceFeaturesOffset );  
    masks["texture3"]           = BIT( ShaderTexture3        + ResourceFeaturesOffset ); 
    masks["ambientColor"]       = BIT( ShaderAmbientColor    + MaterialFeaturesOffset );
    masks["pointLight"]         = BIT( ShaderPointLight      + MaterialFeaturesOffset );
    masks["emissionColor"]      = BIT( ShaderEmissionColor   + MaterialFeaturesOffset );

    // Create a shader instance
    UbershaderPtr shader = DC_NEW Ubershader;

	// Read the code from an input stream
	String code = Io::DiskFileSystem::readTextFile( fileName );

	// Extract vertex/fragment shader code blocks
	u32 vertexBegin = code.find( vertexShaderMarker );
	u32 fragmentBegin = code.find( fragmentShaderMarker );
    u32 featuresBegin = code.find( featuresMarker );

	if( vertexBegin == String::npos && fragmentBegin == String::npos ) {
		return false;
	}

    if( featuresBegin != String::npos ) {
        u32 featuresCodeStart = featuresBegin + strlen( featuresMarker );
        Array<String> features = split( code.substr( featuresCodeStart, vertexBegin - featuresCodeStart ), "\r\n" );

        for( Array<String>::const_iterator i = features.begin(), end = features.end(); i != end; ++i ) {
            Array<String> value = split( *i, " \t=" );
            shader->addFeature( masks[value[1]], value[0] );
            LogVerbose( "shader", "feature %s = %s (0x%x) added\n", value[0].c_str(), value[1].c_str(), masks[value[1]] );
        }
    }

	if( vertexBegin != String::npos ) {
		u32 vertexCodeStart = vertexBegin + strlen( vertexShaderMarker );
		String vertex = code.substr( vertexCodeStart, fragmentBegin > vertexBegin ? fragmentBegin - vertexCodeStart : String::npos );
        shader->setVertex( vertex );
	}

	if( fragmentBegin != String::npos ) {
		u32 fragmentCodeStart = fragmentBegin + strlen( fragmentShaderMarker );
		String fragment = code.substr( fragmentCodeStart, vertexBegin > fragmentBegin ? vertexBegin - fragmentCodeStart : String::npos );
        shader->setFragment( fragment );
	}

    // ----------------------------------------------------
    shader->addInclude(
            "                                                       \n\
                struct CBufferScene    { vec4 ambient; };           \n\
                struct CBufferView     { mat4 transform; };    \n\
                struct CBufferInstance { mat4 transform; };         \n\
                struct CBufferMaterial { vec4 diffuse; vec4 specular; vec4 emission; };         \n\
                struct CBufferLight    { vec3 position; float range; vec3 color; float intensity; };         \n\
                uniform CBufferScene    Scene;                      \n\
                uniform CBufferView     View;                       \n\
                uniform CBufferInstance Instance;                   \n\
                uniform CBufferMaterial Material;                   \n\
                uniform CBufferLight    Light;                      \n\
            "
        );

	return shader;
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
            VertexFormat vf( VertexFormat::Normal | VertexFormat::Uv0 | VertexFormat::Uv1  );
            const Mesh::VertexBuffer& vb = mesh->vertexBuffer( 0 );
            const Mesh::IndexBuffer& ib = mesh->indexBuffer( 0 );
            node.vertexBuffer = m_context->createVertexBuffer( &vb[0], vb.size(), vf, vf );
            node.indexBuffer  = m_context->createIndexBuffer( &ib[0], ib.size() );
            node.inputLayout  = m_context->createInputLayout( vf );
            node.indexCount   = ib.size();
            LogVerbose( "renderScene", "reloaded static mesh renderable with %d vertices and %d indices\n", vb.size(), ib.size() );
        }
    }
}

// ** RenderScene::createPointCloudNode
RenderScene::PointCloudNode RenderScene::createPointCloudNode( const Ecs::Entity& entity )
{
    const Transform*  transform  = entity.get<Transform>();
    const PointCloud* pointCloud = entity.get<PointCloud>();

    PointCloudNode node;

    initializeInstanceNode( entity, node );

    node.vertexCount    = pointCloud->vertexCount();
    node.material       = pointCloud->material();
    node.inputLayout    = m_context->createInputLayout( pointCloud->vertexFormat() );
    node.vertexBuffer   = m_context->createVertexBuffer( pointCloud->vertices(), pointCloud->vertexCount(), pointCloud->vertexFormat(), pointCloud->vertexFormat() );

    return node;
}

// ** RenderScene::createLightNode
RenderScene::LightNode RenderScene::createLightNode( const Ecs::Entity& entity )
{
    LightNode light;

    light.transform         = entity.get<Transform>();
    light.matrix            = &light.transform->matrix();
    light.light             = entity.get<Light>();
    light.constantBuffer    = m_context->createConstantBuffer( sizeof( CBuffer::Light ), CBuffer::Light::Layout );
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
    camera.constantBuffer   = m_context->createConstantBuffer( sizeof( CBuffer::View ), CBuffer::View::Layout );
    camera.parameters       = DC_NEW CBuffer::View;

    return camera;
}

// ** RenderScene::createStaticMeshNode
RenderScene::StaticMeshNode RenderScene::createStaticMeshNode( const Ecs::Entity& entity )
{
    StaticMeshNode mesh;

    initializeInstanceNode( entity, mesh );
    mesh.mesh = entity.get<StaticMesh>();
    mesh.material = mesh.mesh->material( 0 );
    mesh.timestamp = -1;

    mesh.mesh->mesh().readLock();

    return mesh;
}

// ** RenderScene::initializeInstanceNode
void RenderScene::initializeInstanceNode( const Ecs::Entity& entity, InstanceNode& instance )
{
    instance.transform          = entity.get<Transform>();
    instance.matrix             = &instance.transform->matrix();
    instance.constantBuffer     = m_context->createConstantBuffer( sizeof( CBuffer::Instance ), CBuffer::Instance::Layout );
    instance.materialConstants  = m_context->createConstantBuffer( sizeof( CBuffer::Material ), CBuffer::Material::Layout );
    instance.materialParameters = DC_NEW CBuffer::Material;
    instance.instanceParameters = DC_NEW CBuffer::Instance;
}

} // namespace Scene

DC_END_DREEMCHEST