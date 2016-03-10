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

#ifndef __DC_Scene_RenderingSystemDeprecated_H__
#define __DC_Scene_RenderingSystemDeprecated_H__

#include "../Components/Rendering.h"
#include "../Components/Transform.h"

#include "RenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

#if DEV_DEPRECATED_SCENE_RENDERER

	//! Base class for all rendering systems.
	class RenderingSystemBase : public RefCounted {
	public:

								//! Constructs the RenderingSystem instance.
								RenderingSystemBase( Ecs::EcsWPtr ecs, const String& name, const Ecs::Aspect& aspect );

		//! Adds a new render pass to this system.
		template<typename TPass>
		void					addPass( void );

		//! Renders all active cameras to their viewports.
		void					render( RenderingContextPtr context );

	protected:

		//! Renders the scene from a camera.
		virtual void			renderFromCamera( RenderingContextPtr context, Ecs::Entity& entity, Camera& camera, Transform& transform );

	private:

		//! Container type to store nested render passes.
		typedef Array<RenderPassBasePtr> RenderPasses;

		Ecs::EcsWPtr			m_ecs;		//!< Entity system.
		String					m_name;		//!< Rendering system name.
		Ecs::IndexPtr			m_cameras;	//!< All active cameras.
		RenderPasses			m_passes;	//!< All render passes to be performed by this rendering system.
	};

	// ** RenderingSystemBase::addPass
	template<typename TPass>
	void RenderingSystemBase::addPass( void )
	{
		m_passes.push_back( DC_NEW TPass( m_ecs ) );
	}

	//! Generic render system class.
	template<typename TRenderer>
	class RenderingSystem : public RenderingSystemBase {
	public:

								//! Constructs RenderingSystem instance
								RenderingSystem( Ecs::EcsWPtr ecs, const String& name )
									: RenderingSystemBase( ecs, name, Ecs::Aspect::all<Camera, Transform, TRenderer>() ) {}

		//! Extracts the renderer component before rendering.
		virtual void			renderFromCamera( RenderingContextPtr context, Ecs::Entity& entity, Camera& camera, Transform& transform ) DC_DECL_OVERRIDE;

	protected:

		TRenderer*				m_renderer;	//!< Renderer component.
	};

	// ** RenderingSystem::renderFromCamera
	template<typename TRenderer>
	void RenderingSystem<TRenderer>::renderFromCamera( RenderingContextPtr context, Ecs::Entity& entity, Camera& camera, Transform& transform )
	{
		m_renderer = entity.get<TRenderer>();
		RenderingSystemBase::renderFromCamera( context, entity, camera, transform );
	}

	//! Generic class for single pass rendering systems.
	template<typename TRenderer, typename TPass>
	class SinglePassRenderingSystem : public RenderingSystem<TRenderer> {
	public:
								//! Constructs SinglePassRenderingSystem instance.
								SinglePassRenderingSystem( Ecs::EcsWPtr ecs )
									: RenderingSystem<TRenderer>( ecs, "SinglePassRenderingSystem" ) { this->template addPass<TPass>(); }
	};

#endif  /*  DEV_DEPRECATED_SCENE_RENDERER   */

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingSystemDeprecated_H__    */