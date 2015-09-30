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

#include "Entity/Entity.h"
#include "Entity/Index.h"
#include "System/SystemGroup.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

IMPLEMENT_LOGGER( log )

// ** Ecs::Ecs
Ecs::Ecs( const EntityIdGeneratorPtr& entityIdGenerator ) : m_entityId( entityIdGenerator )
{
}

// ** Ecs::create
EcsPtr Ecs::create( const EntityIdGeneratorPtr& entityIdGenerator )
{
	DC_BREAK_IF( entityIdGenerator == EntityIdGeneratorPtr() );
	return DC_NEW Ecs( entityIdGenerator );
}

// ** Ecs::createEntity
EntityPtr Ecs::createEntity( const EntityId& id )
{
	DC_BREAK_IF( isUsedId( id ) );
	Entity* entity = DC_NEW Entity( this, id );
	m_entities[id] = entity;
	return EntityPtr( entity );
}

// ** Ecs::createEntity
EntityPtr Ecs::createEntity( void )
{
	EntityId id = m_entityId->generate();
	return createEntity( id );
}

// ** Ecs::findEntity
EntityPtr Ecs::findEntity( const EntityId& id ) const
{
	Entities::const_iterator i = m_entities.find( id );
	return i != m_entities.end() ? i->second : EntityPtr();
}

// ** Ecs::removeEntity
void Ecs::removeEntity( const EntityId& id )
{
	DC_BREAK_IF( !isUsedId( id ) );

	Entities::iterator i = m_entities.find( id );

	if( i == m_entities.end() ) {
		return;
	}

	i->second->markAsRemoved();
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
	Indices::iterator i = m_indices.find( aspect );

	if( i != m_indices.end() ) {
		return i->second;
	}

	IndexPtr index( DC_NEW Index( this, name, aspect ) );
	m_indices[aspect] = index;

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
	DC_BREAK_IF( !isUsedId( id ) );

	Entities::iterator i = m_entities.find( id );

	if( i != m_entities.end() ) {
		m_changed.insert( i->second );
	}
}

// ** Ecs::update
void Ecs::update( u32 currentTime, f32 dt, u32 systems )
{
	// Process all changed entities.
	while( m_changed.size() ) {
		EntitySet changed = m_changed;
		m_changed.clear();

		for( EntitySet::iterator i = changed.begin(), end = changed.end(); i != end; ++i ) {
			for( Indices::iterator j = m_indices.begin(), jend = m_indices.end(); j != jend; j++ ) {
				j->second->notifyEntityChanged( *i );
			}
		}
	}

	// Remove all queued entities
	while( m_removed.size() ) {
		EntitySet removed = m_removed;
		m_removed.clear();

		for( EntitySet::iterator i = removed.begin(), end = removed.end(); i != end; ++i ) {
			m_entities.erase( (*i)->id() );
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
EntityIdGenerator::EntityIdGenerator( void ) : m_nextId( 1 )
{
}

// ** EntityIdGenerator::generate
EntityId EntityIdGenerator::generate( void )
{
	return m_nextId++;
}

} // namespace Ecs

DC_END_DREEMCHEST