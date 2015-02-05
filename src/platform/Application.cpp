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

namespace platform {

//! Platform-specific application constructor.
extern IApplication* createApplication( void );

// ** Application::Application
Application::Application( IApplication* impl ) : m_impl( impl )
{
    if( !m_impl ) log::warn( "Application::Application : application interface is not implemented on current platform\n" );
}

Application::~Application( void )
{
    DC_DELETE( m_impl )
}

// ** Application::create
Application* Application::create( void )
{
#ifdef DC_PLATFORM_MACOS
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
void Application::launch( ApplicationLaunchedCallback callback )
{
    if( !m_impl ) {
        log::warn( "Application::launch : application is not implemented\n" );
        return;
    }

    m_impl->launch( this, callback );
}

} // namespace platform

DC_END_DREEMCHEST