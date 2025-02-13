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

#include "MacOSInput.h"

DC_BEGIN_DREEMCHEST

namespace Platform {
    
// ** createInput
IInput* createInput( void )
{
    return DC_NEW MacOSInput;
}

// ** MacOSInput::keyDown
bool MacOSInput::keyDown( const Key& key )
{
    NIMBLE_NOT_IMPLEMENTED;
    return false;
}

// ** MacOSInput::mouseY
void MacOSInput::setMouse( s32 x, s32 y )
{
    NIMBLE_NOT_IMPLEMENTED
}

// ** MacOSInput::mouseX
s32 MacOSInput::mouseX( void ) const
{
    NIMBLE_NOT_IMPLEMENTED
    return 0;
}

// ** MacOSInput::mouseY
s32 MacOSInput::mouseY( void ) const
{
    NIMBLE_NOT_IMPLEMENTED
    return 0;
}
    
} // namespace Platform

DC_END_DREEMCHEST
