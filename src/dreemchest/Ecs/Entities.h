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

#ifndef __DC_Ecs_Entities_H__
#define __DC_Ecs_Entities_H__

#include "Ecs.h"

DC_BEGIN_DREEMCHEST

namespace ecs {

	//! Contains all active entities.
	class Entities {
	friend class Entity;
	friend class Archetype;
	public:

		//! Returns an entity handle by id.
		EntityPtr		operator[]( const EntityId& id ) const;

		//! Registeres an Entity instance with a given id.
		EntityPtr		registerEntity( const EntityId& id );

		//! Adds a new Entity instance.
		bool			addEntity( const EntityPtr& entity );

		//! Returns a handle for specified entity, if entity does not exist - creates it.
		EntityPtr		requestEntity( const EntityId& id );

		//! Finds an entity by id.
		EntityPtr		findEntity( const EntityId& id ) const;

		//! Finds entities by an aspect.
		EntitySet		findByAspect( const Aspect& aspect ) const;

		//! Returns a set of component for specified entity.
		ComponentSet	entityComponents( const EntityId& id ) const;

		//! Constructs archetype instance from an entity id.
		template<typename T>
		StrongPtr<T>	archetype( const EntityId& id ) const;

		//! Subscribes for events.
		template<typename T>
		void			subscribe( const typename EventEmitter::Callback<T>::Type& callback );

		//! Unubscribes from events.
		template<typename T>
		void			unsubscribe( const typename EventEmitter::Callback<T>::Type& callback );

		//! Entity changed event.
		struct Changed {
							//! Constructs Changed instance.
							Changed( const EntityPtr& entity )
								: m_entity( entity ) {}
			EntityPtr		m_entity;	//! Changed entity.
		};

		//! New entity has been added to world.
		struct Added {
							//! Constructs Added instance.
							Added( const EntityPtr& entity )
								: m_entity( entity ) {}
			EntityPtr		m_entity;	//! Added entity.
		};

		//! New entity has been added to world.
		struct Removed {
							//! Constructs Removed instance.
							Removed( const EntityPtr& entity )
								: m_entity( entity ) {}
			EntityPtr		m_entity;	//! Removed entity.
		};

	private:

		//! Adds a component to an entity.
		void			addComponent( const EntityId& id, const ComponentPtr& component, TypeIdx type );

		//! Removes a component from an entity.
		void			removeComponent( const EntityId& id, TypeIdx type );

		//! Finds an entity's component.
		ComponentPtr	findComponent( const EntityId& id, TypeIdx type );

		//! Removes an entity from a world.
		void			remove( const EntityId& id );

	private:

		//! Entity container key.
		struct EntityKey {
							//! Constructs an EntityKey instance.
							EntityKey( const EntityPtr& entity );
							//! Constructs an EntityKey instance.
							EntityKey( const EntityId& id );

			//! Compares two keys.
			bool			operator < ( const EntityKey& other ) const;
						
			const EntityId&	m_entityId;	//!< Entity.
			EntityPtr		m_entity;	//!< Entity handle.
		};

		//! Container type to store all components attached to entity.
		typedef Map<TypeIdx, ComponentPtr> ComponentsByType;

		//! Container type to store all active entities.
		typedef Map<EntityKey, ComponentsByType> ActiveEntities;

		//! Active entities.
		ActiveEntities	m_entities;

		//! Entity event emitter.
		EventEmitter	m_eventEmitter;
	};

	// ** Entities::archetype
	template<typename T>
	inline StrongPtr<T> Entities::archetype( const EntityId& id ) const
	{
		const EntityPtr& entity = findEntity( id );
		return entity != EntityPtr() ? T::create( entity ) : StrongPtr<T>();
	}

	// ** Entities::subscribe
	template<typename T>
	inline void Entities::subscribe( const typename EventEmitter::Callback<T>::Type& callback )
	{
		m_eventEmitter.subscribe<T>( callback );
	}

	// ** Entities::unsubscribe
	template<typename T>
	inline void Entities::unsubscribe( const typename EventEmitter::Callback<T>::Type& callback )
	{
		m_eventEmitter.unsubscribe<T>( callback );
	}

} // namespace ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_EntityManager_H__	*/
