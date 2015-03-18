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

#ifndef		__DC_Network_PosixNetwork_H__
#define		__DC_Network_PosixNetwork_H__


#include    "../Network.h"
#include	"PosixNetworkAPI.h"

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET (~0)
#endif

DC_BEGIN_DREEMCHEST

namespace net {

    // ** class PosixNetwork
    class PosixNetwork : public INetwork {
    public:

                                        PosixNetwork( void );
        virtual                         ~PosixNetwork( void );

        // ** INetwork
        virtual const NetworkAddress&   hostIP( void ) const;
        virtual const NetworkAddress&   broadcastIP( void ) const;
        virtual CString					hostName( void ) const;

		// ** PosixNetwork
		static void						setSocketAddressReuse( Socket socket );
		static void						setSocketNonBlocking( Socket socket );
		static void						setSocketBroadcast( Socket socket );
		static void						closeSocket( Socket socket );
		static s32						lastError( void );

    private:

        void                            initialize( void );

        bool                            requestHostName( String& name ) const;
        bool                            requestInterfaces( NetworkAddressArray& broadcast, NetworkAddressArray& host, NetworkAddressArray& mask ) const;

    private:

        String							m_hostName;
        NetworkAddress                  m_host;
        NetworkAddress                  m_broadcast;
        NetworkAddress                  m_mask;
        bool                            m_isAvailable;

	#if defined DC_PLATFORM_WINDOWS
		WSAData*						m_wsa;
	#endif
    };

} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_Network_H__	*/