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

// Open a platform namespace.
using namespace Platform;

// Open a net namespace.
using namespace net;

// A delagate class to listen the server socket events.
class TCPListenerEventHandler : public TCPSocketListenerDelegate {
public:

	//! Handles accepted incomming connection.
	virtual void handleConnectionAccepted( TCPSocketListener* sender, TCPSocket* socket )
	{
		net::log::msg( "Client connected, remote address %s\n", socket->address().toString() );
	}

	//! Handles a remote connection closed.
	virtual void handleConnectionClosed( TCPSocketListener* sender, TCPSocket* socket )
	{
		net::log::msg( "Client disconnected, remote address %s\n", socket->address().toString() );
	}

	//! Called when socket has received data.
	virtual void handleReceivedData( TCPSocketListener* sender, TCPSocket* socket, TCPStream* stream )
	{
		net::log::msg( "%d bytes of data recived from %s:", ( int )stream->bytesAvailable(), socket->address().toString() );
		for( int i = 0; i < stream->bytesAvailable(); i++ ) {
			net::log::msg( " %c", ( s8 )stream->buffer()[i] );
		}
		printf( "\n" );

		// Send a response
		const char* response = "pong";
		socket->sendTo( response, strlen( response ) );
	}
};

// A delegate class to listen the socket events.
class TCPEventHandler : public TCPSocketDelegate {
public:

	TCPEventHandler( Application* app ) : m_application( app ) {}

	//! Called when socket is closed.
	virtual void handleClosed( TCPSocket* sender )
	{
		net::log::msg( "Socket disconnected\n" );
	}

	//! Called when socket has received data.
	virtual void handleReceivedData( TCPSocket* sender, TCPSocket* socket, TCPStream* stream )
	{
		net::log::msg( "%d bytes of data recived from %s:", ( int )stream->bytesAvailable(), socket->address().toString() );
		for( int i = 0; i < stream->bytesAvailable(); i++ ) {
			net::log::msg( " %c", ( s8 )stream->buffer()[i] );
		}
		printf( "\n" );

		static int counter = 0;

		const char* response = "ping";
		socket->sendTo( response, strlen( response ) );
		thread::Thread::sleep( 1000 );

		counter++;
		if( counter >= 5 ) {
			socket->close();
		}
	}

	Application* m_application;
};

thread::ThreadPtr serverThread;

// Application delegate is used to handle an events raised by application instance.
class Server : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application )
	{
        // Setup default loggers
        Platform::log::setStandardHandler();
        net::log::setStandardHandler();

		//! Create a network interface
		Network network;

		TCPSocketListenerPtr server = startServer( application, 20000 );
		serverThread = thread::Thread::create();
		serverThread->start( dcThisMethod( Server::updateServer ), server.get() );

		TCPSocketPtr client = connectToServer( application, 20000 );

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
			reinterpret_cast<TCPSocketListener*>( userData )->update();
			thread::Thread::sleep( 1 );
		}
	}

	// This method connects to a remote socket.
	TCPSocketPtr connectToServer( Application* application, u16 port )
	{
		// Connect to remote server
		TCPSocketPtr socket = TCPSocket::connectTo( NetworkAddress::Localhost, port, new TCPEventHandler( application ) );

		if( socket == NULL ) {
			return TCPSocketPtr();
		}

		// Send message to server
		const char* msg = "ping";
		socket->sendTo( msg, strlen( msg ) );

		// Check for incomming data.
		net::log::msg( "Waiting for server response...\n" );

		return socket;
	}

	// This method starts a TCP server
	TCPSocketListenerPtr startServer( Application* application, u16 port )
	{
		// Create a TCP socket.
		TCPSocketListenerPtr socket = TCPSocketListener::bindTo( port, new TCPListenerEventHandler );

		if( socket == NULL ) {
			return TCPSocketListenerPtr();
		}

		// Check for incomming data.
		net::log::msg( "Waiting for connections...\n" );

		return socket;
	}
};

// Now declare an application entry point with Files application delegate.
dcDeclareApplication( new Server )