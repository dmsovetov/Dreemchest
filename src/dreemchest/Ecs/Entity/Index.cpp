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

#include "Index.h"
#include "Entity.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** Index::Index
Index::Index( EcsWPtr ecs, const String& name, const Aspect& aspect ) : m_ecs( ecs ), m_name( name ), m_aspect( aspect )
{

}

Index::~Index( void )
{

}

// ** Index::entities
const EntitySet& Index::entities( void ) const
{
	return m_entities;
}

// ** Index::entities
EntitySet& Index::entities( void )
{
	return m_entities;
}

// ** Index::size
s32 Index::size( void ) const
{
	return static_cast<s32>( m_entities.size() );
}

// ** Index::events
EventEmitter& Index::events( void )
{
	return m_eventEmitter;
}

// ** Index::notifyEntityChanged
void Index::notifyEntityChanged( const EntityPtr& entity )
{
	bool contains   = m_entities.count( entity ) != 0;
	bool intersects = m_aspect.hasIntersection( entity );

	if( entity->flags() & Entity::Removed ) {
		intersects = false;
	}

	if( !contains && intersects ) {
		processEntityAdded( entity );
	}
	else if( contains && !intersects ) {
		processEntityRemoved( entity );
	}
}

// ** Index::processEntityAdded
void Index::processEntityAdded( const EntityPtr& entity )
{
	LogDebug( "entityIndex", "%s added to %s\n", entity->id().toString().c_str(), m_name.c_str() );
	m_entities.insert( entity );
	m_eventEmitter.notify<Added>( entity );
}

// ** Index::processEntityRemoved
void Index::processEntityRemoved( const EntityPtr& entity )
{
	LogDebug( "entityIndex", "%s removed from %s\n", entity->id().toString().c_str(), m_name.c_str() );
	m_eventEmitter.notify<Removed>( entity );
	m_entities.erase( entity );
}

// ** Index::Added::Added
Index::Added::Added( const EntityPtr& entity ) : entity( entity )
{

}

// ** Index::Removed::Removed
Index::Removed::Removed( const EntityPtr& entity ) : entity( entity )
{

}

} // namespace Ecs

DC_END_DREEMCHEST
