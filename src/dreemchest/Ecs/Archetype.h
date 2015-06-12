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

#ifndef __DC_Ecs_Archetype_H__
#define __DC_Ecs_Archetype_H__

#include "Entity.h"
#include "Component.h"

DC_BEGIN_DREEMCHEST

namespace ecs {

	/*!
	Archetype class is a predefined set of components that compose an entity type, this
	simplifies the creation of complex entities.
	*/
	class Archetype : public RefCounted {
	public:

									//! Constructs an Archetype instance.
									Archetype( const EntityWPtr& entity );
		virtual						~Archetype( void ) {}

		//! Returns an archetype name.
		const String&				name( void ) const;

		//! Returns an entity id.
		const EntityId&				id( void ) const;

		//! Returns entity.
		const EntityWPtr&			entity( void ) const;

		//! Returns an entity component of specified type.
		template<typename TComponent>
		TComponent*					get( void ) const;

	protected:

		//! Initializes a component set.
		static void					construct( const EntityWPtr& entity );

	protected:

		//! Entity handle.
		EntityWPtr					m_entity;
	};

	// ** Archetype::get
	template<typename TComponent>
	inline TComponent* Archetype::get( void ) const
	{
		DC_BREAK_IF( m_entity.expired() );

		TComponent* component = m_entity.lock()->get<TComponent>();
		DC_BREAK_IF( component == NULL );

		return component;
	}

} // namespace ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_Archetype_H__	*/