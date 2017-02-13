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

#include "DataCache.h"
#include "Entity.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** DataCacheBase::DataCacheBase
DataCacheBase::DataCacheBase( EcsWPtr ecs, const Aspect& aspect )
{
    // Request entity index
    m_index = ecs->requestIndex( "DataCache", aspect );

    // Subscribe for index events
    m_index->subscribe<Index::Added>( dcThisMethod( DataCacheBase::handleEntityAdded ) );
    m_index->subscribe<Index::Removed>( dcThisMethod( DataCacheBase::handleEntityRemoved ) );
}

// ** DataCacheBase::~DataCacheBase
DataCacheBase::~DataCacheBase( void )
{
    // Unsubscribe from index events
    m_index->unsubscribe<Index::Added>( dcThisMethod( DataCacheBase::handleEntityAdded ) );
    m_index->unsubscribe<Index::Removed>( dcThisMethod( DataCacheBase::handleEntityRemoved ) );
}

// ** DataCacheBase::indexFromEntity
s32 DataCacheBase::indexFromEntity( const EntityWPtr& entity ) const
{
    IndexByEntity::const_iterator i = m_indices.find( entity.get() );
    return i != m_indices.end() ? i->second : -1;
}

// ** DataCacheBase::populate
void DataCacheBase::populate( void )
{
    // Get all indexed entities.
    const EntitySet& entities = m_index->entities();

    // Add a cache item for each entity.
    for( EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
        // Get entity pointer
        const Entity* entity = i->get();

        // Is it inside a cache already?
        if( indexFromEntity( *i ) != -1 ) {
            continue;
        }

        // Add entity to a cache
        addEntity( entity );
    }
}

// ** DataCacheBase::addEntity
void DataCacheBase::addEntity( const Entity* entity )
{
    // Add new cache entry
    s32 index = addCacheEntry( entity );

    // Register the mapping from entity pointer to this index
    m_indices[entity] = index;
}

// ** DataCacheBase::handleEntityAdded
void DataCacheBase::handleEntityAdded( const Index::Added& e )
{
    addEntity( e.entity.get() );
}

// ** DataCacheBase::handleEntityRemoved
void DataCacheBase::handleEntityRemoved( const Index::Removed& e )
{
    // Get entity pointer
    const Entity* entity = e.entity.get();

    // Find an index of this entity
    IndexByEntity::iterator i = m_indices.find( entity );
    NIMBLE_ABORT_IF( i == m_indices.end(), "entity pointer could not be mapped to an index" );

    // Remove cache item
    removeCacheEntry( i->second );

    // Remove mapping
    m_indices.erase( i );
}

} // namespace Ecs

DC_END_DREEMCHEST
