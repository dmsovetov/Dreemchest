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

#ifndef		__DC_Network_PacketFormatter_H__
#define		__DC_Network_PacketFormatter_H__

#include	"../Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

    // ** class PacketFormatter
    class PacketFormatter {

        typedef std::map<int, const INetworkPacket*> PacketTypes;
        
    public:

                            PacketFormatter( int magic );
                            ~PacketFormatter( void );

        bool                RegisterPacket( int packetId, const INetworkPacket *packet );
        int                 WritePacketToStream( int packetId, const INetworkPacket *packet, io::ByteBufferPtr& stream ) const;
        bool                ParsePacketFromStream( INetworkPacket **packet, int& packetId, io::ByteBufferPtr& stream ) const;
        bool                ParsePacketFromMemory( INetworkPacket **packet, int& packetId, const u8* buffer, u32 size ) const;

    private:

        INetworkPacket*     CreatePacket( int packetId ) const;
        int                 FindPacket( dcMemoryStream stream ) const;
        bool                ParsePacket( INetworkPacket **packet, int& packetId, dcMemoryStream stream ) const;

    private:

        PacketTypes         m_packets;
        const int           m_magic;
    };

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_PacketFormatter_H__	*/