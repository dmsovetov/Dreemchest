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

#ifndef __DC_Network_Socket_H__
#define __DC_Network_Socket_H__

#include "SocketDescriptor.h"

DC_BEGIN_DREEMCHEST

namespace Network {

    //! Base class for different socket types.
    class Socket : public InjectEventEmitter<RefCounted> {
    public:

                                //! Constructs the Socket instance.
                                Socket( SocketDescriptor& descriptor = SocketDescriptor::Invalid );
        virtual                 ~Socket( void ) {}

        //! Returns a socket descriptor.
		const SocketDescriptor& descriptor( void ) const;

        //! Returns true if this socket is valid.
		bool				    isValid( void ) const;

        //! Queues this socket for removal.
        void                    closeLater( void );

        //! Closes a socket.
        virtual void		    close( void );

        //! Reads all incoming data.
        virtual void		    recv( void ) = 0;

    protected:

        SocketDescriptor        m_descriptor;   //!< Socket descriptor.
        Io::ByteBufferPtr       m_data;         //!< Socket receiving buffer.
        bool                    m_shouldClose;  //!< Indicates that a socket should be closed.
    };

} // namespace Network

DC_END_DREEMCHEST

#endif  /*  __DC_Network_Socket_H__ */