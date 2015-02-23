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

#include "MacOSWindow.h"
#include "CocoaWindow.h"

DC_BEGIN_DREEMCHEST

namespace platform {

// ** createWindow
IWindow* createWindow( u32 width, u32 height )
{
    MacOSWindow* window = DC_NEW MacOSWindow;

    if( window->create( width, height ) ) {
        return window;
    }

    delete window;

    return NULL;
}

// ** MacOSWindow::MacOSWindow
MacOSWindow::MacOSWindow( void ) : m_owner( NULL ), m_width( 0 ), m_height( 0 )
{

}

// ** MacOSWindow::~MacOSWindow
MacOSWindow::~MacOSWindow( void )
{
    if( m_window ) {
        [m_window close];
        [m_window dealloc];
    }
}

// ** MacOSWindow::close
void MacOSWindow::close( void )
{
    [m_window close];
}

// ** MacOSWindow::setOwner
void MacOSWindow::setOwner( Window* value )
{
    m_owner = value;
}

// ** MacOSWindow::owner
Window* MacOSWindow::owner( void ) const
{
    return m_owner;
}

// ** MacOSWindow::width
u32 MacOSWindow::width( void ) const
{
    return m_width;
}

// ** MacOSWindow::height
u32 MacOSWindow::height( void ) const
{
    return m_height;
}

// ** MacOSWindow::setCaption
void MacOSWindow::setCaption( const String& value )
{
    [m_window setTitle: [NSString stringWithCString: value.c_str() encoding: [NSString defaultCStringEncoding]]];
}

// ** MacOSWindow::caption
String MacOSWindow::caption( void ) const
{
    return m_window.title.UTF8String;
}

// ** MacOSWindow::handle
void* MacOSWindow::handle( void ) const
{
    return reinterpret_cast<void*>( m_window );
}

// ** MacOSWindow::create
bool MacOSWindow::create( u32 width, u32 height )
{
    // ** Set the window dimensions
    m_width  = width;
    m_height = height;

    // ** Create window
    NSRect		screen		= [[NSScreen mainScreen] frame];
    NSRect		frame		= NSMakeRect( screen.size.width * 0.5 - m_width * 0.5, screen.size.height * 0.5 - m_width * 0.5, m_width, m_height );
    NSUInteger	styleMask	= NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;

    m_window = [[CocoaWindow alloc] initWithContentRect:frame styleMask:styleMask backing:NSBackingStoreBuffered defer:false];
    [m_window makeKeyAndOrderFront: m_window];
    [m_window setAcceptsMouseMovedEvents: YES];
    [m_window setWindow: this];

    log::verbose( "MacOS window created %dx%d...\n", m_width, m_height );

    return true;
}

} // namespace platform

DC_END_DREEMCHEST