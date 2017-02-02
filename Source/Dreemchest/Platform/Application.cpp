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

#ifdef DC_PLATFORM_WINDOWS
    #include <direct.h>
    #define getcwd _getcwd
#else
    #include <unistd.h>
#endif

DC_BEGIN_DREEMCHEST

namespace Platform {

//! Platform-specific application constructor.
extern IApplication* createApplication( void );

//! Platform-specific service application constructor.
extern IApplication* createServiceApplication( void );

// ------------------------------------------------------------------- Application ------------------------------------------------------------------- //

// ** Application::s_application
Application* Application::s_application = NULL;

// ** Application::Application
Application::Application( const Arguments& arguments, IApplication* impl ) : m_impl( impl ), m_delegate( NULL ), m_arguments( arguments )
{
    NIMBLE_ABORT_IF( s_application != NULL, "only a single Application instance is allowed" );
    if( !m_impl ) LogWarning( "application", "%s", "not implemented on current platform\n" );
    s_application = this;
}

Application::~Application( void )
{
    delete m_impl;
}

// ** Application::args
const Arguments& Application::args( void ) const
{
    return m_arguments;
}

// ** Application::sharedInstance
Application* Application::sharedInstance( void )
{
    return s_application;
}
    
// ** Application::currentDirectory
const String& Application::currentDirectory( void ) const
{
    if (m_currentDirectory.empty())
    {
        s8 buffer[256];
        getcwd(buffer, sizeof(buffer));
        m_currentDirectory = buffer;
    }
    
    return m_currentDirectory;
}
    
// ** Application::resourcePath
const String& Application::resourcePath( void ) const
{
    if (!m_impl)
    {
        return currentDirectory();
    }
    
    return m_impl->resourcePath();
}
    
// ** Application::pathForResource
String Application::pathForResource(const String& fileName) const
{
    String resources = resourcePath();
    
    if (resources.empty())
    {
        return fileName;
    }
    
    NIMBLE_ABORT_IF(resources[resources.length() - 1] != '/', "a non-empty resource path should contain a trailing slash");
    return resources + fileName;
}

// ** Application::create
Application* Application::create( const Arguments& args )
{
    if( IApplication* impl = createApplication() ) {
        return DC_NEW Application( args, impl );
    }

    return NULL;
}

// ** Application::quit
void Application::quit( u32 exitCode )
{
    if( !m_impl ) {
        return;
    }

    m_impl->quit( exitCode );
}

// ** Application::launch
s32 Application::launch( ApplicationDelegate* delegate )
{
    if( !m_impl ) {
        return -1;
    }

    m_delegate = delegate;
    notifyPrepareToLaunch();
    return m_impl->launch( this );
}

// ** Application::notifyPrepareToLaunch
void Application::notifyPrepareToLaunch( void )
{
    if( !m_delegate ) {
        LogDebug( "application", "%s", "no application delegate set, prepareToLaunch event ignored\n" );
        return;
    }

    m_delegate->handlePrepareToLaunch( this );
}

// ** Application::notifyLaunched
void Application::notifyLaunched( void )
{
    if( !m_delegate ) {
        LogDebug( "application", "%s", "no application delegate set, launch event ignored\n" );
        return;
    }

    m_delegate->handleLaunched( this );
    LogDebug("application", "working directory %s\n", currentDirectory().c_str());
    LogDebug("application", "resource path %s\n", resourcePath().c_str());
}

// ** Application::notifyUpdate
void Application::notifyUpdate( void )
{
    if( m_delegate ) {
        m_delegate->handleUpdate( this );
    }
}
    
// -------------------------------------------------------------- ApplicationDelegate ---------------------------------------------------------------- //

// ** ApplicationDelegate::ApplicationDelegate
ApplicationDelegate::ApplicationDelegate(const String& tag)
    : m_loggerTag(tag)
{
    
}
    
// ** ApplicationDelegate::debug
void ApplicationDelegate::debug(CString prefix, CString format, ...)
{
    NIMBLE_LOGGER_FORMAT(format);
    Logger::write(Logger::Context(NULL, NULL), Logger::Debug, m_loggerTag.c_str(), prefix, buffer);
}

// ** ApplicationDelegate::verbose
void ApplicationDelegate::verbose(CString prefix, CString format, ...)
{
    NIMBLE_LOGGER_FORMAT(format);
    Logger::write(Logger::Context(NULL, NULL), Logger::Verbose, m_loggerTag.c_str(), prefix, buffer);
}

// ** ApplicationDelegate::warning
void ApplicationDelegate::warning(CString prefix, CString format, ...)
{
    NIMBLE_LOGGER_FORMAT(format);
    Logger::write(Logger::Context(NULL, NULL), Logger::Warning, m_loggerTag.c_str(), prefix, buffer);
}

// ** ApplicationDelegate::error
void ApplicationDelegate::error(CString prefix, CString format, ...)
{
    NIMBLE_LOGGER_FORMAT(format);
    Logger::write(Logger::Context(NULL, NULL), Logger::Error, m_loggerTag.c_str(), prefix, buffer);
}

// ** ApplicationDelegate::fatal
void ApplicationDelegate::fatal(CString prefix, CString format, ...)
{
    NIMBLE_LOGGER_FORMAT(format);
    Logger::write(Logger::Context(NULL, NULL), Logger::Fatal, m_loggerTag.c_str(), prefix, buffer);
}

// -------------------------------------------------------------- ServiceApplication ---------------------------------------------------------------- //

// ** ServiceApplication::ServiceApplication
ServiceApplication::ServiceApplication( const Arguments& args, IApplication* impl ) : Application( args, impl )
{
}

// ** ServiceApplication::create
ServiceApplication* ServiceApplication::create( const Arguments& args )
{
    if( IApplication* impl = createServiceApplication() ) {
        return DC_NEW ServiceApplication( args, impl );
    }

    return NULL;
}
    
// ------------------------------------------------------------ WindowedApplicationDelegate ---------------------------------------------------------- //

// ** WindowedApplicationDelegate::initialize
bool WindowedApplicationDelegate::initialize(s32 width, s32 height)
{
    // Create a window
    m_window = Window::create(width, height);
    
    if (!m_window.valid())
    {
        return false;
    }
    
    // Now subscribe for window events.
    m_window->subscribe<Window::TouchBegan>(dcThisMethod(WindowedApplicationDelegate::handleTouchBegan));
    m_window->subscribe<Window::TouchEnded>(dcThisMethod(WindowedApplicationDelegate::handleTouchEnded));
    m_window->subscribe<Window::TouchMoved>(dcThisMethod(WindowedApplicationDelegate::handleTouchMoved));
#if defined( DC_PLATFORM_WINDOWS ) || defined( DC_PLATFORM_MACOS )
    m_window->subscribe<Window::KeyPressed>(dcThisMethod(WindowedApplicationDelegate::handleKeyPressed));
    m_window->subscribe<Window::KeyReleased>(dcThisMethod(WindowedApplicationDelegate::handleKeyReleased));
#endif  //  #if defined( DC_PLATFORM_WINDOWS ) || defined( DC_PLATFORM_MACOS )
    m_window->subscribe<Window::Update>(dcThisMethod(WindowedApplicationDelegate::handleWindowUpdate));
    
    return true;
}
 
// ** WindowedApplicationDelegate::initialize
void WindowedApplicationDelegate::setCaption(const String& value)
{
    NIMBLE_ABORT_IF(!m_window.valid(), "invalid window");
    m_window->setCaption(value);
}
    
} // namespace Platform

DC_END_DREEMCHEST
