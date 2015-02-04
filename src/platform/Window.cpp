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

#include "Window.h"

DC_BEGIN_DREEMCHEST

namespace platform {

IMPLEMENT_LOGGER( log )

//! Platform-specific window constructor.
extern IWindow* createWindow( u32 width, u32 height );

// ** Window::Window
Window::Window( IWindow* impl ) : m_impl( impl )
{
    if( !impl ) log::warn( "Window::Window : windows are not available on current platform\n" );
}

// ** Window::~Window
Window::~Window( void )
{
    DC_DELETE( m_impl )
}

// ** Window::create
Window* Window::create( u32 width, u32 height )
{
#ifdef DC_PLATFORM_OSX
    if( IWindow* impl = createWindow( width, height ) ) {
        return DC_NEW Window( impl );
    }

    return NULL;
#endif

    return DC_NEW Window( NULL );
}

// ** Window::caption
String Window::caption( void ) const
{
    if( m_impl == NULL ) {
        log::warn( "Window::caption : window is not implemented\n" );
        return "";
    }

    return m_impl->caption();
}

// ** Window::setCaption
void Window::setCaption( const String& value )
{
    if( m_impl == NULL ) {
        log::warn( "Window::setCaption : window is not implemented\n" );
        return;
    }

    m_impl->setCaption( value );
}

} // namespace platform

DC_END_DREEMCHEST