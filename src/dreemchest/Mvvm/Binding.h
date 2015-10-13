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

#ifndef __DC_Mvvm_Binding_H__
#define __DC_Mvvm_Binding_H__

#include "Value.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	//! Base property binding.
	class IBinding : public RefCounted {
	public:

		virtual					~IBinding( void ) {}

		//! Clones this binding.
		virtual BindingPtr		clone( void ) const = 0;

		//! Returns the binding value type.
		virtual ValueTypeIdx	type( void ) const = 0;

		//! Binds to a property.
		virtual bool			bind( ValueWPtr value, Widget widget ) = 0;

        //! Refreshes the bound view.
        virtual void            handleViewChanged( void ) = 0;

        //! Refreshes the bound property.
        virtual void            handleValueChanged( void ) = 0;
	};

	//! A template class to bind a property of a specified type TValue.
	template<typename TBinding, typename TValue>
	class Binding : public IBinding {
	public:

		virtual					~Binding( void );

		//! Clones this binding instance.
		virtual BindingPtr		clone( void ) const;

		//! Returns the binding value type.
		virtual ValueTypeIdx	type( void ) const;

		//! Binds to a property.
		virtual bool			bind( ValueWPtr value, Widget widget );

		//! By default the binding doesn't update the value after the widget change.
		virtual void			handleViewChanged( void ) {}

	protected:

		WeakPtr<TValue>			m_property;	//!< The bound property instance.
		Widget					m_widget;	//!< Widget that is bound to value.
	};

	// ** Binding::~Binding
	template<typename TBinding, typename TValue>
	Binding<TBinding, TValue>::~Binding( void )
	{
		bind( ValueWPtr(), NULL );
	}

	// ** Binding::clone
	template<typename TBinding, typename TValue>
	BindingPtr Binding<TBinding, TValue>::clone( void ) const
	{
		return DC_NEW TBinding;
	}

	// ** Binding::clone
	template<typename TBinding, typename TValue>
	ValueTypeIdx Binding<TBinding, TValue>::type( void ) const
	{
		return Value::valueType<TValue>();
	}

	// ** Binding::bind
	template<typename TBinding, typename TValue>
	bool Binding<TBinding, TValue>::bind( ValueWPtr value, Widget widget )
	{
		if( m_property.valid() ) {
			m_property->removeBinding( this );
		}

		m_property = castTo<TValue>( value );
		m_widget   = widget;

		if( !m_property.valid() ) {
			return false;
		}

		m_property->addBinding( this );

		return true;
	}

	//! Binding factory creates the binding instance by value & widget value type.
	class BindingFactory : public RefCounted {
	public:

		//! Creates new binding instance by widget & value types.
		BindingPtr			create( ValueTypeIdx valueType, ValueTypeIdx widgetType );

		//! Registers binding with widget & value type.
		template<typename TWidget, typename TBinding>
		void				registerBinding( void );

	private:

		//! Container type to store widget value type to binding mapping.
		typedef Map<ValueTypeIdx, BindingPtr>			BindingByWidgetType;

		//! Container type to store value type to bindings mapping.
		typedef Map<ValueTypeIdx, BindingByWidgetType>	Bindings;

		Bindings			m_bindings;	//!< Registered bindings.
	};

	// ** BindingFactory::registerBinding
	template<typename TWidget, typename TBinding>
	void BindingFactory::registerBinding( void )
	{
		// Create binding instance
		TBinding* binding = DC_NEW TBinding;

		// Get the value types.
		ValueTypeIdx valueTypeIdx  = binding->type();
		ValueTypeIdx widgetTypeIdx = Value::valueType<TWidget>();

		m_bindings[valueTypeIdx][widgetTypeIdx] = DC_NEW TBinding;
	}

	//! Bindings instance links a value with a single binding instance.
	class Bindings : public RefCounted {
	public:

		//! Binds the widget to a value with specified URI.
		bool				bind( const String& widget, const String& uri );

		//! Binds the widget to a value.
		bool				bind( const String& widget, const ValueWPtr& value );

		//! Creates Bindings instance.
		static BindingsPtr	create( const BindingFactoryPtr& factory, const ObjectWPtr& root );

	protected:

							//! Constructs the Bindings instance.
							Bindings( const BindingFactoryPtr& factory, const ObjectWPtr& root );

		//! Returns the widget value type.
		ValueTypeIdx		widgetValueType( const String& widget ) const;

		//! Returns the widget by name.
		Widget				findWidget( const String& name ) const;

	private:

		BindingFactoryPtr	m_factory;	//!< Binding factory instance.
		ObjectWPtr			m_root;		//!< Root object.
		Array<BindingPtr>	m_bindings;	//!< All bindings that reside on a view.
	};

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_Binding_H__   */
