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

#ifndef __DC_Scene_System_H__
#define __DC_Scene_System_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! The basic scene system
	template<typename T>
	class SceneSystem : public ecs::EntityWithComponentsSystem<T> {
	public:

								//! Constructs the SceneSystem instance
								SceneSystem( ecs::Entities& entities, const String& name )
									: ecs::EntityWithComponentsSystem<T>( entities, name ) {}

		//! Casts the entity to a SceneObject type
		virtual void			process( u32 currentTime, f32 dt, ecs::EntityPtr& entity, T& component )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL );
			process( currentTime, dt, *sceneObject, component );
		}

		//! Processes the scene object
		virtual void			process( u32 currentTime, f32 dt, SceneObject& sceneObject, T& component )
		{
			DC_BREAK
		}

	protected:

		//! Parent scene
		//Scene*					m_scene;
	};

	//! The basic scene system for two component types
	template<typename TComponent1, typename TComponent2>
	class SceneSystem2 : public ecs::EntityWithComponentsSystem2<TComponent1, TComponent2> {
	public:

								//! Constructs the SceneSystem2 instance
								SceneSystem2( ecs::Entities& entities, const String& name )
									: ecs::EntityWithComponentsSystem2<TComponent1, TComponent2>( entities, name ) {}

		//! Casts the entity to a SceneObject type
		virtual void			process( u32 currentTime, f32 dt, ecs::EntityPtr& entity, TComponent1& component1, TComponent2& component2 )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL );
			process( currentTime, dt, *sceneObject, component1, component2 );
		}

		//! Processes the scene object
		virtual void			process( u32 currentTime, f32 dt, SceneObject& sceneObject, TComponent1& component1, TComponent2& component2 )
		{
			DC_BREAK
		}

	protected:

		//! Parent scene
		//Scene*					m_scene;
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_System_H__    */