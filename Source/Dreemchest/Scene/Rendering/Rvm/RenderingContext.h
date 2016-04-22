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

#ifndef __DC_Scene_RenderingContext_H__
#define __DC_Scene_RenderingContext_H__

#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Rendering context.
    class RenderingContext : public RefCounted {
    public:

        //! Returns a parent rendering HAL instance.
        Renderer::HalWPtr           hal( void ) const;

        //! Creates a RenderingContext instance.
        static RenderingContextPtr  create( Renderer::HalWPtr hal );

    private:

                                    //! Constructs a RenderingContext instance.
                                    RenderingContext( Renderer::HalWPtr hal );

    private:

        Renderer::HalWPtr           m_hal;  //!< A rendering HAL to be used.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContext_H__    */