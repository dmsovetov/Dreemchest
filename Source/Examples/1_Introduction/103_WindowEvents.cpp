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

// Declare the log tag in global namespace
DREEMCHEST_LOGGER_TAG( Windows )

// Application delegate is used to handle an events raised by application instance.
class WindowEvents : public Platform::ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Platform::Application* application ) {
        // Set the default log handler.
        Logger::setStandardLogger();
        
        // Create a 800x600 window like we did in previous example.
        Platform::Window* window = Platform::Window::create( 800, 600 );

        // Now subscribe for window events.
        window->subscribe<Platform::Window::TouchBegan>( dcThisMethod( WindowEvents::handleTouchBegan ) );
        window->subscribe<Platform::Window::TouchEnded>( dcThisMethod( WindowEvents::handleTouchEnded ) );
        window->subscribe<Platform::Window::TouchMoved>( dcThisMethod( WindowEvents::handleTouchMoved ) );
    #ifdef DC_PLATFORM_KEYBOARD
        window->subscribe<Platform::Window::KeyPressed>( dcThisMethod( WindowEvents::handleKeyPressed ) );
        window->subscribe<Platform::Window::KeyReleased>( dcThisMethod( WindowEvents::handleKeyReleased ) );
    #endif  //  #ifdef DC_PLATFORM_KEYBOARD
        window->subscribe<Platform::Window::Update>( dcThisMethod( WindowEvents::handleUpdate ) );
    }

    // This method is called when mouse/touch is pressed.
    void handleTouchBegan( const Platform::Window::TouchBegan& e ) {
        LogVerbose( "touchBegan", "%d %d\n", e.x, e.y );
    }

    // This method is called when mouse/touch is released.
    void handleTouchEnded( const Platform::Window::TouchEnded& e ) {
        LogVerbose( "touchEnded", "%d %d\n", e.x, e.y );
    }

    // This method is called when mouse/touch is moved.
    void handleTouchMoved( const Platform::Window::TouchMoved& e ) {
        LogVerbose( "touchMoved", "%d %d\n", e.x, e.y );
    }

#ifdef DC_PLATFORM_KEYBOARD
    // This method is called when key is pressed.
    void handleKeyPressed( const Platform::Window::KeyPressed& e ) {
        LogVerbose( "keyPressed", "%d\n", e.key );
    }

    // This method is called when key is released.
    void handleKeyReleased( const Platform::Window::KeyReleased& e ) {
        LogVerbose( "keyReleased", "%d\n", e.key );
    }
#endif  //  #ifdef DC_PLATFORM_KEYBOARD

    // This method is called each frame
   void handleUpdate( const Platform::Window::Update& e ) {
        
    }
};

// Now declare an application entry point with WindowEvents application delegate.
dcDeclareApplication( new WindowEvents )
