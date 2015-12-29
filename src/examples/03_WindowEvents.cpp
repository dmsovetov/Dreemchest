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

// Include the engine header file.
#include <Dreemchest.h>

// Open a root engine namespace
DC_USE_DREEMCHEST

// Open a platform namespace to use shorter types.
using namespace Platform;

// Application delegate is used to handle an events raised by application instance.
class WindowEvents : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application ) {
        
        Platform::log::setStandardHandler();
        
        // Create a 800x600 window like we did in previous example.
        Window* window = Window::create( 800, 600 );

        // Now subscribe for window events.
        window->subscribe<Window::TouchBegan>( dcThisMethod( WindowEvents::handleTouchBegan ) );
        window->subscribe<Window::TouchEnded>( dcThisMethod( WindowEvents::handleTouchEnded ) );
        window->subscribe<Window::TouchMoved>( dcThisMethod( WindowEvents::handleTouchMoved ) );
        window->subscribe<Window::KeyPressed>( dcThisMethod( WindowEvents::handleKeyPressed ) );
        window->subscribe<Window::KeyReleased>( dcThisMethod( WindowEvents::handleKeyReleased ) );
        window->subscribe<Window::Update>( dcThisMethod( WindowEvents::handleUpdate ) );
    }

    // This method is called when mouse/touch is pressed.
    void handleTouchBegan( const Window::TouchBegan& e ) {
        Platform::log::msg( "handleMouseDown : %d %d\n", e.x, e.y );
    }

    // This method is called when mouse/touch is released.
    void handleTouchEnded( const Window::TouchEnded& e ) {
        Platform::log::msg( "handleMouseUp : %d %d\n", e.x, e.y );
    }

    // This method is called when mouse/touch is moved.
    void handleTouchMoved( const Window::TouchMoved& e ) {
        Platform::log::msg( "handleMouseMove : %d %d\n", e.x, e.y );
    }

    // This method is called when key is pressed.
    void handleKeyPressed( const Window::KeyPressed& e ) {
        Platform::log::msg( "handleKeyDown : %d\n", e.key );
    }

    // This method is called when key is released.
    void handleKeyReleased( const Window::KeyReleased& e ) {
        Platform::log::msg( "handleKeyUp : %d\n", e.key );
    }

    // This method is called each frame
   void handleUpdate( const Window::Update& e ) {
        
    }
};

// Now declare an application entry point with WindowEvents application delegate.
dcDeclareApplication( new WindowEvents )