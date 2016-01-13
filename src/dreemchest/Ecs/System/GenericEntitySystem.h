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

#ifndef __DC_Ecs_GenericEntitySystem_H__
#define __DC_Ecs_GenericEntitySystem_H__

#include "EntitySystem.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

#ifndef DC_CPP11_DISABLED

	//! Generic entity system to process entities that contain all components from a specified set.
	template<typename TSystem, typename ... TComponents>
	class GenericEntitySystem : public EntitySystem {
	public:

						//! Constructs GenericEntitySystem instance.
						GenericEntitySystem( const String& name = TypeInfo<TSystem>::name() )
							: EntitySystem( name, Aspect::all<TComponents...>() ) {}

	protected:

		//! Component types.
		typedef std::tuple<TComponents...> Types;

		//! Tuple indices
		typedef IndexTupleBuilder<sizeof...(TComponents)> Indices;

		//! Performs an update of a system
		virtual void	update( u32 currentTime, f32 dt ) DC_DECL_OVERRIDE;

		//! Called when entity was added.
		virtual void	entityAdded( const Entity& entity ) DC_DECL_OVERRIDE;

		//! Generic processing function that should be overriden in a subclass.
		virtual void	process( u32 currentTime, f32 dt, Entity& entity, TComponents& ... components );

		//! Called when entity was added.
        virtual void	entityAdded( const Entity& entity, TComponents& ... components ) {}

	private:

		//! Dispatches the entity components to processing
		template<s32 ... Idxs> 
		void dispatchProcess( u32 currentTime, f32 dt, Entity& entity, IndexesTuple<Idxs...> const& )  
		{ 
			process( currentTime, dt, entity, *entity.get<typename std::tuple_element<Idxs, Types>::type>()... );
		}

		//! Calls entityAdded method with components
		template<s32 ... Idxs> 
		void dispatchEntityAdded( const Entity& entity, IndexesTuple<Idxs...> const& )  
		{ 
			entityAdded( entity, *entity.get<typename std::tuple_element<Idxs, Types>::type>()... );
		}
	};

	// ** GenericEntitySystem::update
	template<typename TSystem, typename ... TComponents>
	void GenericEntitySystem<TSystem, TComponents...>::update( u32 currentTime, f32 dt )
	{
		if( !begin( currentTime ) ) {
			return;
		}

		EntitySet& entities = m_index->entities();

		for( EntitySet::iterator i = entities.begin(), n = entities.end(); i != n; ++i ) {
			dispatchProcess( currentTime, dt, *i->get(), typename Indices::Indexes() );
		}

		end();	
	}

	// ** GenericEntitySystem::entityAdded
	template<typename TSystem, typename ... TComponents>
	void GenericEntitySystem<TSystem, TComponents...>::entityAdded( const Entity& entity )
	{
		dispatchEntityAdded( entity, typename Indices::Indexes() );
	}

	// ** GenericEntitySystem::process
	template<typename TSystem, typename ... TComponents>
	void GenericEntitySystem<TSystem, TComponents...>::process( u32 currentTime, f32 dt, Entity& entity, TComponents& ... components )
	{
		DC_BREAK
	}

#endif	/*	!DC_CPP11_DISABLED	*/

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_GenericEntitySystem_H__	*/