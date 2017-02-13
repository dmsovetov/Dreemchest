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

#include "iOSWindow.h"

DC_BEGIN_DREEMCHEST

namespace Platform
{

// ** createWindow
IWindow* createWindow( u32 width, u32 height )
{
    iOSWindow* window = DC_NEW iOSWindow;

    if( window->initialize() )
    {
        return window;
    }

    delete window;
    return NULL;
}

// ** iOSWindow::iOSWindow
iOSWindow::iOSWindow( void ) : m_owner( NULL ), m_window( NULL )
{

}

// ** iOSWindow::~iOSWindow
iOSWindow::~iOSWindow( void )
{
    LogWarning( "window", "%s", "iOS window destroyed\n" );
}

// ** iOSWindow::close
void iOSWindow::close( void )
{

}

// ** iOSWindow::setOwner
void iOSWindow::setOwner( Window* value )
{
    m_owner = value;
}

// ** iOSWindow::owner
Window* iOSWindow::owner( void ) const
{
    return m_owner;
}

// ** iOSWindow::width
u32 iOSWindow::width( void ) const
{
    return [m_window width];
}

// ** iOSWindow::height
u32 iOSWindow::height( void ) const
{
    return [m_window height];
}

// ** iOSWindow::setCaption
void iOSWindow::setCaption( const String& value )
{
    LogWarning( "window", "%s", "window caption is not supported on iOS\n" );
}

// ** iOSWindow::caption
String iOSWindow::caption( void ) const
{
    return "";
}
    
// ** iOSWindow::mapCursorToWindow
void iOSWindow::mapCursorToWindow( s32& x, s32& y ) const
{
    NIMBLE_NOT_IMPLEMENTED
}

// ** iOSWindow::handle
void* iOSWindow::handle( void ) const
{
    return reinterpret_cast<void*>( m_window );
}

// ** iOSWindow::initialize
bool iOSWindow::initialize( void )
{
    m_window = [[UIKitWindow alloc] initWithWindow: this];
    [m_window makeKeyAndVisible];
 
    return true;
}

} // namespace Platform

DC_END_DREEMCHEST
