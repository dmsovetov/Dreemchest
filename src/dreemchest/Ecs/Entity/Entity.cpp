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
Entity::Entity( void ) : m_isRemoved( false )
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
	m_isRemoved = true;
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

} // namespace Ecs

DC_END_DREEMCHEST