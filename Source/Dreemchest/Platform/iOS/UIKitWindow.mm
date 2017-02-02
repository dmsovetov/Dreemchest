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

#include "UIKitWindow.h"
#include "iOSWindow.h"

@implementation UIKitWindow

// ** initWithWindow
- ( id )initWithWindow: ( Platform::iOSWindow* )window
{
    m_window = window;

    UIScreen*       mainScreen  = [UIScreen mainScreen];
    UIApplication*  app         = [UIApplication sharedApplication];

    self.contentScaleFactor = mainScreen.scale;

    return [super initWithFrame: [mainScreen bounds]];
}

// ** width
- ( float ) width
{
    return [UIScreen mainScreen].bounds.size.width * self.contentScaleFactor;
}

// ** height
- ( float ) height
{
    return [UIScreen mainScreen].bounds.size.height * self.contentScaleFactor;
}

// ** update
- ( void )update
{
    m_window->owner()->notifyUpdate();
}

// ** transformTouch
- ( CGPoint ) transformTouch : ( CGPoint )point
{
    CGPoint p = { point.x, point.y };

    p.x = p.x * self.contentScaleFactor;
    p.y = p.y * self.contentScaleFactor;

    return p;
}

// ** touchesBegan
- ( void ) touchesBegan: ( NSSet* ) touches withEvent: ( UIEvent* ) event
{
    for( UITouch* touch in touches )
    {
        CGPoint location    = [touch locationInView: self];
        CGPoint p           = [self transformTouch: location];

        m_window->owner()->notifyMouseDown( Platform::Window::LeftMouseButton, p.x, p.y, [self addTouch:touch] );
    }
}

// ** touchesEnded
- ( void ) touchesEnded: ( NSSet* ) touches withEvent: ( UIEvent* ) event
{
    for( UITouch *touch in touches )
    {
        CGPoint location    = [touch locationInView: self];
        CGPoint p           = [self transformTouch: location];

        m_window->owner()->notifyMouseUp( Platform::Window::LeftMouseButton, p.x, p.y, [self removeTouch:touch] );
    }
}

// ** touchesMoved
- ( void ) touchesMoved: ( NSSet* ) touches withEvent: ( UIEvent* ) event
{
    for( UITouch *touch in touches )
    {
        CGPoint previous = [touch previousLocationInView: self];
        CGPoint current  = [touch locationInView: self];

        CGPoint start   = [self transformTouch: previous];
        CGPoint end     = [self transformTouch: current];

        m_window->owner()->notifyMouseMove( Platform::Window::LeftMouseButton, start.x, start.y, end.x, end.y, [self getTouchId:touch] );
    }
}

// ** addTouch
- ( int )addTouch : ( UITouch* )touch
{
    for( int i = 0; i < MaxTouches; i++ )
    {
        if( !m_touches[i] )
        {
            m_touches[i] = touch;
            return i;
        }

        NIMBLE_BREAK_IF( m_touches[i] == touch );
    }

    return -1;
}

// ** removeTouch
- ( int )removeTouch : ( UITouch* )touch
{
    for( int i = 0; i < MaxTouches; i++ ) {
        if( m_touches[i] == touch ) {
            m_touches[i] = NULL;
            return i;
        }
    }

    NIMBLE_BREAK_IF( true );
    return -1;
}

// ** getTouchId
- ( int )getTouchId  : ( UITouch* )touch
{
    for( int i = 0; i < MaxTouches; i++ ) {
        if( m_touches[i] == touch ) {
            return i;
        }
    }

    return -1;
}

@end
