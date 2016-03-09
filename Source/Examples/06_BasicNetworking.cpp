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

// Open a root engine namespace
DC_USE_DREEMCHEST

// Declare the log tag in global namespace
DREEMCHEST_LOGGER_TAG( BasicNetworking )

// Application delegate is used to handle an events raised by application instance.
class Server : public Platform::ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Platform::Application* application ) {
        // Set the default log handler.
        Logger::setStandardLogger();

		// Create a network interface and show host info
		net::Network network;
		LogVerbose( "basicNetworking", "IP: %s\n", network.hostIP().toString() );
		LogVerbose( "basicNetworking", "Hostname: %s\n", network.hostName() );
		LogVerbose( "basicNetworking", "Broadcast IP: %s\n", network.broadcastIP().toString() );

		// Now quit
		application->quit();
    }
};

// Now declare an application entry point with Files application delegate.
dcDeclareApplication( new Server )