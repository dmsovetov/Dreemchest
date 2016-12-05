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

#include "Entity.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** Entity::Entity
Entity::Entity( void ) : m_flags( 0 )
{

}

// ** Entity::setEcs
void Entity::setEcs( EcsWPtr value )
{
	m_ecs = value;
}

// ** Entity::ecs
EcsWPtr Entity::ecs( void ) const
{
	return m_ecs;
}

// ** Entity::setId
void Entity::setId( const EntityId& value )
{
	m_id = value;
}

// ** Entity::id
const EntityId& Entity::id( void ) const
{
	return m_id;
}

// ** Entity::clear
void Entity::clear( void )
{
    m_components.clear();
}

// ** Entity::isSerializable
bool Entity::isSerializable( void ) const
{
    return m_flags.is( Serializable );
}

// ** Entity::setSerializable
void Entity::setSerializable( bool value )
{
    m_flags.set( Serializable, value );
}

// ** Entity::mask
const Bitset& Entity::mask( void ) const
{
	return m_mask;
}

// ** Entity::components
const Entity::Components& Entity::components( void ) const
{
	return m_components;
}

// ** Entity::components
Entity::Components& Entity::components( void )
{
	return m_components;
}

// ** Entity::queueRemoval
void Entity::queueRemoval( void )
{
	m_ecs->removeEntity( m_id );
}

// ** Entity::markAsRemoved
void Entity::markAsRemoved( void )
{
	m_flags.on( Removed );
}

// ** Entity::flags
u8 Entity::flags( void ) const
{
	return m_flags;
}

// ** Entity::setFlags
void Entity::setFlags( u8 value )
{
	m_flags = value;
}

// ** Entity::updateComponentBit
void Entity::updateComponentBit( u32 bit, bool value )
{
	if( value ) {
		m_mask.set( bit );
	} else {
		m_mask.clear( bit );
	}
	
	if( m_ecs.valid() ) {
		m_ecs->notifyEntityChanged( m_id );
	}
}

// ** Entity::detachById
void Entity::detachById( TypeIdx id )
{
	NIMBLE_BREAK_IF( m_flags.is( Removed ), "this entity was removed" );

	Components::iterator i = m_components.find( id );
	NIMBLE_ABORT_IF( i == m_components.end(), "component does not exist" );

	updateComponentBit( i->second->typeIndex(), false );
    i->second->setParentEntity( NULL );
	m_components.erase( i );
}

#if DC_ECS_ENTITY_CLONING

// ** Entity::deepCopy
EntityPtr Entity::deepCopy( const EntityId& id ) const
{
    NIMBLE_ABORT_IF( !m_ecs.valid(), "entity that is not added to any Ecs could not be copied\n" );
    return const_cast<Ecs*>( m_ecs.get() )->copyEntity( const_cast<Entity*>( this ), id );
}

#endif  /*  DC_ECS_ENTITY_CLONING   */

} // namespace Ecs

DC_END_DREEMCHEST
