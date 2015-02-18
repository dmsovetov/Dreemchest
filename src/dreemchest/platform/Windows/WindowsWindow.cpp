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

#include "WindowsWindow.h"

DC_BEGIN_DREEMCHEST

namespace platform {

// ** createWindow
IWindow* createWindow( u32 width, u32 height )
{
    WindowsWindow* window = DC_NEW WindowsWindow;

    if( window->create( width, height ) ) {
        return window;
    }

    delete window;
    return NULL;
}

// ** WindowsWindow::WindowsWindow
WindowsWindow::WindowsWindow( void ) : m_owner( NULL ), m_window( NULL )
{

}

// ** WindowsWindow::~WindowsWindow
WindowsWindow::~WindowsWindow( void )
{

}

// ** WindowsWindow::setOwner
void WindowsWindow::setOwner( Window* value )
{
    m_owner = value;
}

// ** WindowsWindow::owner
Window* WindowsWindow::owner( void ) const
{
    return m_owner;
}

// ** WindowsWindow::width
u32 WindowsWindow::width( void ) const
{
    RECT rect;
	GetWindowRect( m_window, &rect );
	return rect.right - rect.left;
}

// ** WindowsWindow::height
u32 WindowsWindow::height( void ) const
{
	RECT rect;
	GetWindowRect( m_window, &rect );
	return rect.bottom - rect.top;
}

// ** WindowsWindow::setCaption
void WindowsWindow::setCaption( const String& value )
{
    log::warn( "WindowsWindow::setCaption : window caption is not supported on Windows\n" );
}

// ** WindowsWindow::caption
String WindowsWindow::caption( void ) const
{
    return "";
}

// ** WindowsWindow::handle
void* WindowsWindow::handle( void ) const
{
    return reinterpret_cast<void*>( m_window );
}

// ** WindowsWindow::create
bool WindowsWindow::create( u32 width, u32 height )
{
	m_applicationInstance = GetModuleHandle( 0 );

	memset( &m_windowClass, 0, sizeof( m_windowClass ) );

	m_windowClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	m_windowClass.hInstance		= m_applicationInstance;
	m_windowClass.lpfnWndProc	= NULL;
	m_windowClass.lpszClassName	= L"dreemchest";
	m_windowClass.style			= CS_HREDRAW | CS_VREDRAW;
	m_windowClass.hbrBackground	= ( HBRUSH )COLOR_GRAYTEXT;

	if( !RegisterClass( &m_windowClass ) ) {
		log::error( "WindowsWindow::create : failed to register window class" );
		return false;
	}

	u32 style	= WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
	u32 x		= ( GetSystemMetrics( SM_CXSCREEN ) / 2 ) - ( width  / 2 );
	u32 y		= ( GetSystemMetrics( SM_CYSCREEN ) / 2 ) - ( height / 2 );

	m_window = CreateWindowEx( 0, 
		m_windowClass.lpszClassName, L"",
		style,
		x, y,
		width, height,
		NULL, NULL, m_applicationInstance, NULL );

	if( !m_window ) {
		log::error( "WindowsWindow::create : failed to create window" );
		return false;
	}

	ShowWindow( m_window, SW_SHOW );
	SetFocus( m_window );

    return true;
}

} // namespace platform

DC_END_DREEMCHEST