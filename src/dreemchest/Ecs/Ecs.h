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

#ifndef __DC_Ecs_H__
#define __DC_Ecs_H__

#include "../Dreemchest.h"

#include "../io/serialization/Serializable.h"
#include "../event/EventEmitter.h"

#ifndef FOO_INCLUDED
	#include <Foo/Foo.h>
#endif

DC_BEGIN_DREEMCHEST

namespace Ecs {

	DECLARE_LOG( log )

	class Aspect;

	//! Entity id type.
	typedef u32 EntityId;

	dcDeclarePtrs( Ecs )
	dcDeclarePtrs( Entity )
	dcDeclarePtrs( ComponentBase )
	dcDeclarePtrs( Index )
	dcDeclarePtrs( System )
	dcDeclarePtrs( SystemGroup )

	//! Container type to store the set of entities.
	typedef Set<EntityPtr> EntitySet;

	//! Container type to store the set of components.
	//typedef Set<ComponentBasePtr> ComponentSet;

	//! Event emitter type alias.
	typedef event::EventEmitter EventEmitter;

	//! Ecs is a root class of an entity component system.
	class Ecs : public RefCounted {
	friend class Entity;
	public:

		//! Creates a new entity.
		/*!
		\param id Entity id, must be unique to construct a new entity.
		\return Returns a strong pointer to created entity
		*/
		EntityPtr		createEntity( const EntityId& id );

		//! Creates a new system group.
		SystemGroupPtr	createGroup( const String& name, u32 mask );

		//! Returns the entity index instance by it's aspect or creates a new one.
		IndexPtr		requestIndex( const String& name, const Aspect& aspect );

		//! Removes an entity by it's id.
		void			removeEntity( const EntityId& id );

		//! Returns true if an entity with specified id exists.
		bool			isUsedId( const EntityId& id ) const;

		//! Updates the entity component system.
		void			update( u32 currentTime, f32 dt, u32 systems = ~0 );

		//! Constructs a new component of specified type.
		template<typename TComponent, typename ... Args>
		TComponent*		createComponent( Args ... args )
		{
			return DC_NEW TComponent( args... );
		}

		//! Creates a new Ecs instance.
		static EcsPtr	create( void );

	private:

						//! Constructs Ecs instance.
						Ecs( void );

		//! Notifies the ECS about an entity changes.
		void			notifyEntityChanged( const EntityId& id );

	private:

		//! Container type to store all active entities.
		typedef Map<EntityId, EntityPtr>	Entities;

		//! Container type to store all system groups.
		typedef Array<SystemGroupPtr>		SystemGroups;

		//! Container type to store entity indices.
		typedef Map<Aspect, IndexPtr>		Indices;

		Entities		m_entities;	//!< Active entities reside here.
		SystemGroups	m_systems;	//!< All systems reside in system groups.
		Indices			m_indices;	//!< All entity indices are cached here.

		EntitySet		m_changed;	//!< Entities that was changed.
		EntitySet		m_removed;	//!< Entities that will be removed.
	};

} // namespace Ecs

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "Component/Component.h"
	#include "Entity/Entity.h"
	#include "Entity/Aspect.h"
	#include "Entity/Index.h"
	#include "System/GenericEntitySystem.h"
	#include "System/ImmutableEntitySystem.h"
#endif

#endif	/*	!__DC_Ecs_H__	*/