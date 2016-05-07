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

DC_BEGIN_DREEMCHEST

namespace Scene {

    class ForwardRenderSystem : public RenderSystem<RenderForwardLit> {
    public:

                                        ForwardRenderSystem( RenderingContext& context, RenderScene& renderScene );

    protected:

        //! Alias the shadow constant buffer type.
        typedef RenderScene::CBuffer::Shadow ShadowParameters;

        virtual void			        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const RenderForwardLit& renderForwardLit ) NIMBLE_OVERRIDE;

        //! Generate commands to render a light pass for a single light source.
        void                            renderLight( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, u8 shadows );

        //! Generate commands to render a shadow map.
        u8                              renderShadows( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, s32 dimensions, const ShadowParameters& parameters );

        //! Creates a shadow parameters for a spot light.
        ShadowParameters                spotLightShadows( const RenderScene::LightNode& light, s32 dimensions ) const;

        //! Creates a shadow parameters for a directional light
        ShadowParameters                directionalLightShadows( const Camera& camera, const Matrix4& cameraInverseTransform, const Vec3& cameraPosition, const RenderScene::LightNode& light, s32 dimensions, s32 split, s32 maxSplits ) const;

        //! Calculates a world-space bounding box of a frustum split.
        Bounds                          calculateSplitBounds( const Camera& camera, const Matrix4& cameraInverseTransform, const Matrix4& lightTransform, f32 near, f32 far ) const;

    private:

        UbershaderPtr                   m_phongShader;
        UbershaderPtr                   m_ambientShader;
        UbershaderPtr                   m_shadowShader;
        ShadowParameters                m_shadowParameters;
        RenderResource                  m_shadowCBuffer;
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_ForwardRenderSystem_H__    */