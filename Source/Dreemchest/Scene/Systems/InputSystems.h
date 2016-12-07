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

#include "../Components/Rendering.h"
#include "../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Input systems are used for processing user's input for entities that contain all components from a specified set.
	class InputSystemBase {
    friend class Scene;
	public:

							//! Constructs InputSystemBase instance.
							InputSystemBase( Scene& scene, const Ecs::Aspect& cameraAspect, const Ecs::Aspect& entityAspect );
		virtual				~InputSystemBase( void );

        //! Dispatches all queued viewport events to all eligible entities.
        void                update( void );

    private:

        //! Dispatches a single touch event to all active entities.
        virtual void        dispatchEvent( const InputEvent& e );

        //! A virtual function that should be overriden in a subclass to handle TouchBegan event.
        virtual void        touchBegan( Ecs::Entity& entity, u8 flags, const TouchEvent& e ) {}

        //! A virtual function that should be overriden in a subclass to handle TouchMoved event.
        virtual void        touchMoved( Ecs::Entity& entity, u8 flags, const TouchEvent& e ) {}

        //! A virtual function that should be overriden in a subclass to handle TouchEnded event.
        virtual void        touchEnded( Ecs::Entity& entity, u8 flags, const TouchEvent& e ) {}

    private:

        Scene&              m_scene;    //!< A parent scene instance.
        Ecs::IndexPtr	    m_cameras;  //!< All active cameras that are processed by this input system.
        Ecs::IndexPtr       m_entities; //!< All entities that are processed by this input system.
	};

#if DREEMCHEST_CPP11

	//! Generic input processing entity system for entities that contain all components from a specified set.
	template<typename TSystem, typename TCamera, typename ... TComponents>
	class InputSystem : public InputSystemBase {
	public:

							//! Constructs InputSystem instance.
							InputSystem( Scene& scene );

	protected:

		//! Component types.
		typedef std::tuple<TComponents...> Types;

		//! Tuple indices
		typedef IndexTupleBuilder<sizeof...(TComponents)> Indices;

		//! Generic processing function that should be overriden in a subclass.
		virtual void		touchBegan( Ecs::Entity& entity, u8 flags, const TouchEvent& e, TComponents& ... components ) {}

		//! Generic processing function that should be overriden in a subclass.
		virtual void		touchEnded( Ecs::Entity& entity, u8 flags, const TouchEvent& e, TComponents& ... components ) {}

		//! Generic processing function that should be overriden in a subclass.
		virtual void		touchMoved( Ecs::Entity& entity, u8 flags, const TouchEvent& e, TComponents& ... components ) {}

		//! Dispatches the entity components to processing touch began event
		template<s32 ... Idxs> 
		void dispatchBegan( Ecs::Entity& entity, u8 flags, const TouchEvent& e, IndexesTuple<Idxs...> const& )  
		{ 
			touchBegan( entity, flags, e, *entity.get<typename std::tuple_element<Idxs, Types>::type>()... );
		}

		//! Dispatches the entity components to processing touch ended event
		template<s32 ... Idxs> 
		void dispatchEnded( Ecs::Entity& entity, u8 flags, const TouchEvent& e, IndexesTuple<Idxs...> const& )  
		{ 
			touchEnded( entity, flags, e, *entity.get<typename std::tuple_element<Idxs, Types>::type>()... );
		}

		//! Dispatches the entity components to processing touch moved event
		template<s32 ... Idxs> 
		void dispatchMoved( Ecs::Entity& entity, u8 flags, const TouchEvent& e, IndexesTuple<Idxs...> const& )  
		{ 
			touchMoved( entity, flags, e, *entity.get<typename std::tuple_element<Idxs, Types>::type>()... );
		}

	private:

		//! Extracts components from entity and dispatches them to a abstract function that should be overridden in a subclass.
		virtual void		touchBegan( Ecs::Entity& entity, u8 flags, const TouchEvent& e ) NIMBLE_OVERRIDE;

		//! Extracts components from entity and dispatches them to a abstract function that should be overridden in a subclass.
		virtual void		touchEnded( Ecs::Entity& entity, u8 flags, const TouchEvent& e ) NIMBLE_OVERRIDE;

		//! Extracts components from entity and dispatches them to a abstract function that should be overridden in a subclass.
		virtual void		touchMoved( Ecs::Entity& entity, u8 flags, const TouchEvent& e ) NIMBLE_OVERRIDE;
	};

	// ** InputSystem::InputSystem
	template<typename TSystem, typename TCamera, typename ... TComponents>
	InputSystem<TSystem, TCamera, TComponents...>::InputSystem( Scene& scene )
        : InputSystemBase( scene, Ecs::Aspect::all<Camera, Transform, Viewport, TCamera>(), Ecs::Aspect::all<TComponents...>() )
	{
	}

	// ** InputSystem::touchBeganEvent
	template<typename TSystem, typename TCamera, typename ... TComponents>
	void InputSystem<TSystem, TCamera, TComponents...>::touchBegan( Ecs::Entity& entity, u8 flags, const TouchEvent& e )
	{
		dispatchBegan( entity, flags, e, typename Indices::Indexes() );
	}

	// ** InputSystem::touchEndedEvent
	template<typename TSystem, typename TCamera, typename ... TComponents>
	void InputSystem<TSystem, TCamera, TComponents...>::touchEnded( Ecs::Entity& entity, u8 flags, const TouchEvent& e )
	{
		dispatchEnded( entity, flags, e, typename Indices::Indexes() );
	}

	// ** InputSystem::touchMovedEvent
	template<typename TSystem, typename TCamera, typename ... TComponents>
	void InputSystem<TSystem, TCamera, TComponents...>::touchMoved( Ecs::Entity& entity, u8 flags, const TouchEvent& e )
	{
		dispatchMoved( entity, flags, e, typename Indices::Indexes() );
	}

#endif	/*	#if DREEMCHEST_CPP11	*/

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_InputSystems_H__    */
