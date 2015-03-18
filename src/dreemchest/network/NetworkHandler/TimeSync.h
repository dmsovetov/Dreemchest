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

#ifndef		__DC_Network_TimeSync_H__
#define		__DC_Network_TimeSync_H__

#include	"../Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

    // ** class TimeSync
    class TimeSync {

        // ** struct sTimeSnapshot
        struct sTimeShapshot {
            int             m_latency;
            int             m_local;
            int             m_remote;

                            sTimeShapshot( void )
                                : m_latency( INT_MAX ), m_local( INT_MAX ), m_remote( INT_MAX ) {}
        };

    public:

                            TimeSync( void );
                            ~TimeSync( void );

        bool                IsReady( void ) const;
    //    void                SyncTo( int *remoteTime, dcTime time, NetworkHandler *local, int iterations, int connection );

    private:

        void                SendPing( int timestamp );
        void                OnTimeSynchronized( int localTime );
    //    void                OnPacketReceived( const dcEvent e );

    private:

        NetworkHandler*     m_local;
    //    dcTime              m_localTime;
        int*                m_remoteTime;
        bool                m_isReady;
        bool                m_isRunning;

        sTimeShapshot       m_bestTime;
        int                 m_iterations;
    };
    
} // namespace net
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Network_NetworkTime_H__	*/