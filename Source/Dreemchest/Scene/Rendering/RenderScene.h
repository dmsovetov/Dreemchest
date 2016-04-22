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

#ifndef __DC_Scene_RenderScene_H__
#define __DC_Scene_RenderScene_H__

#include "../Scene.h"
#include "Rvm/RenderFrame.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    typedef u32 RenderResource;

    //! Render scene contains all renderable entities, performs culling and constructs command buffers.
    class RenderScene : public RefCounted {
    public:

        //! A constant buffer types
        struct CBuffer {
            typedef Renderer::ConstantBufferLayout BufferLayout;
            struct Scene {
                static BufferLayout Layout[];
                Rgba                ambient;
            };
            struct View {
                static BufferLayout Layout[];
                Matrix4             transform;
            };
            struct Instance {
                static BufferLayout Layout[];
                Matrix4             transform;
            };
            struct Material {
                static BufferLayout Layout[];
                Rgba                diffuse;
                Rgba                specular;
                Rgba                emission;
            };
            struct Light {
                static BufferLayout Layout[];
                Vec3                position;
                f32                 range;
                Rgb                 color;
                f32                 intensity;
            };
        };

        //! Base class for all renderable entities.
        struct Node {
            const Transform*                    transform;          //!< Node transform component.
            const Matrix4*                      matrix;             //!< Node transform affine matrix.
            RenderResource                      constantBuffer;     //!< Node constant buffer.
        };

        //! Stores info about a scene instance.
        struct InstanceNode : public Node {
            MaterialHandle                      material;
            RenderResource                      materialConstants;  //!< A point cloud material options.   
            AutoPtr<CBuffer::Material>          materialParameters; //!< Material constant buffer.
            AutoPtr<CBuffer::Instance>          instanceParameters; //!< Instance constant buffer.
        };

        //! Stores info about a renderable point cloud.
        struct PointCloudNode : public InstanceNode {
            RenderResource                      vertexBuffer;       //!< A point cloud vertex buffer.
            RenderResource                      inputLayout;        //!< A point cloud input layout.    
            s32                                 vertexCount;        //!< A total number of vertices inside a point cloud.
        };

        //! Stores info about a renderable light.
        struct LightNode : public Node {
            const Light*                        light;              //!< Light component.
            AutoPtr<CBuffer::Light>             parameters;         //!< Light constant buffer.
        };

        //! Stores info about a camera.
        struct CameraNode : public Node {
            const Camera*                       camera;             //!< Camera component.
            AutoPtr<CBuffer::View>              parameters;         //!< View constant buffer.
        };

        //! Stores info about a static mesh.
        struct StaticMeshNode : public InstanceNode {
            const StaticMesh*                   mesh;               //!< Mesh component.
            u32                                 timestamp;          //!< Asset modification timestamp.
            u32                                 indexCount;         //!< A total number of indices in a mesh.
            RenderResource                      vertexBuffer;       //!< A mesh vertex buffer.
            RenderResource                      indexBuffer;        //!< A mesh index buffer.
            RenderResource                      inputLayout;        //!< A mesh input layout.  
        };

        //! A fixed array with renderable point clouds inside.
        typedef FixedArray<PointCloudNode>      PointClouds;

        //! A fixed array with light nodes inside.
        typedef FixedArray<LightNode>           Lights;

        //! A fixed array with camera nodes inside.
        typedef FixedArray<CameraNode>          Cameras;

        //! A fixed array with static mesh nodes inside.
        typedef FixedArray<StaticMeshNode>      StaticMeshes;

        //! Returns parent scene.
        SceneWPtr                               scene( void ) const;

        //! Returns renderable point clouds.
        const PointClouds&                      pointClouds( void ) const;

        //! Returns light nodes.
        const Lights&                           lights( void ) const;

        //! Returns camera nodes.
        const Cameras&                          cameras( void ) const;

        //! Returns static mesh nodes.
        const StaticMeshes&                     staticMeshes( void ) const;

        //! Returns a camera node by a component.
        const CameraNode&                       findCameraNode( Ecs::EntityWPtr camera ) const;

		//! Adds a new render system to the scene.
		template<typename TRenderSystem, typename ... TArgs>
		void						            addRenderSystem( const TArgs& ... args );

        //! Captures scene rendering state and returns an array of resulting command buffers.
        RenderFrameUPtr                         captureFrame( void );

        //! Creates a ubershader from a file.
        UbershaderPtr                           createShader( const String& fileName ) const;

        //! Creates a new render scene.
        static RenderScenePtr                   create( SceneWPtr scene, RenderingContextWPtr context );

    private:

                                                //! Constructs RenderScene instance.
                                                RenderScene( SceneWPtr scene, RenderingContextWPtr context );

        //! Creates a point cloud node from an entity.
        PointCloudNode                          createPointCloudNode( const Ecs::Entity& entity );

        //! Creates a light node from an entity.
        LightNode                               createLightNode( const Ecs::Entity& entity );

        //! Creates a camera node from an entity.
        CameraNode                              createCameraNode( const Ecs::Entity& entity );

        //! Creates a static mesh node from an entity.
        StaticMeshNode                          createStaticMeshNode( const Ecs::Entity& entity );

        //! Setups an instance render scene node.
        void                                    initializeInstanceNode( const Ecs::Entity& entity, InstanceNode& instance );

        //! Updates all active constant buffers.
        void                                    updateConstantBuffers( RenderFrame& frame );

        //! Updates all active mesh buffers.
        void                                    updateStaticMeshes( void );

    private:

        //! Entity data cache to store renderable point clouds.
        typedef Ecs::DataCache<PointCloudNode>  PointCloudCache;

        //! Entity data cache to store light nodes.
        typedef Ecs::DataCache<LightNode>       LightCache;

        //! Entity data cache to store camera nodes.
        typedef Ecs::DataCache<CameraNode>      CameraCache;

        //! Entity data cache to store static meshes.
        typedef Ecs::DataCache<StaticMeshNode>  StaticMeshCache;

        RenderingContextWPtr                    m_context;          //!< Parent rendering context.
        s32                                     m_sceneConstants;   //!< Global constant buffer with scene variables.
        AutoPtr<CBuffer::Scene>                 m_sceneParameters;  //!< Scene parameters constant buffer.
        UbershaderPtr                           m_defaultShader;    //!< A default shader that will be used if no shader set by a pass.
        SceneWPtr                               m_scene;            //!< Parent scene instance.
        Array<RenderSystemUPtr>	                m_renderSystems;    //!< Entity render systems.
        Ptr<PointCloudCache>                    m_pointClouds;      //!< Renderable point clouds cache.
        Ptr<LightCache>                         m_lights;           //!< Light nodes cache.
        Ptr<CameraCache>                        m_cameras;          //!< Camera nodes cache.
        Ptr<StaticMeshCache>                    m_staticMeshes;     //!< Static mesh nodes cache.
    };

	// ** RenderScene::addRenderSystem
	template<typename TRenderSystem, typename ... TArgs>
	void RenderScene::addRenderSystem( const TArgs& ... args )
	{
		m_renderSystems.push_back( DC_NEW TRenderSystem( *this, args... ) );
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderScene_H__    */