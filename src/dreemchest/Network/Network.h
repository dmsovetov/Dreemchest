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
#include "../Io/KeyValue.h"

#if defined( DC_PLATFORM_WINDOWS )
    #define     _WINSOCK_DEPRECATED_NO_WARNINGS
    #include	<winsock2.h>
    #include	<Ws2tcpip.h>
#else
    #include    <netinet/in.h>
	#include	<netinet/tcp.h>
    #include    <arpa/inet.h>
    #include    <sys/socket.h>
    #include    <sys/select.h>
    #include    <sys/poll.h>
    #include    <fcntl.h>
    #include    <netdb.h>
    #include    <errno.h>
    #include    <unistd.h>

    #if !defined( DC_PLATFORM_ANDROID )
        #include    <ifaddrs.h>
    #endif
#endif

DC_BEGIN_DREEMCHEST

namespace Network {

    NIMBLE_LOGGER_TAG( Network )

	class TCPSocket;
	class UDPSocket;
	class TCPSocketListener;
	class SocketDescriptor;
	class Connection;

    namespace Packets {

        struct Ping;
        struct RemoteCall;
        struct RemoteCallResponse;

    } // namespace Packets

    //! Declare smart pointer types.
	dcDeclarePtrs( Application )
    dcDeclarePtrs( ApplicationTCP )
    dcDeclarePtrs( TCPSocket )
    dcDeclarePtrs( UDPSocket )
    dcDeclarePtrs( TCPSocketListener )
    dcDeclarePtrs( Connection )
    dcDeclarePtrs( Connection_ )

    //! Unique packet identifier type.
    typedef TypeId PacketTypeId;

    //! Network packet unique pointer.
    typedef AutoPtr<class AbstractPacket> PacketUPtr;

    //! Command unique pointer.
    typedef AutoPtr<class AbstractCommand> CommandUPtr;

	//! Connection middleware unique pointer.
	typedef AutoPtr<class ConnectionMiddleware> ConnectionMiddlewareUPtr;

	//! Socket list type.
	typedef List<TCPSocketPtr> TCPSocketList;

	//! Connection list type.
	typedef List<ConnectionPtr> ConnectionList;

	//! Connection weak array type.
	typedef Array<ConnectionWPtr> ConnectionWeakArray;

    //! A helper class to represent a network address.
    class Address {
    public:

        static Address           Null;		//!< Empty address.
        static Address           Localhost;	//!< Localhost address.

    public:

										//! Constructs Address from a 32-bit integer.
                                        Address( u32 address = 0 );
										
										//! Constructs a Address from string.
        explicit                        Address( CString address );

		//! Converts a network address to unsigned 32-bit integer.
        operator                        u32( void ) const;

		//! Converts a network address to string.
        CString							toString( void ) const;

    private:

		//! Actual network address.
        u32								m_address;
    };

	//! Array of network addresses.
    typedef Array<Address> NetworkAddressArray;

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
        const Address&           hostIP( void ) const;

		//! Returns a broadcast IP address.
        const Address&           broadcastIP( void ) const;

		//! Returns current host name.
        CString							hostName( void ) const;

        //! Converts the network address and port to a sockaddr_in structure.
		static sockaddr_in				toSockaddr( const Address& address, u16 port );

    private:

        //! Performs network initialization.
        void                            initialize( void );

        //! Returns the local host name.
        bool                            requestHostName( String& name ) const;

        //! Returns list of available network interfaces.
        bool                            requestInterfaces( NetworkAddressArray& broadcast, NetworkAddressArray& host, NetworkAddressArray& mask ) const;

    private:

        String							m_hostName;     //!< Local host name.
        Address                  m_host;         //!< Local host address.
        Address                  m_broadcast;    //!< Broadcast address.
        Address                  m_mask;         //!< Network mask.
        bool                            m_isAvailable;  //!< Indicates that network was successfully initialized.

	#if defined( DC_PLATFORM_WINDOWS )
		WSAData*						m_wsa;          //!< WSA struct.
	#endif  /*  DC_PLATFORM_WINDOWS */
    };

	//! Base class for all remote call types.
	template<typename TRemoteCall, typename TArgument, typename TResponse = Void>
	struct RemoteCall {
		typedef TArgument Argument;
		typedef TResponse Response;
		static StringHash id( void ) { return StringHash( name() ); }
		static CString    name( void ) { return TypeInfo<TRemoteCall>::name(); }
	};

	//! Base class for all remote call argument types.
	template<typename T>
	struct RemoteCallArgument : public Io::SerializableT<T> {
	};

	//! Base call for all remote call response types.
	template<typename T>
	struct RemoteCallResponse : public Io::SerializableT<T> {
	};

	//! Base class for all replicated event types.
	template<typename T>
	struct ReplicatedEvent : public Io::SerializableT<T> {
	};

	//! Remote call error response.
	struct Error : public Io::SerializableT<Error> {
		//! Error codes
		enum {
			NoError			 = 0		//!< No error recorded.
		,	NotImplemented   = 10001
		,	InvalidProcedure = 10002
		,	InvalidCall		 = 10003
		,	InvalidEnum		 = 10004
		,	InvalidValue	 = 10005
		,	UserDefinedCodes = 20000
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
    
} // namespace Network
    
DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "NetworkHandler/Connection.h"
    #include "NetworkHandler/ApplicationTCP.h"
	#include "Sockets/TCPSocketListener.h"
	#include "Sockets/TCPSocket.h"
	#include "Sockets/UDPSocket.h"
    #include "Packets/PacketHandler.h"
    #include "Packets/Ping.h"
#endif

#endif	/*	!__DC_Network_H__	*/