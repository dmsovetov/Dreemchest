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

#ifndef __DC_Scene_Rendering_RopEmitter_H__
#define __DC_Scene_Rendering_RopEmitter_H__

#include "../RenderingContext.h"
#include "../Rvm/Rvm.h"

#include "../../Components/Rendering.h"
#include "../../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Render operation emitter takes renderable entities as an input and emits render operations as an output.
	class AbstractRopEmitter {
	public:

								//! Constructs AbstractRopEmitter instance.
								AbstractRopEmitter( RenderingContext& context, Ecs::IndexPtr index )
                                    : m_context( context ), m_index( index ) {}
        virtual                 ~AbstractRopEmitter( void ) {}

		//! Emits render operations for entities in scene.
		virtual void			emit( const Vec3& camera ) = 0;

	protected:

        RenderingContext&       m_context;      //!< Parent rendering context.
		Ecs::IndexPtr			m_index;		//!< Entities used for render operation emission.
	};

#if DEV_VIRTUAL_EMITTERS
	//! Generic render operation emitter.
	template<typename TRenderable>
	class RopEmitter : public AbstractRopEmitter {
	public:

								//! Constructs RopEmitter instance.
								RopEmitter( RenderingContext& context )
									: AbstractRopEmitter( context, context.scene()->ecs()->requestIndex( "", Ecs::Aspect::all<TRenderable, Transform>() ) ) {}

		//! Emits render operations for renderable entities in scene.
		virtual void			emit( const Vec3& camera ) DC_DECL_OVERRIDE;

	protected:

		//! Emits render operation for a single renderable entity.
		virtual void			emit( const Vec3& camera, const TRenderable& renderable, const Transform& transform ) = 0;
	};

	// ** RopEmitter::emit
	template<typename TRenderable>
	void RopEmitter<TRenderable>::emit( const Vec3& camera )
	{
		const Ecs::EntitySet& entities = m_index->entities();

		for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
			emit( camera, *(*i)->get<TRenderable>(), *(*i)->get<Transform>() );
		}
	}
