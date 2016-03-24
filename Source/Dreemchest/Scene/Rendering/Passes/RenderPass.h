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

#ifndef __DC_Scene_Rendering_RenderPass_H__
#define __DC_Scene_Rendering_RenderPass_H__

#include "../RenderAssets.h"
#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Performs a single rendering pass.
	class RenderPassBase {
	public:

                                    //! Constructs RenderPassBase instance.
                                    RenderPassBase( RenderScene& renderScene )
                                        : m_renderScene( renderScene ) {}

		//! Renders a pass to active render target.
		virtual void			    render( Commands& commands, const Vec3& camera, ShaderSourceHandle shader ) = 0;

	protected:

        RenderScene&                m_renderScene;  //!< Parent render scene.
	};

	//! Generic render pass class.
	template<typename TRenderable>
	class RenderPass : public RenderPassBase {
	public:

							        //! Constructs RenderPass instance.
							        RenderPass( RenderingContext& context );

		//! Processes the entity family and emits render operations to be executed by RVM.
		virtual void		        render( Commands& commands, const Vec3& camera, ShaderSourceHandle shader ) DC_DECL_OVERRIDE;

	protected:

		//! Emits render operation for a single renderable entity.
		virtual void		        render( Commands& commands, const Vec3& camera, ShaderSourceHandle shader, const TRenderable& renderable, const Transform& transform ) = 0;

	protected:

		Ecs::IndexPtr		        m_index;        //!< Entity index to be processed.
	};

	// ** RenderPass::RenderPass
	template<typename TRenderable>
	RenderPass<TRenderable>::RenderPass( RenderingContext& context )
	{
		m_index = context.scene()->ecs()->requestIndex( "RenderPass<>", Ecs::Aspect::all<TRenderable, Transform>() );
	}

	// ** RenderPass::render
	template<typename TRenderable>
	void RenderPass<TRenderable>::render( Commands& commands, const Vec3& camera, ShaderSourceHandle shader )
	{
        // Get the entity set from index
		const Ecs::EntitySet& entities = m_index->entities();

        // Process each entity
		for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
			render( commands, camera, shader, *(*i)->get<TRenderable>(), *(*i)->get<Transform>() );
		}
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_RenderPass_H__    */