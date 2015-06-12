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
#include "Archetype.h"
#include "Component.h"

DC_BEGIN_DREEMCHEST

namespace ecs {

// ** Entity::s_aliveCount
u32 Entity::s_aliveCount = 0;

// ** Entity::Entity
Entity::Entity( Entities& entities, const EntityId& id ) : m_entities( entities ), m_id( id )
{
	s_aliveCount++;
}

Entity::~Entity( void )
{
	--s_aliveCount;
}

// ** Entity::activeCount
u32 Entity::activeCount( void )
{
	return s_aliveCount;
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

// ** Entity::isValid
bool Entity::isValid( void ) const
{
	return m_id != EntityId();
}

// ** Entity::destroy
void Entity::destroy( void )
{
	m_mask.clear();
	m_entities.remove( m_id );
}

// ** Entity::create
EntityPtr Entity::create( Entities& entities, const EntityId& id )
{
	return EntityPtr( new Entity( entities, id ) );
}

} // namespace ecs

DC_END_DREEMCHEST
