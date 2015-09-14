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

#ifndef __DC_Scene_Systems_WireframeRenderPass_H__
#define __DC_Scene_Systems_WireframeRenderPass_H__

#include "../StaticMeshRenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Renders the wireframes of static meshes
	class WireframeRenderPass : public StaticMeshRenderPass {
	public:

										//! Constructs the WireframeRenderPass instance.
										WireframeRenderPass( Ecs::Entities& entities, const Rendering& rendering );

	private:

		//! Extracts the renderer component from camera before rendering the pass.
		virtual void					render( const Ecs::Entity& camera, u32 currentTime, f32 dt, const Matrix4& viewProjection );

		//! Called every frame before any entites are rendered.
		virtual bool					begin( u32 currentTime );

		//! Called every frame after all entities has been rendered.
		virtual void					end( void );

	private:

		const RenderWireframeMeshes*	m_settings;		//!< Wireframe rendering settings.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Systems_WireframeRenderPass_H__    */