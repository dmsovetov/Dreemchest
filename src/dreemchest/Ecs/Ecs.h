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

namespace ecs {

	DECLARE_LOG(log)

	//! Entities container forward declaration.
	class Entities;

	//! Entity aspect forward declaration.
	class Aspect;

	//! Entity id type.
	typedef u32 EntityId;

	//! Entity pointer type
	typedef StrongPtr<class Entity> EntityPtr;

	//! Entity weak pointer type.
	typedef WeakPtr<class Entity> EntityWPtr;

	//! Component pointer type.
	typedef StrongPtr<class Component> ComponentPtr;

	//! Entity family pointer type.
	typedef StrongPtr<class Family> FamilyPtr;

	//! Entity system pointer type.
	typedef StrongPtr<class System> SystemPtr;

	//! Container type to store the set of entities.
	typedef Set<EntityPtr> EntitySet;

	//! Container type to store the set of components.
	typedef Set<ComponentPtr> ComponentSet;

	//! Event emitter type alias.
	typedef event::EventEmitter EventEmitter;

} // namespace ecs

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "EntitySystem.h"
	#include "Component.h"
	#include "Entities.h"
	#include "Entity.h"
	#include "Aspect.h"
	#include "Family.h"
	#include "Archetype.h"
#endif

#endif	/*	!__DC_Ecs_H__	*/