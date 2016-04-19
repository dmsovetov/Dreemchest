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

        //! Returns parent scene.
        SceneWPtr                       scene( void ) const;

		//! Adds a new render system to the scene.
		template<typename TRenderSystem, typename ... TArgs>
		void						    addRenderSystem( const TArgs& ... args );

        //! Captures scene rendering state and returns an array of resulting command buffers.
        RenderFrameUPtr                 captureFrame( Renderer::HalWPtr hal );

        //! Creates a new render scene.
        static RenderScenePtr           create( SceneWPtr scene );

    private:

                                        //! Constructs RenderScene instance.
                                        RenderScene( SceneWPtr scene );

    private:

        SceneWPtr                       m_scene;            //!< Parent scene instance.
        Ecs::IndexPtr                   m_cameras;          //!< All cameras that reside in scene.
        Array<RenderSystemUPtr>	        m_renderSystems;    //!< Entity render systems.
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