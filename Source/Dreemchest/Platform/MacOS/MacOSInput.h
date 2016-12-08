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

#ifndef __DC_Platform_MacOSInput_H__
#define __DC_Platform_MacOSInput_H__

#include "../Input.h"


DC_BEGIN_DREEMCHEST

namespace Platform {
    
    // ** class MacOSInput
    class MacOSInput : public IInput {
    public:
        
        // ** IInput
        virtual bool        keyDown( const Key& key ) NIMBLE_OVERRIDE;
        virtual void        setMouse( s32 x, s32 y ) NIMBLE_OVERRIDE;
        virtual s32            mouseX( void ) const NIMBLE_OVERRIDE;
        virtual s32            mouseY( void ) const NIMBLE_OVERRIDE;
    };
    
} // namespace Platform

DC_END_DREEMCHEST

#endif  /*  !__DC_Platform_MacOSInput_H__   */
