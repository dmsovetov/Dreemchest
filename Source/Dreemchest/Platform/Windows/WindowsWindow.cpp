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

namespace Platform {

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

WindowsWindow::Windows WindowsWindow::s_windows;

u32 WindowsWindow::s_windowCount = 0;

// ** WindowsWindow::WindowsWindow
WindowsWindow::WindowsWindow( void ) : m_owner( NULL ), m_window( NULL )
{

}

// ** WindowsWindow::~WindowsWindow
WindowsWindow::~WindowsWindow( void )
{

}

// ** WindowsWindow::close
void WindowsWindow::close( void )
{
    NIMBLE_BREAK_IF( m_window == NULL, "invalid handle" );

    // ** Destroy window
    if( !DestroyWindow( m_window ) ) {
        return;
    }

    // ** Free the window class
    if( !UnregisterClass( m_className.c_str(), m_applicationInstance ) ) {
        return;
    }

    // ** Remove window from registry
    s32 removed = s_windows.erase( m_window );
    NIMBLE_BREAK_IF( removed != 1, "failed to remove window from a shared list" );

    m_window                = NULL;
    m_applicationInstance    = NULL;
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
    GetClientRect( m_window, &rect );
    return rect.right - rect.left;
}

// ** WindowsWindow::height
u32 WindowsWindow::height( void ) const
{
    RECT rect;
    GetClientRect( m_window, &rect );
    return rect.bottom - rect.top;
}

// ** WindowsWindow::mapCursorToWindow
void WindowsWindow::mapCursorToWindow( s32& x, s32& y ) const
{
    POINT point = { x, y };
    ScreenToClient( m_window, &point );
    x = point.x;
    y = point.y;
}

// ** WindowsWindow::setCaption
void WindowsWindow::setCaption( const String& value )
{
    SetWindowText( m_window, value.c_str() );
}

// ** WindowsWindow::caption
String WindowsWindow::caption( void ) const
{
    s8 text[128];
    GetWindowText( m_window, text, 128 );
    return text; 
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

    m_className = "dreemchest" + toString( s_windowCount++ );

    m_windowClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    m_windowClass.hInstance        = m_applicationInstance;
    m_windowClass.lpfnWndProc    = ( WNDPROC )windowProc;
    m_windowClass.lpszClassName    = m_className.c_str();
    m_windowClass.style            = CS_HREDRAW | CS_VREDRAW;
    m_windowClass.hbrBackground    = ( HBRUSH )COLOR_GRAYTEXT;

    if( !RegisterClass( &m_windowClass ) ) {
        LogError( "window", "failed to register window class" );
        return false;
    }

    // ** Initialize the window style
    u32 style = WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_MINIMIZEBOX;

    // ** Adjust the window rect to make sure the client are size is right.
    adjustWindowSize( style, width, height );

    // ** Caclulate the window position
    u32 x = ( GetSystemMetrics( SM_CXSCREEN ) / 2 ) - ( width  / 2 );
    u32 y = ( GetSystemMetrics( SM_CYSCREEN ) / 2 ) - ( height / 2 );

    m_window = CreateWindowEx( 0, 
        m_windowClass.lpszClassName, "",
        style,
        x, y,
        width, height,
        NULL, NULL, m_applicationInstance, NULL );

    if( !m_window ) {
        LogError( "window", "failed to create window" );
        return false;
    }

    s_windows[m_window] = this;

    ShowWindow( m_window, SW_SHOW );
    SetFocus( m_window );

    return true;
}

// ** WindowsWindow::adjustWindowSize
void WindowsWindow::adjustWindowSize( u32 style, u32& width, u32& height )
{
    RECT rect = { 0, 0, width, height };
    AdjustWindowRect( &rect, style, FALSE );

    width  = rect.right  - rect.left;
    height = rect.bottom - rect.top;
}

// ** WindowsWindow::windowProc
LRESULT WindowsWindow::windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    WindowsWindow* window = s_windows[hWnd];

    switch( message ) {
    case WM_CLOSE:            window->owner()->release();
                            if( s_windows.size() == 0 ) {
                                PostQuitMessage( 0 );
                            }
                            break;

    case WM_KEYDOWN:        window->owner()->notifyKeyDown( translateKey( ( u32 )wParam ) );
                            break;

    case WM_KEYUP:            window->owner()->notifyKeyUp( translateKey( ( u32 )wParam ) );
                            break;

    case WM_LBUTTONDOWN:    window->owner()->notifyMouseDown( Window::LeftMouseButton, LOWORD( lParam ), HIWORD( lParam ) );
                            break;

    case WM_LBUTTONUP:        window->owner()->notifyMouseUp(Window::LeftMouseButton,  LOWORD( lParam ), HIWORD( lParam ) );
                            break;

    case WM_MOUSEMOVE:        window->owner()->notifyMouseMove( Window::LeftMouseButton, LOWORD( lParam ), HIWORD( lParam ), LOWORD( lParam ), HIWORD( lParam ) );
                            break;

    case WM_RBUTTONDOWN:    window->owner()->notifyMouseDown( Window::RightMouseButton, LOWORD( lParam ), HIWORD( lParam ) );
                            break;

    case WM_RBUTTONUP:        window->owner()->notifyMouseDown( Window::RightMouseButton, LOWORD( lParam ), HIWORD( lParam ) );
                            break;

    case WM_CHAR:            break;

    }

