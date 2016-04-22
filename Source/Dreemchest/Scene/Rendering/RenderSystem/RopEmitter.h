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

#ifndef __DC_Scene_Rendering_RopEmitter_H__
#define __DC_Scene_Rendering_RopEmitter_H__

#include "../RenderScene.h"
#include "../Rvm/RenderingContext.h"
#include "../../Assets/Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Render operation emitter takes renderable entities as an input and emits render operations as an output.
	class RopEmitter {
	public:

        //! Filters objects before emitting a render operations.
        struct Filter {
            u8                  renderModes;    //!< Eligible rendering modes.
            u8                  lightingModels; //!< Eligible lighting models.

                                //! Constructs a Filter instance.
                                Filter( void )
                                    : renderModes( ~0 )
                                    , lightingModels( ~0 )
                                    {
                                    }
        };

								//! Constructs RopEmitter instance.
								RopEmitter( RenderScene& renderScene )
                                    : m_renderScene( renderScene ) {}
        virtual                 ~RopEmitter( void ) {}

		//! Emits render operations for entities in scene.
		virtual void			emit( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Filter& filter = Filter() ) = 0;

	protected:

        RenderScene&            m_renderScene;  //!< Parent render scene.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_RopEmitter_H__    */