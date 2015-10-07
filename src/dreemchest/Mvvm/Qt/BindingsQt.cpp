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

#include "BindingsQt.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {


// ---------------------------------------- QtVisibilityBinding ---------------------------------------- //

// ** QtVisibilityBinding::QtVisibilityBinding
QtVisibilityBinding::QtVisibilityBinding( View* view, const String& name, const Property& property, bool inversed )
    : QtPropertyBinding( view, name, property ), m_isInversed( inversed )
{

}

// ** QtVisibilityBinding::handlePropertyChanged
void QtVisibilityBinding::handlePropertyChanged( const Value& value )
{
    m_widget->setVisible( m_isInversed ? !value : value );
}

// ----------------------------------------- QtEnabledBinding ---------------------------------------- //

// ** QtEnabledBinding::QtEnabledBinding
QtEnabledBinding::QtEnabledBinding( View* view, const String& name, const Property& property, bool inversed )
    : QtPropertyBinding( view, name, property ), m_isInversed( inversed )
{

}

// ** QtEnabledBinding::handlePropertyChanged
void QtEnabledBinding::handlePropertyChanged( const Value& value )
{
    m_widget->setEnabled( m_isInversed ? !value : value );
}

/*
// ---------------------------------------- MyGUIEditViewBinding ---------------------------------------- //

// ** MyGUIEditViewBinding::MyGUIEditViewBinding
MyGUIEditViewBinding::MyGUIEditViewBinding( View* view, const String& name, const Property& property )
	: MyGUIPropertyBinding( view, name, property )
{

}

// ** MyGUIEditViewBinding::handlePropertyChanged
void MyGUIEditViewBinding::handlePropertyChanged( const Value& value )
{
    m_widget->setCaption( "" );

	for( int i = 0, n = ( int )value.size(); i < n; i++ ) {
		m_widget->addText( value[i] + "\n" );
	}

	m_widget->setTextCursor( m_widget->getCaption().length() );
}

// ----------------------------------------- MyGUICaptionBinding ----------------------------------------- //

// ** MyGUICaptionBinding::MyGUICaptionBinding
MyGUICaptionBinding::MyGUICaptionBinding( View* view, const String& name, const Property& property )
    : MyGUIPropertyBinding( view, name, property )
{

}

// ** MyGUICaptionBinding::handlePropertyChanged
void MyGUICaptionBinding::handlePropertyChanged( const Value& value )
{
    if( value != m_widget->getCaption().asUTF8() ) {
        m_widget->setCaption( value );
    }
}
*/
// ----------------------------------------- QtLineEditBinding ----------------------------------------- //

// ** QtLineEditBinding::QtLineEditBinding
QtLineEditBinding::QtLineEditBinding( View* view, const String& name, const Property& property )
    : QtPropertyBinding( view, name, property )
{
//    m_widget->eventEditTextChange += MyGUI::newDelegate( this, &MyGUILineEditBinding::handleTextChanged );
}

QtLineEditBinding::~QtLineEditBinding( void )
{
 //   m_widget->eventEditTextChange -= MyGUI::newDelegate( this, &MyGUILineEditBinding::handleTextChanged );
}

// ** QtLineEditBinding::handlePropertyChanged
void QtLineEditBinding::handlePropertyChanged( const Value& value )
{
    if( value.c_str() != m_widget->text() ) {
        m_widget->setText( value.c_str() );
    }
}

// ** QtLineEditBinding::handleTextChanged
void QtLineEditBinding::handleTextChanged( QLineEdit* sender )
{
//    m_property.set( m_widget->getCaption() );
}
/*
// ------------------------------------------ MyGUIButtonBinding ------------------------------------------ //

// ** MyGUIButtonBinding::MyGUIButtonBinding
MyGUIButtonBinding::MyGUIButtonBinding( View* view, const String& name, const String& event ) : Binding( view ), m_event( event )
{
	m_button = static_cast<MyGUI::Button*>( static_cast<MyGUIView*>( view )->widget()->findWidget( name ) );
	DC_BREAK_IF( m_button == NULL );

	m_button->eventMouseButtonClick += MyGUI::newDelegate( this, &MyGUIButtonBinding::handleClick );
}

MyGUIButtonBinding::~MyGUIButtonBinding( void )
{
	m_button->eventMouseButtonClick -= MyGUI::newDelegate( this, &MyGUIButtonBinding::handleClick );
}

// ** MyGUIButtonBinding::handleClick
void MyGUIButtonBinding::handleClick( MyGUI::Widget* sender )
{
    m_view->notify( m_event );
}*/

} // namespace mvvm

DC_END_DREEMCHEST
