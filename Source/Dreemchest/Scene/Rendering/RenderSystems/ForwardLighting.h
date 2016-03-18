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

#ifndef __DC_Scene_Rendering_ForwardLighting_H__
#define __DC_Scene_Rendering_ForwardLighting_H__

#include "RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! This render system outputs a scene with multipass forward lighting.
    class ForwardLighting : public RenderSystem<RenderForwardLit> {
    public:

                                //! Constructs the ForwardLighting instance.
                                ForwardLighting( RenderingContext& context );

    protected:

        //! Emits render operations to output the forward-lit scene to a render target.
        virtual void            emitRenderOperations( const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const RenderForwardLit& forwardLit ) DC_DECL_OVERRIDE;

    private:

        ShaderSourceHandle      m_ambientShader;    //!< Ambient pass shader.
        ShaderSourceHandle      m_additiveShader;   //!< Additive pass shader.
        ShaderSourceHandle      m_phongShader;      //!< Phong lighting shader.
        RenderPassUPtr          m_solidTransparent; //!< Solid and transparent pass instance.
        RenderPassUPtr          m_additive;         //!< Additive pass instance.

        RopEmitterUPtr          m_opaque;           //!< Emits render operations for opaque & cutout objects.
        RopEmitterUPtr          m_translucent;      //!< Emits render operations for translucent objects.
        Ecs::IndexPtr           m_lights;           //!< Light entity index.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_ForwardLighting_H__    */