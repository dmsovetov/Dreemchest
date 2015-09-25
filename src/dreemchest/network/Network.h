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

#ifndef __DC_Network_H__
#define __DC_Network_H__

#include "../Dreemchest.h"

#include "../io/Io.h"
#include "../io/streams/ByteBuffer.h"
#include "../io/serialization/Storage.h"
#include "../io/serialization/BinarySerializer.h"
#include "../io/serialization/Serializable.h"
#include "../io/serialization/Serializer.h"
#include "../event/EventEmitter.h"
#include <time.h>

DC_BEGIN_DREEMCHEST

namespace net {

	DECLARE_LOG(log)

	class TCPSocketDelegate;
	class TCPSocket;
	class UDPSocketDelegate;
	class UDPSocket;
	class TCPSocketListenerDelegate;
	class TCPSocketListener;
	class SocketDescriptor;
	class TCPStream;

	class Connection;
	class NetworkHandler;
		class ServerHandler;
		class ClientHandler;

	class NetworkEvents;
    class INetwork;

	//! Alias io::Serializable as NetworkPacket
	typedef io::Serializable NetworkPacket;

	//! TCP socket strong ptr.
	typedef StrongPtr<TCPSocket> TCPSocketPtr;

	//! TCP socket event delegate ptr.
	typedef StrongPtr<TCPSocketDelegate> TCPSocketDelegatePtr;

	//! UDP socket strong ptr.
	typedef StrongPtr<UDPSocket> UDPSocketPtr;

	//! UDP socket event delegate ptr.
	typedef StrongPtr<UDPSocketDelegate> UDPSocketDelegatePtr;

	//! TCP stream strong ptr.
	typedef StrongPtr<TCPStream> TCPStreamPtr;

	//! TCP socket listener ptr.
	typedef StrongPtr<TCPSocketListener> TCPSocketListenerPtr;

	//! TCP socket listener delegate ptr.
	typedef StrongPtr<TCPSocketListenerDelegate> TCPSocketListenerDelegatePtr;

	//! Network handler strong ptr.
	typedef StrongPtr<NetworkHandler> NetworkHandlerPtr;

	//! Server network handler strong ptr.
	typedef StrongPtr<ServerHandler> ServerHandlerPtr;

	//! Client network handler strong ptr.
	typedef StrongPtr<ClientHandler> ClientHandlerPtr;

	//! Connection weak ptr.
	typedef WeakPtr<Connection> ConnectionWPtr;

	//! Connection strong ptr.
	typedef StrongPtr<Connection> ConnectionPtr;

	//! Socket list type.
	typedef List<TCPSocketPtr> TCPSocketList;

	//! Connection list type.
	typedef List<ConnectionPtr> ConnectionList;

    //! A helper class to represent a network address.
    class NetworkAddress {
    public:

        static NetworkAddress           Null;		//!< Empty address.
        static NetworkAddress           Localhost;	//!< Localhost address.

    public:

										//! Constructs NetworkAddress from a 32-bit integer.
                                        NetworkAddress( u32 address = 0 );
										
										//! Constructs a NetworkAddress from string.
        explicit                        NetworkAddress( CString address );

		//! Converts a network address to unsigned 32-bit integer.
        operator                        u32( void ) const;

		//! Converts a network address to string.
        CString							toString( void ) const;

    private:

		//! Actual network address.
        u32								m_address;
    };

	//! Array of network addresses.
    typedef Array<NetworkAddress> NetworkAddressArray;

	//! Unix time structure.
	struct UnixTime {
						//! Constructs a current Unix time.
						UnixTime( void )
							: m_value( 0 ) {}

						//! Constructs a Unix time from a timestamp.
						UnixTime( time_t value )
							: m_value( value ) {}

		//! Subtracts two Unix time values and returns amount of seconds between them.
		s32				operator - ( const UnixTime& other ) const { return static_cast<s32>( m_value - other.m_value ); }

		//! Subtracts a given amount of seconds from this UnixTime and returns a seconds between them.
		s32				operator - ( s32 seconds ) const { return static_cast<s32>( m_value ) - seconds; }

