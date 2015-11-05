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

	//! A base class for all components.
	/*!
	A component is all the data for one aspect of the entity. Component is just a plain
	data and doesn't contain any processing logic.
	*/
	class ComponentBase : public io::Serializable {
	public:

									//! Constructs ComponentBase instance.
									ComponentBase( void )
										: m_flags( 0 ) {}

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

	#ifndef DC_BSON_DISABLED
		//! Returns the component BSON.
		virtual io::Bson			bson( void ) const;

		//! Sets the component BSON.
		virtual void				setBson( const io::Bson& value );
	#endif	/*	!DC_BSON_DISABLED	*/

	private:

		//! Container type to store internal system state inside a component.
		typedef Map< TypeIdx, StrongPtr<InternalBase> > InternalDataHolder;

		InternalDataHolder			m_internal;	//!< The internal data.
		u32							m_flags;	//!< Component flags.
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
			return i->second;
		}

		return typename Internal<T>::Ptr();
	}

#if !DC_BSON_DISABLED
	// ** ComponentBase::bson
	inline io::Bson ComponentBase::bson( void ) const
	{
		log::warn( "Component::bson : is not implemented for '%s'\n", typeName() );
		return io::Bson::kNull;
	}

	// ** ComponentBase::setBson
	inline void ComponentBase::setBson( const io::Bson& value )
	{
		log::warn( "Component::setBson : is not implemented for '%s'\n", typeName() );
	}
#endif	/*	!DC_BSON_DISABLED	*/

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

	//! Generic component class.
	template<typename T>
	class Component : public ComponentBase {
	public:

		IoOverrideSerializableSuper( T, ComponentBase )


		//! Weak pointer type.
		typedef WeakPtr<T>		WPtr;

		//! Strong pointer type.
		typedef StrongPtr<T>	Ptr;

		static const Bitset&	bit( void ) { static Bitset result = Bitset::withSingleBit( TypeIndex<T>::idx() ); return result; }
	};

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_Component_H__	*/
