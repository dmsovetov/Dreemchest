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

#ifndef __DC_Ecs_ImmutableEntitySystem_H__
#define __DC_Ecs_ImmutableEntitySystem_H__

#include "EntitySystem.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

#if DREEMCHEST_CPP11

    //! Immutable entity system stores cached entity components and can't change the component layout of an entity in update loop.
    template<typename ... TComponents>
    class ImmutableEntitySystem : public EntitySystem {
    public:

                        //! Constructs ImmutableEntitySystem instance.
                        ImmutableEntitySystem( const String& name )
                            : EntitySystem( name, Aspect::all<TComponents...>() ) {}

    protected:

        //! Component types.
        typedef std::tuple<TComponents...> Types;

        //! Tuple indices
        typedef IndexTupleBuilder<sizeof...(TComponents)> Indices;

        //! Performs an update of a system
        virtual void    update( u32 currentTime, f32 dt );

        //! Generic processing function that should be overriden in a subclass.
        virtual void    process( u32 currentTime, f32 dt, const Array<TComponents*>& ... );

        //! Called when entity was added.
        virtual void    entityAdded( const Entity& entity );

        //! Called when entity was removed.
        virtual void    entityRemoved( const Entity& entity );

    private:

        //! Dispatches the entity components to processing
        template<s32 ... Idxs> 
        void dispatch( u32 currentTime, f32 dt, IndexesTuple<Idxs...> const& )  
        { 
            process( currentTime, dt, std::get<Idxs>( m_cache )... );
        }

        //! Extracts entity components to cache.
        template<s32 Index = 0>
        void extract( const Entity& entity )
        {
            typedef typename std::tuple_element<Index, Types>::type T;
            std::get<Index>( m_cache ).push_back( entity.get<T>() );

            extract<Index + 1>( entity );
        }

    #if defined(DC_PLATFORM_WINDOWS) // VS only NDK :(
        //! Terminates the recursion
        template<>
        void extract<sizeof...(TComponents)>( const Entity& entity )
        {
    
        }
    #endif    /*    !ANDROID    */

    protected:

        std::tuple< Array<TComponents*>... >    m_cache;    //!< Component cache.
    };

    // ** ImmutableEntitySystem::update
    template<typename ... TComponents>
    void ImmutableEntitySystem<TComponents...>::update( u32 currentTime, f32 dt )
    {
        if( !begin( currentTime ) ) {
            return;
        }

        dispatch( currentTime, dt, Indices::Indexes() );

        end();    
    }

    // ** ImmutableEntitySystem::process
    template<typename ... TComponents>
    void ImmutableEntitySystem<TComponents...>::process( u32 currentTime, f32 dt, const Array<TComponents*>& ... )
    {
        NIMBLE_NOT_IMPLEMENTED
    }

    // ** ImmutableEntitySystem::process
    template<typename ... TComponents>
    void ImmutableEntitySystem<TComponents...>::entityAdded( const Entity& entity )
    {
        extract( entity );
    }

    // ** ImmutableEntitySystem::process
    template<typename ... TComponents>
    void ImmutableEntitySystem<TComponents...>::entityRemoved( const Entity& entity )
    {
        NIMBLE_NOT_IMPLEMENTED
    }

#endif    /*    #if DREEMCHEST_CPP11    */

} // namespace Ecs

DC_END_DREEMCHEST

#endif    /*    !__DC_Ecs_ImmutableEntitySystem_H__    */
