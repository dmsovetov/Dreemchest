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

#ifndef __DC_Ecs_System_H__
#define __DC_Ecs_System_H__

#include "../Ecs.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! System is a base class for all systems that process components.
	/*!
	System contains all the code for the one aspect of the entities, with
	each System running continuously as if it has a private internal thread,
	performing global actions on every Entity that possesses a Component of
	the same aspect as that System.
	*/
	class System : public RefCounted {
	public:

		virtual			~System( void ) {}

		//! Returns system name.
		const String&	name( void ) const;

		//! Attaches the system instance to ecs.
		virtual bool	initialize( EcsWPtr ecs );

		//! System logic is done here.
		virtual void	update( u32 currentTime, f32 dt ) = 0;

	protected:

						//! Constructs System instance.
						System( const String& name );

	protected:

		EcsWPtr			m_ecs;	//!< Parent ECS instance.
		String			m_name;	//!< System name.
	};

	// ** System::System
	inline System::System( const String& name ) : m_name( name )
	{
	
	}

	// ** System::name
	inline const String& System::name( void ) const
	{
		return m_name;
	}

	// ** System::initialize
	inline bool System::initialize( EcsWPtr ecs )
	{
		m_ecs = ecs;
		return true;
	}

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_System_H__	*/