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

#ifndef __DC_Scene_Rendering_ForwardRenderSystem_H__
#define __DC_Scene_Rendering_ForwardRenderSystem_H__

#include "../RenderSystem/RenderSystem.h"
#include "../RenderSystem/StreamedRenderPass.h"
#include "../Passes/DebugRenderPasses.h"
#include "CascadedShadowMaps.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! A debug render pass that renders a camera frustum splits.
    class DebugCsmSplits : public StreamedRenderPass<Camera> {
    public:

                            //! Constructs a DebugCsmSplits instance.
                            DebugCsmSplits( RenderingContext& context, RenderScene& renderScene );

        //! Sets a bounding box color.
        void                setup( const CascadedShadowMaps& csm, const Rgba* colors );

        virtual void		emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack ) { StreamedRenderPass::emitRenderOperations( frame, commands, stateStack ); }

    protected:

        //! Emits render operations to render a single bounding box.
        virtual void        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform ) NIMBLE_OVERRIDE;

    private:

        Ecs::EntityWPtr     m_light;            //!< A directional light entity.
        const Rgba*         m_colors;           //!< Split colors.
        CascadedShadowMaps  m_csm;              //!< A CSM instance being visualized.
    };

    class ForwardRenderSystem : public RenderSystem<ForwardRenderer> {
    friend class DebugCsmSplits;
    public:

                                        ForwardRenderSystem( RenderingContext& context, RenderScene& renderScene );

    protected:

        //! Alias the shadow constant buffer type.
        typedef RenderScene::CBuffer::Shadow ShadowParameters;

        //! Alias the clip planes constant buffer type.
        typedef RenderScene::CBuffer::ClipPlanes ClipPlanesParameters;

        virtual void			        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const ForwardRenderer& forwardRenderer ) NIMBLE_OVERRIDE;

        //! Generate commands to render a light pass for a single light source.
        void                            renderLight( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, u8 shadows );

        //! Generate commands to render a shadow map.
        u8                              renderShadows( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, s32 dimensions, const ShadowParameters& parameters );

        //! Creates a shadow parameters for a spot light.
        ShadowParameters                spotLightShadows( const RenderScene::LightNode& light, s32 dimensions ) const;

        //! Renders a debug info for shadow map cascades.
        void                            debugRenderCsm( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Matrix4& light, const CascadedShadowMaps& csm );

    private:

        UbershaderPtr                   m_phongShader;
        UbershaderPtr                   m_debugShader;
        UbershaderPtr                   m_ambientShader;
        UbershaderPtr                   m_shadowShader;
        ShadowParameters                m_shadowParameters;
        RenderResource                  m_shadowCBuffer;
        ClipPlanesParameters            m_clipPlanesParameters;
        RenderResource                  m_clipPlanesCBuffer;
        DebugCsmSplits                  m_debugCsmSplits;
        DebugRenderTarget               m_debugRenderTarget;
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_ForwardRenderSystem_H__    */