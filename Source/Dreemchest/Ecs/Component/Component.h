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

#ifndef __DC_Ecs_Component_H__
#define __DC_Ecs_Component_H__

#include "../Ecs.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! A base class for internal system data attached to a components.
	struct MixInBase : public RefCounted {
	};

	//! A template class used for declaring system-specific internal data.
	template<typename T>
	struct MixIn : public MixInBase {
		typedef StrongPtr<T> Ptr;	//!< The internal data pointer.
	};

	//! Internal component flags.
	enum ComponentFlags {
		IsEnabled = BIT( 31 )	//!< Indicates that a component is enabled.
	};

	//! A base class for all components.
	/*!
	A component is all the data for one aspect of the entity. Component is just a plain
	data and doesn't contain any processing logic.
	*/
	class ComponentBase : public RefCounted {
	friend class Entity;
	public:

        INTROSPECTION_ABSTRACT( ComponentBase )

									//! Constructs ComponentBase instance.
									ComponentBase( void )
										: m_flags( IsEnabled ) {}

		//! Sets the internal data.
		template<typename T>
		void						setInternal( MixInBase* value );

		//! Returns the internal data.
		template<typename T>
		typename MixIn<T>::Ptr      mixIn( void ) const;

		//! Returns component flags.
		u32							flags( void ) const;

		//! Sets component flags.
		void						setFlags( u32 value );

		//! Returns true if component is enabled.
		bool						isEnabled( void ) const;

        //! Returns parent entity instance.
        EntityWPtr                  entity( void ) const;

        //! Generates component type index for a specified type.
        template<typename T>
        static TypeIdx              typeId( void ) { return GroupedTypeIndex<T, ComponentBase>::idx(); }

        //! Returns component type index.
        virtual TypeIdx             typeIndex( void ) const NIMBLE_ABSTRACT;

    #if DC_ECS_ENTITY_CLONING
        //! Copies a component by value and returns a new instance.
        virtual ComponentPtr        deepCopy( void ) const NIMBLE_ABSTRACT;
    #endif  /*  DC_ECS_ENTITY_CLONING   */

	protected:

		//! Sets the component's enabled flag.
		void						setEnabled( bool value );

        //! Sets the component's parent entity.
        void                        setParentEntity( const EntityWPtr& value );

	protected:

		//! Container type to store internal system state inside a component.
		typedef Map< TypeIdx, StrongPtr<MixInBase> > InternalDataHolder;

        EntityWPtr                  m_entity;   //!< Entity instance this component is attached to.
		InternalDataHolder			m_internal;	//!< The internal data.
		FlagSet32					m_flags;	//!< Component flags.
	};

	// ** ComponentBase::setInternal
	template<typename T>
	inline void ComponentBase::setInternal( MixInBase* value )
	{
		m_internal[TypeIndex<T>::idx()] = value;
	}

	// ** ComponentBase::internal
	template<typename T>
	inline typename MixIn<T>::Ptr ComponentBase::mixIn( void ) const
	{
		InternalDataHolder::const_iterator i = m_internal.find( TypeIndex<T>::idx() );

		if( i != m_internal.end() ) {
			return static_cast<T*>( i->second.get() );
		}

		return typename MixIn<T>::Ptr();
	}

	// ** ComponentBase::flags
	inline u32 ComponentBase::flags( void ) const
	{
		return m_flags;
	}

	// ** ComponentBase::setFlags
	inline void ComponentBase::setFlags( u32 value )
	{
		m_flags = value;
	}

	// ** ComponentBase::isEnabled
	inline bool ComponentBase::isEnabled( void ) const
	{
		return m_flags.is( IsEnabled );
	}

	// ** ComponentBase::setEnabled
	inline void ComponentBase::setEnabled( bool value )
	{
		m_flags.set( IsEnabled, value );
	}

    // ** ComponentBase::entity
    inline EntityWPtr ComponentBase::entity( void ) const
    {
        return m_entity;
    }

    // ** ComponentBase::setParentEntity
    inline void ComponentBase::setParentEntity( const EntityWPtr& value )
    {
        NIMBLE_BREAK_IF( value.valid() && m_entity.valid() && m_entity != value, "parent entity of a component is already set" );
        m_entity = value;
    }

	//! Generic component class.
	template<typename T>
	class Component : public ComponentBase {
	public:


		//! Weak pointer type.
		typedef WeakPtr<T>		WPtr;

		//! Strong pointer type.
		typedef StrongPtr<T>	Ptr;

        //! Returns component type index.
        virtual TypeIdx         typeIndex( void ) const NIMBLE_OVERRIDE { return typeId<T>(); }

		static const Bitset&	bit( void ) { static Bitset result = Bitset::withSingleBit( ComponentBase::typeId<T>() ); return result; }

    #if DC_ECS_ENTITY_CLONING
        virtual ComponentPtr    deepCopy( void ) const NIMBLE_OVERRIDE;
    #endif  /*  DC_ECS_ENTITY_CLONING   */
	};

#if DC_ECS_ENTITY_CLONING
    // ** Component::deepCopy
    template<typename T>
    ComponentPtr Component<T>::deepCopy( void ) const
    {
        T* instance = DC_NEW T;
        *instance = *static_cast<const T*>( this );
        instance->setParentEntity( NULL );
        return instance;
    }
#endif  /*  DC_ECS_ENTITY_CLONING   */

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_Component_H__	*/
