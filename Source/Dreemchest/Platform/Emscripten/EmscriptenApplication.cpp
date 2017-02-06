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

#include "EmscriptenApplication.h"
#include <unistd.h>

DC_BEGIN_DREEMCHEST

namespace Platform
{

// ** createApplication
IApplication* createApplication( void )
{
    return DC_NEW EmscriptenApplication;
}

// ** currentTime
u64 currentTime( void )
{
    timeval time;
    gettimeofday(&time, NULL);
    return static_cast<u64>(((time.tv_sec) * 1000 + time.tv_usec/1000.0) + 0.5);
}

// ------------------------------------------------------------------------ EmscriptenApplication ------------------------------------------------------------------------ //

// ** EmscriptenApplication::quit
void EmscriptenApplication::quit( u32 exitCode )
{
    exit( exitCode );
}

// ** EmscriptenApplication::launch
s32 EmscriptenApplication::launch( Application* application )
{
    // Save the application instance
    m_application = application;

	// Notify application about a launch.
    application->notifyLaunched();

	// Start an event loop
	//loop();

	return 0;
}

// ** EmscriptenApplication::resourcePath
const String& EmscriptenApplication::resourcePath( void ) const
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

} // namespace Platform

DC_END_DREEMCHEST
