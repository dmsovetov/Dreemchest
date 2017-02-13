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

#include "CocoaWindow.h"
#include "MacOSWindow.h"

@implementation CocoaWindow

enum KeyCodes
{
     KEY_A = 0,
     KEY_S = 1,
     KEY_D = 2,
     KEY_F = 3,
     KEY_H = 4,
     KEY_G = 5,
     KEY_Z = 6,
     KEY_X = 7,
     KEY_C = 8,
     KEY_V = 9,

     KEY_B = 11,
     KEY_Q = 12,
     KEY_W = 13,
     KEY_E = 14,
     KEY_R = 15,
     KEY_Y = 16,
     KEY_T = 17,
     KEY_1 = 18,
     KEY_2 = 19,
     KEY_3 = 20,
     KEY_4 = 21,
     KEY_6 = 22,
     KEY_5 = 23,
     KEY_EQUALS = 24,
     KEY_9 = 25,
     KEY_7 = 26,
     KEY_MINUS = 27,
     KEY_8 = 28,
     KEY_0 = 29,
     KEY_RIGHTBRACKET = 30,
     KEY_O = 31,
     KEY_U = 32,
     KEY_LEFTBRACKET = 33,
     KEY_I = 34,
     KEY_P = 35,
     KEY_RETURN = 36,
     KEY_L = 37,
     KEY_J = 38,
     KEY_APOSTROPHE = 39,
     KEY_K = 40,
     KEY_SEMICOLON = 41,
     KEY_FRONTSLASH = 42,
     KEY_COMMA = 43,
     KEY_BACKSLASH = 44,
     KEY_N = 45,
     KEY_M = 46,
     KEY_PERIOD = 47,
     KEY_TAB = 48,

     KEY_BACKAPOSTROPHE = 50,
     KEY_DELETE = 51,

     KEY_ESCAPE = 53,

     KEY_COMMAND = 55,
     KEY_SHIFT = 56,
     KEY_CAPSLOCK = 57,
     KEY_OPTION = 58,
     KEY_CONTROL = 59,

     KEY_UP = 126,
     KEY_DOWN = 125,
     KEY_LEFT = 123,
     KEY_RIGHT = 124,
};

Platform::Key convertKeyCode(u16 keyCode)
{
    switch (keyCode)
    {
        case KEY_A: return Platform::Key::A;
        case KEY_S: return Platform::Key::S;
        case KEY_D: return Platform::Key::D;
        case KEY_F: return Platform::Key::F;
        case KEY_H: return Platform::Key::H;
        case KEY_G: return Platform::Key::G;
        case KEY_Z: return Platform::Key::Z;
        case KEY_X: return Platform::Key::X;
        case KEY_C: return Platform::Key::C;
        case KEY_V: return Platform::Key::V;
            
        case KEY_B: return Platform::Key::B;
        case KEY_Q: return Platform::Key::Q;
        case KEY_W: return Platform::Key::W;
        case KEY_E: return Platform::Key::E;
        case KEY_R: return Platform::Key::R;
        case KEY_Y: return Platform::Key::Y;
        case KEY_T: return Platform::Key::T;
        default: NIMBLE_NOT_IMPLEMENTED;
    }
}
    
- ( void )setWindow : ( Platform::MacOSWindow* )window
{
    m_window = window;
}

// ** transformPoint
- ( NSPoint )transformPoint : ( NSEvent* )event
{
    NSPoint point = [event locationInWindow];
    NSView* view  = [self contentView];
    point.y       = view.bounds.size.height - point.y;

    return point;
}

// ** update
- ( void )update
{
    if( m_window ) {
        m_window->owner()->notifyUpdate();
    }

    [super update];
}

// ** mouseDown
- ( void )mouseDown : ( NSEvent* )event
{
    NSPoint point = [self transformPoint: event];

    if( point.y >= 0 ) {
        m_window->owner()->notifyMouseDown( Platform::Window::LeftMouseButton, point.x, point.y );
    }

    [super mouseDown:event];
}

// ** mouseUp
- ( void )mouseUp : ( NSEvent* )event
{
    NSPoint point = [self transformPoint: event];

    if( point.y >= 0 ) {
        m_window->owner()->notifyMouseUp( Platform::Window::LeftMouseButton, point.x, point.y );
    }

    [super mouseUp:event];
}

// ** mouseMoved
- ( void )mouseMoved : ( NSEvent* )event
{
    NSRect  rect  = self.frame;
    NSPoint point = [self transformPoint: event];

    if( point.y >= 0 && NSPointInRect( [NSEvent mouseLocation], rect ) ) {
        m_window->owner()->notifyMouseMove( Platform::Window::LeftMouseButton, m_prevMousePos.x, m_prevMousePos.y, point.x, point.y );
        m_prevMousePos = point;
    }

    [super mouseMoved:event];
}

// ** mouseDragged
- ( void )mouseDragged : ( NSEvent* )event
{
    NSPoint point = [self transformPoint: event];

    if( point.y >= 0 ) {
        m_window->owner()->notifyMouseMove( Platform::Window::LeftMouseButton, m_prevMousePos.x, m_prevMousePos.y, point.x, point.y );
        m_prevMousePos = point;
    }

    [super mouseDragged:event];
}

// ** rightMouseDown
- ( void )rightMouseDown : ( NSEvent* )event
{
    NSPoint point = [self transformPoint: event];
    
    if (point.y >= 0)
    {
        m_window->owner()->notifyMouseDown(Platform::Window::RightMouseButton, point.x, point.y);
    }
    
    [super rightMouseDown:event];
}

// ** rightMouseUp
- ( void )rightMouseUp : ( NSEvent* )event
{
    NSPoint point = [self transformPoint: event];
    
    if (point.y >= 0)
    {
        m_window->owner()->notifyMouseUp(Platform::Window::RightMouseButton, point.x, point.y);
    }
    
    [super rightMouseUp:event];
}

// ** rightMouseDragged
- ( void )rightMouseDragged : ( NSEvent* )event
{
    NSPoint point = [self transformPoint: event];
    
    if (point.y >= 0)
    {
        m_window->owner()->notifyMouseMove(Platform::Window::RightMouseButton, m_prevMousePos.x, m_prevMousePos.y, point.x, point.y);
        m_prevMousePos = point;
    }
    
    [super rightMouseDragged:event];
}

// ** keyDown
- ( void )keyDown : ( NSEvent* )event
{
    m_window->owner()->notifyKeyDown(convertKeyCode([event keyCode]));
    [super keyDown:event];
}

// ** keyUp
- ( void )keyUp : ( NSEvent* )event
{
    m_window->owner()->notifyKeyUp(convertKeyCode([event keyCode]));
    [super keyUp:event];
}

@end
