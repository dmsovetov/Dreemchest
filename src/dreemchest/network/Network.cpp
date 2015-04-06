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

#include "Network.h"
#include "Sockets/Posix/PosixNetwork.h"

#define CHECK_TYPE_SIZE( type, size ) if( sizeof( type ) != size ) log::error( "Invalid size of %s, expected %d got %d\n", #type, size, sizeof( type ) );

DC_BEGIN_DREEMCHEST

namespace net {

IMPLEMENT_LOGGER(log)

// ** Network::Network
Network::Network( void ) : m_impl( NULL )
{
	CHECK_TYPE_SIZE( u8,  1 )
	CHECK_TYPE_SIZE( s8,  1 )
	CHECK_TYPE_SIZE( u16, 2 )
	CHECK_TYPE_SIZE( s16, 2 )
	CHECK_TYPE_SIZE( u32, 4 )
	CHECK_TYPE_SIZE( s32, 4 )
	CHECK_TYPE_SIZE( u64, 8 )
	CHECK_TYPE_SIZE( s64, 8 )
	CHECK_TYPE_SIZE( f32, 4 )
	CHECK_TYPE_SIZE( f64, 8 )
	CHECK_TYPE_SIZE( Guid, Guid::Size )

    m_impl = DC_NEW PosixNetwork;

    if( m_impl ) {
        m_impl->m_parent = this;
    }
}

Network::~Network( void )
{
    delete m_impl;
}

// ** Network::hostIP
const NetworkAddress& Network::hostIP( void ) const
{
    DC_CHECK_IMPL( NetworkAddress::Null );
    return m_impl->hostIP();
}

// ** Network::broadcastIP
const NetworkAddress& Network::broadcastIP( void ) const
{
    DC_CHECK_IMPL( NetworkAddress::Null );
    return m_impl->broadcastIP();
}

// ** Network::hostName
CString Network::hostName( void ) const
{
    DC_CHECK_IMPL( "" );
    return m_impl->hostName();
}

// --------------------------------------------- NetworkAddress --------------------------------------------- //

// ** NetworkAddress::Null
NetworkAddress NetworkAddress::Null = 0;

// ** NetworkAddress::Localhost
NetworkAddress NetworkAddress::Localhost = NetworkAddress( "127.0.0.1" );

// ** NetworkAddress::NetworkAddress
NetworkAddress::NetworkAddress( u32 address ) : m_address( address )
{

}

NetworkAddress::NetworkAddress( CString address )
{
    m_address = inet_addr( address );
}

// ** NetworkAddress::operator ulong
NetworkAddress::operator u32( void ) const
{
    return m_address;
}

// ** NetworkAddress::toString
CString NetworkAddress::toString( void ) const
{
	CString result = inet_ntoa( *( in_addr* )&m_address );
    return result;
}

} // namespace net

DC_END_DREEMCHEST
