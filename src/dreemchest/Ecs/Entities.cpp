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

#include "Entities.h"
#include "Entity.h"
#include "Archetype.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** Entities::operator[]
EntityPtr Entities::operator[]( const EntityId& id ) const
{
	return findEntity( id );
}

// ** Entities::requestEntity
EntityPtr Entities::requestEntity( const EntityId& id )
{
	EntityPtr entity = findEntity( id );

	if( entity == EntityPtr() ) {
		entity = registerEntity( id );
	}

	return entity;
}

// ** Entities::findEntity
EntityPtr Entities::findEntity( const EntityId& id ) const
{
	ActiveEntities::const_iterator i = m_entities.find( id );
	return i != m_entities.end() ? i->first.m_entity : EntityPtr();
}

// ** Entities::entityComponents
ComponentSet Entities::entityComponents( const EntityId& id ) const
{
	ActiveEntities::const_iterator i = m_entities.find( id );

	if( i == m_entities.end() ) {
		return ComponentSet();
	}

	ComponentSet result;

	for( ComponentsByType::const_iterator j = i->second.begin(), end = i->second.end(); j != end; ++j ) {
		result.insert( j->second );
	}

	return result;
}

// ** Entities::registerEntity
EntityPtr Entities::registerEntity( const EntityId& id )
{
	EntityPtr entity = Entity::create( *this, id );

	if( addEntity( entity ) ) {
		return entity;
	}

	return EntityPtr();
}

// ** Entities::addEntity
bool Entities::addEntity( const EntityPtr& entity )
{
	if( findEntity( entity->id() ) != EntityPtr() ) {
		return false;
	}

	m_entities[entity] = ComponentsByType();

	return true;
}

// ** Entities::addComponent
void Entities::addComponent( const EntityId& id, const ComponentPtr& component, TypeIdx type )
{
	// ** Lookup a component container for this entity
	ActiveEntities::iterator i = m_entities.find( id );
	DC_BREAK_IF( i == m_entities.end() );

	// ** Add a new component
	i->second[type] = component;

	// ** Notify systems about a change
	m_changed.insert( i->first.m_entity );
}

// ** Entities::removeComponent
void Entities::removeComponent( const EntityId& id, TypeIdx type )
{
	// ** Lookup a component container for this entity
	ActiveEntities::iterator i = m_entities.find( id );
	DC_BREAK_IF( i == m_entities.end() );

	// ** Remove a component
	i->second.erase( type );

	// ** Notify systems about a change
	m_changed.insert( i->first.m_entity );
}

// ** Entities::findComponent
ComponentPtr Entities::findComponent( const EntityId& id, TypeIdx type )
{
	// ** Lookup a component container for this entity
	ActiveEntities::iterator i = m_entities.find( id );

	if( i == m_entities.end() ) {
		return ComponentPtr();
	}

	ComponentsByType::iterator j = i->second.find( type );
	return j != i->second.end() ? j->second : ComponentPtr();
}

// ** Entities::remove
void Entities::remove( const EntityId& id )
{
	// ** Get an entity by id
	ActiveEntities::iterator i = m_entities.find( id );
	DC_BREAK_IF( i == m_entities.end() );

	// ** Remove entity from processing
	m_changed.insert( i->first.m_entity );
	m_removed.insert( i->first.m_entity );

	// ** Remove entity from a list
	m_entities.erase( i );

#if USE_MONGO
	log::verbose( "World: entity %s removed\n", id.toString().c_str() );
#else
	log::verbose( "World: entity %d removed\n", id );
#endif
}

// ** Entities::notify
void Entities::notify( void )
{
	for( EntitySet::const_iterator i = m_changed.begin(), end = m_changed.end(); i != end; ++i ) {
		m_eventEmitter.emit<Changed>( *i );
	}

	for( EntitySet::const_iterator i = m_removed.begin(), end = m_removed.end(); i != end; ++i ) {
		m_eventEmitter.emit<Removed>( *i );
	}

	m_changed.clear();
	m_removed.clear();
}

// ** Entities::EntityKey::EntityKey
Entities::EntityKey::EntityKey( const EntityPtr& entity ) : m_entityId( entity->id() ), m_entity( entity )
{
	DC_BREAK_IF( m_entity == EntityPtr() );
}

// ** Entities::EntityKey::EntityKey
Entities::EntityKey::EntityKey( const EntityId& id ) : m_entityId( id )
{
}

// ** Entities::EntityKey::operator <
bool Entities::EntityKey::operator < ( const EntityKey& other ) const
{
	return m_entityId < other.m_entityId;
}

} // namespace Ecs

DC_END_DREEMCHEST
