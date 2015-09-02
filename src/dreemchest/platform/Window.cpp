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

namespace Platform {

IMPLEMENT_LOGGER( log )

//! Platform-specific window constructor.
extern IWindow* createWindow( u32 width, u32 height );

// ** Window::Window
Window::Window( IWindow* impl ) : m_impl( impl )
{
    if( m_impl ) m_impl->setOwner( this );
    else         log::warn( "Window::Window : windows are not available on current platform\n" );
}

// ** Window::~Window
Window::~Window( void )
{
    DC_DELETE( m_impl )
}

// ** Window::create
Window* Window::create( u32 width, u32 height )
{
#if defined( DC_PLATFORM )
    if( IWindow* impl = createWindow( width, height ) ) {
        return DC_NEW Window( impl );
    }

    return NULL;
#endif

    return DC_NEW Window( NULL );
}

// ** Window::release
void Window::release( void )
{
	if( m_delegate != NULL ) {
		m_delegate->handleClosed( this );
	}

	if( m_impl ) {
		m_impl->close();
	}

	delete this;
}

// ** Window::width
u32 Window::width( void ) const
{
    if( m_impl ) {
        return m_impl->width();
    }

    log::warn( "Window::width : window is not implemented\n" );
    return 0;
}

// ** Window::height
u32 Window::height( void ) const
{
    if( m_impl ) {
        return m_impl->height();
    }

    log::warn( "Window::height : window is not implemented\n" );
    return 0;
}

// ** Window::aspectRatio
f32 Window::aspectRatio( void ) const
{
	return f32( width() ) / height();
}

// ** Window::mapCursorToWindow
void Window::mapCursorToWindow( s32& x, s32& y ) const
{
	DC_CHECK_IMPL();
	m_impl->mapCursorToWindow( x, y );
}

// ** Window::setDelegate
void Window::setDelegate( WindowDelegate* delegate )
{
    m_delegate = delegate;
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

// ** Window::handle
void* Window::handle( void ) const
{
    if( m_impl == NULL ) {
        log::warn( "Window::handle : window is not implemented\n" );
        return NULL;
    }

    return m_impl->handle();
}

// ** Window::notifyUpdate
void Window::notifyUpdate( void )
{
    if( m_delegate != NULL ) {
        m_delegate->handleUpdate( this );
    }
}

// ** Window::notifyMouseUp
void Window::notifyMouseUp( u32 x, u32 y, int touchId )
{
    if( m_delegate != NULL ) {
        m_delegate->handleMouseUp( this, x, y, touchId );
    }
}

// ** Window::notifyMouseDown
void Window::notifyMouseDown( u32 x, u32 y, int touchId )
{
    if( m_delegate != NULL ) {
        m_delegate->handleMouseDown( this, x, y, touchId );
    }
}

// ** Window::notifyMouseMove
void Window::notifyMouseMove( u32 sx, u32 sy, u32 ex, u32 ey, int touchId )
{
    if( m_delegate != NULL ) {
        m_delegate->handleMouseMove( this, sx, sy, ex, ey, touchId );
    }
}

// ** Window::notifyKeyDown
void Window::notifyKeyDown( Key key )
{
    if( m_delegate != NULL ) {
        m_delegate->handleKeyDown( this, key );
    }
}

// ** Window::notifyKeyUp
void Window::notifyKeyUp( Key key )
{
    if( m_delegate != NULL ) {
        m_delegate->handleKeyUp( this, key );
    }
}

} // namespace Platform

DC_END_DREEMCHEST