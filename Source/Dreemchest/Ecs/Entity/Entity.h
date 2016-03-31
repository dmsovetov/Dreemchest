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
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CALIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#ifndef __DC_Ecs_Entity_H__
#define __DC_Ecs_Entity_H__

#include "../Component/Component.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {


#if DEV_DEPRECATED_SERIALIZATION
    typedef Io::Serializable EntitySuperClass;
#else
    typedef RefCounted EntitySuperClass;
#endif  /*  #if !DEV_DEPRECATED_SERIALIZATION    */

	//! Entity handle contains an entity id & parent world.
	/*!
	Entity is just a unique ID that tags each game-object as a separate
	item. Entity should not contain any processing logic or object-specific
	game data. Entity's behaviour in a game world is defined by a set of
	data components.
	*/
	class Entity : public EntitySuperClass {
	friend class Ecs;

        INTROSPECTION( Entity
            , PROPERTY( flags, flags, setFlags, "The entity flags." )
            )

	public:

		//! Available entity flags
		enum Flags {
			  Disabled	    = BIT( 0 )	//!< Marks this entity as disabled.
			, Removed	    = BIT( 1 )  //!< Marks this entity as removed.
            , Serializable  = BIT( 2 )  //!< Marks this entity as serializable.
			, TotalFlags    = 3		    //!< Total number of entity flags.
		};

                            #if DEV_DEPRECATED_SERIALIZATION
								ClassEnableTypeInfoSuper( Entity, Io::Serializable )
                            #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

		//! Container type to store components.
		typedef Map<TypeIdx, ComponentPtr> Components;

		//! Returns an entity identifier.
		const EntityId&			id( void ) const;

		//! Returns a component mask.
		const Bitset&			mask( void ) const;

        //! Removes all attached components.
        void                    clear( void );

		//! Returns entity flags.
		u8						flags( void ) const;

		//! Sets entity flags.
		void					setFlags( u8 value );

        //! Returns true if this entity is serializable.
        bool                    isSerializable( void ) const;

        //! Sets entity's serializable flag.
        void                    setSerializable( bool value );

		//! Returns entity components.
		const Components&		components( void ) const;
		Components&				components( void );

		//! Returns parent entity component system.
		EcsWPtr					ecs( void ) const;

		//! Returns true if entity has a component of given type.
		template<typename TComponent>
		TComponent*				has( void ) const;

		//! Resturns an entity's component by type.
		template<typename TComponent>
		TComponent*				get( void ) const;

		//! Enables the component of specified type.
		template<typename TComponent>
		void					enable( void );

		//! Disables the component of specified type.
		template<typename TComponent>
		void					disable( void );

		//! Enables of disables the component of specified type.
		template<typename TComponent>
		void					setComponentEnabled( bool value );

		//! Returns true if the component of specified type is enabled.
		template<typename TComponent>
		bool					isEnabled( void ) const;

		//! Attaches the created component to an entity.
		template<typename TComponent>
		TComponent*				attachComponent( TComponent* component );

    #if DC_ECS_ENTITY_CLONING
        //! Copies component from a specified entity and attaches it.
        template<typename TComponent>
        TComponent*             attachFrom( EntityWPtr entity );

        //! Makes a full copy of this entity.
        virtual EntityPtr       deepCopy( const EntityId& id = EntityId() ) const;
    #endif  /*  DC_ECS_ENTITY_CLONING   */

    #ifndef DC_ECS_NO_SERIALIZATION

    #if DEV_DEPRECATED_SERIALIZATION
		//! Reads archetype from a storage.
		virtual void		    read( const Io::Storage* storage ) DC_DECL_OVERRIDE;

		//! Writes archetype to a storage.
		virtual void		    write( Io::Storage* storage ) const DC_DECL_OVERRIDE;

		//! Writes this entity to a key-value archive.
		virtual void            serialize( SerializationContext& ctx, Archive& ar ) const;

		//! Reads this entity from a key-value archive.
		virtual void		    deserialize( SerializationContext& ctx, const Archive& ar );
    #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

    #endif  /*  !DC_ECS_NO_SERIALIZATION    */

	#ifndef DC_CPP11_DISABLED
		//! Constructs a new component and attaches it to this entity.
		template<typename TComponent, typename ... Args>
		TComponent*				attach( Args ... args );
	#endif	/*	!DC_CPP11_DISABLED	*/

        //! Removes a component by type id from this entity.
        void                    detachById( TypeIdx id );

		//! Removes a component from this entity.
		template<typename TComponent>
		void					detach( void );

		//! Queues removal of this entity from the world and all systems.
		void					queueRemoval( void );

	protected:

								//! Constructs Entity instance.
								Entity( void );

		//! Marks this entity as queued for removal.
		void					markAsRemoved( void );

		//! Sets the entity id.
		void					setId( const EntityId& value );

		//! Sets the parent entity component system reference.
		void					setEcs( EcsWPtr value );

		//! Updates the entity component mask.
		void					updateComponentBit( u32 bit, bool value );

	private:

		EcsWPtr					m_ecs;			//!< Parent ECS instance.
		EntityId				m_id;			//!< Entity identifier.
		Components				m_components;	//!< Attached components.
		Bitset					m_mask;			//!< Component mask.
		FlagSet8				m_flags;		//!< Entity flags.
	};

	// ** Entity::has
	template<typename TComponent>
	TComponent* Entity::has( void ) const
	{
	//	DC_BREAK_IF( m_flags.is( Removed ) );
		Components::const_iterator i = m_components.find( ComponentBase::typeId<TComponent>() );
		return i == m_components.end() ? NULL : static_cast<TComponent*>( i->second.get() );
	}

	// ** Entity::get
	template<typename TComponent>
	TComponent* Entity::get( void ) const
	{
	//	DC_BREAK_IF( m_flags.is( Removed ) );

		TypeIdx idx = ComponentBase::typeId<TComponent>();
		Components::const_iterator i = m_components.find( idx );
		DC_ABORT_IF( i == m_components.end(), "the specified component does not exist" );

    #if DEV_DEPRECATED_SERIALIZATION
        TComponent* result = castTo<TComponent>( i->second.get() );
		DC_ABORT_IF( result == NULL, "component type mismatch" );
    #else
        TComponent* result = static_cast<TComponent*>( i->second.get() );
    #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

		return result;
	}

	// ** Entity::attachComponent
	template<typename TComponent>
	TComponent* Entity::attachComponent( TComponent* component )
	{
		DC_BREAK_IF( m_flags.is( Removed ), "this entity was removed" );
		DC_ABORT_IF( has<TComponent>(), "entity already has this component" );

		TypeIdx idx = component->typeIndex();

		m_components[idx] = component;
		updateComponentBit( idx, true );

        component->setParentEntity( this );
		
		return component;	
	}

    // ** Entity::attachFrom
    template<typename TComponent>
    TComponent* Entity::attachFrom( EntityWPtr entity )
    {
        if( TComponent* component = entity->has<TComponent>() ) {
            return attachComponent<TComponent>( static_cast<TComponent*>( component->deepCopy().get() ) );
        }

        return NULL;
    }

	// ** Entity::enable
	template<typename TComponent>
	void Entity::enable( void )
	{
		setComponentEnabled<TComponent>( true );
	}

	// ** Entity::disable
	template<typename TComponent>
	void Entity::disable( void )
	{
		setComponentEnabled<TComponent>( false );	
	}

	// ** Entity::setComponentEnabled
	template<typename TComponent>
	void Entity::setComponentEnabled( bool value )
	{
		TComponent* component = get<TComponent>();
		component->setEnabled( value );
		updateComponentBit( component->typeIndex(), value );		
	}

	// ** Entity::isEnabled
	template<typename TComponent>
	bool Entity::isEnabled( void ) const
	{
		return get<TComponent>()->isEnabled();
	}

	// ** Entity::attach
#ifndef DC_CPP11_DISABLED
	template<typename TComponent, typename ... Args>
	TComponent* Entity::attach( Args ... args )
	{
		TComponent* component = m_ecs->createComponent<TComponent>( args... );
		return attachComponent<TComponent>( component );
	}
#endif	/*	!DC_CPP11_DISABLED	*/

	// ** Entity::detach
	template<typename TComponent>
	void Entity::detach( void )
	{
        detachById( ComponentBase::typeId<TComponent>() );
	}

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	! __DC_Ecs_Entity_H__	*/