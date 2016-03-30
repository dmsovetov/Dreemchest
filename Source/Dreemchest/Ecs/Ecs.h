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

#ifndef __DC_Ecs_H__
#define __DC_Ecs_H__

#include "../Dreemchest.h"

#include <Io/KeyValue.h>
#include <Io/serialization/Serializable.h>
#include <Reflection/MetaObject/Class.h>

#define DC_ECS_ITERATIVE_INDEX_REBUILD  (1) // Enable to rebuild indicies after each system update
#define DC_ECS_ENTITY_CLONING           (1) // Enables cloning entities with deepCopy method

DC_BEGIN_DREEMCHEST

namespace Ecs {

    NIMBLE_LOGGER_TAG( Ecs )

	class Aspect;

	//! Entity id type.
#if DC_ECS_INTEGER_IDS
	typedef u32 EntityId;
#else
	typedef Guid EntityId;
#endif

	dcDeclarePtrs( Ecs )
	dcDeclarePtrs( EntityIdGenerator )
	dcDeclarePtrs( Entity )
	dcDeclareNamedPtrs( ComponentBase, Component )
	dcDeclareNamedPtrs( ArchetypeBase, Archetype )
	dcDeclarePtrs( Index )
	dcDeclarePtrs( System )
	dcDeclarePtrs( SystemGroup )

	//! Container type to store the set of entities.
	typedef Set<EntityPtr> EntitySet;

	//! Container type to store the list of entities.
	typedef List<EntityPtr> EntityList;

    //! Container type to store a list of weak pointers to entities.
    typedef List<EntityWPtr> EntityWeakList;

    //! Container type to store an array of entities.
    typedef Array<EntityPtr> EntityArray;

    //! Container type to store an array of weak pointers to entities.
    typedef Array<EntityWPtr> EntityWeakArray;

    //! Container type to store a list of weak pointers to components.
    typedef List<ComponentWPtr> ComponentWeakList;

    //! Converts container of archetypes to an array of entities.
    template<typename TContainer>
    EntityArray toEntityArray( const TContainer& container )
    {
        EntityArray result;

        for( typename TContainer::const_iterator i = container.begin(), end = container.end(); i != end; ++i ) {
            result.push_back( *i );
        }

        return result;
    }

	//! Entity id generator.
	class EntityIdGenerator : public RefCounted {
	public:

							//! Constructs EntityIdGenerator instance.
							EntityIdGenerator( void );
		virtual				~EntityIdGenerator( void ) {}

		//! Generates the next entity id.
		virtual EntityId	generate( void );

	private:

		EntityId			m_nextId;
	};

    //! Ecs serialization context.
    class SerializationContext : public Composition {
    public:

                                //! Constructs SerializationContext instance.
                                SerializationContext( EcsWPtr ecs );

        //! Returns parent entity component system.
        EcsWPtr                 ecs( void ) const;

        //! Returns an entity by unique id.
        virtual EntityPtr       findById( const EntityId& id ) const;

        //! Returns an archetype instance by unique id.
        template<typename TArchetype>
        StrongPtr<TArchetype>   find( const EntityId& id ) const;

        //! Creates component by name.
        ComponentPtr            createComponent( const String& name ) const;

        //! Creates entity by class name.
        EntityPtr               createEntity( const String& name ) const;

    private:

        EcsWPtr                 m_ecs;  //!< Parent entity component system.
    };

    // ** SerializationContext::find
    template<typename TArchetype>
    StrongPtr<TArchetype> SerializationContext::find( const EntityId& id ) const
    {
        EntityPtr entity = findById( id );
        StrongPtr<TArchetype> instance = castTo<TArchetype>( entity.get() );
        DC_ABORT_IF( !instance.valid(), "archetype mismatch" );
        return instance;
    }

	//! Ecs is a root class of an entity component system.
	class Ecs : public RefCounted {
	friend class Entity;
	public:

		//! Creates the archetype instance by name.
		ArchetypePtr	createArchetypeByName( const String& name, const EntityId& id = EntityId(), const Archive* data = NULL ) const;

		//! Creates the component instance by name.
		ComponentPtr	createComponentByName( const String& name, const Archive* data = NULL ) const;

		//! Creates a new archetype instance.
		template<typename TArchetype>
		StrongPtr<TArchetype>	createArchetype( const EntityId& id = EntityId(), const Archive* data = NULL ) const;

        //! Clones an archetype instance.
        template<typename TArchetype>
        StrongPtr<TArchetype>   cloneArchetype( const EntityId& id, WeakPtr<const TArchetype> source ) const;

		//! Creates an array of archetype instances from data.
		template<typename TArchetype>
		Array<StrongPtr<TArchetype>>	createArchetypes( const Archives& data ) const;

		//! Creates a new component instance.
		template<typename TComponent>
		StrongPtr<TComponent>	createComponent( const Archive* data = NULL ) const;

		//! Registers the archetype type.
		template<typename TArchetype>
		bool			registerArchetype( void );

		//! Registers the component type.
		template<typename TComponent>
		bool			registerComponent( void );

		//! Creates a new entity.
		/*!
		\param id Entity id, must be unique to construct a new entity.
		\return Returns a strong pointer to created entity
		*/
		EntityPtr		createEntity( const EntityId& id );

		//! Creates a new entity with a generated id.
		EntityPtr		createEntity( void );

        //! Makes a full copy of an entity.
        EntityPtr       copyEntity( const EntityWPtr& entity, const EntityId& id = EntityId() );

    #if !DC_ECS_ENTITY_CLONING
        //! Clones entity.
        EntityPtr       cloneEntity( EntityWPtr entity );
    #endif  /*  !DC_ECS_ENTITY_CLONING  */

