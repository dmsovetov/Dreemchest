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

#include "WindowsInput.h"
#include <windows.h>

DC_BEGIN_DREEMCHEST

namespace Platform {

// ** createInput
IInput* createInput( void )
{
    return DC_NEW WindowsInput;
}

// ** WindowsInput::keyDown
bool WindowsInput::keyDown( const Key& key )
{
    return GetKeyState( s_virtualKeysWinAPI[key] ) & 0x80 ? true : false;
}

// ** WindowsInput::mouseY
void WindowsInput::setMouse( s32 x, s32 y )
{
    SetCursorPos( x, y );
}

// ** WindowsInput::mouseX
s32 WindowsInput::mouseX( void ) const
{
    POINT point;
    GetCursorPos( &point );
    return point.x;
}

// ** WindowsInput::mouseY
s32 WindowsInput::mouseY( void ) const
{
    POINT point;
    GetCursorPos( &point );
    return point.y;
}

// ** WindowsInput::s_virtualKeysWinAPI
s32 WindowsInput::s_virtualKeysWinAPI[] = {
    // ** Numbers
    0x30,    // 0
    0x31,    // 1
    0x32,    // 2
    0x33,    // 3
    0x34,    // 4
    0x35,    // 5
    0x36,    // 6
    0x37,    // 7
    0x38,    // 8
    0x39,    // 9

    VK_NUMPAD0,
    VK_NUMPAD1,
    VK_NUMPAD2,
    VK_NUMPAD3,
    VK_NUMPAD4,
    VK_NUMPAD5,
    VK_NUMPAD6,
    VK_NUMPAD7,
    VK_NUMPAD8,
    VK_NUMPAD9,

    // ** Characters
    0x41,    // A
    0x42,    // B
    0x43,    // C
    0x44,    // D
    0x45,    // E
    0x46,    // F
    0x47,    // G
    0x48,    // H
    0x49,    // I
    0x4A,    // J
    0x4B,    // K
    0x4C,    // L
    0x4D,    // M
    0x4E,    // N
    0x4F,    // O
    0x50,    // P
    0x51,    // Q
    0x52,    // R
    0x53,    // S
    0x54,    // T
    0x55,    // U
    0x56,    // V
    0x57,    // W
    0x58,    // X
    0x59,    // Y
    0x5A,    // Z

    // ** Functional
    VK_F1,
    VK_F2,
    VK_F3,
    VK_F4,
    VK_F5,
    VK_F6,
    VK_F7,
    VK_F8,
    VK_F9,
    VK_F10,
    VK_F11,
    VK_F12,

    VK_ESCAPE,
    VK_TAB,
    VK_CAPITAL,
    VK_SHIFT,
    VK_CONTROL,
    VK_LWIN,
    VK_MENU,
    VK_SPACE,

    VK_RETURN,
    VK_BACK,

    VK_PAUSE,
    VK_SNAPSHOT,
    VK_SCROLL,

    VK_NEXT,
    VK_PRIOR,
    VK_HOME,
    VK_END,
    VK_INSERT,
    VK_DELETE,

    VK_UP,
    VK_DOWN,
    VK_LEFT,
    VK_RIGHT,

    // ** Numpad
    VK_NUMLOCK,
    VK_DIVIDE,
    VK_MULTIPLY,
    VK_SUBTRACT,
    VK_ADD,
    VK_DECIMAL,

    // ** Mouse
    VK_RBUTTON,
    VK_LBUTTON,
    VK_MBUTTON
};

} // namespace Platform

DC_END_DREEMCHEST