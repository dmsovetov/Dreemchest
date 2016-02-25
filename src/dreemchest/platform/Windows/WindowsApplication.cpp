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

#pragma comment( lib, "winmm.lib" )

DC_BEGIN_DREEMCHEST

namespace Platform {

// ** createApplication
IApplication* createApplication( void )
{
    return DC_NEW WindowsApplication;
}

// ** currentTime
u32 currentTime( void )
{
	return timeGetTime();
}

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

} // namespace Platform

DC_END_DREEMCHEST