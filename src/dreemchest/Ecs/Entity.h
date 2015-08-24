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

#ifndef __DC_Ecs_Entity_H__
#define __DC_Ecs_Entity_H__

#include "Ecs.h"
#include "Entities.h"
#include "Component.h"

DC_BEGIN_DREEMCHEST

#define OverrideEntity( Type, Super )		\
    ClassEnableTypeInfoSuper( Type, Super )	\

namespace ecs {

	//! Entity handle contains an entity id & parent world.
	/*!
	Entity is just a unique ID that tags each game-object as a separate
	item. Entity should not contain any processing logic or object-specific
	game data. Entity's behaviour in a game world is defined by a set of
	data components.
	*/
	class Entity : public RefCounted {
	public:

								ClassEnableTypeInfo( Entity )

		virtual					~Entity( void );

		//! Returns true if this is a valid Entity, e.g. it has a generated identifier.
		bool					isValid( void ) const;

		//! Returns an entity identifier.
		const EntityId&			id( void ) const;

		//! Returns a component mask.
		const Bitset&			mask( void ) const;

		//! Returns true if entity has a component of given type.
		template<typename TComponent>
		bool					has( void ) const;

		//! Resturns a entity's component by type.
		template<typename TComponent>
		TComponent*				get( void ) const;

		//! Adds a new component to this entity.
		template<typename TComponent>
		TComponent*				attach( TComponent* component );

		//! Adds a new component to this entity.
		template<typename TComponent>						  TComponent* attach( void );

		//! Adds a new component to this entity.
		template<typename TComponent, TemplateFunctionTypes1> TComponent* attach( TemplateFunctionArgs1 );

		//! Adds a new component to this entity.
		template<typename TComponent, TemplateFunctionTypes2> TComponent* attach( TemplateFunctionArgs2 );

		//! Adds a new component to this entity.
		template<typename TComponent, TemplateFunctionTypes3> TComponent* attach( TemplateFunctionArgs3 );

		//! Adds a new component to this entity.
		template<typename TComponent, TemplateFunctionTypes4> TComponent* attach( TemplateFunctionArgs4 );

		//! Removes a component from this entity.
		/*!
		\param component A component to be removed.
		\return Returns true if component has been successfully detached, otherwise false.
		*/
		template<typename TComponent>
		void					detach();

		//! Removes this entity from the world and all systems.
		void					destroy( void );

		//! Creates a new Entity instance.
		static EntityPtr		create( Entities& entities, const EntityId& id );

		//! Returns a total number of active handles.
		static u32				activeCount( void );

	protected:

								//! Constructs Entity instance.
								Entity( Entities& entities, const EntityId& id = EntityId() );

	private:

		//! The total number of entity handles alive.
		static u32				s_aliveCount;

		//! Parent entity manager.
		Entities&				m_entities;

		//! Entity identifier.
		EntityId				m_id;

		//! Component mask.
		Bitset					m_mask;
	};

	// ** Entity::has
	template<typename TComponent>
	inline bool Entity::has( void ) const
	{
		bool result = m_entities.findComponent( m_id, TypeIndex<TComponent>::idx() ) != ComponentPtr();
		return result;
	}

	// ** Entity::get
	template<typename TComponent>
	inline TComponent* Entity::get( void ) const
	{
		ComponentPtr component = m_entities.findComponent( m_id, TypeIndex<TComponent>::idx() );
		if( component == ComponentPtr() ) {
			return NULL;
		}

        TComponent* result = castTo<TComponent>( component.get() );
		assert( result != NULL );
		return result;
	}

	// ** Entity::detach
	template<typename TComponent>
	inline void Entity::detach( void )
	{
		m_mask.clear( TypeIndex<TComponent>::idx() );
		m_entities.removeComponent( m_id, TypeIndex<TComponent>::idx() );
	}

	// ** Entity::attach
	template<typename TComponent>
	inline TComponent* Entity::attach( TComponent* component )
	{
		m_mask.set( component->typeIndex() );
		m_entities.addComponent( m_id, ComponentPtr( component ), component->typeIndex() );
		return component;
	}

	// ** Entity::attach
	template<typename TComponent>
	inline TComponent* Entity::attach( void )
	{
		return attach<TComponent>( new TComponent );
	}

	// ** Entity::attach
	template<typename TComponent, TemplateFunctionTypes1>
	inline TComponent* Entity::attach( TemplateFunctionArgs1 )
	{
		return attach<TComponent>( new TComponent( arg0 ) );
	}

	// ** Entity::attach
	template<typename TComponent, TemplateFunctionTypes2>
	inline TComponent* Entity::attach( TemplateFunctionArgs2 )
	{
		return attach<TComponent>( new TComponent( arg0, arg1 ) );
	}

	// ** Entity::attach
	template<typename TComponent, TemplateFunctionTypes3>
	inline TComponent* Entity::attach( TemplateFunctionArgs3 )
	{
		return attach<TComponent>( new TComponent( arg0, arg1, arg2 ) );
	}

	// ** Entity::attach
	template<typename TComponent, TemplateFunctionTypes4>
	inline TComponent* Entity::attach( TemplateFunctionArgs4 )
	{
		return attach<TComponent>( new TComponent( arg0, arg1, arg2, arg3 ) );
	}

} // namespace ecs

DC_END_DREEMCHEST

#endif	/*	! __DC_Ecs_Entity_H__	*/
