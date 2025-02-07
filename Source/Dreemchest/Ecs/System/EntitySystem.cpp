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
#include "../Entity/Aspect.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** EntitySystem::EntitySystem
EntitySystem::EntitySystem( const String& name, const Aspect& aspect ) : System( name ), m_aspect( aspect )
{

}

// ** EntitySystem::initialize
bool EntitySystem::initialize( EcsWPtr ecs )
{
    if( !System::initialize( ecs ) ) {
        return false;
    }

    // Request index and subdcribe for entity events
    m_index = ecs->requestIndex( m_name, m_aspect );
    m_index->subscribe<Index::Added>( dcThisMethod( EntitySystem::handleEntityAdded ) );
    m_index->subscribe<Index::Removed>( dcThisMethod( EntitySystem::handleEntityRemoved ) );

    // Run event handler for all entities that reside in an index
    for( EntitySet::const_iterator i = m_index->entities().begin(), end = m_index->entities().end(); i != end; ++i ) {
        entityAdded( *i->get() );
    }

    return true;
}

// ** EntitySystem::entityCount
s32 EntitySystem::entityCount( void ) const
{
    return m_index->size();
}

// ** EntitySystem::begin
bool EntitySystem::begin( u32 currentTime, f32 dt )
{
    return true;
}

// ** EntitySystem::end
void EntitySystem::end( void )
{
}

// ** EntitySystem::processEntity
void EntitySystem::processEntity( u32 currentTime, f32 dt, Entity& entity )
{
}

// ** EntitySystem::update
void EntitySystem::update( u32 currentTime, f32 dt )
{
    if( !begin( currentTime, dt ) ) {
        return;
    }

    NIMBLE_BREADCRUMB_CALL_STACK;

    EntitySet& entities = m_index->entities();

    for( EntitySet::iterator i = entities.begin(); i != entities.end(); ) {
        Entity& entity = *(i++)->get();
        processEntity( currentTime, dt, entity );
    }

    end();
}

// ** EntitySystem::handleEntityAdded
void EntitySystem::handleEntityAdded( const Index::Added& e )
{
    NIMBLE_BREADCRUMB_CALL_STACK;
    entityAdded( *e.entity.get() );
}

// ** EntitySystem::handleEntityRemoved
void EntitySystem::handleEntityRemoved( const Index::Removed& e )
{
    NIMBLE_BREADCRUMB_CALL_STACK;
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
