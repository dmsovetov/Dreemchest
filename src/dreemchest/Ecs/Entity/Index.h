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

#ifndef __DC_Ecs_Index_H__
#define __DC_Ecs_Index_H__

#include "Aspect.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! Entity index represents a set of entities grouped by a certain criteria.
	class Index : public RefCounted {
	friend class Ecs;
	public:

		virtual					~Index( void );

		//! Returns a family size.
		s32						size( void ) const;

		//! Returns a set of entities 
		const EntitySet&		entities( void ) const;

		//! Returns a set of entities 
		EntitySet&				entities( void );

		//! Returns event emitter
		EventEmitter&			events( void );

		//! New entity has been added to index.
		struct Added {
								//! Constructs Added instance.
								Added( const EntityPtr& entity )
									: entity( entity ) {}
			EntityPtr			entity;	//! Added entity.
		};

		//! Entity has been removed from index.
		struct Removed {
								//! Constructs Removed instance.
								Removed( const EntityPtr& entity )
									: entity( entity ) {}
			EntityPtr			entity;	//! Removed entity.
		};

	protected:

								//! Constructs Index instance.
								Index( EcsWPtr ecs, const String& name, const Aspect& aspect );

		//! Processes entity addition.
		virtual void			processEntityAdded( const EntityPtr& entity );

		//! Processes entity removal.
		virtual void			processEntityRemoved( const EntityPtr& entity );

	private:

		//! Processes the entity change
		void					notifyEntityChanged( const EntityPtr& entity );

	protected:

		EcsWPtr					m_ecs;				//!< Parent ECS instance.
		String					m_name;				//!< Index name.
		Aspect					m_aspect;			//!< Entity aspect.
		EntitySet				m_entities;			//!< Entity set.
		EventEmitter			m_eventEmitter;		//!< Index event emitter.
	};

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_Index_H__	*/
