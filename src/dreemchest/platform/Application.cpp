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

#include "Application.h"

DC_BEGIN_DREEMCHEST

namespace Platform {

//! Platform-specific application constructor.
extern IApplication* createApplication( void );

// ** Application::s_application
Application* Application::s_application = NULL;

// ** Application::Application
Application::Application( IApplication* impl ) : m_impl( impl ), m_delegate( NULL )
{
    DC_BREAK_IF( s_application != NULL );
    if( !m_impl ) log::warn( "Application::Application : application interface is not implemented on current platform\n" );
    s_application = this;
}

Application::~Application( void )
{
    DC_DELETE( m_impl )
}

// ** Application::sharedInstance
Application* Application::sharedInstance( void )
{
    return s_application;
}

// ** Application::create
Application* Application::create( void )
{
#if defined( DC_PLATFORM )
    if( IApplication* impl = createApplication() ) {
        return DC_NEW Application( impl );
    }

    return NULL;
#endif

    return DC_NEW Application( NULL );
}

// ** Application::quit
void Application::quit( u32 exitCode )
{
    if( !m_impl ) {
        log::warn( "Application::quit : application is not implemented\n" );
        return;
    }

    m_impl->quit( exitCode );
}

// ** Application::launch
int Application::launch( ApplicationDelegate* delegate )
{
    if( !m_impl ) {
        log::warn( "Application::launch : application is not implemented\n" );
        return -1;
    }

    m_delegate = delegate;

    return m_impl->launch( this );
}

// ** Application::notifyLaunched
void Application::notifyLaunched( void )
{
    if( !m_delegate ) {
        log::verbose( "Application::notifyLaunched : no application delegate set, event ignored\n" );
        return;
    }

    m_delegate->handleLaunched( this );
}

} // namespace Platform

DC_END_DREEMCHEST