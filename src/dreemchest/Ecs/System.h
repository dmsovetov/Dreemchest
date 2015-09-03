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

#include "Ecs.h"

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

		//! System logic is done here.
		virtual void	update( u32 currentTime, f32 dt ) = 0;
	};

	//! A helper class to store systems.
	class Systems {
	public:

						//! Constructs Systems instance.
						Systems( Entities& entities )
							: m_entities( entities ) {}

		//! Updates all systems
		void			update( u32 currentTime, f32 dt );

		//! Adds a new system
		template<typename T>							StrongPtr<T> add( void );
		template<typename T, TemplateFunctionTypes1>	StrongPtr<T> add( TemplateFunctionArgs1 );
		template<typename T, TemplateFunctionTypes2>	StrongPtr<T> add( TemplateFunctionArgs2 );
		template<typename T, TemplateFunctionTypes3>	StrongPtr<T> add( TemplateFunctionArgs3 );
		template<typename T, TemplateFunctionTypes4>	StrongPtr<T> add( TemplateFunctionArgs4 );

		//! Removes a system
		template<typename T>
		void			remove( void );

		//! Returns a system.
		template<typename T>
		T*				get( void ) const;

	private:

		//! A container type to store systems.
		typedef Map<TypeIdx, SystemPtr>	ActiveSystems;

		//! Runtime entities.
		Entities&		m_entities;

		//! Active systems.
		ActiveSystems	m_systems;
	};

	// ** Systems::update
	inline void Systems::update( u32 currentTime, f32 dt )
	{
		for( ActiveSystems::iterator i = m_systems.begin(), end = m_systems.end(); i != end; ++i ) {
			i->second->update( currentTime, dt );
		}
	}

	// ** Systems::add
	template<typename T>
	inline StrongPtr<T> Systems::add( void )
	{
		T* system = new T( m_entities );
		m_systems[TypeIndex<T>::idx()] = SystemPtr( system );
		return StrongPtr<T>( system );
	}

	// ** Systems::add
	template<typename T, TemplateFunctionTypes1>
	inline StrongPtr<T> Systems::add( TemplateFunctionArgs1 )
	{
		T* system = new T( m_entities, arg0 );
		m_systems[TypeIndex<T>::idx()] = SystemPtr( system );
		return StrongPtr<T>( system );
	}

	// ** Systems::add
	template<typename T, TemplateFunctionTypes2>
	inline StrongPtr<T> Systems::add( TemplateFunctionArgs2 )
	{
		T* system = new T( m_entities, arg0, arg1 );
		m_systems[TypeIndex<T>::idx()] = SystemPtr( system );
		return StrongPtr<T>( system );
	}

	// ** Systems::add
	template<typename T, TemplateFunctionTypes3>
	inline StrongPtr<T> Systems::add( TemplateFunctionArgs3 )
	{
		T* system = new T( m_entities, arg0, arg1, arg2 );
		m_systems[TypeIndex<T>::idx()] = SystemPtr( system );
		return StrongPtr<T>( system );
	}

	// ** Systems::add
	template<typename T, TemplateFunctionTypes4>
	inline StrongPtr<T> Systems::add( TemplateFunctionArgs4 )
	{
		T* system = new T( m_entities, arg0, arg1, arg2, arg3 );
		m_systems[TypeIndex<T>::idx()] = SystemPtr( system );
		return StrongPtr<T>( system );
	}

	// ** Systems::remove
	template<typename T>
	inline void Systems::remove( void )
	{
		m_systems.erase( m_systems.find( TypeIndex<T>::idx() ) );
	}

	// ** Systems::get
	template<typename T>
	inline T* Systems::get( void ) const
	{
		ActiveSystems::const_iterator i = m_systems.find( TypeIndex<T>::idx() );

		if( i == m_systems.end() ) {
			return NULL;
		}

		return static_cast<T*>( i->second.get() );
	}

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_System_H__	*/
