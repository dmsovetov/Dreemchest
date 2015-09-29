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

#include <gtest/gtest.h>
#include <network/Network.h>

DC_USE_DREEMCHEST

using namespace net;

Network network;

struct ListenerDelegate : public TCPSocketListenerDelegate
{
    virtual void handleReceivedData( TCPSocketListener* sender, TCPSocket* socket, TCPStream* stream )
    {
        receivedDataTriggered = true;
        receivedData.resize( stream->length() );
        memcpy( &receivedData[0], stream->buffer(), stream->length() );
    }
    
    virtual void handleConnectionAccepted( TCPSocketListener* sender, TCPSocket* socket )
    {
        connectionAcceptedTriggered = true;
    }
    
    virtual void handleConnectionClosed( TCPSocketListener* sender, TCPSocket* socket )
    {
        connectionClosedTriggered = true;
    }
    
    ListenerDelegate( void ) : receivedDataTriggered( false ), connectionAcceptedTriggered( false ), connectionClosedTriggered( false ) {}
    
    bool receivedDataTriggered;
    bool connectionAcceptedTriggered;
    bool connectionClosedTriggered;
    std::vector<u8> receivedData;
};


class TCPListener : public testing::Test {
private: static u16 kPort;
protected:
    
    virtual void SetUp() {
        port     = kPort++;
        delegate = new ListenerDelegate;
        listener = TCPSocketListener::bindTo( port, delegate );
        ASSERT_NE( listener, TCPSocketListenerPtr() );
    }
    
    virtual void TearDown()
    {
        if( listener != NULL ) {
            listener->close();
        }
    }
    
    virtual void update( void )
    {
        if( listener != NULL ) {
            for( int i = 0; i < 1000; i++ ) {
                listener->update();
            }
        }
    }
    
    u16                  port;
    TCPSocketListenerPtr listener;
    ListenerDelegate*    delegate;
};

u16 TCPListener::kPort = 50000;

class TCPListenerWithConnection : public TCPListener {
protected:
    
    virtual void SetUp() {
        TCPListener::SetUp();
        socket = TCPSocket::connectTo( NetworkAddress::Localhost, listener->port() );
        ASSERT_NE( socket, TCPSocketPtr() );
    }
    
    virtual void TearDown()
    {
        if( socket != NULL ) {
            socket->close();
        }
        
        TCPListener::TearDown();
    }
    
    virtual void update( void )
    {
        for( int i = 0; i < 1000; i++ ) {
            if( listener != NULL ) {
                listener->update();
            }
            if( socket != NULL ) {
                socket->update();
            }
        }
    }
    
    TCPSocketPtr socket;
};

TEST_F(TCPListener, Bind)
{

}

TEST_F(TCPListener, ReturnsValidPort)
{
    EXPECT_EQ( port, listener->port() );
}

TEST_F(TCPListener, ZeroConnectionsAfterCreation)
{
    EXPECT_EQ( 0, listener->connections().size() );
}

TEST_F(TCPListener, NullWhenPortIsUsed)
{
    TCPSocketListenerPtr ptr2 = TCPSocketListener::bindTo( port );
    ASSERT_EQ( ptr2, TCPSocketListenerPtr() );
}

TEST_F(TCPListener, FreesPortAfterClose)
{
    listener->close();
    
    TCPSocketListenerPtr ptr2 = TCPSocketListener::bindTo( port );
    ASSERT_NE( ptr2, TCPSocketListenerPtr() );
}

TEST_F(TCPListener, LotsOfBinds)
{
    listener->close();
    
    for( int i = 0; i < 1000; i++ ) {
        TCPSocketListenerPtr ptr = TCPSocketListener::bindTo( port );
        ASSERT_NE( ptr, TCPSocketListenerPtr() );
        ptr->close();
    }
}

TEST_F(TCPListener, FreesPortAfterDestruction)
{
    listener = TCPSocketListenerPtr();
    
    TCPSocketListenerPtr ptr2 = TCPSocketListener::bindTo( port );
    ASSERT_NE( ptr2, TCPSocketListenerPtr() );
}

TEST_F(TCPListenerWithConnection, AcceptsConnections)
{

}

TEST_F(TCPListenerWithConnection, AddsConnectionsToList)
{
    listener->update();
    EXPECT_NE( 0, listener->connections().size() );
}

TEST_F(TCPListenerWithConnection, RemovesClosedConnections)
{
    update();
    EXPECT_NE( 0, listener->connections().size() );
    
    socket->close();
    update();
    EXPECT_EQ( 0, listener->connections().size() );
}

TEST_F(TCPListenerWithConnection, TriggersAccepted)
{
    EXPECT_EQ( delegate->connectionAcceptedTriggered, false );
    update();
    EXPECT_EQ( delegate->connectionAcceptedTriggered, true );
}

TEST_F(TCPListenerWithConnection, TriggersClosed)
{
    EXPECT_EQ( delegate->connectionAcceptedTriggered, false );
    EXPECT_EQ( delegate->connectionClosedTriggered, false );
    update();
    EXPECT_EQ( delegate->connectionAcceptedTriggered, true );
    EXPECT_EQ( delegate->connectionClosedTriggered, false );
    
    socket->close();
    update();
    
    EXPECT_EQ( delegate->connectionClosedTriggered, true );
}

TEST_F(TCPListenerWithConnection, TriggersReceived)
{
    EXPECT_EQ( delegate->receivedDataTriggered, false );
    
    const char* data = "hello";
    socket->sendTo( data, strlen( data ) );
    
    update();
    
    EXPECT_EQ( delegate->receivedDataTriggered, true );
}

TEST_F(TCPListenerWithConnection, ReceivesData)
{
    EXPECT_EQ( delegate->receivedData.size(), 0 );
    
    const char* data = "hello";
    socket->sendTo( data, strlen( data ) );
    
    update();
    
    EXPECT_EQ( delegate->receivedData.size(), strlen( data ) );
    
    for( int i = 0; i < delegate->receivedData.size(); i++ ) {
        EXPECT_EQ( data[i], delegate->receivedData[i] );
    }
}