#else
    //! Generic render operation emitter.
    template<typename TRenderable, typename TRenderEntity, s32 TInitialSize = 0>
    class FixedRopEmitter : public AbstractRopEmitter {
    public:

        //! Alias an array type to store renderable entities.
        typedef FixedArray<TRenderEntity>   RenderEntities; 

								    //! Constructs FixedRopEmitter instance.
								    FixedRopEmitter( RenderingContext& context );
        virtual                     ~FixedRopEmitter( void ) DC_DECL_OVERRIDE;

        //! This method should be called once right after a construction of emitter instance.
        void                        construct( void );

    protected:

        //! Returns an array of render entities.
        const RenderEntities&       renderEntities( void ) const;

        //! This method creates a render entity and should be overridden in a subclass.
        virtual TRenderEntity       createRenderEntity( const Ecs::Entity& entity ) = 0; 

    private:

        //! Handles an addition of a new entity to an index.
        void                        handleEntityAdded( const Ecs::Index::Added& e );

        //! Handles the removal of an entity from an index.
        void                        handleEntityRemoved( const Ecs::Index::Removed& e );

        //! Adds new renderable entity.
        void                        addRenderableEntity( const Ecs::Entity* entity );

        //! Removes a renderable entity.
        void                        removeRenderableEntity( const Ecs::Entity* entity );

    private:

        //! Container type to map from entity pointer to renderable index.
    #ifdef DC_CPP11_DISABLED
        typedef Map<const Ecs::Entity*, s32> IndexByEntity;
    #else
        typedef std::unordered_map<const Ecs::Entity*, s32> IndexByEntity;
    #endif

        RenderEntities              m_entities; //!< An array of renderable entities that is constructed from entity index.
        IndexByEntity               m_indices;  //!< Mapping from entity pointer to renderable index.
    };

    // ** FixedRopEmitter::FixedRopEmitter
    template<typename TRenderable, typename TRenderEntity, s32 TInitialSize>
	FixedRopEmitter<TRenderable, TRenderEntity, TInitialSize>::FixedRopEmitter( RenderingContext& context )
		: AbstractRopEmitter( context, context.scene()->ecs()->requestIndex( "", Ecs::Aspect::all<TRenderable, Transform>() ) )
        , m_entities( TInitialSize )
    {
        // Subscribe for index events
        m_index->events().subscribe<Ecs::Index::Added>( CLOSURE( this, (&FixedRopEmitter<TRenderable, TRenderEntity>::handleEntityAdded) ) );
        m_index->events().subscribe<Ecs::Index::Removed>( CLOSURE( this, (&FixedRopEmitter<TRenderable, TRenderEntity>::handleEntityRemoved) ) );
    }

    // ** FixedRopEmitter::construct
    template<typename TRenderable, typename TRenderEntity, s32 TInitialSize>
    void FixedRopEmitter<TRenderable, TRenderEntity, TInitialSize>::construct( void )
    {
        const Ecs::EntitySet& entities = m_index->entities();

        for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
            addRenderableEntity( i->get() );
        }
    }

    // ** FixedRopEmitter::~FixedRopEmitter
    template<typename TRenderable, typename TRenderEntity, s32 TInitialSize>
	FixedRopEmitter<TRenderable, TRenderEntity, TInitialSize>::~FixedRopEmitter( void )
    {
        // Unsubscribe from index events
        m_index->events().unsubscribe<Ecs::Index::Added>( CLOSURE( this, (&FixedRopEmitter<TRenderable, TRenderEntity>::handleEntityAdded) ) );
        m_index->events().unsubscribe<Ecs::Index::Removed>( CLOSURE( this, (&FixedRopEmitter<TRenderable, TRenderEntity>::handleEntityRemoved) ) );
    }

    // ** FixedRopEmitter::~FixedRopEmitter
    template<typename TRenderable, typename TRenderEntity, s32 TInitialSize>
	const typename FixedRopEmitter<TRenderable, TRenderEntity, TInitialSize>::RenderEntities& FixedRopEmitter<TRenderable, TRenderEntity, TInitialSize>::renderEntities( void ) const
    {
        return m_entities;
    }

    // ** FixedRopEmitter::handleEntityAdded
    template<typename TRenderable, typename TRenderEntity, s32 TInitialSize>
    void FixedRopEmitter<TRenderable, TRenderEntity, TInitialSize>::handleEntityAdded( const Ecs::Index::Added& e )
    {
        addRenderableEntity( e.entity.get() );
    }

    // ** FixedRopEmitter::handleEntityRemoved
    template<typename TRenderable, typename TRenderEntity, s32 TInitialSize>
    void FixedRopEmitter<TRenderable, TRenderEntity, TInitialSize>::handleEntityRemoved( const Ecs::Index::Removed& e )
    {
        removeRenderableEntity( e.entity.get() );
    }

    //! Adds new renderable entity.
    template<typename TRenderable, typename TRenderEntity, s32 TInitialSize>
    void FixedRopEmitter<TRenderable, TRenderEntity, TInitialSize>::addRenderableEntity( const Ecs::Entity* entity )
    {
        // Push new render entity to an array and store it's index
        s32 index = m_entities.push( createRenderEntity( *entity ) );

        // Register the mapping from entity pointer to this index
        m_indices[entity] = index;
    }

    //! Removes a renderable entity.
    template<typename TRenderable, typename TRenderEntity, s32 TInitialSize>
    void FixedRopEmitter<TRenderable, TRenderEntity, TInitialSize>::removeRenderableEntity( const Ecs::Entity* entity )
    {
        // Find an index of this entity
        IndexByEntity::iterator i = m_indices.find( entity );
        DC_ABORT_IF( i == m_indices.end(), "entity pointer could not be mapped to an index" );

        // Remove renderable by index
        m_entities.remove( i->second );

        //! Remove mapping
        m_indices.erase( i );
    }
#endif  /*  DEV_VIRTUAL_EMITTERS  */

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_RopEmitter_H__    */