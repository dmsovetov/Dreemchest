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

    //! Render scene contains all renderable entities, performs culling and constructs command buffers.
    class RenderScene : public RefCounted {
    public:

        //! A constant buffer types
        struct CBuffer {
            struct Camera {
                Matrix4     viewProjection;
            };
            struct Instance {
                Matrix4     transform;
            };
        };

        //! Base class for all renderable entities.
        struct Node {
            const Transform*                    transform;      //!< Mesh transform component.
            const Matrix4*                      matrix;         //!< Mesh transform affine matrix.
        };

        //! Stores info about a renderable point cloud.
        struct PointCloudNode : public Node {
            Renderer::VertexBufferPtr           vertexBuffer;   //!< A point cloud vertex buffer.
            Renderer::InputLayoutPtr            inputLayout;    //!< A point cloud input layout.
            Renderer::ConstantBufferPtr         constantBuffer; //!< A point cloud constant buffer instance.
            s32                                 vertexCount;    //!< A total number of vertices inside a point cloud.
        };

        //! A fixed array with renderable point clouds inside.
        typedef FixedArray<PointCloudNode>      PointClouds;

        //! Returns parent scene.
        SceneWPtr                               scene( void ) const;

        //! Returns renderable point clouds.
        const PointClouds&                      pointClouds( void ) const;

		//! Adds a new render system to the scene.
		template<typename TRenderSystem, typename ... TArgs>
		void						            addRenderSystem( const TArgs& ... args );

        //! Captures scene rendering state and returns an array of resulting command buffers.
        RenderFrameUPtr                         captureFrame( void );

        //! Creates a new render scene.
        static RenderScenePtr                   create( SceneWPtr scene, Renderer::HalWPtr hal );

    private:

                                                //! Constructs RenderScene instance.
                                                RenderScene( SceneWPtr scene, Renderer::HalWPtr hal );

        //! Creates a point cloud node from an entity.
        PointCloudNode                          createPointCloudNode( const Ecs::Entity& entity );

        //! Updates instance constant buffers.
        void                                    updateInstanceConstants( void );

    private:

        //! Entity data cache to store renderable point clouds.
        typedef Ecs::DataCache<PointCloudNode>  PointCloudCache;

        Renderer::HalWPtr                       m_hal;              //!< Rendering HAL to be used.
        SceneWPtr                               m_scene;            //!< Parent scene instance.
        Array<RenderSystemUPtr>	                m_renderSystems;    //!< Entity render systems.
        Ptr<PointCloudCache>                    m_pointClouds;      //!< Renderable point clouds cache.
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