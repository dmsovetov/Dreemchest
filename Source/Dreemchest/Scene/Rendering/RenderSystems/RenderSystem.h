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

#ifndef __DC_Scene_Rendering_RenderSystem_H__
#define __DC_Scene_Rendering_RenderSystem_H__

#include "../Rvm/Commands.h"
#include "../Emitters/StaticMeshEmitter.h"
#include "../../Components/Rendering.h"
#include "../../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Base class for all render systems.
	class RenderSystemBase {
	public:

								//! Constructs the RenderSystemBase instance.
								RenderSystemBase( RenderingContext& context, Ecs::IndexPtr cameras );

		//! Renders all active cameras to their viewports.
		void					render( void );

	protected:

		//! Emits rendering operations to a command buffer for a specified camera.
		virtual void			emitRenderOperations( const Ecs::Entity& entity, const Camera& camera, const Transform& transform ) = 0;

	protected:

        RenderingContext&       m_context;
		Ecs::IndexPtr			m_cameras;	//!< All active cameras that are processed by this render system.
	};

	//! Generic render system class.
	template<typename TRenderer>
	class RenderSystem : public RenderSystemBase {
	public:

								//! Constructs RenderSystem instance
								RenderSystem( RenderingContext& context );

    protected:

        //! Extracts the render component from a camera entity and passes it an abstract method that should be overridden in a subclass.
		virtual void			emitRenderOperations( const Ecs::Entity& entity, const Camera& camera, const Transform& transform ) DC_DECL_OVERRIDE;

        //! Emits rendering operations to a command buffer for a specified camera.
        virtual void            emitRenderOperations( const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const TRenderer& renderer ) = 0;
	};

    // ** RenderSystem::RenderSystem
    template<typename TRenderer>
    RenderSystem<TRenderer>::RenderSystem( RenderingContext& context )
        : RenderSystemBase( context, context.scene()->ecs()->requestIndex( "RenderSystemCameras", Ecs::Aspect::all<Camera, Transform, TRenderer>() ) )
    {
    }

	// ** RenderSystem::emitRenderOperations
	template<typename TRenderer>
	void RenderSystem<TRenderer>::emitRenderOperations( const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
	{
        emitRenderOperations( entity, camera, transform, *entity.get<TRenderer>() );
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_RenderSystem_H__    */