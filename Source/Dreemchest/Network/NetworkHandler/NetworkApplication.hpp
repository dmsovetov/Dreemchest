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

#ifndef __DC_Network_Application_Hpp__
#define __DC_Network_Application_Hpp__

DC_BEGIN_DREEMCHEST

namespace Network {
    
    // ** Application::registerEvent
    template<typename T>
    inline void Application::registerEvent( void )
    {
        m_eventHandlers[TypeInfo<T>::id()] = DC_NEW EventHandler<T>( &m_eventEmitter );
    }
    
    // ** Application::registerRemoteProcedureVoid
    template<typename TRemoteProcedure>
    inline void Application::registerRemoteProcedureVoid( const typename RemoteCallHandler<typename TRemoteProcedure::Argument, Void>::Callback& callback )
    {
        m_remoteCallHandlers[TRemoteProcedure::id()] = DC_NEW RemoteCallHandler<typename TRemoteProcedure::Argument, Void>( callback );
    }
    
    // ** NetworkHandler::registerRemoteProcedure
    template<typename TRemoteProcedure>
    inline void Application::registerRemoteProcedure( const typename RemoteCallHandler<typename TRemoteProcedure::Argument, typename TRemoteProcedure::Response>::Callback& callback )
    {
        m_remoteCallHandlers[TRemoteProcedure::id()] = DC_NEW RemoteCallHandler<typename TRemoteProcedure::Argument, typename TRemoteProcedure::Response>( callback );
    }
    
    // ** Application::emitTo
    template<typename T>
    inline void Application::emitTo( const T& e, const ConnectionList& listeners )
    {
        if( listeners.empty() ) {
            LogWarning( "rpc", "no listeners to listen for %s\n", TypeInfo<T>::name() );
            return;
        }
        
        // ** Serialize event to a byte buffer.
        Io::ByteBufferPtr buffer = Io::BinarySerializer::write( e );
        
        for( ConnectionList::const_iterator i = listeners.begin(), end = listeners.end(); i != end; ++i ) {
            const_cast<ConnectionPtr&>( *i )->send<Packets::Event>( TypeInfo<T>::id(), buffer->array() );
        }
    }
    
    // ** Application::addPacketHandler
    template<typename TPacketHandler>
    void Application::addPacketHandler( TPacketHandler* instance )
    {
        // Register the packet type inside a factory
        m_packetFactory.declare<typename TPacketHandler::Packet>( TypeInfo<typename TPacketHandler::Packet>::id() );
        
        // Add this packet handler instance to a list
        PacketTypeId type = instance->packetTypeId();
        m_packetHandlers[type].push_back( instance );
    }
    
#if DREEMCHEST_CPP11
    // ** Application::addPacketHandler
    template<typename TPacketHandler, typename ... TArgs>
    void Application::addPacketHandler( const TArgs& ... args )
    {
        addPacketHandler( DC_NEW TPacketHandler( args... ) );
    }
    
    // ** Application::emit
    template<typename TEvent, typename ... TArgs>
    void Application::emit( const TArgs& ... args )
    {
        TEvent e( args... );
        emitTo( e, eventListeners() );
    }
#endif  /*  #if DREEMCHEST_CPP11   */
    
} // namespace Network

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_Application_Hpp__	*/
