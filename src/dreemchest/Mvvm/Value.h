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

#ifndef __DC_Mvvm_Value_H__
#define __DC_Mvvm_Value_H__

#include "Mvvm.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	//! Base class for a value types.
	class Value : public RefCounted {
	public:

		virtual						~Value( void ) {}

		//! Returns the value type.
		virtual ValueTypeIdx		type( void ) const;

		//! Returns true if the value type matches the specified one.
		virtual bool				is( ValueTypeIdx expected ) const;

		//! Notifies bindings about a value change.
		virtual void				notifyValueChanged( void );

		//! Generates the value type index.
		template<typename TValue>
		static ValueTypeIdx			valueType( void );

		//! Sets the parent object.
		void						setParent( ObjectWPtr value );

		//! Adds binding to this value.
		void						addBinding( BindingWPtr binding );

		//! Removes binding from this value.
		void						removeBinding( BindingWPtr binding );

	protected:

		//! Container type to store bindings attached to this value.
		typedef Set<BindingWPtr>	Bindings;

		ObjectWPtr					m_parent;	//!< Parent object instance.
		Bindings					m_bindings;	//!< Attached bindings.
	};

	// ** Value::valueType
	template<typename TValue>
	ValueTypeIdx Value::valueType( void )
	{
		return GroupedTypeIndex<TValue, Value>::idx();
	}

	//! Generic class to declare primitive value types.
	template<typename TValue>
	class PrimitiveValue : public Value {
	public:

		typedef TValue					ValueType;	//! Alias the value type.
		typedef PrimitiveValue<TValue>	Type;		//!< Alias for this type.
		typedef StrongPtr<Type>			Ptr;		//!< Strong pointer type.
		typedef WeakPtr<Type>			WPtr;		//!< Weak pointer type.

										//! Constructs the PrimitiveValue instance.
										PrimitiveValue( const TValue& value = TValue() );

		//! Returns the actual value type index.
		virtual ValueTypeIdx			type( void ) const;

		//! Returns the property value.
		const TValue&					get( void ) const;

		//! Sets the property value.
		void							set( const TValue& value );

	protected:

		TValue							m_value;	//!< Actual value.
	};

	// ** PrimitiveValue::PrimitiveValue
	template<typename TValue>
	PrimitiveValue<TValue>::PrimitiveValue( const TValue& value ) : m_value( value )
	{
	}

	// ** PrimitiveValue::type
	template<typename TValue>
	ValueTypeIdx PrimitiveValue<TValue>::type( void ) const
	{
		return Value::valueType<Type>();
	}

	// ** PrimitiveValue::get
	template<typename TValue>
	const TValue& PrimitiveValue<TValue>::get( void ) const
	{
		return m_value;
	}

	// ** PrimitiveValue::set
	template<typename TValue>
	void PrimitiveValue<TValue>::set( const TValue& value )
	{
		m_value = value;
	}

	//! Object class is a container of named properties.
	class Object : public Value {
	public:

											//! Constructs Object instance.
											Object( void );

		//! Returns the object value type index.
		virtual ValueTypeIdx				type( void ) const;

		//! Returns true if the object type matches the specified one.
		virtual bool						is( ValueTypeIdx expected ) const;

		//! Returns the isValid property pointer.
		Boolean::WPtr						isValid( void ) const;

		//! Runs the data validation routine and returns the result.
		bool								check( void );

		//! Returns true if an object has a property with specified name.
		bool								has( const String& name ) const;

		//! Returns the property value with specified name.
		ValueWPtr							get( const String& name ) const;

		//! Resolves the property value with specified URI.
		ValueWPtr							resolve( const String& uri ) const;

	protected:

		//! Returns true if the object is valid.
		virtual bool						validate( void ) const;

		//! Adds a new typed property to this object.
		template<typename TValue>
		typename WeakPtr<TValue>			add( const String& name/*, const TValue& defaultValue = TValue()*/ );

	protected:

		//! Container type to store object properties.
		typedef Map<String, ValuePtr>		Properties;

		Boolean::WPtr						m_isValid;		//!< This property is updated by the data validation routine.
		Properties							m_properties;	//!< Object properties.
	};

	// ** Object::add
	template<typename TValue>
	WeakPtr<TValue> Object::add( const String& name/*, const TValue& defaultValue*/ )
	{
		StrongPtr<TValue> value( DC_NEW TValue );
		value->setParent( this );
		m_properties[name] = value;
		return value;
	}

	//! Generic object type used for object type declarations.
	template<typename TObject>
	class GenericObject : public Object {
	public:

		typedef TObject			Type;		//!< Alias for this type.
		typedef StrongPtr<Type>	Ptr;		//!< Strong pointer type.
		typedef WeakPtr<Type>	WPtr;		//!< Weak pointer type.

		//! Returns the actual object value type index.
		virtual ValueTypeIdx	type( void ) const;

		//! Returns true if the object type matches the specified one.
		virtual bool			is( ValueTypeIdx expected ) const;
	};

	// ** GenericObject::type
	template<typename TObject>
	ValueTypeIdx GenericObject<TObject>::type( void ) const
	{
		return valueType<TObject>();
	}

	// ** GenericObject::is
	template<typename TObject>
	bool GenericObject<TObject>::is( ValueTypeIdx expected ) const
	{
		if( expected == valueType<TObject>() ) {
			return true;
		}

		return Object::is( expected );
	}

	//! Array value type.
	class Array : public Object {
	public:

		//! Returns array value type.
		virtual ValueTypeIdx	type( void ) const;

		//! Returns true if the array matches the expected type.
		virtual bool			is( ValueTypeIdx expected ) const;

		//! Returns array size.
		s32						size( void ) const;

		//! Pushes a new value to an array.
		void					push( const ValuePtr& value );

	protected:

		::Array<ValuePtr>		m_values;	//!< Actual values stored inside the array.
	};

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_Value_H__   */