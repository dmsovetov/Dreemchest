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

	//! Observable value base class.
	class ValueBase : public RefCounted {
	public:

		//! Notifies bindings about a change.
		void				notify( void ) const;

		//! Returns the value type.
		virtual ValueType	type( void ) const { return 0; }

		//! Generates the value type id.
		template<typename TValue>
		static ValueType	valueType( void );
	};

	// ** ValueBase::valueType
	template<typename TValue>
	ValueType ValueBase::valueType( void )
	{
		return GroupedTypeIndex<TValue, ValueBase>::idx();
	}

	//! Generic value type used for primitive type declaration.
	template<typename TValue>
	class Value : public ValueBase {
	public:

		//! Returns the value type generated from TValue.
		virtual ValueType	type( void ) const;

		//! Returns property value.
		const TValue&		value( void ) const;
		TValue&				value( void );

		//! Sets property value.
		void				set( const TValue& value );

	protected:

		TValue				m_value;	//!< Actual value.
	};

	// ** Value::type
	template<typename TValue>
	ValueType Value<TValue>::type( void ) const
	{
		return ValueBase::valueType<TValue>();
	}

	// ** Value::value
	template<typename TValue>
	const TValue& Value<TValue>::value( void ) const
	{
		return m_value;
	}

	// ** Value::value
	template<typename TValue>
	TValue& Value<TValue>::value( void )
	{
		return m_value;
	}

	// ** Value::set
	template<typename TValue>
	void Value<TValue>::set( const TValue& value )
	{
		if( m_value == value ) {
			return;
		}

		m_value = value;
		notify();
	}

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_Value_H__   */