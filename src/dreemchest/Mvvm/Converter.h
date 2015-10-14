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

#ifndef __DC_Mvvm_Converter_H__
#define __DC_Mvvm_Converter_H__

#include "Binding.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	//! Property converter binding.
	template<typename TConverter, typename TInput, typename TOutput>
	class Converter : public Binding<TConverter, TInput> {
	public:

		typedef WeakPtr<TInput>					InputWPtr;	//!< Input weak pointer type.
		typedef WeakPtr<TOutput>				OutputWPtr;	//!< Output weak pointer type.


												//! Constructs Converter instance.
												Converter( void );

		//! Returns the output value.
		OutputWPtr								output( void ) const;

		//! Sets the converter output.
		void									setOutput( OutputWPtr value );

	protected:

		typename TOutput::Ptr					m_internal;	//!< Internal output target.
		OutputWPtr								m_output;	//!< Active output target.
	};

	// ** Converter::Converter
	template<typename TConverter, typename TInput, typename TOutput>
	Converter<TConverter, TInput, TOutput>::Converter( void )
	{
		m_internal = TOutput::create();
		m_output   = m_internal;
	}

	// ** Converter::output
	template<typename TConverter, typename TInput, typename TOutput>
	WeakPtr<TOutput> Converter<TConverter, TInput, TOutput>::output( void ) const
	{
		return m_output;
	}

	// ** Converter::setOutput
	template<typename TConverter, typename TInput, typename TOutput>
	void Converter<TConverter, TInput, TOutput>::setOutput( OutputWPtr value )
	{
		m_output = value;

		if( !m_output.valid() ) {
			m_output = m_internal;
		}
	}

	//! Converts integer to a text.
	class IntegerToTextConverter : public Converter<IntegerToTextConverter, Integer, Text> {
	protected:

		//! Converts the integer to a string.
		virtual void				handleValueChanged( void );		
	};

	// ** IntegerToTextConverter::handleValueChanged
	inline void IntegerToTextConverter::handleValueChanged( void )
	{
		m_output->set( std::to_string( m_property->get() ) );
	}

	//! Guid to a text converter
	class GuidToTextConverter : public Converter<GuidToTextConverter, Guid, Text> {
	protected:

		//! Converts the GUID to a string.
		virtual void				handleValueChanged( void );						
	};

	// ** GuidToTextConverter::handleValueChanged
	inline void GuidToTextConverter::handleValueChanged( void )
	{
		m_output->set( m_property->get().toString() );
	}

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_Converter_H__   */