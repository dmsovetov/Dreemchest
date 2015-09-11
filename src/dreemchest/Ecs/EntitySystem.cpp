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

#include "EntitySystem.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

IMPLEMENT_LOGGER(log)

// ** EntitySystem::EntitySystem
EntitySystem::EntitySystem( Entities& entities, const String& name, const Aspect& aspect ) : m_entities( entities )
{
	m_family = Family::create( entities, name, aspect );
	m_family->events().subscribe<Family::Added>( dcThisMethod( EntitySystem::handleEntityAdded ) );
	m_family->events().subscribe<Family::Removed>( dcThisMethod( EntitySystem::handleEntityRemoved ) );
}

// ** EntitySystem::begin
bool EntitySystem::begin( u32 currentTime )
{
	return true;
}

// ** EntitySystem::end
void EntitySystem::end( void )
{
}

// ** EntitySystem::process
void EntitySystem::process( u32 currentTime, f32 dt, Entity& entity )
{
}

// ** EntitySystem::update
void EntitySystem::update( u32 currentTime, f32 dt )
{
	if( !begin( currentTime ) ) {
		return;
	}

	const EntitySet& entities = m_family->entities();

	for( EntitySet::const_iterator i = entities.begin(); i != entities.end(); ) {
		const Entity& entity = *(i++)->get();
		process( currentTime, dt, const_cast<Entity&>( entity ) );
	}

	end();
}

// ** EntitySystem::handleEntityAdded
void EntitySystem::handleEntityAdded( const Family::Added& e )
{
	entityAdded( *e.entity.get() );
}

// ** EntitySystem::handleEntityRemoved
void EntitySystem::handleEntityRemoved( const Family::Removed& e )
{
	entityRemoved( *e.entity.get() );
}

// ** EntitySystem::entityAdded
void EntitySystem::entityAdded( const Entity& entity )
{
}

// ** EntitySystem::entityRemoved
void EntitySystem::entityRemoved( const Entity& entity )
{
}

} // namespace Ecs

DC_END_DREEMCHEST
