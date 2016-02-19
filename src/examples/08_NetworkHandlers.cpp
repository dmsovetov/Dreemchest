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

#define LOCAL_MODE	(1)

// Open a root engine namespace
DC_USE_DREEMCHEST

// Declare the log tag in global namespace
DREEMCHEST_LOGGER_TAG( "NetworkHandlers" )

Threads::ThreadPtr serverThread;

// Application delegate is used to handle an events raised by application instance.
class NetworkHandlers : public Platform::ApplicationDelegate {

	struct LolEvent : public Io::Serializable {
		ClassEnableTypeId( LolEvent )
		ClassEnableCloning( LolEvent )

		int x;
		std::string bar;

		IoBeginSerializer
			IoField( x )
			IoField( bar )
		IoEndSerializer

		LolEvent( int x = 0, std::string bar = "" ) : x( x ), bar( bar ) {}
	};

	struct ExtraEvent : public Io::Serializable {
		ClassEnableTypeId( ExtraEvent )
		ClassEnableCloning( ExtraEvent )
	};

    // This method will be called once an application is launched.
    virtual void handleLaunched( Platform::Application* application )
	{
        // Set the default log handler.
        Logger::setStandardLogger();

		//! Create a network interface
		net::Network network;

	#if LOCAL_MODE
		net::ServerHandlerPtr server = startServer( application, 20000 );
		serverThread = Threads::Thread::create();
		serverThread->start( dcThisMethod( NetworkHandlers::updateServer ), server.get() );

		net::ClientHandlerPtr client = connectToServer( application, 20000 );
		client->registerEvent<LolEvent>();
		client->registerEvent<ExtraEvent>();
		client->subscribe<LolEvent>( dcThisMethod( NetworkHandlers::handleLolEvent ) );
		client->subscribe<ExtraEvent>( dcThisMethod( NetworkHandlers::handleExtraEvent ) );

		while( true ) {
			client->update( 16 );
		}
	#else
		ClientHandlerPtr client = connectToServer( application, 20000 );
		if( client != NULL ) {
			while( true ) {
				client->update();
			}
		}

		ServerHandlerPtr server = startServer( application, 20000 );

		updateServer( server.get() );
	#endif

		// Now quit
		application->quit();
    }

	void handleLolEvent( const LolEvent& e )
	{
		LogVerbose( "events", "handleLolEvent : x=%d, bar=%s\n", e.x, e.bar.c_str() );
	}

	void handleExtraEvent( const ExtraEvent& e )
	{
		LogVerbose( "events", "handleExtraEvent\n" );
	}

	void updateServer( void* userData )
	{
		net::ServerHandler* server = reinterpret_cast<net::ServerHandler*>( userData );

		int counter = 1;

		while( true ) {
			server->update( 16 );
			Threads::Thread::sleep( 1 );

			if( (++counter % 500) == 0 ) {
				LogDebug( "server", "emit : %d\n", counter );
				server->emit<LolEvent>( counter, "counter!" );
				server->emit<ExtraEvent>();
			}
		}
	}

	// This method connects to a remote socket.
	net::ClientHandlerPtr connectToServer( Platform::Application* application, u16 port )
	{
		// Connect to remote server
		net::ClientHandlerPtr client = net::ClientHandler::create( net::NetworkAddress::Localhost, port );

		if( client == NULL ) {
			return net::ClientHandlerPtr();
		}

		return client;
	}

	// This method starts a TCP server
	net::ServerHandlerPtr startServer( Platform::Application* application, u16 port )
	{
		// Create a server-side network handler
		net::ServerHandlerPtr server = net::ServerHandler::create( port );

		if( server == NULL ) {
			return net::ServerHandlerPtr();
		}

		// Check for incomming data.
		LogVerbose( "server", "waiting for connections...\n" );

		return server;
	}
};

// Now declare an application entry point with NetworkHandlers application delegate.
dcDeclareApplication( new NetworkHandlers )