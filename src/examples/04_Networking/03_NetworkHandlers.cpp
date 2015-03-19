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

// Include a Platform module header.
#include <platform/Platform.h>

// Include a Network module header.
#include <network/Network.h>

#include <threads/Threads.h>

// Open a root engine namespace
DC_USE_DREEMCHEST

// Open a platform namespace.
using namespace platform;

// Open a net namespace.
using namespace net;

thread::Thread* serverThread = NULL;

// Application delegate is used to handle an events raised by application instance.
class Server : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application )
	{
        // Setup default loggers
        platform::log::setStandardHandler();
        net::log::setStandardHandler();

		//! Create a network interface
		Network network;

		NetworkServerHandlerPtr server = startServer( application, 20000 );
		serverThread = thread::Thread::create();
		serverThread->start( dcThisMethod( Server::updateServer ), server.get() );

		NetworkClientHandlerPtr client = connectToServer( application, 20000 );

		while( true ) {
			client->update();
		}

		// Now quit
		application->quit();
    }

	void updateServer( void* userData )
	{
		while( true ) {
			reinterpret_cast<NetworkServerHandler*>( userData )->update();
			thread::Thread::sleep( 1 );
		}
	}

	// This method connects to a remote socket.
	NetworkClientHandlerPtr connectToServer( Application* application, u16 port )
	{
		// Connect to remote server
		NetworkClientHandlerPtr client = NetworkClientHandler::create( NetworkAddress::Localhost, port );

		if( client == NULL ) {
			return NetworkClientHandlerPtr();
		}

		return client;
	}

	// This method starts a TCP server
	NetworkServerHandlerPtr startServer( Application* application, u16 port )
	{
		// Create a server-side network handler
		NetworkServerHandlerPtr server = NetworkServerHandler::create( port );

		if( server == NULL ) {
			return NetworkServerHandlerPtr();
		}

		// Check for incomming data.
		net::log::msg( "Waiting for connections...\n" );

		return server;
	}
};

// Now declare an application entry point with Files application delegate.
dcDeclareApplication( new Server )