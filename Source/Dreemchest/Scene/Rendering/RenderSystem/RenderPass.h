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

#include "../RenderScene.h"
#include "../Rvm/RenderingContext.h"

#include "../../Components/Rendering.h"
#include "../../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Performs a single rendering pass.
	class RenderPassBase {
	public:

                                                //! Constructs RenderPassBase instance.
                                                RenderPassBase( RenderingContext& context, RenderScene& renderScene );

        //! Performs a render pass with a diffuse material.
        void                                    renderWithColor( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Rgba& color );

		//! Renders a pass to active render target.
        virtual void			                emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack ) {}

        //! Begins a pass rendering.
        virtual void                            begin( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack ) {}

        //! Ends a pass rendering.
        virtual void                            end( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack ) {}

        //! Emits rendering operations for static meshes that reside in scene.
        static void                             emitStaticMeshes( const RenderScene::StaticMeshes& staticMeshes, RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, u8 mask = ~0 );

        //! Emits rendering operations for point clouds that reside in scene.
        static void                             emitPointClouds( const RenderScene::PointClouds& pointClouds, RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, u8 mask = ~0 );

        //! Constructs a material constant buffer with a specified color.
        static RenderScene::CBuffer::Material   diffuseMaterial( const Rgba& color );

        //! Constructs a view constant buffer with an ortho projection.
        static RenderScene::CBuffer::View       orthoView( const Viewport& viewport );

	protected:

        RenderingContext&                       m_context;          //!< A parent rendering context.
        RenderScene&                            m_renderScene;      //!< Parent render scene.
        RenderResource                          m_materialCBuffer;  //!< A material constant buffer.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_RenderPass_H__    */