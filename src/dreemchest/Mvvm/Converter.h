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
#include "Property.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	//! Property converter binding.
	template<typename TInput, typename TOutput>
	class Converter : public Binding<TInput> {
	public:

		typedef WeakPtr< Property<TInput> >		Input;	//!< Alias the input property type.
		typedef WeakPtr< Property<TOutput> >	Output;	//!< Alias the output property type.

												//! Constructs Converter instance.
												Converter( ViewWPtr view, Input input, Output output = Output() )
													: Binding<TInput>( view, input ), m_output( output.valid() ? output : &m_internal ), m_internal( NULL ) {}

		//! Returns the output value.
		Output									value( void ) const { return m_output; }

	protected:

		Property<TOutput>						m_internal;	//!< Internal output target.
		Output									m_output;	//!< Active output target.
	};

	//! Guid to string converter
	class GuidToStringConverter : public Converter<Guid, String> {
	public:

									//! Constructs GuidToStringConverter instance.
									GuidToStringConverter( ViewWPtr view, const Input& input, Output output = Output() )
										: Converter( view, input, output ) {}

	private:

		//! Converts the GUID to a string.
		virtual void				handlePropertyChanged( const Value& value )
		{
			m_output->set( value.toString() );
		}							
	};

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_Converter_H__   */