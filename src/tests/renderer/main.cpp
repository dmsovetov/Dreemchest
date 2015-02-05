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

#include <renderer/Renderer.h>

DC_USE_DREEMCHEST

using namespace renderer;
using namespace platform;

using namespace platform;
using namespace renderer;

class AppWindow : public WindowDelegate {
public:

    AppWindow( Hal* hal ) : m_clearColor( Black ), m_hal( hal ) {}

    virtual void handleMouseMove( Window* window, u32 sx, u32 sy, u32 ex, u32 ey ) {
        platform::log::msg( "handleMouseMove : %d %d\n", ex, ey );
        m_clearColor.r = sx / ( float )window->width();
        m_clearColor.g = sy / ( float )window->height();
    }

    virtual void handleUpdate( Window* window ) {
        m_hal->clear( m_clearColor );

        m_hal->present();
    }

private:

    Rgba    m_clearColor;
    Hal*    m_hal;
};

void handleApplicationLaunched( Application* app )
{
    platform::log::msg( "handleApplicationLaunched\n" );

    Window*     window = Window::create( 800, 600 );
    RenderView* view   = Hal::createOpenGLView( window->handle() );
    Hal*        hal    = Hal::create( OpenGL, view );

    window->setCaption( "Hello" );
    window->setDelegate( new AppWindow( hal ) );
}

int testRenderer( int argc, char **argv )
{
    ::platform::log::setStandardHandler();
    ::renderer::log::setStandardHandler();

    Application* app = Application::create();
    app->launch( dcStaticFunction( handleApplicationLaunched ) );

    return 0;
}