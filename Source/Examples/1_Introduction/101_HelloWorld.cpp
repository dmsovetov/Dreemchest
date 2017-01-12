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

// Include the engine header file.
#include <Dreemchest.h>

#include <iostream>

// This macro hides a 'using namespace [identifier]' expression and just
// opens a root engine namespace.
//
// Note: by default a namespace identifier is 'dreemchest', but this can be
//       customized (changed to a different name or even completly removed,
//       if you don't want to wrap engine code into a namespace).
DC_USE_DREEMCHEST

// Declare the log tag in global namespace
DREEMCHEST_LOGGER_TAG( HelloWorld )

// Now the application delegate class is declared. Application delegate class
// should be inherited from platform::ApplicationDelegate class.
//
// Application delegate is used to handle an events raised by application instance.
class HelloWorld : public Platform::ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Platform::Application* application ) {
        
        // The log callback can be overridden by custom function (for example to write logs to file),
        // but now we just set a standard log handler, to print all messages to console.
        Logger::setStandardLogger();

        // Just write a message to a console.
        LogVerbose( "helloWorld", "%s", "application launched!\n" );

        // Now the hello world application is ready to quit - just do it! :)
        application->quit();
    }
};

// Now declare an application entry point. We pass an instance of HelloWorld class,
// which is an application delegate.
//
// Note: this line of code is a MUST HAVE, this wraps a platform-specific entry
//       point for application. Without this line of code your app won't event link :)
dcDeclareApplication( new HelloWorld )
