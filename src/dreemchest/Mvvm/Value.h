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

	// ---------------------------------------------------------- Value ---------------------------------------------------------- //

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

	// ---------------------------------------------------------- PrimitiveValue ---------------------------------------------------------- //

	//! Generic class to declare primitive value types.
	template<typename TValue>
	class PrimitiveValue : public Value {
	public:

		typedef TValue					ValueType;	//! Alias the value type.
		typedef PrimitiveValue<TValue>	Type;		//!< Alias for this type.
		typedef StrongPtr<Type>			Ptr;		//!< Strong pointer type.
		typedef WeakPtr<Type>			WPtr;		//!< Weak pointer type.

										//! Casts primitive value to a internal data type.
										operator const TValue& ( void ) const;

		//! Compares two values.
		bool							operator == ( const TValue& value ) const;

		//! Returns the actual value type index.
		virtual ValueTypeIdx			type( void ) const;

		//! Returns the property value.
		const TValue&					get( void ) const;

		//! Sets the property value.
		void							set( const TValue& value );

		//! Creates the PrimitiveValue instance.
		static Ptr						create( const TValue& value = TValue() );

	protected:

										//! Constructs the PrimitiveValue instance.
										PrimitiveValue( const TValue& value = TValue() );

	protected:

		TValue							m_value;	//!< Actual value.
	};

	// ** PrimitiveValue::PrimitiveValue
	template<typename TValue>
	PrimitiveValue<TValue>::PrimitiveValue( const TValue& value ) : m_value( value )
	{
	}

	// ** PrimitiveValue::operator TValue
	template<typename TValue>
	PrimitiveValue<TValue>::operator const TValue &( void ) const
	{
		return m_value;
	}

	// ** PrimitiveValue::operator TValue
	template<typename TValue>
	bool PrimitiveValue<TValue>::operator == ( const TValue& value ) const
	{
		return m_value == value;
	}

	// ** PrimitiveValue::type
	template<typename TValue>
	typename PrimitiveValue<TValue>::Ptr PrimitiveValue<TValue>::create( const TValue& value )
	{
		return Ptr( DC_NEW PrimitiveValue<TValue>( value ) );
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
		if( m_value == value ) {
			return;
		}

		m_value = value;
		notifyValueChanged();
	}

	// ** operator == ( PrimitiveValue<TValue>&, TValue& )
	template<typename TValue>
	inline bool operator == ( const TValue& b, const PrimitiveValue<TValue>& a )
	{
		return a.get() == b;
	}

	// ---------------------------------------------------------- ObjectValue ---------------------------------------------------------- //

	//! ObjectValue class is a container of named properties.
	class ObjectValue : public Value {
	public:

											//! Constructs ObjectValue instance.
											ObjectValue( void );

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

		//! Sets the property value.
		void								set( const String& name, ValuePtr value );

		//! Resolves the property value with specified URI.
		ValueWPtr							resolve( const String& uri ) const;

	protected:

		//! Returns true if the object is valid.
		virtual bool						validate( void ) const;

		//! Adds a new typed property to this object.
		template<typename TValue, typename ... Args>
		typename WeakPtr<TValue>			add( const String& name, const Args& ... args );

	protected:

		//! Container type to store object properties.
		typedef Map<String, ValuePtr>		Properties;

		Boolean::WPtr						m_isValid;		//!< This property is updated by the data validation routine.
		Properties							m_properties;	//!< Object properties.
	};

	// ** ObjectValue::add
	template<typename TValue, typename ... Args>
	WeakPtr<TValue> ObjectValue::add( const String& name, const Args& ... args )
	{
		DC_BREAK_IF( has( name ) );

		StrongPtr<TValue> value( TValue::create( args... ) );
		set( name, value );
		return value;
	}

	// ---------------------------------------------------------- Object ---------------------------------------------------------- //

	//! Generic object type used for object type declarations.
	template<typename TObject>
	class Object : public ObjectValue {
	public:

		typedef TObject			Type;		//!< Alias for this type.
		typedef StrongPtr<Type>	Ptr;		//!< Strong pointer type.
		typedef WeakPtr<Type>	WPtr;		//!< Weak pointer type.

		//! Returns the actual object value type index.
		virtual ValueTypeIdx	type( void ) const;

		//! Returns true if the object type matches the specified one.
		virtual bool			is( ValueTypeIdx expected ) const;

		//! Generic object constructor
		template<typename ... Args>
		static Ptr				create( const Args& ... args )
		{
			Ptr result = DC_NEW Type( args... );
			result->check();
			return result;
		}
	};

	// ** Object::type
	template<typename TObject>
	ValueTypeIdx Object<TObject>::type( void ) const
	{
		return valueType<TObject>();
	}

	// ** Object::is
	template<typename TObject>
	bool Object<TObject>::is( ValueTypeIdx expected ) const
	{
		if( expected == valueType<TObject>() ) {
			return true;
		}

		return ObjectValue::is( expected );
	}

	// ---------------------------------------------------------- ArrayValue ---------------------------------------------------------- //

	//! Array value type.
	template<typename TValue>
	class ArrayValue : public ObjectValue {
	public:

		typedef ArrayValue<TValue>	Type;		//!< Alias for this type.
		typedef StrongPtr<Type>		Ptr;		//!< Strong pointer type.
		typedef WeakPtr<Type>		WPtr;		//!< Weak pointer type.
		typedef StrongPtr<TValue>	ItemType;	//! Alias the item type.

		//! Returns array value type.
		virtual ValueTypeIdx		type( void ) const;

		//! Returns true if the array matches the expected type.
		virtual bool				is( ValueTypeIdx expected ) const;

		//! Returns array size.
		s32							size( void ) const;

		//! Returns the value by index.
		const ItemType&				get( s32 index ) const;

		//! Pushes a new value to an array.
		void						push( const ItemType& value );

		//! Creates the ArrayValue instance.
		static Ptr					create( void );

	protected:

									//! Constructs ArrayValue instance.
									ArrayValue( void ) {}

	protected:

		Array<ItemType>				m_values;	//!< Actual values stored inside the array.
	};

	// ** ArrayValue::create
	template<typename TValue>
	typename ArrayValue<TValue>::Ptr ArrayValue<TValue>::create( void )
	{
		return Ptr( DC_NEW Type );
	}

	// ** ArrayValue::type
	template<typename TValue>
	ValueTypeIdx ArrayValue<TValue>::type( void ) const
	{
		return Value::valueType<ArrayValue>();
	}

	// ** ArrayValue::is
	template<typename TValue>
	bool ArrayValue<TValue>::is( ValueTypeIdx expected ) const
	{
		ValueTypeIdx actual = valueType< ArrayValue<TValue> >();

		if( expected == actual ) {
			return true;
		}

		return Value::is( expected );
	}

	// ** ArrayValue::size
	template<typename TValue>
	s32 ArrayValue<TValue>::size( void ) const
	{
		return ( s32 )m_values.size();
	}

	// ** ArrayValue::size
	template<typename TValue>
	const typename ArrayValue<TValue>::ItemType& ArrayValue<TValue>::get( s32 index ) const
	{
		DC_BREAK_IF( index < 0 || index >= size() );
		return m_values[index];
	}

	// ** ArrayValue::push
	template<typename TValue>
	void ArrayValue<TValue>::push( const ItemType& value )
	{
		m_values.push_back( value );
		notifyValueChanged();
	}

	// ---------------------------------------------------------- CommandValue ---------------------------------------------------------- //

	//! Command value wraps the functional object inside and used to bind commands with widgets.
	class CommandValue : public Value {
	public:

		//! Returns command value type.
		virtual ValueTypeIdx	type( void ) const;

		//! Returns true if the command matches the expected type.
		virtual bool			is( ValueTypeIdx expected ) const;

		//! Invokes the command instance.
		virtual void			invoke( void ) = 0;
	};

	//! Command with a functional object inside.
	class Command : public CommandValue {
	public:

		
		typedef cClosure<void()>	Callback;	//!< Command callback type.
		typedef StrongPtr<Command>	Ptr;		//!< Alias the strong pointer type.


									//! Constructs the Command instance.
									Command( const Callback& callback );

		//! Invokes the functional object.
		virtual void				invoke( void );

		//! Creates the Command instance.
		static Ptr					create( const Callback& callback );

	protected:

		Callback					m_callback;	//!< Functional object to be called.
	};

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_Value_H__   */