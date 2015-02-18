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

// Open a root engine namespace
DC_USE_DREEMCHEST

// Application delegate is used to handle an events raised by application instance.
class CreatingWindows : public platform::ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( platform::Application* application ) {
		platform::log::setStandardHandler();

        // This line of code creates a window with 800x600 dimensions.
        //
        // Note: on mobile devices you won't see anything.
        platform::Window* window = platform::Window::create( 800, 600 );

        // Now we can work with a Window instance. In this example we will
        // just set a caption for window.
        window->setCaption( "This is how windows are created in Dreemchest." );

        // You can also create another one window :)
        platform::Window::create( 320, 240 )->setCaption( "Little window" );
    }
};

// Now declare an application entry point with CreatingWindows application delegate.
dcDeclareApplication( new CreatingWindows )