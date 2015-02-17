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

// Include a Renderer module header.
//
// This module contains all hardware rendering stuff like
// textures, vertex buffers, shaders, etc.
#include <renderer/Renderer.h>

// Open a root engine namespace
DC_USE_DREEMCHEST

// Open a platform namespace to use shorter types.
using namespace platform;

// Open a renderer namespace.
using namespace renderer;

// This class will handle notifyUpdate and just clear the viewport
class WindowHandler : public WindowDelegate {
public:
    
    // Constructs a WindowHandler instance, we pass a HAL pointer
    // to be able to clear the viewport.
    WindowHandler( Hal* hal ) : m_hal( hal ) {}

    // Called each frame and renders a single frame
    virtual void handleUpdate( Window* window ) {
        // First clear a viewport with a color
        m_hal->clear( Rgba( 0.2f, 0.2f, 0.2f ) );

        // And now just present all rendered data to the screen
        m_hal->present();
    }

    // The previously created HAL instance.
    Hal* m_hal;
};

// Application delegate is used to handle an events raised by application instance.
class RendererInitialization : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void notifyLaunched( Application* application ) {
        platform::log::setStandardHandler();
        renderer::log::setStandardHandler();

        // Create a 800x600 window like we did in previous example.
        // This window will contain a rendering viewport.
        Window* window = Window::create( 800, 600 );

        // Create a rendering view.
        RenderView* view   = Hal::createOpenGLView( window->handle() );

        // Now create the main renderer interface called HAL (hardware abstraction layer).
        Hal* hal = Hal::create( OpenGL, view );

        // Finally set the window delegate to handle updates.
        window->setDelegate( new WindowHandler( hal ) );
    }
};

// Now declare an application entry point with Windows application delegate.
dcDeclareApplication( new RendererInitialization )