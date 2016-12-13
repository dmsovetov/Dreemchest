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

#ifndef __DC_Scene_Rendering_DebugRenderPasses_H__
#define __DC_Scene_Rendering_DebugRenderPasses_H__

#include "../RenderSystem/StreamedRenderPass.h"
#include "../Debug/CascadedShadowMaps.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Renders bounding boxes for all static meshes that reside in scene.
    class DebugStaticMeshPass : public StreamedRenderPass<StaticMesh> {
    public:

                            //! Constructs a DebugStaticMeshPass instance.
                            DebugStaticMeshPass( RenderingContext& context, RenderScene& renderScene );

    protected:

        //! Emits render operations to render a single bounding box.
        virtual void        emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const StaticMesh& staticMesh, const Transform& transform ) NIMBLE_OVERRIDE;
    };

    //! Renders fixtures for all lights that reside in scene.
    class DebugLightPass : public StreamedRenderPass<Light> {
    public:

                            //! Constructs a DebugLightPass instance.
                            DebugLightPass( RenderingContext& context, RenderScene& renderScene );

    protected:

        //! Emits render operations to render a single bounding box.
        virtual void        emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Light& light, const Transform& transform ) NIMBLE_OVERRIDE;
    };

    //! Renders frustums for all cameras that reside in scene.
    class DebugCameraPass : public StreamedRenderPass<Camera> {
    public:

                            //! Constructs a DebugCameraPass instance.
                            DebugCameraPass( RenderingContext& context, RenderScene& renderScene );

    protected:

        //! Emits render operations to render a single frustum.
        virtual void        emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform ) NIMBLE_OVERRIDE;
    };

    //! A debug render pass that renders intermediate render targets.
    class DebugRenderTarget : public StreamedRenderPassBase {
    public:

                            //! Constructs a DebugRenderTarget instance.
                            DebugRenderTarget( RenderingContext& context, RenderScene& renderScene );

        //! Emits render operations to render a target as a 2D rectangle.
        void                render( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Viewport& viewport, u8 slot, Renderer::RenderTargetAttachment attachment, s32 size, s32 x, s32 y );

    private:

        ConstantBuffer_     m_cbuffer;      //!< A view constant buffer instance.
        Program             m_shader;       //!< A shader to be used for rendering.
    };

    //! A debug render pass that renders a debug info for cascaded shadow mapping
    class DebugCascadedShadows : public StreamedRenderPass<Camera> {
    public:

                            //! Constructs a DebugCascadedShadows instance.
                            DebugCascadedShadows( RenderingContext& context, RenderScene& renderScene );

        //! Emits render operations to show a cascaded shadows debug info.
        void                render( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const CascadedShadowMaps& csm, const Rgba colors[] = NULL );

    protected:

        //! Emits render operations to render a single bounding box.
        virtual void        emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform ) NIMBLE_OVERRIDE;

    private:

        const Rgba*         m_colors;           //!< Split colors.
        CascadedShadowMaps  m_csm;              //!< A CSM instance being visualized.
        Program             m_shader;           //!< A shader to be used for rendering.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_DebugRenderPasses_H__    */
