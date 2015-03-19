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

#include	"PosixNetwork.h"

#ifdef DC_PLATFORM_WINDOWS
	#include <iphlpapi.h>
	#pragma comment( lib, "iphlpapi.lib" )
#endif

DC_BEGIN_DREEMCHEST

namespace net {

// ** PosixNetwork::PosixNetwork
PosixNetwork::PosixNetwork( void )
{
    initialize();
}
    
PosixNetwork::~PosixNetwork( void )
{
#ifdef DC_PLATFORM_WINDOWS
	delete m_wsa;
	WSACleanup();
#endif
}

// ** PosixNetwork::lastError
s32 PosixNetwork::lastError( void )
{
#ifdef DC_PLATFORM_WINDOWS
	return WSAGetLastError();
#else
	return errno;
#endif
}

// ** PosixNetwork::initialize
void PosixNetwork::initialize( void )
{
#ifdef DC_PLATFORM_WINDOWS
	WORD wVersionRequested = MAKEWORD( 2, 2 );

	m_wsa = DC_NEW WSAData;

	if( WSAStartup( wVersionRequested, m_wsa ) ) {
		DC_BREAK
		return;
	}

	if( LOBYTE( m_wsa->wVersion ) != 2 || HIBYTE( m_wsa->wVersion ) != 2 ) {
		DC_BREAK
		WSACleanup();
		return;
	}
#endif

    NetworkAddressArray broadcast, host, mask;
    
    bool result = requestHostName( m_hostName );
    DC_BREAK_IF( result == false );

    m_isAvailable = requestInterfaces( broadcast, host, mask );
    if( !m_isAvailable ) {
        return;
    }

    m_host      = host[0];
    m_mask      = mask[0];
    m_broadcast = broadcast[0];

    // ??
    m_broadcast = INADDR_BROADCAST;
}

// ** PosixNetwork::requestHostName
bool PosixNetwork::requestHostName( String& name ) const
{
	s8 hostName[256] = "";

    // ** Get host name
	int result = gethostname( hostName, 256 );
    if( result != 0 ) {
        return false;
    }

    name = hostName;

    return true;
}

// ** PosixNetwork::requestInterfaces
bool PosixNetwork::requestInterfaces( NetworkAddressArray& broadcast, NetworkAddressArray& host, NetworkAddressArray& mask ) const
{
#ifndef DC_PLATFORM_WINDOWS
    ifaddrs *interfaces     = NULL;
    s32     totalInterfaces = 0;

    s32 success = getifaddrs( &interfaces );
    if( success != 0 ) {
        log::error( "PosixNetwork::RequestInterfaces : getifaddrs failed, %s\n", strerror( errno ) );
        return false;
    }

    for( ifaddrs *i = interfaces; i != NULL; i = i->ifa_next ) {
        sa_family_t sa_type = i->ifa_addr->sa_family;
        
        if( sa_type != AF_INET && sa_type != AF_INET6 ) {
            continue;
        }

        if( i->ifa_dstaddr == NULL ) {
            continue;
        }

        // Check if interface is enX which is the wifi/lan connection
        //
        // en0, en1 - is a physical wired/wireless cards.
        // lo0      - is the loopback interface
        // fw0      - is networking over firewire
        // vmnet    - VMware for network communication with virtual machines
        if( strcmp( i->ifa_name, "en0" ) && strcmp( i->ifa_name, "en1" ) ) {
            continue;
        }

        in_addr ia = *(&((sockaddr_in* )i->ifa_addr)->sin_addr);
        in_addr ma = *(&((sockaddr_in* )i->ifa_netmask)->sin_addr);
        in_addr ba = *(&((sockaddr_in* )i->ifa_dstaddr)->sin_addr);

        broadcast.push_back( ba.s_addr );
        host.push_back( ia.s_addr );
        mask.push_back( ma.s_addr );

        totalInterfaces++;
    }

    freeifaddrs( interfaces );
    return totalInterfaces != 0;
#else
	PIP_ADAPTER_INFO	pAdapterInfo = ( IP_ADAPTER_INFO* )malloc( sizeof( IP_ADAPTER_INFO ) );
	PIP_ADAPTER_INFO	pAdapter	 = NULL;
	DWORD				dwRetVal	 = 0;
    ULONG				ulOutBufLen  = sizeof( IP_ADAPTER_INFO );

    if( GetAdaptersInfo( pAdapterInfo, &ulOutBufLen ) == ERROR_BUFFER_OVERFLOW ) 
	{
        free( pAdapterInfo );
        pAdapterInfo = ( IP_ADAPTER_INFO* ) malloc( ulOutBufLen );
        if( pAdapterInfo == NULL ) 
		{
			log::error( "PosixNetwork::requestInterfaces : error allocating memory needed to call GetAdaptersinfo\n" );
            return false;
        }
    }

	dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen );
	if( dwRetVal != NO_ERROR ) {
		return false;
	}

