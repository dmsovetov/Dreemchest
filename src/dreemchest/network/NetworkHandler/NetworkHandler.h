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

#ifndef		__DC_Network_NetworkHandler_H__
#define		__DC_Network_NetworkHandler_H__

#include    "../Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

    // ** class INetworkPacket
    class INetworkPacket : public ISerializable {
    public:

        virtual                 ~INetworkPacket( void ) {}

        virtual void            read( dcStream stream )         = 0;
        virtual void            write( dcStream stream ) const  = 0;
        virtual INetworkPacket* clone( void ) const             = 0;
    };

    // ** enum eBuiltInPackets
    enum eBuiltInPackets {
        PacketTimeSync          = -1,
        PacketRemoteCall        = -2,
        PacketRemoteResponse    = -3,
        PacketSendFile          = -4,
        PacketFileChunk         = -5,
    };

    // ** packet sTimeSyncPacket
    struct sTimeSyncPacket {
        int timestamp;
       int localTime;
	};

    // ** packet sRemoteCallPacket
    struct sRemoteCallPacket {
        std::string         name;
        int                 identifier;
        std::vector<cValue> args;
    };

    // ** packet sRemoteResponsePacket
    struct sRemoteResponsePacket {
        int    identifier;
	//	cValue value;
    };

    // ** packet sSendFile
    struct sSendFile {
        std::string fileName;
        int         length;
    };

    // ** packet sFileChunk
    struct sFileChunk {
		std::vector<u8> data;
    };

    // ** class NetworkHandler
    class NetworkHandler : public event::EventDispatcher {

        // ** enum eFlags
        enum eFlags {
            Connected           = BIT( 0 ),
            ListeningDatagrams  = BIT( 1 ),
            ListeningBroadcast  = BIT( 2 ),
        };

        enum { PacketMagic = 0xef, MaxPacketBufferSize = 4096, MaxFileChunkSize = 4000 };

        DC_DECLARE_IS( NetworkServerHandler, ServerHandler, NULL )
        DC_DECLARE_IS( NetworkClientHandler, ClientHandler, NULL )

        typedef std::map<int, dcMemoryStream> InputStreams;
        

    public:

                                NetworkHandler( dcContext ctx );
        virtual                 ~NetworkHandler( void );

        void                    Disconnect( void );
        virtual bool            Update( int dt );
        bool                    ListenDatagrams( u16 port );
        bool                    ListenBroadcast( u16 port );
        bool                    SendFile( dcStream file, const char *fileName, int connection = -1 );
        bool                    SendPacket( int packetId, const INetworkPacket *packet, int connection = -1 );
        bool                    SendDatagram( int packetId, const INetworkPacket *packet, const NetworkAddress& address, u16 port );
        bool                    SendBroadcast( int packetId, const INetworkPacket *packet, const NetworkAddress& address, u16 port );
        bool                    RegisterPacket( int packetId, const INetworkPacket *packet );
        int                     GetMaxPacketPayload( void ) const;
        bool                    IsConnected( void ) const;
        bool                    IsListeningDatagrams( void ) const;
        bool                    IsListeningBroadcast( void ) const;

        bool                    RegisterRemoteProcedure( const char *name, const RemoteProcedureCallback& callback );
        bool                    InvokeRemoteProcedure( const char *name, const cValue *args, int count );
        bool                    InvokeRemoteProcedure( const char *name, const cValue *args, int count, const RemoteProcedureResponseCallback& callback );

    protected:

        dcMemoryStream          GetOrCreateInputStream( int connection );
        void                    ParseReceivedPackets( dcMemoryStream stream, const NetworkAddress& address, int connection );
        bool                    SendUDP( dcUDPSocket socket, int packetId, const INetworkPacket *packet, const NetworkAddress& address, u16 port );

        virtual void            ProcessReceivedPacket( int packetId, const INetworkPacket *packet, const NetworkAddress& address, int connection );
        virtual void            ProcessConnection( const NetworkAddress& address );
        virtual void            ProcessDisconnection( void );
        virtual void            ProcessFailure( void );

    private:

        // ** Socket event handlers
        void                    OnDataReceived( const dcEvent e );
        void                    OnDisconnected( const dcEvent e );
        void                    OnPacketUDP( const dcEvent e );

    protected:

        dcTCPSocket             m_netTCP;
        dcUDPSocket             m_netDatagram;
        dcUDPSocket             m_netBroadcast;
        cFlagSet8               m_flags;
        class PacketFormatter*  m_packetParser;
        dcMemoryStream          m_outputStream;
        InputStreams            m_inputStreams;
        class RemoteProcedure*  m_remoteProcedure;
    };

    // ** class NetworkHandlerEvent
    class NetworkHandlerEvent : public event::Event {

        dcDeclareEventID( Connected, const NetworkAddress& address )
        dcDeclareEventID( FileAccepted, const char *fileName, int length )
        dcDeclareEventID( FileChunk, const u8 *data, int length )
        dcDeclareEventID( FileDownloaded )
        dcDeclareEventID( Disconnected )
        dcDeclareEventID( Failure )
        dcDeclareEventID( ConnectionAccepted, int connection, const NetworkAddress& address )
        dcDeclareEventID( ConnectionClosed, int connection )
        dcDeclareEventID( PacketReceived, int packetId, const INetworkPacket *packet, int connection, const NetworkAddress& address )

        dcBeginClass( NetworkHandlerEvent )
        dcEndClass

    public:

                                NetworkHandlerEvent( dcContext ctx = NULL ) : Event( ctx ) { m_type = "NetworkHandlerEvent"; }

    public:

        int                     m_packetId;
        const INetworkPacket*   m_packet;
        int                     m_size;
        int                     m_connection;
        NetworkAddress          m_address;
        const char*             m_fileName;
        const u8*             m_data;
        int                     m_length;
    };
    
} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_NetworkHandler_H__	*/