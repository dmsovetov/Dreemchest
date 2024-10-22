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

#ifndef    __DC_Network_RemoteCallHandler_H__
#define    __DC_Network_RemoteCallHandler_H__

#include "../Packets/RemoteCall.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Remote procedure call handler interface class.
    class IRemoteCallHandler {
    public:

        virtual            ~IRemoteCallHandler( void ) {}

        //! Packet handler callback.
        virtual void    handle( ConnectionWPtr connection, const Packets::RemoteCall& packet ) = 0;
    };

    //! Remote call response object.
    template<typename T>
    class Response {
    public:

        //! Constructs a Response instance.
        Response( const ConnectionWPtr& connection, u16 id ) 
            : m_connection( connection ), m_id( id ), m_wasSent( false ) {}

        bool wasSent( void ) const { return m_wasSent; }

        //! Send a response to caller.
        bool operator()( const T& value, const Error& error = Error() );

    private:

        //! Parent connection.
        ConnectionWPtr    m_connection;

        //! Remote call identifier.
        u16                m_id;

        //! Flag indicating that a response was sent.
        bool            m_wasSent;
    };

    //! Template class that handles a RemoteCall packet and invokes a local procedure.
    template<typename T, typename R>
    class RemoteCallHandler : public IRemoteCallHandler {
    public:

        //! Response type.
        typedef Response<R> ResponseType;

        //! Function type to handle remote calls.
        typedef cClosure<void(ConnectionWPtr,ResponseType&,const T&)> Callback;

                        //! Constructs RemoteCallHandler instance.
                        RemoteCallHandler( const Callback& callback )
                            : m_callback( callback ) {}

        //! Reads a payload from an Event packet and emits it as local event.
        virtual void    handle( ConnectionWPtr connection, const Packets::RemoteCall& packet ) NIMBLE_OVERRIDE;

    private:

        //! Local procedure
        Callback        m_callback;
    };


    //! Remote response handler interface.
    class IRemoteResponseHandler {
    public:

        virtual            ~IRemoteResponseHandler( void ) {}

        virtual void    handle( ConnectionWPtr connection, const Packets::RemoteCallResponse& packet ) = 0;
    };

    //! Template class that handles a RemoteCallResponse packet and invokes a local procedure.
    template<typename T>
    class RemoteResponseHandler : public IRemoteResponseHandler {
    public:

        //! Function type to handle remote calls.
        typedef cClosure<bool(ConnectionWPtr,const Error&,const T&)> Callback;

                        //! Constructs RemoteResponseHandler instance.
                        RemoteResponseHandler( const Callback& callback )
                            : m_callback( callback ) {}

        //! Reads a payload from an Event packet and emits it as local event.
        virtual void    handle( ConnectionWPtr connection, const Packets::RemoteCallResponse& packet ) NIMBLE_OVERRIDE;

    private:

        //! Local procedure
        Callback        m_callback;
    };

} // namespace Network
    
DC_END_DREEMCHEST

#endif    /*    !__DC_Network_RemoteCallHandler_H__    */
