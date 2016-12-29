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

#include "WindowsApplication.h"
#include "WindowsWindow.h"
#include <direct.h>
#define getcwd _getcwd

#pragma comment( lib, "winmm.lib" )

DC_BEGIN_DREEMCHEST

namespace Platform {

// ** createApplication
IApplication* createApplication( void )
{
    return DC_NEW WindowsApplication;
}

// ** createServiceApplication
IApplication* createServiceApplication( void )
{
    return DC_NEW WindowsService;
}

// ** currentTime
u32 currentTime( void )
{
	return timeGetTime();
}

// ------------------------------------------------------------------------ WindowsApplication ------------------------------------------------------------------------ //

// ** WindowsApplication::quit
void WindowsApplication::quit( u32 exitCode )
{
    exit( exitCode );
}

// ** WindowsApplication::launch
s32 WindowsApplication::launch( Application* application )
{
    // Save the application instance
    m_application = application;

	// Notify application about a launch.
    application->notifyLaunched();

	// Start an event loop
	loop();

	return 0;
}

// ** WindowsApplication::loop
void WindowsApplication::loop( void )
{
	MSG msg;

	while( 1 ) {
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
			if( msg.message == WM_QUIT ) {
				break;
			}

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else {
            // Update an application
            m_application->notifyUpdate();

            // Update all windows
			for( WindowsWindow::Windows::iterator i = WindowsWindow::s_windows.begin(), end = WindowsWindow::s_windows.end(); i != end; ++i ) {
				i->second->owner()->notifyUpdate();
			}
		}
	}
}

// ** WindowsApplication::resourcePath
const String& WindowsApplication::resourcePath( void ) const
{
    if (m_workingDirectory.empty())
    {
        s8 buffer[256];
        getcwd(buffer, sizeof(buffer));
        m_workingDirectory = buffer;
        m_workingDirectory += "/";
    }
    
    return m_workingDirectory;
}

// ------------------------------------------------------------------------ WindowsService ------------------------------------------------------------------------ //

// ** WindowsService::s_instance
WindowsService* WindowsService::s_instance = NULL;

// ** WindowsService::WindowsService
WindowsService::WindowsService( void ) : m_application( NULL ), m_statusHandle( NULL ), m_stopEvent( INVALID_HANDLE_VALUE )
{
    NIMBLE_ABORT_IF( s_instance != NULL, "only a single WindowsService instance is allowed" );

    s_instance = this;
    ZeroMemory( &m_status, sizeof( m_status ) );
    ZeroMemory( m_name, sizeof( m_name ) );
}

// ** WindowsService::quit
void WindowsService::quit( u32 exitCode )
{
    exit( exitCode );
}

// ** WindowsService::launch
s32 WindowsService::launch( Application* application )
{
    // Save the parent application instance
    m_application = application;

    // Get the application arguments
    const Arguments& args = application->args();

    // Get the service name from arguments
    String name = args.string( "service" );
    NIMBLE_ABORT_IF( name.empty(), "service name cannot be empty" );
    NIMBLE_ABORT_IF( name.length() >= MaxServiceNameLength, "service name is too long" );

    // Save the service name
    memcpy( m_name, name.c_str(), name.length() );
    
    // Declare the service
    SERVICE_TABLE_ENTRY service[] = 
    {
        { m_name, static_cast<LPSERVICE_MAIN_FUNCTION>( entryPoint ) },
        { NULL,   NULL                                                      }
    };

    LogVerbose( "service", "starting application as a service '%s'...\n", m_name );

    // Start the service application
    if( StartServiceCtrlDispatcher( service ) == FALSE )
    {
        LogError( "service", "failed to start service control dispatcher\n" );
        return GetLastError();
    }

	return 0;
}

// ** WindowsService::resourcePath
const String& WindowsService::resourcePath( void ) const
{
    if (m_workingDirectory.empty())
    {
        s8 buffer[256];
        getcwd(buffer, sizeof(buffer));
        m_workingDirectory = buffer;
        m_workingDirectory += "/";
    }
    
    return m_workingDirectory;
}

// ** WindowsService::configure
bool WindowsService::configure( void )
{
    DWORD status = E_FAIL;
 
    // Register our service control handler with the SCM
    m_statusHandle = RegisterServiceCtrlHandler( m_name, controlHandler );
 
    if( m_statusHandle == NULL )  {
        LogError( "service", "failed to register service control handler\n" );
        return false;
    }
 
    // Tell the service controller we are starting
    ZeroMemory( &m_status, sizeof( m_status ) );
    m_status.dwServiceType              = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwControlsAccepted         = 0;
    m_status.dwCurrentState             = SERVICE_START_PENDING;
    m_status.dwWin32ExitCode            = 0;
    m_status.dwServiceSpecificExitCode  = 0;
    m_status.dwCheckPoint               = 0;
 
    if( SetServiceStatus( m_statusHandle , &m_status ) == FALSE ) {
        LogWarning( "service", "failed to set service status while configuring\n" );
    }
 
    // Create a service stop event to wait on later
    m_stopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    // Failed to create an event - stop and exit
    if( m_stopEvent == NULL ) {
        switchToState( 0, SERVICE_STOPPED, 1 );
        return false;
    }

    // Now swith to a running state
    switchToState( SERVICE_ACCEPT_STOP, SERVICE_RUNNING, 0 );

    return true;
}

// ** WindowsService::switchToState
void WindowsService::switchToState( DWORD controlsAccepted, DWORD state, DWORD checkpoint )
{
    m_status.dwControlsAccepted = controlsAccepted;
    m_status.dwCurrentState     = state;
    m_status.dwWin32ExitCode    = GetLastError();
    m_status.dwCheckPoint       = checkpoint;
 
    if( SetServiceStatus( m_statusHandle, &m_status ) == FALSE ) {
        LogWarning( "service", "failed to set service status to %d\n", state );
	}
}

// ** WindowsService::launchServiceThread
void WindowsService::launchServiceThread( void )
{
    // Start a thread that will perform the main task of the service
    HANDLE hThread = CreateThread( NULL, 0, thread, NULL, 0, NULL );
   
    // Wait until our worker thread exits signaling that the service needs to stop
    WaitForSingleObject( hThread, INFINITE );
   
    // Close stop handle
    CloseHandle( m_stopEvent );
}

// ** WindowsService::isRunning
bool WindowsService::isRunning( void ) const
{
    return m_status.dwCurrentState == SERVICE_RUNNING;
}

// ** WindowsService::wasStopped
bool WindowsService::wasStopped( void ) const
{
    return WaitForSingleObject( m_stopEvent, 0 ) == WAIT_OBJECT_0;
}

// ** WindowsService::entryPoint
VOID WINAPI WindowsService::entryPoint( DWORD argc, LPTSTR* argv )
{
    // Configure the service instance
    if( !s_instance->configure() ) {
        return;
    }

    // Notify about a launch
    s_instance->m_application->notifyLaunched();

    // Launch the service thread
    s_instance->launchServiceThread();

    // Finally stop the service
    LogVerbose( "service", "thread was aborted, switching to a stopped state\n" );
    s_instance->switchToState( 0, SERVICE_STOPPED, 3 );
}

// ** WindowsService::controlHandler
VOID WINAPI WindowsService::controlHandler( DWORD code )
{
    switch( code ) {
    case SERVICE_CONTROL_STOP:
        if( !s_instance->isRunning() ) {
            LogWarning( "service", "service is not running now\n" );
            return;
        }

        // Switch to a stopped state
        LogVerbose( "service", "switching to a stop pending state\n" );
        s_instance->switchToState( 0, SERVICE_STOP_PENDING, 4 );

        // This will signal the worker thread to start shutting down
        SetEvent( s_instance->m_stopEvent );
        break;
 
    default:    break;
    }
}

// ** WindowsService::thread
DWORD WINAPI WindowsService::thread( LPVOID param )
{
    //  Periodically check if the service has been requested to stop
    while( !s_instance->wasStopped() ) {  
        // Update parent application here
        s_instance->m_application->notifyUpdate();
    }
 
    return ERROR_SUCCESS;
}

} // namespace Platform

DC_END_DREEMCHEST