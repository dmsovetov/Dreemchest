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

#ifndef __DC_Ecs_EntitySystem_H__
#define __DC_Ecs_EntitySystem_H__

#include "System.h"
#include "../Entity/Index.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! Entity system is used for processing entities.
	class EntitySystem : public System {
	public:

		//! System logic is done here.
		virtual void	update( u32 currentTime, f32 dt );

		//! Initializes the entity system.
		virtual bool	initialize( EcsWPtr ecs );

		//! Returns the total number of entities that are processed by this system.
		s32				entityCount( void ) const;

	protected:

						//! Constructs a EntitySystem instance.
						EntitySystem( const String& name, const Aspect& aspect );

		//! Called every update tick before processing entities.
		virtual bool	begin( u32 currentTime );

		//! Called every update tick after all entities are processed.
		virtual void	end( void );

		//! Processes a single entity.
		virtual void	process( u32 currentTime, f32 dt, Entity& entity );

		//! Called when entity was added.
		virtual void	entityAdded( const Entity& entity );

		//! Called when entity was removed.
		virtual void	entityRemoved( const Entity& entity );

		//! Handles an entity added event.
		void			handleEntityAdded( const Index::Added& e );

		//! Handles an entity removed event.
		void			handleEntityRemoved( const Index::Removed& e );

	protected:

		Aspect			m_aspect;	//!< Entity aspect.
		IndexPtr		m_index;	//!< Entity index used by this system.
	};
} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_EntitySystem_H__	*/