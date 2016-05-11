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


DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Renders bounding boxes for all static meshes that reside in scene.
    class DebugStaticMeshPass : public StreamedRenderPass<StaticMesh> {
    public:

                            //! Constructs a DebugStaticMeshPass instance.
                            DebugStaticMeshPass( RenderingContext& context, RenderScene& renderScene );

        //! Returns a bounding box color.
        const Rgba&         color( void ) const;

        //! Sets a bounding box color.
        void                setColor( const Rgba& value );


    protected:

        //! Emits render operations to render a single bounding box.
        virtual void        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const StaticMesh& staticMesh, const Transform& transform ) NIMBLE_OVERRIDE;

    private:

        Rgba                m_color;    //!< A bounding box color.
    };

    //! Renders fixtures for all lights that reside in scene.
    class DebugLightPass : public StreamedRenderPass<Light> {
    public:

                            //! Constructs a DebugLightPass instance.
                            DebugLightPass( RenderingContext& context, RenderScene& renderScene );

        //! Returns a bounding box color.
        const Rgba&         color( void ) const;

        //! Sets a bounding box color.
        void                setColor( const Rgba& value );


    protected:

        //! Emits render operations to render a single bounding box.
        virtual void        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Light& light, const Transform& transform ) NIMBLE_OVERRIDE;

    private:

        Rgba                m_color;    //!< A light fixture color.
    };

    //! Renders frustums for all cameras that reside in scene.
    class DebugCameraPass : public StreamedRenderPass<Camera> {
    public:

                            //! Constructs a DebugCameraPass instance.
                            DebugCameraPass( RenderingContext& context, RenderScene& renderScene );

        //! Returns a frustum color.
        const Rgba&         color( void ) const;

        //! Sets a frustum color.
        void                setColor( const Rgba& value );


    protected:

        //! Emits render operations to render a single frustum.
        virtual void        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform ) NIMBLE_OVERRIDE;

    private:

        Rgba                m_color;    //!< A camera frustum color.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_DebugRenderPasses_H__    */