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

#ifndef		__DC_Network_UDPSocket_H__
#define		__DC_Network_UDPSocket_H__

#include    "Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

    class IUDPSocket;

    // ** class UDPSocket
    class UDPSocket {
    public:

                            UDPSocket( bool broadcast = false );
        virtual             ~UDPSocket( void );

        int                 Send( const NetworkAddress& address, u16 port, const void *buffer, int size );
        bool                Listen( u16 port );
        void                Update( void );

    private:

        IUDPSocket*         m_impl;
    };
	/*
    // ** class UDPSocketEvent
    class UDPSocketEvent : public event::Event {

        dcDeclareEventID( Listening )
        dcDeclareEventID( Failed )
        dcDeclareEventID( Data, const NetworkAddress& address, const u8 *data, int size )

        dcBeginClass( UDPSocketEvent )
        dcEndClass

    public:

                            UDPSocketEvent( dcContext ctx = NULL ) : Event( ctx ) { m_type = "UDPSocketEvent"; }

    public:

        const u8*         m_data;
        int                 m_size;
        NetworkAddress      m_address;
    };
	*/
    // ** class IUDPSocket
    class IUDPSocket {
    friend class UDPSocket;
    public:

        virtual             ~IUDPSocket( void ) {}

        virtual int         Send( const NetworkAddress& address, u16 port, const void *buffer, int size ) = 0;
        virtual bool        Listen( u16 port ) = 0;
        virtual void        Update( void ) = 0;

    protected:

        UDPSocket*          m_parent;
    };

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_UDPSocket_H__	*/