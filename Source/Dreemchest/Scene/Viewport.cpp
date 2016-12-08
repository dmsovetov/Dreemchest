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

#include "Viewport.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ---------------------------------------- AbstractViewport ---------------------------------------- //

// ** AbstractViewport::rect
Rect AbstractViewport::rect( void ) const
{
    return Rect( 0.0f, 0.0f, width(), height() );
}

// ------------------------------------------ WindowViewport ------------------------------------------ //

// ** WindowViewport::WindowViewport
WindowViewport::WindowViewport( const Platform::WindowWPtr& window )
    : m_window( window )
    , m_lastX( -1 )
    , m_lastY( -1 )
{
    NIMBLE_ABORT_IF( !window.valid(), "invalid window" );
    m_window->subscribe<Platform::Window::TouchBegan>( dcThisMethod( WindowViewport::handleTouchBegan ) );
    m_window->subscribe<Platform::Window::TouchMoved>( dcThisMethod( WindowViewport::handleTouchMoved ) );
    m_window->subscribe<Platform::Window::TouchEnded>( dcThisMethod( WindowViewport::handleTouchEnded ) );
}

// ** WindowViewport::~WindowViewport
WindowViewport::~WindowViewport( void )
{
    if( !m_window.valid() ) {
        return;
    }

    m_window->unsubscribe<Platform::Window::TouchBegan>( dcThisMethod( WindowViewport::handleTouchBegan ) );
    m_window->unsubscribe<Platform::Window::TouchMoved>( dcThisMethod( WindowViewport::handleTouchMoved ) );
    m_window->unsubscribe<Platform::Window::TouchEnded>( dcThisMethod( WindowViewport::handleTouchEnded ) );
}

// ** WindowViewport::create
WindowViewportPtr WindowViewport::create( const Platform::WindowWPtr& window )
{
    return WindowViewportPtr( DC_NEW WindowViewport( window ) );
}

// ** WindowViewport::width
s32 WindowViewport::width( void ) const
{
    return m_window->width();
}

// ** WindowViewport::height
s32 WindowViewport::height( void ) const
{
    return m_window->height();
}

// ** WindowViewport::handleTouchBegan
void WindowViewport::handleTouchBegan( const Platform::Window::TouchBegan& e )
{
    m_lastX = e.x;
    m_lastY = e.y;
    notify<TouchBegan>( this, e.id, e.x, e.y );
}

// ** WindowViewport::handleTouchMoved
void WindowViewport::handleTouchMoved( const Platform::Window::TouchMoved& e )
{
    notify<TouchMoved>( this, e.id, e.x, e.y, e.x - m_lastX, e.y - m_lastY );
    m_lastX = e.x;
    m_lastY = e.y;
}

// ** WindowViewport::handleTouchEnded
void WindowViewport::handleTouchEnded( const Platform::Window::TouchEnded& e )
{
    notify<TouchEnded>( this, e.id, e.x, e.y );
}

} // namespace Scene

DC_END_DREEMCHEST
