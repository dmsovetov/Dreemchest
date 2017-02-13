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

#include "MacOSApplication.h"
#include "MacOSApplicationDelegate.h"

#include <Cocoa/Cocoa.h>

DC_BEGIN_DREEMCHEST

namespace Platform {

// ** createApplication
IApplication* createApplication( void* userData )
{
    return DC_NEW MacOSApplication;
}
    
// ** createServiceApplication
IApplication* createServiceApplication( void )
{
    NIMBLE_NOT_IMPLEMENTED
    return NULL;
}
    
// ** currentTime
u64 currentTime( void )
{
    timeval time;
    gettimeofday(&time, NULL);
    return static_cast<u64>(((time.tv_sec) * 1000 + time.tv_usec/1000.0) + 0.5);
}

// ** MacOSApplication::quit
void MacOSApplication::quit( u32 exitCode )
{
    exit( exitCode );
}

// ** MacOSApplication::launch
int MacOSApplication::launch( Application* application )
{
    NSApplication* app = [NSApplication sharedApplication];

    [app setDelegate: [[MacOSApplicationDelegate alloc] init]];
    [app run];

    return 0;
}
    
// ** MacOSApplication::resourcePath
const String& MacOSApplication::resourcePath( void ) const
{
    static const String s_prefix = "file://";
    
    if (m_resourcePath.empty())
    {
        m_resourcePath = [[[[NSBundle mainBundle] resourceURL] absoluteString] UTF8String];
        size_t prefix = m_resourcePath.find(s_prefix);
        
        if (prefix != String::npos)
        {
            m_resourcePath.replace(prefix, s_prefix.length(), "");
        }
    }
    
    return m_resourcePath;
}

} // namespace Platform

DC_END_DREEMCHEST
