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

// Open a root engine namespace
DC_USE_DREEMCHEST

// Open a platform namespace.
using namespace platform;

// Open a net namespace.
using namespace net;

// A delegate class to listen the socket events.
class TCPEventHandler : public TCPSocketDelegate {
public:

	TCPEventHandler( Application* app ) : m_application( app ) {}

	//! Called each time the socket is successfully connected to an address.
	virtual void handleConnected( TCPSocket* sender, const NetworkAddress& address, SocketDescriptor socket )
	{
		net::log::msg( "Connected to %s, socket %d\n", address.toString(), socket );
	}

	//! Called when socket failed to connect.
	virtual void handleConnectionFailed( TCPSocket* sender, SocketDescriptor socket )
	{
		net::log::msg( "Failed to connect\n" );
	}

	//! Called when socket is closed.
	virtual void handleDisconnected( TCPSocket* sender, SocketDescriptor socket )
	{
		net::log::msg( "Socket disconnected, socket %d\n", socket );
	}

	//! Called when socket has received data.
	virtual void handleReceivedData( TCPSocket* sender, TCPSocket* socket, const u8 *data, u32 size )
	{
		net::log::msg( "%d bytes of data recived from %s:", size, socket->address().toString() );
		for( int i = 0; i < size; i++ ) {
			net::log::msg( " %c", ( s8 )data[i] );
		}
		printf( "\n" );

		// Send a response
		if( sender->isServer() ) {
			const char* response = "hi there!";
			sender->sendTo( response, strlen( response ), socket->descriptor() );
		} else {
			sender->disconnect();
			m_application->quit();
		}
	}

	//! Called when a server socket has accepted an incomming connection.
	virtual void handleConnectionAccepted( TCPSocket* sender, TCPSocket* socket )
	{
		net::log::msg( "Client connected, remote address %s\n", socket->address().toString() );
	}

	//! Called when client socket is closed.
	virtual void handleConnectionClosed( TCPSocket* sender, TCPSocket* socket )
	{
		net::log::msg( "Client disconnected, remote address %s\n", socket->address().toString() );
	}

	Application* m_application;
};

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

		// Connect to a remote server
		TCPSocketPtr socket = connectToServer( application, 20000 );
		
		// If failed to connect - start server
		if( socket == NULL ) {
			net::log::msg( "Failed to connect to server - launching a new one...\n" );
			socket = startServer( application, 20000 );
		}

		while( true ) {
			socket->update();
		}

		// Now quit
		application->quit();
    }

	// This method connects to a remote socket.
	TCPSocketPtr connectToServer( Application* application, u16 port )
	{
		// Create a TCP socket.
		TCPSocketPtr socket = TCPSocket::create( new TCPEventHandler( application ) );

		// Connect to localhost server
		if( !socket->connectTo( NetworkAddress::Localhost, port ) ) {
			return TCPSocketPtr();
		}

		// Send message to server
		const char* msg = "Hello!";
		socket->sendTo( msg, strlen( msg ) );

		// Check for incomming data.
		net::log::msg( "Waiting for server response...\n" );

		return socket;
	}

	// This method starts a TCP server
	TCPSocketPtr startServer( Application* application, u16 port )
	{
		// Create a TCP socket.
		TCPSocketPtr socket = TCPSocket::create( new TCPEventHandler( application ) );

		// Do not pass a remote address to create a server socket and listen for connections.
		socket->connectTo( NetworkAddress::Null, port );

		// Check for incomming data.
		net::log::msg( "Waiting for connections...\n" );

		return socket;
	}
};

// Now declare an application entry point with Files application delegate.
dcDeclareApplication( new Server )