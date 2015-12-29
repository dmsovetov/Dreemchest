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

#ifndef __DC_Scene_InputSystems_H__
#define __DC_Scene_InputSystems_H__

#include "../Viewport.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Touch system are used for processing user's touches for entities that contain all components from a specified set.
	class TouchSystem : public Ecs::EntitySystem {
	public:

							//! Constructs TouchSystem instance.
							TouchSystem( const String& name, const Ecs::Aspect& aspect, ViewportWPtr viewport );
		virtual				~TouchSystem( void );

	protected:

		//! Handles touch began event and runs the press processing function.
		void				handleTouchBegan( const Viewport::TouchBegan& e );

		//! Handles touch ended event and runs the press processing function.
		void				handleTouchEnded( const Viewport::TouchEnded& e );

		//! Handles touch moved event and runs the press processing function.
		void				handleTouchMoved( const Viewport::TouchMoved& e );

		//! Processes a touch began event for a single entity.
		virtual void		touchBeganEvent( Viewport::TouchBegan&, Ecs::Entity& entity ) {}

		//! Processes a touch ended event for a single entity.
		virtual void		touchEndedEvent( Viewport::TouchEnded&, Ecs::Entity& entity ) {}

		//! Processes a touch moved event for a single entity.
		virtual void		touchMovedEvent( Viewport::TouchMoved&, Ecs::Entity& entity ) {}

	protected:

		ViewportWPtr		m_viewport;	//!< Viewport instance this system is bound to.
	};

#ifndef DC_CPP11_DISABLED

	//! Generic touch processing entity system for entities that contain all components from a specified set.
	template<typename TSystem, typename ... TComponents>
	class GenericTouchSystem : public TouchSystem {
	public:

							//! Constructs GenericTouchSystem instance.
							GenericTouchSystem( ViewportWPtr viewport );

	protected:

		//! Component types.
		typedef std::tuple<TComponents...> Types;

		//! Tuple indices
		typedef IndexTupleBuilder<sizeof...(TComponents)> Indices;

		//! Generic processing function that should be overriden in a subclass.
		virtual void		touchBeganEvent( Viewport::TouchBegan& e, Ecs::Entity& entity, TComponents& ... components ) {}

		//! Generic processing function that should be overriden in a subclass.
		virtual void		touchEndedEvent( Viewport::TouchEnded& e, Ecs::Entity& entity, TComponents& ... components ) {}

		//! Generic processing function that should be overriden in a subclass.
		virtual void		touchMovedEvent( Viewport::TouchMoved& e, Ecs::Entity& entity, TComponents& ... components ) {}

		//! Dispatches the entity components to processing pressed event
		template<s32 ... Idxs> 
		void dispatchPressed( Viewport::TouchBegan& e, Ecs::Entity& entity, IndexesTuple<Idxs...> const& )  
		{ 
			touchBeganEvent( e, entity, *entity.get<typename std::tuple_element<Idxs, Types>::type>()... );
		}

		//! Dispatches the entity components to processing released event
		template<s32 ... Idxs> 
		void dispatchReleased( Viewport::TouchEnded& e, Ecs::Entity& entity, IndexesTuple<Idxs...> const& )  
		{ 
			touchEndedEvent( e, entity, *entity.get<typename std::tuple_element<Idxs, Types>::type>()... );
		}

		//! Dispatches the entity components to processing moved event
		template<s32 ... Idxs> 
		void dispatchMoved( Viewport::TouchMoved& e, Ecs::Entity& entity, IndexesTuple<Idxs...> const& )  
		{ 
			touchMovedEvent( e, entity, *entity.get<typename std::tuple_element<Idxs, Types>::type>()... );
		}

	private:

		//! Extracts components from entity and dispatches them to a abstract function that should be overridden in a subclass.
		virtual void		touchBeganEvent( Viewport::TouchBegan& e, Ecs::Entity& entity ) DC_DECL_OVERRIDE;

		//! Extracts components from entity and dispatches them to a abstract function that should be overridden in a subclass.
		virtual void		touchEndedEvent( Viewport::TouchEnded& e, Ecs::Entity& entity ) DC_DECL_OVERRIDE;

		//! Extracts components from entity and dispatches them to a abstract function that should be overridden in a subclass.
		virtual void		touchMovedEvent( Viewport::TouchMoved& e, Ecs::Entity& entity ) DC_DECL_OVERRIDE;
	};

	// ** GenericTouchSystem::GenericTouchSystem
	template<typename TSystem, typename ... TComponents>
	GenericTouchSystem<TSystem, TComponents...>::GenericTouchSystem( ViewportWPtr viewport ) : TouchSystem( TypeInfo<TSystem>::name(), Ecs::Aspect::all<TComponents...>(), viewport )
	{
	}

	// ** GenericTouchSystem::touchBeganEvent
	template<typename TSystem, typename ... TComponents>
	void GenericTouchSystem<TSystem, TComponents...>::touchBeganEvent( Viewport::TouchBegan& e, Ecs::Entity& entity )
	{
		dispatchPressed( e, entity, typename Indices::Indexes() );
	}

	// ** GenericTouchSystem::touchEndedEvent
	template<typename TSystem, typename ... TComponents>
	void GenericTouchSystem<TSystem, TComponents...>::touchEndedEvent( Viewport::TouchEnded& e, Ecs::Entity& entity )
	{
		dispatchReleased( e, entity, typename Indices::Indexes() );
	}

	// ** GenericTouchSystem::touchMovedEvent
	template<typename TSystem, typename ... TComponents>
	void GenericTouchSystem<TSystem, TComponents...>::touchMovedEvent( Viewport::TouchMoved& e, Ecs::Entity& entity )
	{
		dispatchMoved( e, entity, typename Indices::Indexes() );
	}

#endif	/*	!DC_CPP11_DISABLED	*/

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_InputSystems_H__    */