		//! Subtracts a given amount of seconds from this UnixTime and returns a seconds between them.
		s32				operator - ( u32 seconds ) const { return static_cast<s32>( m_value ) - seconds; }

		//! Adds a given amount of seconds to Unix time value.
		UnixTime		operator + ( s32 seconds ) const { return UnixTime( static_cast<u32>( m_value + seconds ) ); }

		//! Compares two Unix time values.
		bool			operator >= ( const UnixTime& other ) const { return m_value >= other.m_value; }

		//! Compares two Unix time values.
		bool			operator <= ( const UnixTime& other ) const { return m_value <= other.m_value; }

		//! Convers Unix time to unsigned int.
		u32				value( void ) const { return static_cast<u32>( m_value ); }

		//! Converts Unix time to UTC string.
		String			utc( void ) const;

		//! Returns a current Unix time.
		static UnixTime	current( void ) { return UnixTime( time( NULL ) ); }

		time_t			m_value;	//! Actual value.
	};

	// ** UnixTime::utc
	inline String UnixTime::utc( void ) const
	{
		tm* ptm = gmtime( &m_value );
		s8  buffer[256];
		strftime( buffer, 256, "%a, %d %b %Y %H:%M:%S +0000", ptm );
		return buffer;
	}

    //! Network interface class.
    class Network {
    public:

										//! Constructs Network instance.
                                        Network( void );
        virtual                         ~Network( void );

		//! Returns a current host IP address.
        const NetworkAddress&           hostIP( void ) const;

		//! Returns a broadcast IP address.
        const NetworkAddress&           broadcastIP( void ) const;

		//! Returns current host name.
        CString							hostName( void ) const;

    private:

		//! Network implementation.
        INetwork*                       m_impl;
    };

    // ** class INetwork
    class INetwork {
    friend class Network;
    public:


        virtual                         ~INetwork( void ) {}

        virtual const NetworkAddress&   hostIP( void ) const = 0;
        virtual const NetworkAddress&   broadcastIP( void ) const = 0;
        virtual CString					hostName( void ) const = 0;

    protected:

        Network*                        m_parent;
    };

	//! Base class for all remote call argument types.
	template<typename T>
	struct RemoteCallArgument : public io::SerializableT<T> {
	};

	//! Base call for all remote call response types.
	template<typename T>
	struct RemoteCallResponse : public io::SerializableT<T> {
	};

	//! Base class for all replicated event types.
	template<typename T>
	struct ReplicatedEvent : public io::SerializableT<T> {
	};

	//! Base class for all network packets.
	template<typename T>
	struct Packet : public io::SerializableT<T> {
	};

	//! Remote call error response.
	struct Error : public io::SerializableT<Error> {
		//! Error codes
		enum {
			NoError			 = 0		//!< No error recorded.
		,	NotImplemented   = 10001
		,	InvalidProcedure = 20001
		,	InvalidCall		 = 30001
		,	InvalidEnum		 = 40001
		,	InvalidValue	 = 50001
		};

					//! Constructs an Error instance.
					Error( u16 code = NoError, const String& message = String() )
						: code( code ), message( message ) {}

		u16			code;		//!< Recorded error code.
		String		message;	//!< Error message.

		//! Compares the error with a specified code.
		bool		operator == ( u16 error ) const { return code == error; }

					//! Converts the error object to boolean value.
					operator bool( void ) const { return code != 0; }

		IoBeginSerializer
			IoField( code )
			IoField( message )
		IoEndSerializer
	};
    
} // namespace net
    
DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "NetworkHandler/Connection.h"
	#include "NetworkHandler/ServerHandler.h"
	#include "NetworkHandler/ClientHandler.h"
	#include "Sockets/TCPSocketListener.h"
	#include "Sockets/TCPSocket.h"
	#include "Sockets/UDPSocket.h"
	#include "Sockets/TCPStream.h"
#endif

#endif	/*	!__DC_Network_H__	*/