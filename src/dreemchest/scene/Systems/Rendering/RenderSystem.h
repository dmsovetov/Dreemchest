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

#ifndef __DC_Scene_RenderSystem_H__
#define __DC_Scene_RenderSystem_H__

#include "../SceneSystem.h"

#include "../../Components/Rendering.h"	//!< Include the rendering components here
#include "../../Components/Transform.h"	//!< Everything that is rendered should have a transform component, so include this file here

DC_BEGIN_DREEMCHEST

namespace scene {

	//! A helper struct to wrap HAL & 2D renderer instances.
	struct Renderer {
		renderer::HalPtr		m_hal;			//!< Rendering HAL.
		renderer::Renderer2DPtr	m_renderer2d;	//!< 2D renderer.

								//! Constructs empty Renderer instance.
								Renderer( void )
									{}

								//! Constructs Renderer instance.
								Renderer( const renderer::HalPtr& hal )
									: m_hal( hal ) {}

								//! Constructs Renderer2D instance.
								Renderer( const renderer::Renderer2DPtr& renderer2d )
									: m_renderer2d( renderer2d ) {}
	};
	 
	//! Base class for all render systems.
	class RenderSystemBase : public Ecs::EntitySystem {
	public:

								//! Constructs RenderSystemBase instance
								RenderSystemBase( Ecs::Entities& entities, const String& name, const Ecs::Aspect& aspect, const Renderer& renderer )
									: EntitySystem( entities, name, aspect ), m_renderer( renderer ) {}

		//! Adds a new render pass to this system.
		template<typename T>
		void					addPass( void );

	protected:

		//! Renders all nested render passes using the camera.
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity );

	protected:

		//! Container type to store nested render passes.
		typedef List<RenderPassBasePtr> RenderPasses;

		Renderer				m_renderer;	//!< Renderers wraper.
		RenderPasses			m_passes;	//!< Render passes.
	};

	// ** RenderSystemBase::addPass
	template<typename T>
	void RenderSystemBase::addPass( void )
	{
		m_passes.push_back( DC_NEW T( m_entities, m_renderer ) );
	}

	//! Generic render system to subclass user-defined rendering systems from
	template<typename TRender>
	class RenderSystem : public RenderSystemBase {
	public:

								//! Constructs RenderSystem instance
								RenderSystem( Ecs::Entities& entities, const String& name, const Renderer& renderer )
									: RenderSystemBase( entities, name, Ecs::Aspect::all<Camera, Transform, TRender>(), renderer ) {}
	};

	//! Generic single pass renderer
	template<typename TRenderer, typename TPass>
	class SinglePassRenderer : public RenderSystem<TRenderer> {
	public:

								SinglePassRenderer( Ecs::Entities& entities, const Renderer& renderer )
									: RenderSystem( entities, "SinglePassRenderer", renderer ) { addPass<TPass>(); }
	};

	//! Base class for all render passes.
	class RenderPassBase : public Ecs::EntitySystem {
	public:

								//! Constructs RenderPassBase instance
								RenderPassBase( Ecs::Entities& entities, const String& name, const Ecs::Aspect& aspect, const Renderer& renderer )
									: EntitySystem( entities, name, aspect ), m_renderer( renderer ) {}

		//! Renders the pass.
		void					render( u32 currentTime, f32 dt, const Matrix4& viewProjection );

	protected:

		//! Processes the entity.
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity );

	protected:

		Renderer				m_renderer;	//!< Rendering HAL.
		Matrix4					m_viewProj;	//!< The view-projection matrix.
	};

	//! Generic render pass to subclass user-defined render passes from
	template<typename TComponent>
	class RenderPass : public RenderPassBase {
	public:

								//! Constructs RenderPass instance
								RenderPass( Ecs::Entities& entities, const String& name, const Renderer& renderer )
									: RenderPassBase( entities, name, Ecs::Aspect::all<TComponent, Transform>(), renderer ) {}

	private:

		//! Extracts the components from entity.
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity );

		//! This method should be overridden in subclass
		virtual void			process( u32 currentTime, f32 dt, SceneObject& sceneObject, TComponent& component, Transform& transform );
	};

	// ** RenderPass::process
	template<typename TComponent>
	void RenderPass<TComponent>::process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity )
	{
		SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
		DC_BREAK_IF( sceneObject == NULL )

		process( currentTime, dt, *sceneObject, *entity->get<TComponent>(), *entity->get<Transform>() );
	}

	// ** RenderPass::pricess
	template<typename TComponent>
	void RenderPass<TComponent>::process( u32 currentTime, f32 dt, SceneObject& sceneObject, TComponent& component, Transform& transform )
	{
		DC_BREAK
	}

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderSystem_H__    */