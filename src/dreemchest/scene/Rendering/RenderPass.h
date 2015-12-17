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

#ifndef __DC_Scene_RenderPass_H__
#define __DC_Scene_RenderPass_H__

#include "../Scene.h"
#include "../Components/Rendering.h"
#include "../Components/Transform.h"

#include "Rvm.h"
#include "ShaderCache.h"
#include "RenderingContext.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Performs a single rendering pass.
	class RenderPassBase : public RefCounted {
	public:

		//! Begins the rendering pass.
		virtual void				begin( RenderingContextPtr context, const Camera& camera, const Transform& transform, const Matrix4& viewProjection );

		//! Renders a pass to active render target.
		virtual void				render( RenderingContextPtr context );

		//! Ends the rendering pass.
		virtual void				end( RenderingContextPtr context );

	protected:

		//! Adds a new render operation emitter to this pass.
		void						addEmitter( RopEmitterBase* emitter );

		//! Sets the clipper.
		void						setClipper( const PlaneClipper& value );

		//! Emits all render operations.
		void						emit( RenderingContext& ctx, Rvm& rvm, ShaderCache& shaders );

		//! Setups the rendering states before rendering.
		virtual void				setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection );

	protected:

		Array<RopEmitterBasePtr>	m_emitters;		//!< Render operation emitters used by pass.
		const Camera*				m_camera;		//!< Active camera.
		const Transform*			m_transform;	//!< Active camera transform.
	};

	//! Generic render pass class.
	template<typename TRenderable>
	class RenderPass : public RenderPassBase {
	public:

							//! Constructs RenderPass instance.
							RenderPass( Ecs::EcsWPtr ecs, const String& name );

		//! Processes the entity family and emits render operations to be executed by RVM.
		virtual void		render( RenderingContextPtr context ) DC_DECL_OVERRIDE;

	protected:

		//! Emits render operation for a single renderable entity.
		virtual void		render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const TRenderable& renderable, const Transform& transform );

	protected:

		Ecs::IndexPtr		m_index;		//!< Entity index to be processed.
	};

	// ** RenderPass::RenderPass
	template<typename TRenderable>
	RenderPass<TRenderable>::RenderPass( Ecs::EcsWPtr ecs, const String& name )
	{
		m_index = ecs->requestIndex( name, Ecs::Aspect::all<TRenderable, Transform>() );
	}

	// ** RenderPass::render
	template<typename TRenderable>
	void RenderPass<TRenderable>::render( RenderingContextPtr context )
	{
		Rvm& rvm = *context->rvm().get();
		ShaderCache& shaders = *context->shaders().get();

		const Ecs::EntitySet& entities = m_index->entities();

		for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
			render( context, rvm, shaders, *(*i)->get<TRenderable>(), *(*i)->get<Transform>() );
		}
	}

	// ** RenderPass::render
	template<typename TRenderable>
	void RenderPass<TRenderable>::render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const TRenderable& renderable, const Transform& transform )
	{
		DC_BREAK
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderPass_H__    */