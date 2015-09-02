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

namespace Scene {

	//! The basic scene system
	template<typename T>
	class SceneSystem : public Ecs::EntityWithComponentsSystem<T> {
	public:

								//! Constructs the SceneSystem instance
								SceneSystem( Ecs::Entities& entities, const String& name )
									: Ecs::EntityWithComponentsSystem<T>( entities, name ) {}

		//! Casts the entity to a SceneObject type
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity, T& component )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL )
			process( currentTime, dt, *sceneObject, component );
		}

		//! Casts the entity to a SceneObject type
		virtual void			entityAdded( const Ecs::EntityPtr& entity )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL )
			sceneObjectAdded( *sceneObject );
		}

		//! Casts the entity to a SceneObject type
		virtual void			entityRemoved( const Ecs::EntityPtr& entity )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL )
			sceneObjectRemoved( *sceneObject );
		}

		//! Processes the scene object
		virtual void			process( u32 currentTime, f32 dt, SceneObject& sceneObject, T& component )
		{
			DC_BREAK
		}

		//! Processes the added scene object
		virtual void			sceneObjectAdded( SceneObject& sceneObject )
		{
			sceneObjectAdded( sceneObject, *sceneObject.get<T>() );
		}

		//! Processes the added scene object
		virtual void			sceneObjectAdded( SceneObject& sceneObject, T& component )
		{
		}

		//! Processes the removed scene object
		virtual void			sceneObjectRemoved( SceneObject& sceneObject )
		{
			sceneObjectRemoved( sceneObject, *sceneObject.get<T>() );
		}

		//! Processes the removed scene object
		virtual void			sceneObjectRemoved( SceneObject& sceneObject, T& component )
		{
		}
	};

	//! The basic scene system for two component types
	template<typename TComponent1, typename TComponent2>
	class SceneSystem2 : public Ecs::EntityWithComponentsSystem2<TComponent1, TComponent2> {
	public:

								//! Constructs the SceneSystem2 instance
								SceneSystem2( Ecs::Entities& entities, const String& name )
									: Ecs::EntityWithComponentsSystem2<TComponent1, TComponent2>( entities, name ) {}

		//! Casts the entity to a SceneObject type
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity, TComponent1& component1, TComponent2& component2 )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL );
			process( currentTime, dt, *sceneObject, component1, component2 );
		}

		//! Casts the entity to a SceneObject type
		virtual void			entityAdded( const Ecs::EntityPtr& entity )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL )
			sceneObjectAdded( *sceneObject );
		}

		//! Casts the entity to a SceneObject type
		virtual void			entityRemoved( const Ecs::EntityPtr& entity )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL )
			sceneObjectRemoved( *sceneObject );
		}

		//! Processes the scene object
		virtual void			process( u32 currentTime, f32 dt, SceneObject& sceneObject, TComponent1& component1, TComponent2& component2 )
		{
			DC_BREAK
		}

		//! Processes the added scene object
		virtual void			sceneObjectAdded( SceneObject& sceneObject )
		{
			sceneObjectAdded( sceneObject, *sceneObject.get<TComponent1>(), *sceneObject.get<TComponent2>() );
		}

		//! Processes the added scene object
		virtual void			sceneObjectAdded( SceneObject& sceneObject, TComponent1& component1, TComponent2& component2 )
		{
		}

		//! Processes the removed scene object
		virtual void			sceneObjectRemoved( SceneObject& sceneObject )
		{
			sceneObjectRemoved( sceneObject, *sceneObject.get<TComponent1>(), *sceneObject.get<TComponent2>() );
		}

		//! Processes the removed scene object
		virtual void			sceneObjectRemoved( SceneObject& sceneObject, TComponent1& component1, TComponent2& component2 )
		{
		}
	};

	//! The basic scene system for two component types
	template<typename TComponent1, typename TComponent2, typename TComponent3>
	class SceneSystem3 : public Ecs::EntityWithComponentsSystem3<TComponent1, TComponent2, TComponent3> {
	public:

								//! Constructs the SceneSystem3 instance
								SceneSystem3( Ecs::Entities& entities, const String& name )
									: Ecs::EntityWithComponentsSystem3<TComponent1, TComponent2, TComponent3>( entities, name ) {}

		//! Casts the entity to a SceneObject type
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity, TComponent1& component1, TComponent2& component2, TComponent3& component3 )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL );
			process( currentTime, dt, *sceneObject, component1, component2, component3 );
		}

		//! Casts the entity to a SceneObject type
		virtual void			entityAdded( const Ecs::EntityPtr& entity )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL )
			sceneObjectAdded( *sceneObject );
		}

		//! Casts the entity to a SceneObject type
		virtual void			entityRemoved( const Ecs::EntityPtr& entity )
		{
			SceneObject* sceneObject = castTo<SceneObject>( entity.get() );
			DC_BREAK_IF( sceneObject == NULL )
			sceneObjectRemoved( *sceneObject );
		}

		//! Processes the scene object
		virtual void			process( u32 currentTime, f32 dt, SceneObject& sceneObject, TComponent1& component1, TComponent2& component2, TComponent3& component3 )
		{
			DC_BREAK
		}

		//! Processes the added scene object
		virtual void			sceneObjectAdded( SceneObject& sceneObject )
		{
			sceneObjectAdded( sceneObject, *sceneObject.get<TComponent1>(), *sceneObject.get<TComponent2>(), *sceneObject.get<TComponent3>() );
		}

		//! Processes the added scene object
		virtual void			sceneObjectAdded( SceneObject& sceneObject, TComponent1& component1, TComponent2& component2, TComponent3& component3 )
		{
		}

		//! Processes the removed scene object
		virtual void			sceneObjectRemoved( SceneObject& sceneObject )
		{
			sceneObjectRemoved( sceneObject, *sceneObject.get<TComponent1>(), *sceneObject.get<TComponent2>(), *sceneObject.get<TComponent3>() );
		}

		//! Processes the removed scene object
		virtual void			sceneObjectRemoved( SceneObject& sceneObject, TComponent1& component1, TComponent2& component2, TComponent3& component3 )
		{
		}
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_System_H__    */