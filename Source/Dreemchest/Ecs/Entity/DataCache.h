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

#ifndef __DC_Ecs_DataCache_H__
#define __DC_Ecs_DataCache_H__

#include "Index.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

    //! Base class for all data caches.
    class DataCacheBase : public RefCounted {
    friend class Ecs;
    public:

        virtual                     ~DataCacheBase( void );

        //! Returns cache index by entity pointer.
        s32                         indexFromEntity( const EntityWPtr& entity ) const;

    protected:

                                    //! Constructs DataCacheBase instance.
                                    DataCacheBase( EcsWPtr ecs, const Aspect& aspect );

        //! Populates data cache.
        void                        populate( void );

        //! Adds entity to a cache.
        void                        addEntity( const Entity* entity );

        //! Handles an addition of a new entity to an index.
        void                        handleEntityAdded( const Index::Added& e );

        //! Handles the removal of an entity from an index.
        void                        handleEntityRemoved( const Index::Removed& e );

        //! Constructs new data instance associated with an entity.
        virtual s32                 addCacheEntry( const Entity* entity ) = 0;

        //! Removes a renderable entity.
        virtual void                removeCacheEntry( s32 index ) = 0;

    private:

        //! Container type to map from an entity pointer to a cache index.
        typedef HashMap<const Entity*, s32> IndexByEntity;

        IndexPtr			        m_index;    //!< Index that is used to access entities.
        IndexByEntity               m_indices;  //!< Maps from entity pointer to cache index.
    };

	//! Data cache associates some user data with a set of entities specified by an aspect.
    template<typename TData>
    class DataCache : public DataCacheBase {
    friend class Ecs;
    public:

        //! Container type to store a data associated with an entity.
        typedef FixedArray<TData>   Container;

        //! Factory function to create cache items.
        typedef cClosure<TData(const Entity&)>  Factory;

        //! Returns cached data.
        const Container&            data( void ) const;
        Container&                  data( void );

        //! Returns a data by entity.
        const TData&                dataFromEntity( const EntityWPtr& entity ) const;

    private:

                                    //! Constructs DataCache instance.
                                    DataCache( EcsWPtr ecs, const Aspect& aspect, const Factory& factory );

        //! Constructs new data instance associated with an entity.
        virtual s32                 addCacheEntry( const Entity* entity ) DC_DECL_OVERRIDE;

        //! Removes a data cache entry by index.
        virtual void                removeCacheEntry( s32 index ) DC_DECL_OVERRIDE;

    private:

        Factory                     m_factory;  //!< Cache item factory function.
        Container                   m_data;     //!< Array of data associated with entities.
    };

    // ** DataCache::DataCache
    template<typename TData>
	DataCache<TData>::DataCache( EcsWPtr ecs, const Aspect& aspect, const Factory& factory )
        : DataCacheBase( ecs, aspect )
        , m_factory( factory )
    {
    }

    // ** DataCache::data
    template<typename TData>
	const typename DataCache<TData>::Container& DataCache<TData>::data( void ) const
    {
        return m_data;
    }

    // ** DataCache::data
    template<typename TData>
	typename DataCache<TData>::Container& DataCache<TData>::data( void )
    {
        return m_data;
    }

    // ** DataCache::dataFromEntity
    template<typename TData>
    const TData& DataCache<TData>::dataFromEntity( const EntityWPtr& entity ) const
    {
        s32 index = indexFromEntity( entity );
        DC_ABORT_IF( index == -1, "the specified entity does not exist in cache" );
        return m_data[index];
    }

    // ** DataCache::addCacheEntry
    template<typename TData>
    s32 DataCache<TData>::addCacheEntry( const Entity* entity )
    {
        DC_ABORT_IF( entity == NULL, "cache entry could not be created from an invalid entity" );
        return m_data.push( m_factory( *entity ) );
    }

    // ** DataCache::removeCacheEntry
    template<typename TData>
    void DataCache<TData>::removeCacheEntry( s32 index )
    {
        m_data.remove( index );
    }

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_DataCache_H__	*/
