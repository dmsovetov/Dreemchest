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

#ifndef __DC_Network_SocketResult_H__
#define __DC_Network_SocketResult_H__

#include "../Network.h"

DC_BEGIN_DREEMCHEST

namespace net {

    //! Socket error class wraps the standard return codes and helps to format error to a string.
    class SocketResult {
    public:

                    //! Constructs SocketResult instance.
                    SocketResult( s32 value );

                    //! Returns an actual value.
                    operator s32( void ) const;

        //! Returns true if the WOULDBLOCK/EAGAIN recorded.
        bool        wouldBlock( void ) const;

        //! Returns true if error was recorded.
        bool        isError( void ) const;

        //! Returns the error code.
        s32         errorCode( void ) const;

        //! Returns the formatted error message.
        String      errorMessage( void ) const;

    private:

        s32         m_value; //!< Value returned by a socket call.
    };

} // namespace net

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_SocketResult_H__	*/