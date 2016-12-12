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

#ifndef __DC_Scene_Rendering_DebugRenderSystem_H__
#define __DC_Scene_Rendering_DebugRenderSystem_H__

#include "../RenderSystem/RenderSystem.h"
#include "../Passes/DebugRenderPasses.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Renders a debug information for scene objects that reside in scene.
    class DebugRenderSystem : public RenderSystem<DebugRenderer> {
    public:

                                        DebugRenderSystem( RenderingContext& context, RenderScene& renderScene );

    protected:

        //! Emits render operations for all objects that reside in scene.
        virtual void                    emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const DebugRenderer& debugRenderer ) NIMBLE_OVERRIDE;

    private:

        RenderId                        m_debugShader;      //!< A default shader that is used for debug rendering.
        DebugStaticMeshPass             m_staticMeshes;     //!< Renders bounding boxes for all static meshes.
        DebugLightPass                  m_lights;           //!< Renders bounding boxes for all lights.
        DebugCameraPass                 m_cameras;          //!< Renders bounding boxes for all cameras.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_DebugRenderSystem_H__    */
