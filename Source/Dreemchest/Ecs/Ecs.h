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
#include <Reflection/MetaObject/Property.h>
#include <Reflection/MetaObject/Class.h>
#include <Reflection/MetaObject/Assembly.h>
#include <Reflection/MetaObject/Instance.h>
#include <Reflection/Serialization/Serializer.h>

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
    dcDeclareNamedPtrs( DataCacheBase, DataCache )
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
        virtual                ~EntityIdGenerator( void ) {}

        //! Generates the next entity id.
        virtual EntityId    generate( void );

    private:

        EntityId            m_nextId;
    };

    //! Ecs is a root class of an entity component system.
    class Ecs : public RefCounted {
    friend class Entity;
    public:

        //! Creates a new entity.
        /*!
        \param id Entity id, must be unique to construct a new entity.
        \return Returns a strong pointer to created entity
        */
        EntityPtr        createEntity( const EntityId& id );

        //! Creates a new entity with a generated id.
        EntityPtr        createEntity( void );

        //! Creates new data cache instance that will be populated inside next update
        template<typename TDataCache>
        Ptr<TDataCache> createDataCache( const Aspect& aspect, const typename TDataCache::Factory& factory );

        //! Makes a full copy of an entity.
        EntityPtr       copyEntity( const EntityWPtr& entity, const EntityId& id = EntityId() );

    #if !DC_ECS_ENTITY_CLONING
        //! Clones entity.
        EntityPtr       cloneEntity( EntityWPtr entity );
    #endif  /*  !DC_ECS_ENTITY_CLONING  */

        //! Returns the entity with specified id.
        EntityPtr        findEntity( const EntityId& id ) const;

        //! Returns a list of entities that match a specified aspect.
        EntitySet        findByAspect( const Aspect& aspect ) const;

        //! Rebuild all system indices.
        void            rebuildIndices( void );

        //! Rebuild indices for changed entitites.
        void            rebuildChangedEntities( void );

        //! Cleans up removed entities.
        void            cleanupRemovedEntities( void );

        //! Creates a new system group.
        SystemGroupPtr    createGroup( const String& name, u32 mask );

        //! Returns the entity index instance by it's aspect or creates a new one.
        IndexPtr        requestIndex( const String& name, const Aspect& aspect );

        //! Rebuilds the specified index.
        void            rebuildIndex( IndexWPtr index );

        //! Removes an entity by it's id.
        void            removeEntity( const EntityId& id );

        //! Adds a new entity.
        void            addEntity( EntityPtr entity );

        //! Adds an array of entities, returns the total number of entities that were added.
        s32             addEntities( const EntityArray& entities );

        //! Returns true if an entity with specified id exists.
        bool            isUsedId( const EntityId& id ) const;

        //! Updates the entity component system.
        void            update( u32 currentTime, f32 dt, u32 systems = ~0 );

        //! Sets the entity id generator to be used.
        void            setEntityIdGenerator( const EntityIdGeneratorPtr& value );

        //! Constructs a new component of specified type.
        template<typename TComponent, typename ... Args>
        TComponent*        createComponent( Args ... args )
        {
            return DC_NEW TComponent( args... );
        }

        //! Creates a new Ecs instance.
        static EcsPtr    create( const EntityIdGeneratorPtr& entityIdGenerator = DC_NEW EntityIdGenerator );

    private:

                        //! Constructs Ecs instance.
                        Ecs( const EntityIdGeneratorPtr& entityIdGenerator );

        //! Notifies the ECS about an entity changes.
        void            notifyEntityChanged( const EntityId& id );

        //! Generates the unique entity id.
        EntityId        generateId( void ) const;

    private:

        //! Container type to store all active entities.
        typedef Map<EntityId, EntityPtr>    Entities;

        //! Container type to store all system groups.
        typedef Array<SystemGroupPtr>        SystemGroups;

        //! Container type to store entity indices.
        typedef Map<Aspect, IndexPtr>        Indices;

        //! Container type to store modified indices.
        typedef Set<IndexWPtr>              IndexSet;

        //! Container type to store created data caches.
        typedef List<DataCachePtr>          DataCacheList;

        mutable EntityIdGeneratorPtr        m_entityId;            //!< Used for unique entity id generation.
        Entities                            m_entities;            //!< Active entities reside here.
        SystemGroups                        m_systems;            //!< All systems reside in system groups.
        Indices                                m_indices;            //!< All entity indices are cached here.

        EntitySet                            m_changed;            //!< Entities that was changed.
        EntitySet                            m_removed;            //!< Entities that will be removed.
        IndexSet                            m_changedIndices;   //!< Indices that were changed.
        DataCacheList                       m_dataCaches;       //!< List of data caches that should be populated.
    };


    // ** Ecs::createDataCache
    template<typename TDataCache>
    Ptr<TDataCache> Ecs::createDataCache( const Aspect& aspect, const typename TDataCache::Factory& factory )
    {
        Ptr<TDataCache> dataCache = DC_NEW TDataCache( this, aspect, factory );
        m_dataCaches.push_back( dataCache );
        return dataCache;
    }

} // namespace Ecs

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
    #include "EntitySerializer.h"
    #include "Component/Component.h"
    #include "Entity/Entity.h"
    #include "Entity/Aspect.h"
    #include "Entity/Index.h"
    #include "Entity/DataCache.h"
    #include "System/SystemGroup.h"
    #include "System/GenericEntitySystem.h"
    #include "System/ImmutableEntitySystem.h"
#endif

#endif    /*    !__DC_Ecs_H__    */