    return DefWindowProc( hWnd, message, wParam, lParam );
}

// ** WindowsWindow::translateKey
Key WindowsWindow::translateKey( u32 vkey )
{
    switch( vkey ) {
        // ** Numbers
    case 0x30: return Key::_0; break;    // 0
    case 0x31: return Key::_1; break;    // 1
    case 0x32: return Key::_2; break;    // 2
    case 0x33: return Key::_3; break;    // 3
    case 0x34: return Key::_4; break;    // 4
    case 0x35: return Key::_5; break;    // 5
    case 0x36: return Key::_6; break;    // 6
    case 0x37: return Key::_7; break;    // 7
    case 0x38: return Key::_8; break;    // 8
    case 0x39: return Key::_9; break;    // 9

    case VK_NUMPAD0: return Key::Num0; break;
    case VK_NUMPAD1: return Key::Num1; break;
    case VK_NUMPAD2: return Key::Num2; break;
    case VK_NUMPAD3: return Key::Num3; break;
    case VK_NUMPAD4: return Key::Num4; break;
    case VK_NUMPAD5: return Key::Num5; break;
    case VK_NUMPAD6: return Key::Num6; break;
    case VK_NUMPAD7: return Key::Num7; break;
    case VK_NUMPAD8: return Key::Num8; break;
    case VK_NUMPAD9: return Key::Num9; break;

        // ** Characters
    case 0x41: return Key::A; break;    // A
    case 0x42: return Key::B; break;    // B
    case 0x43: return Key::C; break;    // C
    case 0x44: return Key::D; break;    // D
    case 0x45: return Key::E; break;    // E
    case 0x46: return Key::F; break;    // F
    case 0x47: return Key::G; break;    // G
    case 0x48: return Key::H; break;    // H
    case 0x49: return Key::I; break;    // I
    case 0x4A: return Key::J; break;    // J
    case 0x4B: return Key::K; break;    // K
    case 0x4C: return Key::L; break;    // L
    case 0x4D: return Key::M; break;    // M
    case 0x4E: return Key::N; break;    // N
    case 0x4F: return Key::O; break;    // O
    case 0x50: return Key::P; break;    // P
    case 0x51: return Key::Q; break;    // Q
    case 0x52: return Key::R; break;    // R
    case 0x53: return Key::S; break;    // S
    case 0x54: return Key::T; break;    // T
    case 0x55: return Key::U; break;    // U
    case 0x56: return Key::V; break;    // V
    case 0x57: return Key::W; break;    // W
    case 0x58: return Key::X; break;    // X
    case 0x59: return Key::Y; break;    // Y
    case 0x5A: return Key::Z; break;    // Z

        // ** Functional
    case VK_F1: return Key::F1; break;
    case VK_F2: return Key::F2; break;
    case VK_F3: return Key::F3; break;
    case VK_F4: return Key::F4; break;
    case VK_F5: return Key::F5; break;
    case VK_F6: return Key::F6; break;
    case VK_F7: return Key::F7; break;
    case VK_F8: return Key::F8; break;
    case VK_F9: return Key::F9; break;
    case VK_F10: return Key::F10; break;
    case VK_F11: return Key::F11; break;
    case VK_F12: return Key::F12; break;

    case VK_ESCAPE: return Key::Escape; break;
    case VK_TAB: return Key::Tab; break;
    case VK_CAPITAL: return Key::Capital; break;
    case VK_SHIFT: return Key::Shift; break;
    case VK_CONTROL: return Key::Control; break;
    case VK_LWIN: return Key::Win; break;
    case VK_MENU: return Key::Alt; break;
    case VK_SPACE: return Key::Space; break;

    case VK_RETURN: return Key::Return; break;
    case VK_BACK: return Key::Back; break;

    case VK_PAUSE: return Key::Pause; break;
    case VK_SNAPSHOT: return Key::Snapshot; break;
    case VK_SCROLL: return Key::Scroll; break;

    case VK_NEXT: return Key::PgDown; break;
    case VK_PRIOR: return Key::PgUp; break;
    case VK_HOME: return Key::Home; break;
    case VK_END: return Key::End; break;
    case VK_INSERT: return Key::Insert; break;
    case VK_DELETE: return Key::Delete; break;

    case VK_UP: return Key::Up; break;
    case VK_DOWN: return Key::Down; break;
    case VK_LEFT: return Key::Left; break;
    case VK_RIGHT: return Key::Right; break;

        // ** Numpad
    case VK_NUMLOCK: return Key::Numlock; break;
    case VK_DIVIDE: return Key::Divide; break;
    case VK_MULTIPLY: return Key::Multiply; break;
    case VK_SUBTRACT: return Key::Subtract; break;
    case VK_ADD: return Key::Add; break;
    case VK_DECIMAL: return Key::Decimal; break;

        // ** Mouse
    case VK_RBUTTON: return Key::RButton; break;
    case VK_LBUTTON: return Key::LButton; break;
    case VK_MBUTTON: return Key::MButton; break;
    };

    return Key::Total;
}

} // namespace Platform

DC_END_DREEMCHEST