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

#ifndef __DC_Mvvm_Validation_H__
#define __DC_Mvvm_Validation_H__

#include "Binding.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

    //! A base template class for all property validators.
    template<typename T>
    class GenericValidator : public GenericConverter<T, bool> {
    public:

                                //! Constructs GenericValidator instance.
                                GenericValidator( View* view, const GenericProperty<T>& property, const BoolProperty* isValid )
                                    : GenericConverter<T, bool>( view, property, isValid ) {}

        //! Validates property.
        virtual bool validate( const T& value ) const
        {
            return false;
        }

    private:

        //! Handles property change and validates it's value.
        virtual void handlePropertyChanged( const T& value )
        {
            this->m_output->set( validate( value ) );
        }
    };

    //! Ensures that a property value matches a reference.
    template<typename T>
    class EqualityValidator : public GenericValidator<T> {
    public:

                                //! Constructs EqualityValidator instance.
                                EqualityValidator( View* view, const GenericProperty<T>& property, const BoolProperty* isValid, const T& reference )
                                    : GenericValidator<T>( view, property, isValid ), m_reference( reference ) {}

    private:

        //! Compares to values.
        virtual bool validate( const T& value ) const
        {
            return value == m_reference;
        }

    private:

        T                       m_reference;    //!< Reference property value.
    };

	//! Ensures that a container size is inside the specified range.
	template<typename T>
	class SizeValidator : public GenericValidator<T> {
	public:

								//! Constructs SizeValidator instance.
								SizeValidator( View* view, const GenericProperty<T>& property, const BoolProperty* isValid, u32 min = 0, u32 max = UINT_MAX )
									: GenericValidator<T>( view, property, isValid ), m_min( min ), m_max( max ) {}

		//! Checks the container size.
		virtual bool validate( const T& value ) const
		{
			return value.size() >= m_min && value.size() <= m_max;
		}

	private:

		u32						m_min;	//!< Minimum size value.
		u32						m_max;	//!< Maximum size value.
	};

} // namespace mvvm

DC_END_DREEMCHEST

#endif // __DC_Mvvm_Validation_H__
