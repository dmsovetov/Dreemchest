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

// Include a Platform module header.
#include <platform/Platform.h>

#include <renderer/Renderer.h>

// Open a root engine namespace
DC_USE_DREEMCHEST

// Open a platform namespace to shorted the types.
using namespace platform;

// This class is a key for handling events raised by Window.
// Yes, this is a window delegate class, and it works in the
// same way as an application delegate.
class WindowHandler : public WindowDelegate {

    // This method is called when mouse/touch is pressed.
    virtual void handleMouseDown( Window* window, u32 x, u32 y, int touchId ) {
        platform::log::msg( "handleMouseDown : %d %d\n", x, y );
    }

    // This method is called when mouse/touch is released.
    virtual void handleMouseUp( Window* window, u32 x, u32 y, int touchId ) {
        platform::log::msg( "handleMouseUp : %d %d\n", x, y );
    }

    // This method is called when mouse/touch is moved.
    virtual void handleMouseMove( Window* window, u32 sx, u32 sy, u32 ex, u32 ey, int touchId ) {
        platform::log::msg( "handleMouseMove : %d %d\n", ex, ey );
    }

    // This method is called when key is pressed.
    virtual void handleKeyDown( Window* window, Key key, int touchId ) {
        platform::log::msg( "handleKeyDown : %d\n", key );
    }

    // This method is called when key is released.
    virtual void handleKeyUp( Window* window, Key key ) {
        platform::log::msg( "handleKeyUp : %d\n", key );
    }
};

// Application delegate is used to handle an events raised by application instance.
class WindowEvents : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void notifyLaunched( Application* application ) {
        platform::log::setStandardHandler();
        
        // Create a 800x600 window like we did in previous example.
        Window* window = Window::create( 800, 600 );

        // Now set a window delegate.
        window->setDelegate( new WindowHandler );
    }
};

// Now declare an application entry point with Windows application delegate.
dcDeclareApplication( new WindowEvents )