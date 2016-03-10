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

#ifndef __DC_Scene_RenderingContext_H__
#define __DC_Scene_RenderingContext_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Render command buffer
    class RenderCommandBuffer {
    public:

        void            push( s32 renderable );

        void            flush( const Array<RenderableHandle>& renderables, Renderer::HalWPtr hal );

    private:

        s32             m_commandSize;  //!< Maximum command size.
        s32             m_meshBits;     //!< The total number of command bits available for the renderable index.
        Array<s32>      m_commands;     //!< Actual command buffer.
    };

	//! Rendering context.
	class RenderingContext : public RefCounted {
	public:

        //! Begins rendering with this context.
        void                                begin( void );

        //! Ends rendering with this context.
        void                                end( void );

        //! Returns the parent rendering HAL instance.
        Renderer::HalWPtr                   hal( void ) const;

		//! Creates new rendering context.
		static RenderingContextPtr			create( Assets::Assets& assets, Renderer::HalWPtr hal, SceneWPtr scene );

	private:

											//! Constructs the RenderingContext instance.
											RenderingContext( Assets::Assets& assets, Renderer::HalWPtr hal, SceneWPtr scene );

        //! Returns the renderable asset for a specified mesh.
        s32                                 requestRenderable( MeshHandle mesh );

        //! Renders the scene from a camera point of view.
        void                                renderFromCamera( RenderCommandBuffer& commands, Ecs::Entity& entity, Camera& camera, Transform& transform );

        //! Renders all static meshes.
        void                                renderStaticMeshes( RenderCommandBuffer& commands );

	private:

        //! Container type to map from a mesh asset to a renderable asset index.
        typedef Map<MeshHandle, s32> RenderableByMesh;

        Renderer::HalWPtr                   m_hal;          //!< Parent HAL instance.
        SceneWPtr                           m_scene;        //!< Parent scene instance.
        Assets::Assets&                     m_assets;       //!< Asset manager used to create render assets.

        Ecs::IndexPtr                       m_staticMeshes;
        Ecs::IndexPtr                       m_lights;
        RenderableByMesh                    m_renderableByMesh;
        Array<RenderableHandle>             m_renderableHandles;
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContext_H__    */