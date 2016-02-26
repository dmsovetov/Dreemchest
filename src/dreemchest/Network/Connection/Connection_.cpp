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

#include "Connection_.h"

DC_BEGIN_DREEMCHEST

namespace Network {

// ** Connection::Connection
Connection_::Connection_( void )
    : m_totalBytesReceived( 0 )
    , m_totalBytesSent( 0 )
{
}

// ** Connection::close
void Connection_::close( void )
{
}

// ** Connection::trackReceivedAmount
void Connection_::trackReceivedAmount( s32 value )
{
    m_totalBytesReceived += value;
}

// **  Connection::trackSentAmount
void Connection_::trackSentAmount( s32 value )
{
    m_totalBytesSent += value;
}

// ** Connection::totalBytesReceived
s32 Connection_::totalBytesReceived( void ) const
{
	return m_totalBytesReceived;
}

// ** Connection::totalBytesSent
s32 Connection_::totalBytesSent( void ) const
{
	return m_totalBytesSent;
}

} // namespace Network

DC_END_DREEMCHEST