	pAdapter = pAdapterInfo;
	while (pAdapter) 
	{
		log::verbose("\tComboIndex: \t5d\n", pAdapter->ComboIndex);
		log::verbose("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
		log::verbose("\tAdapter Desc: \t%s\n", pAdapter->Description);
		log::verbose("\tAdapter Addr: \t");
		for (unsigned int i = 0; i < pAdapter->AddressLength; i++) 
		{
			if (i == (pAdapter->AddressLength - 1))
				log::verbose("%.2X\n", (int) pAdapter->Address[i]);
			else
				log::verbose("%.2X-", (int) pAdapter->Address[i]);
		}

		log::verbose("\tIndex: \t%d\n", pAdapter->Index);
		log::verbose("\tType: \t");
		switch (pAdapter->Type) 
		{
		case MIB_IF_TYPE_OTHER:     log::verbose("Other\n");      break;
		case MIB_IF_TYPE_ETHERNET:  log::verbose("Ethernet\n");   break;
		case MIB_IF_TYPE_TOKENRING: log::verbose("Token Ring\n"); break;
		case MIB_IF_TYPE_FDDI:      log::verbose("FDDI\n");       break;
		case MIB_IF_TYPE_PPP:       log::verbose("PPP\n");        break;
		case MIB_IF_TYPE_LOOPBACK:  log::verbose("Lookback\n");   break;
		case MIB_IF_TYPE_SLIP:      log::verbose("Slip\n");       break;
		case IF_TYPE_IEEE80211:     log::verbose("IEEE 802.11 wireless network interface\n"); break;
		default:					log::verbose("Unknown type %ld\n", pAdapter->Type);
									break;
		}

		log::verbose("\tIP Address: \t%s\n", pAdapter->IpAddressList.IpAddress.String);
		log::verbose("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);
		log::verbose("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
		log::verbose("\t***\n");

		u_long host_addr = inet_addr(pAdapter->IpAddressList.IpAddress.String);
		u_long net_mask  = inet_addr(pAdapter->IpAddressList.IpMask.String);
		u_long net_addr  = host_addr & net_mask;         // 172.16.64.0
		u_long dir_bcast_addr = net_addr | (~net_mask); // 172.16.95.255

		broadcast.push_back( /*ba.s_addr*/dir_bcast_addr );
        host.push_back( inet_addr( pAdapter->IpAddressList.IpAddress.String ) );
        mask.push_back( inet_addr( pAdapter->IpAddressList.IpMask.String ) );

	//	m_HostIPMaskStr = pAdapter->IpAddressList.IpMask.String;
	//	m_HostBroadcastIPStr = inet_ntoa(*((in_addr*)(&dir_bcast_addr)));

		pAdapter = pAdapter->Next;
		log::verbose("\n");
	}

	if( pAdapterInfo ) {
        free( pAdapterInfo );
	}

	return true;
#endif
}

// ** PosixNetwork::hostIP
const NetworkAddress& PosixNetwork::hostIP( void ) const
{
    return m_host;
}

// ** PosixNetwork::broadcastIP
const NetworkAddress& PosixNetwork::broadcastIP( void ) const
{
    return m_broadcast;
}

// ** PosixNetwork::hostName
const char* PosixNetwork::hostName( void ) const
{
    return m_hostName.c_str();
}

// ** PosixNetwork::setSocketNonBlocking
void PosixNetwork::setSocketNonBlocking( SocketDescriptor socket )
{
	s32 result = 0;

#if defined( DC_PLATFORM_WINDOWS )
	u_long noBlock = 1;
	result = ioctlsocket( socket, FIONBIO, &noBlock );
	DC_BREAK_IF( result == SOCKET_ERROR );
#else
    result = fcntl( socket, F_SETFL, O_NONBLOCK );
    DC_BREAK_IF( result == -1 );
#endif
}

// ** PosixNetwork::setSocketBroadcast
void PosixNetwork::setSocketBroadcast( SocketDescriptor socket )
{
	s32 result = 0;

#if defined( DC_PLATFORM_WINDOWS )
	s8 one = 1;
#else
	s32 one = 1;
#endif

	result  = setsockopt( socket, SOL_SOCKET, SO_BROADCAST, &one, sizeof( one ) );
	DC_BREAK_IF( result == -1 );
}

// ** PosixNetwork::closeSocket
void PosixNetwork::closeSocket( SocketDescriptor socket )
{
#if defined( DC_PLATFORM_WINDOWS )
	closesocket( socket );
#else
	close( socket );
#endif
}

// ** PosixNetwork::setSocketAddressReuse
void PosixNetwork::setSocketAddressReuse( SocketDescriptor socket )
{
#if defined( DC_PLATFORM_WINDOWS )
	s8 one = 1;
#else
	s32 one = 1;
#endif

    s32 result = setsockopt( socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof( one ) );
    DC_BREAK_IF( result == -1 );
}

} // namespace net

DC_END_DREEMCHEST
