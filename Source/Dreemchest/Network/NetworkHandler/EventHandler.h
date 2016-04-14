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

#ifndef __DC_Network_EventHandler_H__
#define __DC_Network_EventHandler_H__

#include "../Packets/Packet.h"

DC_BEGIN_DREEMCHEST

namespace Network {

	//! Event handler interface class.
	class IEventHandler {
	public:

		virtual			~IEventHandler( void ) {}

		//! Packet handler callback.
		virtual bool	handle( ConnectionWPtr connection, const Packets::Event& packet ) = 0;
	};

	//! Template class that handles an Event packet and emits the local event.
	template<typename T>
	class EventHandler : public IEventHandler {
	public:

								//! Constructs EventHandler instance.
								EventHandler( EventEmitter* eventEmitter )
									: m_eventEmitter( eventEmitter ) {}

		//! Reads a payload from an Event packet and emits it as local event.
		virtual bool handle( ConnectionWPtr connection, const Packets::Event& packet );

	private:

		//! Parent event emitter.
		EventEmitter*			m_eventEmitter;
	};

	// ** EventHandler::handle
	template<typename T>
	inline bool EventHandler<T>::handle( ConnectionWPtr connection, const Packets::Event& packet )
	{
    #if DEV_DEPRECATED_SERIALIZATION
        T event = Io::BinarySerializer::read<T>( packet.payload );
    #else
        T event = Private::readFromStream<T>( Io::ByteBuffer::createFromArray( packet.payload ) );
    #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */
        m_eventEmitter->notify( event );
		return true;
	}

} // namespace Network

DC_END_DREEMCHEST

#endif	/*	!__DC_Network_EventHandler_H__	*/