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

#ifndef __DC_Network_Packet_Ping_H__
#define __DC_Network_Packet_Ping_H__

#include "PacketHandler.h"

DC_BEGIN_DREEMCHEST

namespace Network {

namespace Packets {

	//! Keep alive packet.
	struct KeepAlive : public Packet<KeepAlive> {
	};

	//! Latency test & time sync packet
	struct Ping : public Packet<Ping> {
						//! Constructs a Ping instance.
						Ping( u8 iterations = 0, u32 timestamp = 0, s32 time = 0 )
							: iterations( iterations ), timestamp( timestamp ), time( time ) {}

		u8				iterations;		//!< Current iteration number.
		u32				timestamp;		//!< A timestamp value when the packet was sent.
		u32				time;			//!< Current connection time in milliseconds

        virtual void    serialize( Io::StreamWPtr stream ) const DC_DECL_OVERRIDE
        {
            stream->write( &iterations, sizeof( iterations ) );
            stream->write( &timestamp, sizeof( timestamp ) );
            stream->write( &time, sizeof( time ) );
        }

        virtual void    deserialize( Io::StreamWPtr stream ) DC_DECL_OVERRIDE
        {
            stream->read( &iterations, sizeof( iterations ) );
            stream->read( &timestamp, sizeof( timestamp ) );
            stream->read( &time, sizeof( time ) );        
        }
	};

} // namespace Packets

} // namespace Network

DC_END_DREEMCHEST

#endif  /*  __DC_Network_Packet_Ping_H__ */