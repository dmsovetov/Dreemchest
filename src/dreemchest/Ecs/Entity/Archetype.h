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

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! Archetype is a predefined component composition.
	class ArchetypeBase : public Entity {
	public:

							ClassEnableTypeInfoSuper( ArchetypeBase, Entity )

		//! Constructs archetype instance by adding all components.
		virtual void		construct( void ) = 0;

	#ifndef DC_ECS_NO_SERIALIZATION
		//! Constructs archetype instance before deserialization.
		virtual void		deserialize( SerializationContext& ctx, const Io::KeyValue& value ) DC_DECL_OVERRIDE;
	#endif	/*	!DC_ECS_NO_SERIALIZATION	*/
	};

	//! Generic archetype class to declare archetypes.
	template<typename TArchetype>
	class Archetype : public ArchetypeBase {
	public:

										ClassEnableTypeInfoSuper( TArchetype, ArchetypeBase )

		//! Weak pointer type.
		typedef WeakPtr<TArchetype>		WPtr;

		//! Strong pointer type.
		typedef StrongPtr<TArchetype>	Ptr;
	};

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_Archetype_H__	*/