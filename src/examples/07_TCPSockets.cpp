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
DREEMCHEST_LOGGER_TAG( TCPSockets )

// A delagate class to listen the server socket events.
class TCPListenerEventHandler : public net::TCPSocketListenerDelegate {
public:

	//! Handles accepted incomming connection.
	virtual void handleConnectionAccepted( net::TCPSocketListener* sender, net::TCPSocket* socket )
	{
		LogVerbose( "socketListener", "client connected, remote address %s\n", socket->address().toString() );
	}

	//! Handles a remote connection closed.
	virtual void handleConnectionClosed( net::TCPSocketListener* sender, net::TCPSocket* socket )
	{
		LogVerbose( "socketListener", "client disconnected, remote address %s\n", socket->address().toString() );
	}

	//! Called when socket has received data.
	virtual void handleReceivedData( net::TCPSocketListener* sender, net::TCPSocket* socket, net::TCPStream* stream )
	{
		LogVerbose( "socketListener", "%d bytes of data recived from %s:\n", ( int )stream->bytesAvailable(), socket->address().toString() );
		for( int i = 0; i < stream->bytesAvailable(); i++ ) {
			LogVerbose( "socketListener", "%c\n", ( s8 )stream->buffer()[i] );
		}

		// Send a response
		const char* response = "pong";
		socket->sendTo( response, strlen( response ) );
	}
};

// A delegate class to listen the socket events.
class TCPEventHandler : public net::TCPSocketDelegate {
public:

	TCPEventHandler( Platform::Application* app ) : m_application( app ) {}

	//! Called when socket is closed.
	virtual void handleClosed( net::TCPSocket* sender )
	{
		LogVerbose( "socket", "socket disconnected\n" );
	}

	//! Called when socket has received data.
	virtual void handleReceivedData( net::TCPSocket* sender, net::TCPSocket* socket, net::TCPStream* stream )
	{
		LogVerbose( "socket", "%d bytes of data recived from %s:\n", ( int )stream->bytesAvailable(), socket->address().toString() );
		for( int i = 0; i < stream->bytesAvailable(); i++ ) {
			LogVerbose( "client", " %c\n", ( s8 )stream->buffer()[i] );
		}

		static int counter = 0;

		const char* response = "ping";
		socket->sendTo( response, strlen( response ) );
		Threads::Thread::sleep( 1000 );

		counter++;
		if( counter >= 5 ) {
			socket->close();
		}
	}

	Platform::Application* m_application;
};

Threads::ThreadPtr serverThread;

// Application delegate is used to handle an events raised by application instance.
class Server : public Platform::ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Platform::Application* application )
	{
        // Set the default log handler.
        Logger::setStandardLogger();

		//! Create a network interface
		net::Network network;

		net::TCPSocketListenerPtr server = startServer( application, 20000 );
		serverThread = Threads::Thread::create();
		serverThread->start( dcThisMethod( Server::updateServer ), server.get() );

		net::TCPSocketPtr client = connectToServer( application, 20000 );

		while( true ) {
			if( client != NULL && client->isValid() ) {
				client->update();
			}
		}
/*
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
*/
		// Now quit
		application->quit();
    }

	void updateServer( void* userData )
	{
		while( true ) {
			reinterpret_cast<net::TCPSocketListener*>( userData )->update();
			Threads::Thread::sleep( 1 );
		}
	}

	// This method connects to a remote socket.
	net::TCPSocketPtr connectToServer( Platform::Application* application, u16 port )
	{
		// Connect to remote server
		net::TCPSocketPtr socket = net::TCPSocket::connectTo( net::NetworkAddress::Localhost, port, new TCPEventHandler( application ) );

		if( socket == NULL ) {
			return net::TCPSocketPtr();
		}

		// Send message to server
		const char* msg = "ping";
		socket->sendTo( msg, strlen( msg ) );

		// Check for incomming data.
		LogVerbose( "client", "waiting for server response...\n" );

		return socket;
	}

	// This method starts a TCP server
	net::TCPSocketListenerPtr startServer( Platform::Application* application, u16 port )
	{
		// Create a TCP socket.
		net::TCPSocketListenerPtr socket = net::TCPSocketListener::bindTo( port, new TCPListenerEventHandler );

		if( socket == NULL ) {
			return net::TCPSocketListenerPtr();
		}

		// Check for incomming data.
		LogVerbose( "server", "waiting for connections...\n" );

		return socket;
	}
};

// Now declare an application entry point with Files application delegate.
dcDeclareApplication( new Server )