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

#include <platform/Platform.h>

DC_USE_DREEMCHEST

using namespace platform;

class AppWindow : public WindowDelegate {
public:

    virtual void    handleMouseDown( Window* window, u32 x, u32 y ) { platform::log::msg( "handleMouseDown : %d %d\n", x, y ); }
    virtual void    handleMouseUp( Window* window, u32 x, u32 y ) { platform::log::msg( "handleMouseUp : %d %d\n", x, y ); }
    virtual void    handleKeyDown( Window* window, Key key ) { platform::log::msg( "handleKeyDown : %d\n", key ); }
    virtual void    handleKeyUp( Window* window, Key key ) { platform::log::msg( "handleKeyUp : %d\n", key ); }
    virtual void    handleMouseMove( Window* window, u32 sx, u32 sy, u32 ex, u32 ey ) { platform::log::msg( "handleMouseMove : %d %d\n", ex, ey ); }
};

void handleApplicationLaunched( Application* app )
{
    platform::log::msg( "handleApplicationLaunched\n" );
    
    Window* window = Window::create( 800, 600 );

    window->setCaption( "Hello" );
    window->setDelegate( new AppWindow );
}

int testPlatform( int argc, char **argv )
{
    ::platform::log::setStandardHandler();

    Application* app = Application::create();
    app->launch( dcStaticFunction( handleApplicationLaunched ) );

    return 0;
}