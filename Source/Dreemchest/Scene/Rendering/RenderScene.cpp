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

struct Debug_Structure_To_Track_Data_Size_Used_By_Node_Types {
    enum {
          _Light = sizeof( RenderScene::LightNode )
        , _PointCloud = sizeof( RenderScene::PointCloudNode )
        , _Camera = sizeof( RenderScene::CameraNode )
        , _StaticMesh = sizeof( RenderScene::StaticMeshNode )
    };
};

// ** RenderScene::RenderScene
RenderScene::RenderScene( SceneWPtr scene, Renderer::HalWPtr hal )
    : m_scene( scene )
    , m_hal( hal )
{
    // Get a parent Ecs instance
    Ecs::EcsWPtr ecs = scene->ecs();

    // Create entity caches
    m_pointClouds   = ecs->createDataCache<PointCloudCache>( Ecs::Aspect::all<PointCloud, Transform>(), dcThisMethod( RenderScene::createPointCloudNode ) );
    m_lights        = ecs->createDataCache<LightCache>( Ecs::Aspect::all<Light, Transform>(), dcThisMethod( RenderScene::createLightNode ) );
    m_cameras       = ecs->createDataCache<CameraCache>( Ecs::Aspect::all<Camera, Transform>(), dcThisMethod( RenderScene::createCameraNode ) );
    m_staticMeshes  = ecs->createDataCache<StaticMeshCache>( Ecs::Aspect::all<StaticMesh, Transform>(), dcThisMethod( RenderScene::createStaticMeshNode ) );

    // Create scene constant buffer
    m_sceneConstants = hal->createConstantBuffer( sizeof( CBuffer::Scene ), false );
    m_sceneConstants->addConstant( Renderer::ConstantBuffer::Vec4, offsetof( CBuffer::Scene, ambient ), "Scene.ambient" );
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
    updateConstantBuffers();

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
    defaults.bindConstantBuffer( frame->internConstantBuffer( m_sceneConstants ), RenderState::GlobalConstants );

    // Clear all cameras
    const Cameras& cameras = m_cameras->data();

    for( s32 i = 0, n = cameras.count(); i < n; i++ ) {
        const CameraNode& camera = cameras[i];
        entryPoint.clear( frame->internRenderTarget( camera.camera->target() ), camera.camera->clearColor(), camera.camera->viewport(), camera.camera->clearMask() );
    }

    // Process all render systems
    for( s32 i = 0, n = static_cast<s32>( m_renderSystems.size() ); i < n; i++ ) {
        m_renderSystems[i]->render( *frame.get(), entryPoint );
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
void RenderScene::updateConstantBuffers( void )
{
    // Update scene constant buffer
    CBuffer::Scene* sceneConstants = m_sceneConstants->lock<CBuffer::Scene>();
    sceneConstants->ambient = Rgba( 0.2f, 0.2f, 0.2f, 1.0f );
    m_sceneConstants->unlock();

    // Update camera constant buffers
    Cameras& cameras = m_cameras->data();

    for( s32 i = 0, n = cameras.count(); i < n; i++ ) {
        CameraNode& node = cameras[i];
        {
            CBuffer::View* cameraConstants = node.cameraConstants->lock<RenderScene::CBuffer::View>();
            cameraConstants->transform = node.camera->calculateViewProjection( node.transform->matrix() );
            node.cameraConstants->unlock();
        }
    }

    // Update light constant buffers
    Lights& lights = m_lights->data();

    for( s32 i = 0, n = lights.count(); i < n; i++ ) {
        LightNode& node = lights[i];

        {
            CBuffer::Light* cbuffer = node.lightConstants->lock<CBuffer::Light>();
            cbuffer->position  = node.transform->worldSpacePosition();
            cbuffer->intensity = node.light->intensity();
            cbuffer->color     = node.light->color();
            cbuffer->range     = node.light->range();
            node.lightConstants->unlock();
        }
    }

    // Update point cloud constant buffers
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

    // Update static mesh constant buffers
    StaticMeshes& staticMeshes = m_staticMeshes->data();

    for( s32 i = 0, n = staticMeshes.count(); i < n; i++ ) {
        StaticMeshNode& node = staticMeshes[i];

        {
            CBuffer::Instance* cbuffer = node.instanceConstants->lock<CBuffer::Instance>();
            cbuffer->transform = node.transform->matrix();
            node.instanceConstants->unlock();
        }
        if( node.material.isValid() )
        {
            CBuffer::Material* cbuffer = node.materialConstants->lock<CBuffer::Material>();
            cbuffer->diffuse  = node.material->color( Material::Diffuse );
            cbuffer->specular = node.material->color( Material::Specular );
            cbuffer->emission = node.material->color( Material::Emission );
            node.materialConstants->unlock();
        }
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
            node.vertexBuffer = createVertexBuffer( &vb[0], vb.size(), vf, vf );
            node.indexBuffer  = createIndexBuffer( &ib[0], ib.size() );
            node.inputLayout = createInputLayout( vf );
            node.indexCount = ib.size();
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
    node.inputLayout    = createInputLayout( pointCloud->vertexFormat() );
    node.vertexBuffer   = createVertexBuffer( pointCloud->vertices(), pointCloud->vertexCount(), pointCloud->vertexFormat(), pointCloud->vertexFormat() );

    return node;
}

// ** RenderScene::createLightNode
RenderScene::LightNode RenderScene::createLightNode( const Ecs::Entity& entity )
{
    LightNode light;

    light.transform = entity.get<Transform>();
    light.matrix    = &light.transform->matrix();
    light.light     = entity.get<Light>();

    light.lightConstants = m_hal->createConstantBuffer( sizeof( CBuffer::Light ), false );
    light.lightConstants->addConstant( Renderer::ConstantBuffer::Vec3, offsetof( CBuffer::Light, position ), "Light.position" );
    light.lightConstants->addConstant( Renderer::ConstantBuffer::Float, offsetof( CBuffer::Light, range ), "Light.range" );
    light.lightConstants->addConstant( Renderer::ConstantBuffer::Vec3, offsetof( CBuffer::Light, color ), "Light.color" );
    light.lightConstants->addConstant( Renderer::ConstantBuffer::Float, offsetof( CBuffer::Light, intensity ), "Light.intensity" );

    return light;
}

// ** RenderScene::createCameraNode
RenderScene::CameraNode RenderScene::createCameraNode( const Ecs::Entity& entity )
{
    CameraNode camera;

    camera.transform = entity.get<Transform>();
    camera.matrix    = &camera.transform->matrix();
    camera.camera    = entity.get<Camera>();

    camera.cameraConstants = m_hal->createConstantBuffer( sizeof( CBuffer::View ), false );
    camera.cameraConstants->addConstant( Renderer::ConstantBuffer::Matrix4, offsetof( CBuffer::View, transform ), "View.transform" );

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
    instance.transform = entity.get<Transform>();
    instance.matrix    = &instance.transform->matrix();

    instance.instanceConstants = m_hal->createConstantBuffer( sizeof( CBuffer::Instance ), false );
    instance.instanceConstants->addConstant( Renderer::ConstantBuffer::Matrix4, offsetof( CBuffer::Instance, transform ), "Instance.transform" );

    instance.materialConstants = m_hal->createConstantBuffer( sizeof( CBuffer::Material ), false );
    instance.materialConstants->addConstant( Renderer::ConstantBuffer::Vec4, offsetof( CBuffer::Material, diffuse ), "Material.diffuse" );
    instance.materialConstants->addConstant( Renderer::ConstantBuffer::Vec4, offsetof( CBuffer::Material, specular ), "Material.specular" );
    instance.materialConstants->addConstant( Renderer::ConstantBuffer::Vec4, offsetof( CBuffer::Material, emission ), "Material.emission" );
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

// ** RenderScene::createIndexBuffer
Renderer::IndexBufferPtr RenderScene::createIndexBuffer( const u16* indices, s32 count )
{
    // Create an index buffer instance
    Renderer::IndexBufferPtr indexBuffer = m_hal->createIndexBuffer( count * sizeof( u16 ) );

    // Copy memory to a GPU index buffer
    memcpy( indexBuffer->lock(), indices, count * sizeof( u16 ) );

    // Unlock an index buffer.
    indexBuffer->unlock();

    return indexBuffer;
}

} // namespace Scene

DC_END_DREEMCHEST