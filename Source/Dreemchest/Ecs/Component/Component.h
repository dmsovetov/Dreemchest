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
	struct InternalBase : public RefCounted {
	};

	//! A template class used for declaring system-specific internal data.
	template<typename T>
	struct Internal : public InternalBase {
		typedef StrongPtr<T> Ptr;	//!< The internal data pointer.
	};

	//! Internal component flags.
	enum ComponentFlags {
		IsEnabled = BIT( 31 )	//!< Indicates that a component is enabled.
	};

#if DEV_DEPRECATED_SERIALIZATION
    typedef Io::Serializable ComponentSuperClass;
#else
    typedef RefCounted ComponentSuperClass;
#endif  /*  #if !DEV_DEPRECATED_SERIALIZATION    */

	//! A base class for all components.
	/*!
	A component is all the data for one aspect of the entity. Component is just a plain
	data and doesn't contain any processing logic.
	*/
	class ComponentBase : public ComponentSuperClass {
	friend class Entity;
	public:

        INTROSPECTION_ABSTRACT( ComponentBase )

                                #if DEV_DEPRECATED_SERIALIZATION
                                    ClassEnableTypeInfoSuper( ComponentBase, Io::Serializable )
                                #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

									//! Constructs ComponentBase instance.
									ComponentBase( void )
										: m_flags( IsEnabled ) {}

		//! Sets the internal data.
		template<typename T>
		void						setInternal( InternalBase* value );

		//! Returns the internal data.
		template<typename T>
		typename Internal<T>::Ptr	internal( void ) const;

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

    #if !DEV_DEPRECATED_SERIALIZATION
        //! Returns component type index.
        virtual TypeIdx             typeIndex( void ) const NIMBLE_ABSTRACT;
    #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

    #if DC_ECS_ENTITY_CLONING
        //! Copies a component by value and returns a new instance.
        virtual ComponentPtr        deepCopy( void ) const NIMBLE_ABSTRACT;
    #endif  /*  DC_ECS_ENTITY_CLONING   */

	#ifndef DC_ECS_NO_SERIALIZATION

    #if DEV_DEPRECATED_SERIALIZATION
		//! Reads component from a storage.
		virtual void		        read( const Io::Storage* storage ) DC_DECL_OVERRIDE;

		//! Writes component to a storage.
		virtual void		        write( Io::Storage* storage ) const DC_DECL_OVERRIDE;

		//! Writes this component to a key-value archive.
		virtual void                serialize( SerializationContext& ctx, Archive& ar ) const;

		//! Reads this component from a key-value archive.
		virtual void		        deserialize( SerializationContext& ctx, const Archive& ar );
    #endif  /*  #if !DEV_DEPRECATED_SERIALIZATION   */

	#endif	/*	!DC_ECS_NO_SERIALIZATION	*/

	protected:

		//! Sets the component's enabled flag.
		void						setEnabled( bool value );

        //! Sets the component's parent entity.
        void                        setParentEntity( const EntityWPtr& value );

	protected:

		//! Container type to store internal system state inside a component.
		typedef Map< TypeIdx, StrongPtr<InternalBase> > InternalDataHolder;

        EntityWPtr                  m_entity;   //!< Entity instance this component is attached to.
		InternalDataHolder			m_internal;	//!< The internal data.
		FlagSet32					m_flags;	//!< Component flags.
	};

	// ** ComponentBase::setInternal
	template<typename T>
	inline void ComponentBase::setInternal( InternalBase* value )
	{
		m_internal[TypeIndex<T>::idx()] = value;
	}

	// ** ComponentBase::internal
	template<typename T>
	inline typename Internal<T>::Ptr ComponentBase::internal( void ) const
	{
		InternalDataHolder::const_iterator i = m_internal.find( TypeIndex<T>::idx() );

		if( i != m_internal.end() ) {
			return static_cast<T*>( i->second.get() );
		}

		return typename Internal<T>::Ptr();
	}

#ifndef DC_ECS_NO_SERIALIZATION

#if DEV_DEPRECATED_SERIALIZATION
	// ** ComponentBase::read
	inline void ComponentBase::read( const Io::Storage* storage )
	{
	    Archive ar;
		Variant value;
        Io::BinaryVariantStream( storage->isBinaryStorage()->stream() ).read( value );
		ar = value;

        SerializationContext ctx( NULL );
        deserialize( ctx, ar );
	}

	// ** ComponentBase::read
	inline void ComponentBase::write( Io::Storage* storage ) const
	{
        SerializationContext ctx( NULL );
        Archive ar;

        serialize( ctx, ar );
        Io::BinaryVariantStream( storage->isBinaryStorage()->stream() ).write( ar );
	}

	// ** ComponentBase::serialize
	inline void ComponentBase::serialize( SerializationContext& ctx, Archive& ar ) const
	{
        ar = Archive();
		LogWarning( "serialize", "not implemented for component '%s'\n", typeName() );
	}

	// ** ComponentBase::deserialize
	inline void ComponentBase::deserialize( SerializationContext& ctx, const Archive& ar )
	{
		LogWarning( "deserialize", "not implemented for component '%s'\n", typeName() );
	}
#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

#endif	/*	!DC_ECS_NO_SERIALIZATION	*/

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
        DC_BREAK_IF( value.valid() && m_entity.valid() && m_entity != value, "parent entity of a component is already set" );
        m_entity = value;
    }

	//! Generic component class.
	template<typename T>
	class Component : public ComponentBase {
	public:

                            #if DEV_DEPRECATED_SERIALIZATION
		                    //    ClassEnableTypeInfoSuper( T, ComponentBase )
                            #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */


		//! Weak pointer type.
		typedef WeakPtr<T>		WPtr;

		//! Strong pointer type.
		typedef StrongPtr<T>	Ptr;

    #if !DEV_DEPRECATED_SERIALIZATION
        //! Returns component type index.
        virtual TypeIdx         typeIndex( void ) const NIMBLE_OVERRIDE { return typeId<T>(); }
    #else
        virtual TypeId          typeId( void ) const  { return TypeInfo<T>::id(); }
		virtual CString         typeName( void ) const { return TypeInfo<T>::name(); }		
        virtual TypeIdx         typeIndex( void ) const NIMBLE_OVERRIDE { return ComponentBase::typeId<T>(); }
    #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

		static const Bitset&	bit( void ) { static Bitset result = Bitset::withSingleBit( ComponentBase::typeId<T>() ); return result; }

    #if DC_ECS_ENTITY_CLONING
        virtual ComponentPtr    deepCopy( void ) const DC_DECL_OVERRIDE;
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
