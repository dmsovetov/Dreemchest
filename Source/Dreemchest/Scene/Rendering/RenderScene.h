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

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Render scene contains all renderable entities, performs culling and constructs command buffers.
    class RenderScene : public RefCounted {
    public:

        //! Base class for all renderable entities.
        struct RenderableEntity {
            const Transform*        transform;          //!< Mesh transform component.
            const Matrix4*          matrix;             //!< Mesh transform affine matrix.
        };

        //! Stores info about a single renderable mesh.
        struct RenderableMesh : public RenderableEntity {
            MaterialHandle          material;           //!< Material used for rendering.
            MeshHandle              mesh;               //!< Rendered mesh resource handle.
            RenderAssetIndex        renderable;         //!< Renderable asset index.
            RenderAssetIndex        technique;          //!< Technique asset index.
        };

        //! Stores info about a single light entity.
        struct RenderableLight : public RenderableEntity {
        };

        //! A single frame is just an array of command buffers.
        typedef Array<CommandsUPtr> Frame;

        //! A fixed array with renderable meshes inside.
        typedef FixedArray<RenderableMesh> Meshes;

        //! A fixed array with lights inside.
        typedef FixedArray<RenderableLight> Lights;

		//! Adds a new render system to the scene.
		template<typename TRenderSystem>
		void						    addRenderSystem( void );

        //! Captures scene rendering state and returns an array of resulting command buffers.
        Frame                           captureFrame( void );

        //! Renders a frame by executing all commands.
        void                            display( Frame& frame );

        //! Returns parent rendering context.
        RenderingContextWPtr            context( void ) const;

        //! Returns parent scene.
        SceneWPtr                       scene( void ) const;

        //! Returns renderable meshes.
        const Meshes&                   meshes( void ) const;

        //! Creates a new render scene.
        static RenderScenePtr           create( SceneWPtr scene, RenderingContextWPtr context );

    private:

                                        //! Constructs RenderScene instance.
                                        RenderScene( SceneWPtr scene, RenderingContextWPtr context );

        //! Creates renderable mesh item from a scene object.
        RenderableMesh                  createMeshFromEntity( const Ecs::Entity& entity );

        //! Creates light item item from a scene object.
        RenderableLight                 createLightFromEntity( const Ecs::Entity& entity );

    private:

        //! Entity data cache to store renderable meshes.
        typedef Ecs::DataCache<RenderableMesh>    MeshCache;

        //! Entity data cache to store scene lights.
        typedef Ecs::DataCache<RenderableLight>   LightCache;

        SceneWPtr                       m_scene;            //!< Parent scene instance.
        RenderingContextWPtr            m_context;          //!< Parent rendering context instance.
        Array<RenderSystemUPtr>	        m_renderSystems;    //!< Entity render systems.
        Ptr<MeshCache>                  m_meshes;           //!< Renderable mesh cache.
        Ptr<LightCache>                 m_lights;           //!< Renderable light cache.
        Ecs::IndexPtr					m_cameras;			//!< All cameras that reside in scene.
    };

	// ** RenderScene::addRenderSystem
	template<typename TRenderSystem>
	void RenderScene::addRenderSystem( void )
	{
		m_renderSystems.push_back( DC_NEW TRenderSystem( *this ) );
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderScene_H__    */