		//! Returns the entity with specified id.
		EntityPtr		findEntity( const EntityId& id ) const;

		//! Returns a list of entities that match a specified aspect.
		EntitySet		findByAspect( const Aspect& aspect ) const;

		//! Rebuild all system indices.
		void			rebuildIndices( void );

        //! Rebuild indices for changed entitites.
        void            rebuildChangedEntities( void );

        //! Cleans up removed entities.
        void            cleanupRemovedEntities( void );

		//! Creates a new system group.
		SystemGroupPtr	createGroup( const String& name, u32 mask );

		//! Returns the entity index instance by it's aspect or creates a new one.
		IndexPtr		requestIndex( const String& name, const Aspect& aspect );

		//! Removes an entity by it's id.
		void			removeEntity( const EntityId& id );

		//! Adds a new entity.
		void			addEntity( EntityPtr entity );

        //! Adds an array of entities, returns the total number of entities that were added.
        s32             addEntities( const EntityArray& entities );

		//! Returns true if an entity with specified id exists.
		bool			isUsedId( const EntityId& id ) const;

		//! Updates the entity component system.
		void			update( u32 currentTime, f32 dt, u32 systems = ~0 );

		//! Sets the entity id generator to be used.
		void			setEntityIdGenerator( const EntityIdGeneratorPtr& value );

		//! Constructs a new component of specified type.
		template<typename TComponent, typename ... Args>
		TComponent*		createComponent( Args ... args )
		{
			return DC_NEW TComponent( args... );
		}

		//! Creates a new Ecs instance.
		static EcsPtr	create( const EntityIdGeneratorPtr& entityIdGenerator = DC_NEW EntityIdGenerator );

	private:

						//! Constructs Ecs instance.
						Ecs( const EntityIdGeneratorPtr& entityIdGenerator );

		//! Notifies the ECS about an entity changes.
		void			notifyEntityChanged( const EntityId& id );

        //! Rebuilds the specified index.
        void            rebuildIndex( IndexWPtr index );

		//! Generates the unique entity id.
		EntityId		generateId( void ) const;

	private:

		//! Container type to store all active entities.
		typedef Map<EntityId, EntityPtr>	Entities;

		//! Container type to store all system groups.
		typedef Array<SystemGroupPtr>		SystemGroups;

		//! Container type to store entity indices.
		typedef Map<Aspect, IndexPtr>		Indices;

        //! Container type to store modified indices.
        typedef Set<IndexWPtr>              IndexSet;

		//! Data factory type.
		typedef NamedAbstractFactory<ComponentBase>	ComponentFactory;

		//! Archetype factory type.
		typedef NamedAbstractFactory<ArchetypeBase>	ArchetypeFactory;

		mutable EntityIdGeneratorPtr	m_entityId;	//!< Used for unique entity id generation.
		Entities						m_entities;	//!< Active entities reside here.
		SystemGroups					m_systems;	//!< All systems reside in system groups.
		Indices							m_indices;	//!< All entity indices are cached here.

		EntitySet						m_changed;	        //!< Entities that was changed.
		EntitySet						m_removed;	        //!< Entities that will be removed.
        IndexSet                        m_changedIndices;   //!< Indices that were changed.

		ComponentFactory				m_componentFactory;		//!< Component object factory.
		ArchetypeFactory				m_archetypeFactory;		//!< Archetype object factory.
	};

	// ** Ecs::registerArchetype
	template<typename TArchetype>
	bool Ecs::registerArchetype( void )
	{
		return m_archetypeFactory.declare<TArchetype>();
	}

	// ** Ecs::registerComponent
	template<typename TComponent>
	bool Ecs::registerComponent( void )
	{
		return m_componentFactory.declare<TComponent>();
	}

	// ** Ecs::createArchetype
	template<typename TArchetype>
	StrongPtr<TArchetype> Ecs::createArchetype( const EntityId& id, const Archive* data ) const
	{
		return static_cast<TArchetype*>( createArchetypeByName( TypeInfo<TArchetype>::name(), id, data ).get() );
	}

    // ** Ecs::cloneArchetype
    template<typename TArchetype>
    StrongPtr<TArchetype> Ecs::cloneArchetype( const EntityId& id, WeakPtr<const TArchetype> source ) const
    {
        // Serialize source to a key-value archive
        Archive ar;
        SerializationContext ctx( const_cast<Ecs*>( this ) );
        source->serialize( ctx, ar );

        // Create archetype instance
        StrongPtr<TArchetype> instance = createArchetype<TArchetype>( id, &ar );

        return instance;
    }

	// ** Ecs::createArchetypes
    template<typename TArchetype>
	Array<StrongPtr<TArchetype>> Ecs::createArchetypes( const Archives& data ) const
	{
		Array<StrongPtr<TArchetype>> result;

		for( s32 i = 0, n = data.size(); i < n; i++ ) {
            const Archive& item = data[i];
			result.push_back( createArchetype<TArchetype>( item.as<KeyValue>()["_id"].as<Guid>(), &item ) );
		}

		return result;
	}

	// ** Ecs::createComponent
	template<typename TComponent>
	StrongPtr<TComponent> Ecs::createComponent( const Archive* data ) const
	{
		return static_cast<TComponent*>( createComponentByName( TypeInfo<TComponent>::name(), data ).get() );
	}

} // namespace Ecs

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "Component/Component.h"
	#include "Entity/Entity.h"
	#include "Entity/Aspect.h"
	#include "Entity/Index.h"
	#include "Entity/Archetype.h"
	#include "System/SystemGroup.h"
	#include "System/GenericEntitySystem.h"
	#include "System/ImmutableEntitySystem.h"
#endif

#endif	/*	!__DC_Ecs_H__	*/