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

#ifndef __DC_Ecs_Family_H__
#define __DC_Ecs_Family_H__

#include "Aspect.h"
#include "Entities.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! Entity family represents a set of entities grouped by a certain criteria.
	class Family : public RefCounted {
	public:

		virtual					~Family( void );

		//! Returns a family name.
		const String&			name( void ) const;

		//! Returns a family size.
		s32						size( void ) const;

		//! Returns a set of entities 
		const EntitySet&		entities( void ) const;

		//! Returns event emitter
		EventEmitter&			events( void );

		//! Creates a new Family instance.
		static FamilyPtr		create( Entities& entities, const String& name, const Aspect& aspect );

		//! New entity has been added to family.
		struct Added {
								//! Constructs Added instance.
								Added( const EntityPtr& entity )
									: entity( entity ) {}
			EntityPtr			entity;	//! Added entity.
		};

		//! New entity has been removed from family.
		struct Removed {
								//! Constructs Removed instance.
								Removed( const EntityPtr& entity )
									: entity( entity ) {}
			EntityPtr			entity;	//! Removed entity.
		};

	protected:

								//! Constructs Family instance.
								Family( Entities& entities, const String& name, const Aspect& aspect );

		//! Processes entity addition.
		virtual void			processEntityAdded( const EntityPtr& entity );

		//! Processes entity removal.
		virtual void			processEntityRemoved( const EntityPtr& entity );

	private:

		//! Handles entity change event.
		virtual void			handleEntityChanged( const Entities::Changed& e );

	protected:

		//! Parent entity manager.
		Entities&				m_entities;

		//! Family name.
		String					m_name;

		//! Entity aspect.
		Aspect					m_aspect;

		//! Entity set.
		EntitySet				m_family;

		//! Family event emitter.
		EventEmitter			m_eventEmitter;
	};

	//! Archetype entity family.
	template<typename T>
	class ArchetypeFamily : public Family {
	public:

		//! Archetype family pointer type.
		typedef StrongPtr< ArchetypeFamily<T> >	Ptr;

		//! Entity archetype pointer type.
		typedef StrongPtr<T>		ArchetypePtr;

		//! Container type to store active entities.
		typedef Array<ArchetypePtr>	Archetypes;

		//! Returns archetypes.
		const Archetypes&			archetypes( void ) const;

		//! Creates a new ArchetypeFamily instance.
		static Ptr					create( Entities& entities, const String& name, const Aspect& aspect );

	protected:

									//! Constructs ArchetypeFamily instance.
									ArchetypeFamily( Entities& entities, const String& name, const Aspect& aspect );

		//! Processes entity addition.
		virtual void				processEntityAdded( const EntityPtr& entity );

		//! Processes entity removal.
		virtual void				processEntityRemoved( const EntityPtr& entity );

	private:

		//! Active entity archetypes.
		Archetypes					m_archetypes;
	};

	// ** ArchetypeFamily::ArchetypeFamily
	template<typename T>
	inline ArchetypeFamily<T>::ArchetypeFamily( Entities& entities, const String& name, const Aspect& aspect ) : Family( entities, name, aspect )
	{
	}

	// ** ArchetypeFamily::archetypes
	template<typename T>
	inline const typename ArchetypeFamily<T>::Archetypes& ArchetypeFamily<T>::archetypes( void ) const
	{
		return m_archetypes;
	}

	// ** ArchetypeFamily::create
	template<typename T>
	inline typename ArchetypeFamily<T>::Ptr ArchetypeFamily<T>::create( Entities& entities, const String& name, const Aspect& aspect )
	{
		return Ptr( new ArchetypeFamily<T>( entities, name, aspect ) );
	}

	// ** ArchetypeFamily::processEntityAdded
	template<typename T>
	inline void ArchetypeFamily<T>::processEntityAdded( const EntityPtr& entity )
	{
		// ** Create archetype instance from entity
		ArchetypePtr archetype = T::create( entity );

		if( archetype == ArchetypePtr() ) {
			return;
		}

		// ** Push archetype to array
		m_archetypes.push_back( archetype );

		// ** Add entity to family
		Family::processEntityAdded( entity );
	}

	// ** ArchetypeFamily::processEntityRemoved
	template<typename T>
	inline void ArchetypeFamily<T>::processEntityRemoved( const EntityPtr& entity )
	{
		for( typename Archetypes::iterator i = m_archetypes.begin(), end = m_archetypes.end(); i != end; ++i ) {
			const ArchetypePtr& archetype = *i;

			if( archetype->entity().lock() == entity ) {
				m_archetypes.erase( i );
				break;
			}
		}

		Family::processEntityRemoved( entity );
	}

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_Family_H__	*/
