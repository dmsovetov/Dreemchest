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

		//! Returns the bound value.
		virtual ValueWPtr		value( void ) const = 0;

		//! Returns the converted value.
		virtual ValueWPtr		converted( void ) const = 0;

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

		//! Returns the bound value.
		virtual ValueWPtr		value( void ) const;

		//! Returns the converted value.
		virtual ValueWPtr		converted( void ) const;

		//! Binds to a property.
		virtual bool			bind( ValueWPtr value, Widget widget );

		//! By default the binding doesn't update the value after the widget change.
		virtual void			handleViewChanged( void ) {}

		//! By default the binding doesn't update the widget after the value change.
		virtual void			handleValueChanged( void ) {}

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

	// ** Binding::value
	template<typename TBinding, typename TValue>
	ValueWPtr Binding<TBinding, TValue>::value( void ) const
	{
		return m_property;
	}

	// ** Binding::converted
	template<typename TBinding, typename TValue>
	ValueWPtr Binding<TBinding, TValue>::converted( void ) const
	{
		return ValueWPtr();
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
		BindingPtr			create( ValueTypeIdx valueType, WidgetTypeIdx widgetType, const String& widgetProperty );

		//! Creates new converter instance by input & output types.
		BindingPtr			createConverter( ValueTypeIdx inputType, ValueTypeIdx outputType );

		//! Returns the set of value types for which there are registered bindings.
		ValueTypes			bindableValueTypes( void ) const;

		//! Registers binding with widget & value type.
		template<typename TBinding>
		void				registerBinding( WidgetTypeIdx widgetType, const String& widgetProperty = "" );

		//! Registers converter instance.
		template<typename TConverter>
		void				registerConverter( void );

	protected:

							//! Constructs the BindingFactory instance.
							BindingFactory( void );

	private:

		//! Container type to store converters by output type.
		typedef Map<ValueTypeIdx, BindingPtr>				ConverterByOutputType;

		//! Container type to store converters by input type.
		typedef Map<ValueTypeIdx, ConverterByOutputType>	ConvertersByInputType;

		//! Container type to store bindings by widget property.
		typedef Map<String, BindingPtr>					BindingByProperty;

		//! Container type to store widget value type to binding mapping.
		typedef Map<WidgetTypeIdx, BindingByProperty>	BindingByWidgetType;

		//! Container type to store value type to bindings mapping.
		typedef Map<ValueTypeIdx, BindingByWidgetType>	Bindings;

		Bindings				m_bindings;		//!< Registered bindings.
		ConvertersByInputType	m_converters;	//!< Registered converters.
	};

	// ** BindingFactory::registerBinding
	template<typename TBinding>
	void BindingFactory::registerBinding( WidgetTypeIdx widgetType, const String& widgetProperty )
	{
		// Create binding instance
		TBinding* binding = DC_NEW TBinding;

		// Get the value types.
		ValueTypeIdx valueTypeIdx = binding->type();

		m_bindings[valueTypeIdx][widgetType][widgetProperty] = binding;
	}

	// ** BindingFactory::registerConverter
	template<typename TConverter>
	void BindingFactory::registerConverter( void )
	{
		// Create converter instance
		TConverter* converter = DC_NEW TConverter;

		// Get the converter input & output types.
		ValueTypeIdx inputType  = converter->type();
		ValueTypeIdx outputType = converter->output()->type();

		m_converters[inputType][outputType] = converter;
	}

	//! Bindings instance links a value with a single binding instance.
	class Bindings : public RefCounted {
	public:

		//! Sets the root object.
		void							setRoot( const ObjectWPtr& value );

		//! Returns the root object.
		ObjectWPtr						root( void ) const;

		//! Binds the widget to a value with specified URI.
		bool							bind( const String& widget, const String& uri, ObjectWPtr root = ObjectWPtr() );

		//! Binds the widget to a value.
		bool							bind( const String& widget, const ValueWPtr& value );

	protected:

										//! Constructs the Bindings instance.
										Bindings( const BindingFactoryPtr& factory, const ObjectWPtr& root );

		//! Returns the widget type chain.
		virtual WidgetTypeChain			resolveWidgetTypeChain( const String& widget ) const = 0;

		//! Returns the widget by name.
		virtual Widget					findWidget( const String& name ) const = 0;

		//! Binds the property to a widget.
		bool							createBinding( ValueWPtr value, Widget widget, const WidgetTypeChain& widgetType, const String& key );

	private:

		BindingFactoryPtr				m_factory;	//!< Binding factory instance.
		ObjectWPtr						m_root;		//!< Root object.
		List<BindingPtr>				m_bindings;	//!< All bindings that reside on a view.
	};

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_Binding_H__   */
