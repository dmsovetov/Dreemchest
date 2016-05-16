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

#ifndef __DC_Scene_Rendering_GenericRenderPasses_H__
#define __DC_Scene_Rendering_GenericRenderPasses_H__

#include "../RenderSystem/RenderPass.h"
#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Renders a scene with an ambient light only.
    class AmbientPass : public RenderPassBase {
    public:

                                //! Constructs a AmbientPass instance.
                                AmbientPass( RenderingContext& context, RenderScene& renderScene );

        //! Emits operations to render an ambient lit scene.
        void                    render( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack );

    private:

        UbershaderPtr           m_shader;   //!< An ambient light shader instance.
    };

    //! Renders a scene depth to a render target.
    class ShadowPass : public RenderPassBase {
    public:

                                //! Constructs a ShadowPass instance.
                                ShadowPass( RenderingContext& context, RenderScene& renderScene );

        //! Emits render operations to output a depth to a texture.
        u8                      render( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::CBuffer::Shadow& parameters );

        //! Returns a constant buffer that is used for shadow parameters.
        RenderResource          cbuffer( void ) const;

    private:

        UbershaderPtr           m_shader;   //!< A shadowmap shader instance.
        RenderResource          m_cbuffer;  //!< A shadow parameters constant buffer.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_GenericRenderPasses_H__    */