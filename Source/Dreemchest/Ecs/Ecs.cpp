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

#include "Entity/Aspect.h"
#include "Ecs.h"
#include "EntitySerializer.h"

#include "Entity/Entity.h"
#include "Entity/Index.h"
#include "Entity/Archetype.h"
#include "Entity/DataCache.h"
#include "System/SystemGroup.h"

#include <time.h>

DC_BEGIN_DREEMCHEST

namespace Ecs {

// -------------------------------------------------------------- Ecs -------------------------------------------------------------- //

// ** Ecs::Ecs
Ecs::Ecs( const EntityIdGeneratorPtr& entityIdGenerator ) : m_entityId( entityIdGenerator )
{
}

// ** Ecs::create
EcsPtr Ecs::create( const EntityIdGeneratorPtr& entityIdGenerator )
{
	DC_BREAK_IF( !entityIdGenerator.valid(), "invalid entity id generator" );
	return DC_NEW Ecs( entityIdGenerator );
}

// ** Ecs::setEntityIdGenerator
void Ecs::setEntityIdGenerator( const EntityIdGeneratorPtr& value )
{
	m_entityId = value;
}

// ** Ecs::generateId
EntityId Ecs::generateId( void ) const
{
	EntityId id;

	do {
		id = m_entityId->generate();
	} while( isUsedId( id ) );

	return id;
}

// ** Ecs::addEntity
void Ecs::addEntity( EntityPtr entity )
{
	DC_BREAK_IF( !entity.valid(), "invalid entity" );

	const EntityId& id = entity->id();
    if( id.isNull() ) {
        LogWarning( "entity", "adding entity with an invalid id\n" );
    }

	DC_BREAK_IF( isUsedId( id ), "entity id is already used" );

	// Setup entity
	entity->setEcs( this );

	// Register the entity
	m_entities[id] = entity;

	// Queue entity for notification.
	m_changed.insert( entity );
}

// ** Ecs::addEntities
s32 Ecs::addEntities( const EntityArray& entities )
{
    s32 addedCount = 0;

    for( s32 i = 0, n = static_cast<s32>( entities.size() ); i < n; i++ ) {
        DC_BREAK_IF( !entities[i].valid(), "invalid entity" );

        // Don't add the same entity twice
        if( isUsedId( entities[i]->id() ) ) {
            continue;
        }

        addedCount++;
        addEntity( entities[i] );
    }

    return addedCount;
}

#if DEV_DEPRECATED_ECS_ARCHETYPES
// ** Ecs::createArchetypeByName
ArchetypePtr Ecs::createArchetypeByName( const String& name, const EntityId& id, const Archive* data, Reflection::AssemblyWPtr assembly ) const
{
	// Create archetype instance by name
	ArchetypePtr instance = m_archetypeFactory.construct( name );
	DC_BREAK_IF( !instance.valid(), "failed to create archetype by name" );

	// Ensure we found the archetype type
	if( !instance.valid() ) {
		LogError( "entity", "unknown archetype '%s'\n", name.c_str() );
		return ArchetypePtr();
	}

	// Initialize the entity id
	EntityId eid = id.isNull() ? generateId() : id;
	instance->setId( eid );

	// Construct the archetype
	instance->construct();

	// Set the parent ECS
	instance->setEcs( const_cast<Ecs*>( this ) );

	// Load from data
	if( data ) {
    #if DEV_DEPRECATED_SERIALIZATION
        SerializationContext ctx( const_cast<Ecs*>( this ) );
		instance->deserialize( ctx, *data );
    #else
        Serializer serializer( const_cast<Ecs*>( this ) );
        serializer.deserialize( assembly, instance, data->as<KeyValue>() );
    #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */
	}

	return instance;
}
#endif  /*  #if DEV_DEPRECATED_ECS_ARCHETYPES   */

#if DEV_DEPRECATED_SERIALIZATION
// ** Ecs::createComponentByName
ComponentPtr Ecs::createComponentByName( const String& name, const Archive* data ) const
{
	ComponentPtr instance = m_componentFactory.construct( name );
	DC_BREAK_IF( !instance.valid(), "failed to create component by name" );

	// Ensure we found the component type
	if( !instance.valid() ) {
		LogError( "entity", "unknown component '%s'\n", name.c_str() );
		return ComponentPtr();
	}

	// Load from data
	if( data ) {
    #if DEV_DEPRECATED_SERIALIZATION
        SerializationContext ctx( const_cast<Ecs*>( this ) );
		instance->deserialize( ctx, *data );
    #else
        DC_NOT_IMPLEMENTED;
    #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */
	}

	return instance;
}
#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

// ** Ecs::createEntity
EntityPtr Ecs::createEntity( const EntityId& id )
{
	EntityPtr entity( DC_NEW Entity );
	entity->setId( id );
	//addEntity( entity );

	return entity;
}

// ** Ecs::createEntity
EntityPtr Ecs::createEntity( void )
{
	EntityId id = generateId();
	return createEntity( id );
}

// ** Ecs::copyEntity
EntityPtr Ecs::copyEntity( const EntityWPtr& entity, const EntityId& id )
{
    // Create entity instance
    EntityPtr clone = id.isNull() ? createEntity() : createEntity( id );

    // Now clone components
    for( Entity::Components::const_iterator i = entity->components().begin(), end = entity->components().end(); i != end; ++i ) {
        clone->attachComponent( i->second->deepCopy().get() );
    }

    return clone;
}

#if !DC_ECS_ENTITY_CLONING

// ** Ecs::cloneEntity
EntityPtr Ecs::cloneEntity( EntityWPtr entity )
{
    // Serialize source to a key-value archive
    KeyValue ar;
    SerializationContext ctx( this );
    entity->serialize( ctx, ar );

    // Create entity instance
    EntityPtr instance = createEntity();

    // Deserialize
    instance->deserialize( ctx, ar );

    return instance;
}

#endif  /*  !DC_ECS_ENTITY_CLONING  */

// ** Ecs::findEntity
EntityPtr Ecs::findEntity( const EntityId& id ) const
{
	Entities::const_iterator i = m_entities.find( id );
	return i != m_entities.end() ? i->second : EntityPtr();
}

// ** Ecs::findByAspect
EntitySet Ecs::findByAspect( const Aspect& aspect ) const
{
	EntitySet result;

	for( Entities::const_iterator i = m_entities.begin(), end = m_entities.end(); i != end; ++i ) {
		if( aspect.hasIntersection( i->second ) ) {
			result.insert( i->second );
		}
	}

	return result;
}

// ** Ecs::removeEntity
void Ecs::removeEntity( const EntityId& id )
{
	DC_BREAK_IF( !isUsedId( id ), "entity does not exist" );

	Entities::iterator i = m_entities.find( id );

	if( i == m_entities.end() ) {
		return;
	}

	i->second->markAsRemoved();

	m_changed.insert( i->second );
	m_removed.insert( i->second );
}

// ** Ecs::isUsedId
bool Ecs::isUsedId( const EntityId& id ) const
{
	return m_entities.find( id ) != m_entities.end();
}

// ** Ecs::requestIndex
IndexPtr Ecs::requestIndex( const String& name, const Aspect& aspect )
{
    // Find index by an aspect
	Indices::iterator i = m_indices.find( aspect );

    // Found - just return it
	if( i != m_indices.end() ) {
		return i->second;
	}

    // Create a new index instance
	IndexPtr index( DC_NEW Index( this, name, aspect ) );
	m_indices[aspect] = index;

    // Add this index to a changed set
    m_changedIndices.insert( index );

	return index;
}

// ** Ecs::createGroup
SystemGroupPtr Ecs::createGroup( const String& name, u32 mask )
{
	SystemGroupPtr group( DC_NEW SystemGroup( this, name, mask ) );
	m_systems.push_back( group );
	return group;
}

// ** Ecs::notifyEntityChanged
void Ecs::notifyEntityChanged( const EntityId& id )
{
	if( !isUsedId( id ) ) {
		LogDebug( "entity", "changed with invalid id %s\n", id.toString().c_str() );
		return;
	}

	Entities::iterator i = m_entities.find( id );

	if( i != m_entities.end() ) {
		m_changed.insert( i->second );
	}
}

// **  Ecs::rebuildIndex
void Ecs::rebuildIndex( IndexWPtr index )
{
    // Process all entities
	for( Entities::iterator i = m_entities.begin(), end = m_entities.end(); i != end; ++i ) {
	    index->notifyEntityChanged( i->second );
	}
}

// ** Ecs::rebuildIndices
void Ecs::rebuildIndices( void )
{
	// Rebuild each index
	for( Indices::iterator i = m_indices.begin(), end = m_indices.end(); i != end; i++ ) {
		rebuildIndex( i->second );
	}
}

// ** Ecs::rebuildChangedEntities
void Ecs::rebuildChangedEntities( void )
{
	while( m_changed.size() ) {
		EntitySet changed = m_changed;
		m_changed.clear();

		for( EntitySet::iterator i = changed.begin(), end = changed.end(); i != end; ++i ) {
			for( Indices::iterator j = m_indices.begin(), jend = m_indices.end(); j != jend; j++ ) {
				j->second->notifyEntityChanged( *i );
			}
		}
	}
}

// ** Ecs::cleanupRemovedEntities
void Ecs::cleanupRemovedEntities( void )
{
	while( m_removed.size() ) {
		EntitySet removed = m_removed;
		m_removed.clear();

		for( EntitySet::iterator i = removed.begin(), end = removed.end(); i != end; ++i ) {
			m_entities.erase( (*i)->id() );
		}
	}
}

// ** Ecs::update
void Ecs::update( u32 currentTime, f32 dt, u32 systems )
{
	// Process all changed entities.
    rebuildChangedEntities();

	// Remove all queued entities.
    cleanupRemovedEntities();

    // Populate all data caches
    while( m_dataCaches.size() ) {
        DataCacheList dataCaches = m_dataCaches;
        m_dataCaches.clear();

        for( DataCacheList::iterator i = dataCaches.begin(), end = dataCaches.end(); i != end; ++i ) {
            (*i)->populate();
        }
    }

    // Rebuild all changed indices.
    while( m_changedIndices.size() ) {
        IndexSet changed = m_changedIndices;
        m_changedIndices.clear();

		for( IndexSet::iterator i = changed.begin(), end = changed.end(); i != end; ++i ) {
			rebuildIndex( *i );
		}
    }

	// Update all system groups.
	for( u32 i = 0, n = ( u32 )m_systems.size(); i < n; i++ ) {
		SystemGroupPtr& group = m_systems[i];

		if( group->mask() & systems ) {
			group->update( currentTime, dt );
		}
	}
}

// ** EntityIdGenerator::EntityIdGenerator
EntityIdGenerator::EntityIdGenerator( void )
{
#if DC_ECS_INTEGER_IDS
	m_nextId = 1;
#else
	srand( static_cast<u32>( time( NULL ) ) );
#endif
}

// ** EntityIdGenerator::generate
EntityId EntityIdGenerator::generate( void )
{
#if DC_ECS_INTEGER_IDS
	return m_nextId++;
#else
	return Guid::generate();
#endif
}

#if DEV_DEPRECATED_SERIALIZATION

// ---------------------------------------------------------- SerializationContext ---------------------------------------------------------- //

// ** SerializationContext::SerializationContext
SerializationContext::SerializationContext( EcsWPtr ecs ) : m_ecs( ecs )
{

}

// ** SerializationContext::ecs
EcsWPtr SerializationContext::ecs( void ) const
{
    return m_ecs;
}

// ** SerializationContext::findById
EntityPtr SerializationContext::findById( const EntityId& id ) const
{
    return m_ecs->findEntity( id );
}

// ** SerializationContext::createComponent
ComponentPtr SerializationContext::createComponent( const String& name ) const
{
    return m_ecs->createComponentByName( name );
}

// ** SerializationContext::createEntity
EntityPtr SerializationContext::createEntity( const String& name ) const
{
#if DEV_DEPRECATED_ECS_ARCHETYPES
    if( name == "Entity" ) {
        return const_cast<SerializationContext*>( this )->m_ecs->createEntity();
    }

    return m_ecs->createArchetypeByName( name );
#else
    return const_cast<SerializationContext*>( this )->m_ecs->createEntity();
#endif  /*  #if DEV_DEPRECATED_ECS_ARCHETYPES   */
}

#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

} // namespace Ecs

DC_END_DREEMCHEST