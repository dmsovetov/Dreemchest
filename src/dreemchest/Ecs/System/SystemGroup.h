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

#ifndef __DC_Ecs_SystemGroup_H__
#define __DC_Ecs_SystemGroup_H__

#include "System.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! System group is a collection of system instances that processed one by one.
	class SystemGroup : public RefCounted {
	friend class Ecs;
	public:

		virtual				~SystemGroup( void ) {}

		//! Returns a system group mask.
		u32					mask( void ) const;

		//! Update all systems in a group.
		void				update( u32 currentTime, f32 dt );

		//! Adds a new system to a group.
		template<typename TSystem, typename ... Args>
		WeakPtr<TSystem>	add( Args ... args );

		//! Removes a system from a group.
		template<typename TSystem>
		void				remove( void );

		//! Returns a system by type.
		template<typename TSystem>
		WeakPtr<TSystem>	get( void ) const;

	private:

							//! Constructs SystemGroup instance.
							SystemGroup( EcsWPtr ecs, const String& name, u32 mask );

	private:

		//! This struct holds one system & it's id.
		struct Item {
			TypeIdx			m_idx;		//!< System type index.
			SystemPtr		m_system;	//!< Actual system instance.

							//! Constructs Item instance.
							Item( TypeIdx idx = 0, SystemPtr system = SystemPtr() )
								: m_idx( idx ), m_system( system ) {}

			//! Returns true if the item id matches the specified one.
			bool			operator == ( TypeIdx idx ) const { return m_idx == idx; }
		};

		EcsWPtr				m_ecs;		//!< Parent ECS instance.
		String				m_name;		//!< System group name.
		u32					m_mask;		//!< System group mask.
		Array<Item>			m_systems;	//!< Active systems.
		bool				m_isLocked;	//!< System group is locked inside the update loop.
	};

	// ** SystemGroup::add
	template<typename TSystem, typename ... Args>
	WeakPtr<TSystem> SystemGroup::add( Args ... args )
	{
		DC_BREAK_IF( m_isLocked );
		DC_BREAK_IF( get<TSystem>().valid() );

		StrongPtr<TSystem> system = DC_NEW TSystem( args... );

		if( !system->initialize( m_ecs ) ) {
			return WeakPtr<TSystem>();
		}

		m_systems.push_back( Item( TypeIndex<TSystem>::idx(), system ) );
		return WeakPtr<TSystem>( system );
	}

	// ** SystemGroup::remove
	template<typename TSystem>
	void SystemGroup::remove( void )
	{
		DC_BREAK_IF( m_isLocked );
		DC_BREAK_IF( !get<TSystem>().valid() );
		m_systems.erase( TypeIndex<TSystem>::idx() );
	}

	//! Returns a system by type.
	template<typename TSystem>
	WeakPtr<TSystem> SystemGroup::get( void ) const
	{
		Array<Item>::const_iterator i = std::find( m_systems.begin(), m_systems.end(), TypeIndex<TSystem>::idx() );
		return i != m_systems.end() ? static_cast<TSystem*>( i->m_system.get() ) : WeakPtr<TSystem>();
	}

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_SystemGroup_H__	*/