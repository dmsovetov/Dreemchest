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
        m_window->owner()->notifyMouseDown( point.x, point.y );
    }

    [super mouseDown:event];
}

// ** mouseUp
- ( void )mouseUp : ( NSEvent* )event
{
    NSPoint point = [self transformPoint: event];

    if( point.y >= 0 ) {
        m_window->owner()->notifyMouseUp( point.x, point.y );
    }

    [super mouseUp:event];
}

// ** mouseMoved
- ( void )mouseMoved : ( NSEvent* )event
{
    NSRect  rect  = self.frame;
    NSPoint point = [self transformPoint: event];

    if( point.y >= 0 && NSPointInRect( [NSEvent mouseLocation], rect ) ) {
        m_window->owner()->notifyMouseMove( m_prevMousePos.x, m_prevMousePos.y, point.x, point.y );
        m_prevMousePos = point;
    }

    [super mouseMoved:event];
}

// ** mouseDragged
- ( void )mouseDragged : ( NSEvent* )event
{
    NSPoint point = [self transformPoint: event];

    if( point.y >= 0 ) {
        m_window->owner()->notifyMouseMove( m_prevMousePos.x, m_prevMousePos.y, point.x, point.y );
        m_prevMousePos = point;
    }

    [super mouseDragged:event];
}

// ** keyDown
- ( void )keyDown : ( NSEvent* )event
{
    NIMBLE_NOT_IMPLEMENTED
    [super keyDown:event];
}

// ** keyUp
- ( void )keyUp : ( NSEvent* )event
{
    NIMBLE_NOT_IMPLEMENTED
    [super keyUp:event];
}

@end
