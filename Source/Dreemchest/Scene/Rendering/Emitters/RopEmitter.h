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

#ifndef __DC_Scene_Rendering_RopEmitter_H__
#define __DC_Scene_Rendering_RopEmitter_H__

#include "../RenderingContext.h"
#include "../Rvm/Rvm.h"

#include "../../Components/Rendering.h"
#include "../../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Render operation emitter takes renderable entities as an input and emits render operations as an output.
	class AbstractRopEmitter {
	public:

								//! Constructs AbstractRopEmitter instance.
								AbstractRopEmitter( Ecs::IndexPtr entities )
                                    : m_entities( entities ) {}

		//! Emits render operations for entities in scene.
		virtual void			emit( const Vec3& camera, RenderingContext& ctx, Commands& commands ) = 0;

	protected:

		Ecs::IndexPtr			m_entities;		//!< Entities used for render operation emission.
	};

	//! Generic render operation emitter.
	template<typename TRenderable>
	class RopEmitter : public AbstractRopEmitter {
	public:

								//! Constructs RopEmitter instance.
								RopEmitter( SceneWPtr scene )
									: AbstractRopEmitter( scene->ecs()->requestIndex( "", Ecs::Aspect::all<TRenderable, Transform>() ) ) {}

		//! Emits render operations for renderable entities in scene.
		virtual void			emit( const Vec3& camera, RenderingContext& ctx, Commands& commands ) DC_DECL_OVERRIDE;

	protected:

		//! Emits render operation for a single renderable entity.
		virtual void			emit( const Vec3& camera, RenderingContext& ctx, Commands& commands, const TRenderable& renderable, const Transform& transform ) = 0;
	};

	// ** RopEmitter::emit
	template<typename TRenderable>
	void RopEmitter<TRenderable>::emit( const Vec3& camera, RenderingContext& ctx, Commands& commands )
	{
		const Ecs::EntitySet& entities = m_entities->entities();

        s32 idx = 0;
		for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
			emit( camera, ctx, commands, *(*i)->get<TRenderable>(), *(*i)->get<Transform>() );
		}
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_RopEmitter_H__    */