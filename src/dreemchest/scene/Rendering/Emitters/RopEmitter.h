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

#ifndef __DC_Scene_RopEmitter_H__
#define __DC_Scene_RopEmitter_H__

#include "../RenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Render operation emitter takes renderable entities as an input and emits render operations as an output.
	class RopEmitterBase : public RefCounted {
	public:

								//! Constructs RenderOperationEmitter instance.
								RopEmitterBase( Ecs::EcsWPtr ecs, const String& name, const Ecs::Aspect& aspect );

		//! Emits render operations for entities in scene.
		virtual void			emit( RenderingContext& ctx, Rvm& rvm, ShaderCache& shaders, const Camera& camera, const Transform& transform );

		//! Sets the plane cliper.
		void					setClipper( const PlaneClipper& value );

	protected:

		Ecs::IndexPtr			m_entities;		//!< Entities used for render operation emission.
		PlaneClipper			m_clipper;		//!< Plane clipper for ROP rejection.
		const Camera*			m_camera;		//!< Active camera.
		const Transform*		m_transform;	//!< Active camera transform.
	};

	//! Generic render operation emitter.
	template<typename TRenderable>
	class RopEmitter : public RopEmitterBase {
	public:

								//! Constructs RopEmitter instance.
								RopEmitter( Ecs::EcsWPtr ecs, const String& name )
									: RopEmitterBase( ecs, name, Ecs::Aspect::all<TRenderable, Transform>() ) {}

		//! Emits render operations for renderable entities in scene.
		virtual void			emit( RenderingContext& ctx, Rvm& rvm, ShaderCache& shaders, const Camera& camera, const Transform& transform ) DC_DECL_OVERRIDE;

	protected:

		//! Emits render operation for a single renderable entity.
		virtual void			emit( RenderingContext& ctx, Rvm& rvm, ShaderCache& shaders, const TRenderable& renderable, const Transform& transform );
	};

	// ** RopEmitter::emit
	template<typename TRenderable>
	void RopEmitter<TRenderable>::emit( RenderingContext& ctx, Rvm& rvm, ShaderCache& shaders, const Camera& camera, const Transform& transform )
	{
		RopEmitterBase::emit( ctx, rvm, shaders, camera, transform );

		const Ecs::EntitySet& entities = m_entities->entities();

		for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
			emit( ctx, rvm, shaders, *(*i)->get<TRenderable>(), *(*i)->get<Transform>() );
		}
	}

	// ** RopEmitter::emit
	template<typename TRenderable>
	void RopEmitter<TRenderable>::emit( RenderingContext& ctx, Rvm& rvm, ShaderCache& shaders, const TRenderable& renderable, const Transform& transform )
	{
		DC_BREAK
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RopEmitter_H__    */