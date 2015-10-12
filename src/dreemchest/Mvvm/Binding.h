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
	class IBinding : public RefCounted {
	public:

                                //! Constructs Binding instance.
                                IBinding( ViewWPtr view )
                                    : m_view( view ) {}
		virtual					~IBinding( void ) {}

        //! Refreshes the bound view.
        virtual void            refreshView( void ) {}

        //! Refreshes the bound property.
        virtual void            refreshProperty( void ) {}

    protected:

        ViewWPtr				m_view; //!< Parent view controller.
	};

    //! A template class to bind a property of a specified type TValue.
    template<typename TValue>
    class Binding : public IBinding {
    friend class Property<TValue>;
    public:

		typedef WeakPtr< Property<TValue> >	BoundProperty;	//!< The bound property type.
		typedef WeakPtr< Binding<TValue> >	WPtr;			//!< Alias the weak pointer type.
        typedef TValue						Value;			//!< Alias the value type.

											//! Constructs Binding.
											Binding( ViewWPtr view, BoundProperty property );
        virtual								~Binding( void );

    protected:

        //! Refreshes the bound data.
        virtual void						refreshView( void );

        //! Handles property change.
        virtual void						handlePropertyChanged( const Value& value );

    protected:

        BoundProperty						m_property; //!< Bound property.
    };

    // ** Binding::Binding
    template<typename T>
    Binding<T>::Binding( ViewWPtr view, BoundProperty property )
        : IBinding( view ), m_property( property )
    {
        if( m_property.valid() ) m_property->subscribe( this );
    }

    // ** Binding::~Binding
    template<typename T>
    Binding<T>::~Binding( void )
    {
        if( m_property.valid() ) m_property->unsubscribe( this );
    }

    // ** Binding::refreshView
    template<typename T>
    void Binding<T>::refreshView( void )
    {
        handlePropertyChanged( m_property->value() );
    }

    // ** Binding::handlePropertyChanged
    template<typename T>
    void Binding<T>::handlePropertyChanged( const Value& property )
    {
        DC_BREAK;
    }

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_Binding_H__   */
