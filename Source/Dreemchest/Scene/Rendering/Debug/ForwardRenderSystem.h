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
#include "../Passes/GenericRenderPasses.h"
#include "CascadedShadowMaps.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    class ForwardRenderSystem : public RenderSystem<ForwardRenderer> {
    friend class DebugCsmSplits;
    public:

                                        ForwardRenderSystem( RenderingContext& context, RenderScene& renderScene );

    protected:

        //! Alias the shadow constant buffer type.
        typedef RenderScene::CBuffer::Shadow ShadowParameters;

        virtual void                    emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const ForwardRenderer& forwardRenderer ) NIMBLE_OVERRIDE;

        //! Generate commands to render a light pass for a single light source.
        void                            renderLight( RenderFrame& frame, RenderCommandBuffer& commands, StateStack& stateStack, const RenderScene::LightNode& light, const RenderScene::CBuffer::ClipPlanes* clip, TransientTexture shadows = TransientTexture() );

        //! Emits operations to render a spot light pass.
        void                            renderSpotLight( RenderFrame& frame, RenderCommandBuffer& commands, StateStack& stateStack, const ForwardRenderer& forwardRenderer, const RenderScene::LightNode& light );

        //! Emits operations to render a point light pass.
        void                            renderPointLight( RenderFrame& frame, RenderCommandBuffer& commands, StateStack& stateStack, const ForwardRenderer& forwardRenderer, const RenderScene::LightNode& light );

        //! Emits operations to render a directional light pass.
        void                            renderDirectionalLight( RenderFrame& frame, RenderCommandBuffer& commands, StateStack& stateStack, const ForwardRenderer& forwardRenderer, const Camera& camera, const Transform& cameraTransform, const Viewport& viewport, const RenderScene::LightNode& light );

    private:

        Program                         m_phongShader;
        ConstantBuffer_                 m_clipPlanesCBuffer;
        AmbientPass                     m_ambient;
        ShadowPass                      m_shadows;
        DebugCascadedShadows            m_debugCascadedShadows;
        DebugRenderTarget               m_debugRenderTarget;
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_ForwardRenderSystem_H__    */
