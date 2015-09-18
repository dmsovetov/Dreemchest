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

#include "Family.h"
#include "Entity.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** Family::Family
Family::Family( Entities& entities, const String& name, const Aspect& aspect ) : m_entities( entities ), m_name( name ), m_aspect( aspect )
{
	log::verbose( "Family %s created\n", name.c_str() );
	m_entities.subscribe<Entities::Changed>( dcThisMethod( Family::handleEntityChanged ) );
}

Family::~Family( void )
{
	m_entities.unsubscribe<Entities::Changed>( dcThisMethod( Family::handleEntityChanged ) );
}

// ** Family::create
FamilyPtr Family::create( Entities& entities, const String& name, const Aspect& aspect )
{
	return FamilyPtr( new Family( entities, name, aspect ) );
}

// ** Family::name
const String& Family::name( void ) const
{
	return m_name;
}

// ** Family::entities
const EntitySet& Family::entities( void ) const
{
	return m_family;
}

// ** Family::size
s32 Family::size( void ) const
{
	return static_cast<s32>( m_family.size() );
}

// ** Family::events
EventEmitter& Family::events( void )
{
	return m_eventEmitter;
}

// ** Family::handleEntityChanged
void Family::handleEntityChanged( const Entities::Changed& e )
{
	const EntityPtr& handle = e.m_entity;

	bool contains   = m_family.count( handle ) != 0;
	bool intersects = m_aspect.hasIntersection( handle );

	if( !contains && intersects ) {
		processEntityAdded( handle );
	}
	else if( contains && !intersects ) {
		processEntityRemoved( handle );
	}
}

// ** Family::processEntityAdded
void Family::processEntityAdded( const EntityPtr& handle )
{
	log::verbose( "%s: entity %d added\n", m_name.c_str(), handle->id() );
	m_family.insert( handle );
	m_eventEmitter.emit<Added>( handle );
}

// ** Family::processEntityRemoved
void Family::processEntityRemoved( const EntityPtr& handle )
{
	log::verbose( "%s: entity %d removed\n", m_name.c_str(), handle->id() );
	m_eventEmitter.emit<Removed>( handle );
	m_family.erase( handle );
}

} // namespace Ecs

DC_END_DREEMCHEST
