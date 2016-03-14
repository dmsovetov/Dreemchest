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

#ifndef __DC_Scene_Rendering_DepthComplexity_H__
#define __DC_Scene_Rendering_DepthComplexity_H__

#include "RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! This render system outputs the depth complexity to a render target.
    class DepthComplexity : public RenderSystem<RenderDepthComplexity> {
    public:

                                //! Constructs the DepthComplexity instance.
                                DepthComplexity( RenderingContext& context );

    protected:

        //! Emits render operations to output the depth complexity of a scene to a render target.
        virtual void            emitRenderOperations( const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const RenderDepthComplexity& depthComplexity ) DC_DECL_OVERRIDE;

    private:

        RopEmitterUPtr          m_emitters[TotalRenderModes];   //!< Rop emitter for each of rendering modes.
        TechniqueHandle         m_technique;                    //!< This technique is used for all draw call operations.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_DepthComplexity_H__    */