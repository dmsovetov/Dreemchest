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

// Open a platform namespace.
using namespace Platform;

// Open a net namespace.
using namespace net;

thread::ThreadPtr serverThread;

// Application delegate is used to handle an events raised by application instance.
class NetworkHandlers : public ApplicationDelegate {

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
    virtual void handleLaunched( Application* application )
	{
        // Setup default loggers
        Platform::log::setStandardHandler();
        net::log::setStandardHandler();

		//! Create a network interface
		Network network;

	#if LOCAL_MODE
		ServerHandlerPtr server = startServer( application, 20000 );
		serverThread = thread::Thread::create();
		serverThread->start( dcThisMethod( NetworkHandlers::updateServer ), server.get() );

		ClientHandlerPtr client = connectToServer( application, 20000 );
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
		net::log::verbose( "handleLolEvent : x=%d, bar=%s\n", e.x, e.bar.c_str() );
	}

	void handleExtraEvent( const ExtraEvent& e )
	{
		net::log::verbose( "handleExtraEvent\n" );
	}

	void updateServer( void* userData )
	{
		ServerHandler* server = reinterpret_cast<ServerHandler*>( userData );

		int counter = 1;

		while( true ) {
			server->update( 16 );
			thread::Thread::sleep( 1 );

			if( (++counter % 500) == 0 ) {
				printf( "emit : %d\n", counter );
				server->emit<LolEvent>( counter, "counter!" );
				server->emit<ExtraEvent>();
			}
		}
	}

	// This method connects to a remote socket.
	ClientHandlerPtr connectToServer( Application* application, u16 port )
	{
		// Connect to remote server
		ClientHandlerPtr client = ClientHandler::create( NetworkAddress::Localhost, port );

		if( client == NULL ) {
			return ClientHandlerPtr();
		}

		return client;
	}

	// This method starts a TCP server
	ServerHandlerPtr startServer( Application* application, u16 port )
	{
		// Create a server-side network handler
		ServerHandlerPtr server = ServerHandler::create( port );

		if( server == NULL ) {
			return ServerHandlerPtr();
		}

		// Check for incomming data.
		net::log::msg( "Waiting for connections...\n" );

		return server;
	}
};

// Now declare an application entry point with NetworkHandlers application delegate.
dcDeclareApplication( new NetworkHandlers )