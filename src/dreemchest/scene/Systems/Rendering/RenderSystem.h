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

#include "../../Scene.h"

#include "../../Components/Rendering.h"	//!< Include the rendering components here
#include "../../Components/Transform.h"	//!< Everything that is rendered should have a transform component, so include this file here

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! A helper struct to wrap HAL & 2D renderer instances.
	struct Rendering {
		ShaderCachePtr			m_shaders;		//!< Shader cache.
		Renderer::HalPtr		m_hal;			//!< Rendering HAL.
		Renderer::Renderer2DPtr	m_renderer2d;	//!< 2D renderer.

								//! Constructs empty Rendering instance.
								Rendering( void )
									{}

								//! Constructs Rendering instance.
								Rendering( const Renderer::HalPtr& hal )
									: m_hal( hal ) {}

								//! Constructs Rendering instance.
								Rendering( const Renderer::Renderer2DPtr& renderer2d )
									: m_renderer2d( renderer2d ) {}
	};
	 
	//! Base class for all render systems.
	class RenderSystemBase : public Ecs::EntitySystem {
	public:

								//! Constructs RenderSystemBase instance
								RenderSystemBase( Ecs::Entities& entities, const String& name, const Ecs::Aspect& aspect, const Rendering& rendering )
									: EntitySystem( entities, name, aspect ), m_rendering( rendering ) {}

		//! Adds a new render pass to this system.
		template<typename T>
		void					addPass( void );

	protected:

		//! Renders all nested render passes using the camera.
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity );

	protected:

		//! Container type to store nested render passes.
		typedef List<RenderPassBasePtr> RenderPasses;

		Rendering				m_rendering;	//!< Renderers wraper.
		RenderPasses			m_passes;		//!< Render passes.
	};

	// ** RenderSystemBase::addPass
	template<typename T>
	void RenderSystemBase::addPass( void )
	{
		m_passes.push_back( DC_NEW T( m_entities, m_rendering ) );
	}

	//! Generic render system to subclass user-defined rendering systems from
	template<typename TRender>
	class RenderSystem : public RenderSystemBase {
	public:

								//! Constructs RenderSystem instance
								RenderSystem( Ecs::Entities& entities, const String& name, const Rendering& rendering )
									: RenderSystemBase( entities, name, Ecs::Aspect::all<Camera, Transform, TRender>(), rendering ), m_render( NULL ) {}

	protected:

		const TRender*			m_render;	//!< Handle to render component.
	};

	//! Generic single pass renderer
	template<typename TRenderer, typename TPass>
	class SinglePassRenderer : public RenderSystem<TRenderer> {
	public:

								SinglePassRenderer( Ecs::Entities& entities, const Rendering& rendering )
									: RenderSystem( entities, "SinglePassRenderer", rendering ) { addPass<TPass>(); }
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderSystem_H__    */