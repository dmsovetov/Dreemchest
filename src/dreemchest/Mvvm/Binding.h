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

#include "Mvvm.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	//! Base property binding.
	class Binding : public RefCounted {
	public:

                                //! Constructs Binding instance.
                                Binding( View* view )
                                    : m_view( view ) {}
		virtual					~Binding( void ) {}

        //! Refreshes the bound view.
        virtual void            refreshView( void ) {}

        //! Refreshes the bound property.
        virtual void            refreshProperty( void ) {}

    protected:

        View*                   m_view; //!< Parent view controller.
	};

    //! A template class to bind a property of a specified type TProperty.
    template<typename T>
    class GenericBinding : public Binding {
    friend class GenericProperty<T>;
    public:

        //! Alias the generic property type.
        typedef GenericProperty<T> Property;

        //! Alias the value type.
        typedef T				Value;

                                //! Constructs GenericBinding.
                                GenericBinding( View* view, const Property& property );
        virtual                 ~GenericBinding( void );

    protected:

        //! Refreshes the bound data.
        virtual void            refreshView( void );

        //! Handles property change.
        virtual void            handlePropertyChanged( const Value& value );

    protected:

        Property&               m_property; //!< Bound property.
    };

    // ** GenericBinding::GenericBinding
    template<typename T>
    GenericBinding<T>::GenericBinding( View* view, const Property& property )
        : Binding( view ), m_property( const_cast<Property&>( property ) )
    {
        m_property.subscribe( this );
    }

    // ** GenericBinding::~GenericBinding
    template<typename T>
    GenericBinding<T>::~GenericBinding( void )
    {
        m_property.unsubscribe( this );
    }

    // ** GenericBinding::refreshView
    template<typename T>
    void GenericBinding<T>::refreshView( void )
    {
        handlePropertyChanged( m_property.value() );
    }

    // ** GenericBinding::handlePropertyChanged
    template<typename T>
    void GenericBinding<T>::handlePropertyChanged( const Value& property )
    {
        DC_BREAK;
    }

	//! Property converter binding.
	template<typename TInput, typename TOutput>
	class GenericConverter : public GenericBinding<TInput> {
	public:

									//! Constructs GenericConverter instance.
									GenericConverter( View* view, const GenericProperty<TInput>& input, const GenericProperty<TOutput>* output = NULL )
										: GenericBinding<TInput>( view, input ), m_output( const_cast<GenericProperty<TOutput>*>( output ? output : &m_internal ) ), m_internal( NULL ) {}

		//! Returns the output value.
		const GenericProperty<TOutput>&	value( void ) const { return *m_output; }

	protected:

		GenericProperty<TOutput>	m_internal;	//!< Internal output target.
		GenericProperty<TOutput>*	m_output;	//!< Active output target.
	};

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_Binding_H__   */
