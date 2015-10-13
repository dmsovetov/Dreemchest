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

#ifndef __DC_Mvvm_QtView_H___
#define __DC_Mvvm_QtView_H___

#include "MvvmQt.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	//! Qt view.
/*	class QtView : public View {
	public:

					//! Constructs QtView instance.
					QtView( QWidget* widget );

		//! Returns parent widget.
		QWidget*	widget( void );

		//! Adds a new converter binding to view.
		template<typename TConverter>
		WeakPtr<TConverter> addConverter( const String& target );

		//! Binds the string property to a widget.
		void		bindString( const String& target, const String& uri );

		//! Binds the string property to a widget.
		void		bindString( const String& target, StringProperty::WPtr property );

		//! Binds the int property to a widget.
		void		bindInteger( const String& target, const String& uri );

		//! Binds the string state to a widget.
		void		bindState( const String& target, const String& uri );

		//! Binds the click event.
		void		bindClick( const String& target, const String& event );

		//! Binds the list of strings to a widget.
		void		bindStringList( const String& target, const String& uri );

		//! Binds enabled state to a property.
		void		bindEnabled( const String& target, const String& uri );

	private:

		QWidget*	m_widget;	//!< Parent widget.
	};

	// ** QtView::addConverter
	template<typename TConverter>
	WeakPtr<TConverter>  QtView::addConverter( const String& target )
	{
		typename TConverter::Input input = findProperty<typename TConverter::Value>( target );

		if( !input.valid() ) {
			return WeakPtr<TConverter>();
		}

		TConverter* converter = DC_NEW TConverter( this, input );
		addBinding( converter );

		return converter;
	}*/

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_QtView_H___    */