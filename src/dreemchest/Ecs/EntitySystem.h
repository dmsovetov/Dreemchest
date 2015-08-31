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

#ifndef __DC_Ecs_EntitySystem_H__
#define __DC_Ecs_EntitySystem_H__

#include "System.h"
#include "Family.h"
#include "Entity.h"

DC_BEGIN_DREEMCHEST

namespace ecs {

	//! Entity system is used for processing entities.
	class EntitySystem : public System {
	public:

						//! Constructs a System instance.
						EntitySystem( Entities& entities, const String& name, const Aspect& aspect );

		//! System logic is done here.
		virtual void	update( u32 currentTime, f32 dt );

	protected:

		//! Called every update tick before processing entities.
		virtual bool	begin( u32 currentTime );

		//! Called every update tick after all entities are processed.
		virtual void	end( void );

		//! Processes a single entity.
		virtual void	process( u32 currentTime, f32 dt, EntityPtr& entity );

		//! Called when entity was added.
		virtual void	entityAdded( const EntityPtr& entity );

		//! Called when entity was removed.
		virtual void	entityRemoved( const EntityPtr& entity );

		//! Handles an entity added event.
		void			handleEntityAdded( const Family::Added& e );

		//! Handles an entity removed event.
		void			handleEntityRemoved( const Family::Removed& e );

	protected:

		//! Runtime entities.
		Entities&		m_entities;

		//! Entity family used by this system.
		FamilyPtr		m_family;
	};

	//! A template class to process entities that contain all components from a specified set.
	template<typename TComponent>
	class EntityWithComponentsSystem : public EntitySystem {
	public:

						//! Constructs EntityWithComponentsSystem instance.
						EntityWithComponentsSystem( Entities& entities, const String& name )
							: EntitySystem( entities, name, Aspect::all<TComponent>() ) {}

	protected:

		//! Extracts a specified components from entity and runs a processing callback.
		virtual void	process( u32 currentTime, f32 dt, EntityPtr& entity )
		{
			TComponent* c = entity->get<TComponent>();
			process( currentTime, dt, entity, *c );
		}

		//! Strong typed process method.
		virtual void	process( u32 currentTime, f32 dt, EntityPtr& entity, TComponent& component )
		{
			DC_BREAK
		}
	};

	//! A template class to process entities that contain all components from a specified set.
	template<typename TComponent1, typename TComponent2>
	class EntityWithComponentsSystem2 : public EntitySystem {
	public:

						//! Constructs EntityWithComponentsSystem2 instance.
						EntityWithComponentsSystem2( Entities& entities, const String& name )
							: EntitySystem( entities, name, Aspect::all<TComponent1, TComponent2>() ) {}

	protected:

		//! Extracts a specified components from entity and runs a processing callback.
		virtual void	process( u32 currentTime, f32 dt, EntityPtr& entity )
		{
			TComponent1* c1 = entity->get<TComponent1>();
			TComponent2* c2 = entity->get<TComponent2>();
			process( currentTime, dt, entity, *c1, *c2 );
		}

		//! Strong typed process method.
		virtual void	process( u32 currentTime, f32 dt, EntityPtr& entity, TComponent1& component1, TComponent2& component2 )
		{
			DC_BREAK
		}
	};

	//! A template class to process entities that contain all components from a specified set.
	template<typename TComponent1, typename TComponent2, typename TComponent3>
	class EntityWithComponentsSystem3 : public EntitySystem {
	public:

						//! Constructs EntityWithComponentsSystem3 instance.
						EntityWithComponentsSystem3( Entities& entities, const String& name )
							: EntitySystem( entities, name, Aspect::all<TComponent1, TComponent2, TComponent3>() ) {}

	protected:

		//! Extracts a specified components from entity and runs a processing callback.
		virtual void	process( u32 currentTime, f32 dt, EntityPtr& entity )
		{
			TComponent1* c1 = entity->get<TComponent1>();
			TComponent2* c2 = entity->get<TComponent2>();
			TComponent3* c3 = entity->get<TComponent3>();
			process( currentTime, dt, entity, *c1, *c2, *c3 );
		}

		//! Strong typed process method.
		virtual void	process( u32 currentTime, f32 dt, EntityPtr& entity, TComponent1& component1, TComponent2& component2, TComponent3& component3 )
		{
			DC_BREAK
		}
	};

} // namespace ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_EntitySystem_H__	*/
