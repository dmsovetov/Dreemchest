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

#ifndef		__DC_Network_RemoteProcedure_H__
#define		__DC_Network_RemoteProcedure_H__

#include	"../Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

    // ** class RemoteProcedure
    class RemoteProcedure {

        typedef std::map<strhash, RemoteProcedureCallback>      RemoteProcedures;
        typedef std::map<int, RemoteProcedureResponseCallback>  RemoteProcedureCallbacks;

    public:
        
                                    RemoteProcedure( dcContext ctx, NetworkHandler *networkHandler );
                                    ~RemoteProcedure( void );

        bool                        InvokeRemoteProcedure( const char *name, const cValue *args, int count );
        bool                        InvokeRemoteProcedure( const char *name, const cValue *args, int count, RemoteProcedureResponseCallback callback );
        bool                        RegisterRemoteProcedure( const char *name, const RemoteProcedureCallback& callback );

    private:

        void                        OnPacketReceived( const dcEvent e );

        bool                        SendCallPacket( const char *name, const cValue *args, int count, int identifier = -1 );
        bool                        SendResponsePacket( const cValue& value, int identifier, int caller );
        bool                        InvokeLocalProcedure( const char *name, const cValue *args, int count, int identifier, int caller );
        bool                        InvokeLocalResponse( int identifier, const cValue& value );

    private:

        dcContext                   m_ctx;
        NetworkHandler*             m_networkHandler;
        RemoteProcedures            m_procedures;
        RemoteProcedureCallbacks    m_callbacks;
        int                         m_callbackId;
    };
    
} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_RemoteProcedure_H__	*/