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

#include "QtView.h"
#include "BindingsQt.h"
#include "../Validation.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

// ** QtView::QtView
QtView::QtView( QWidget* widget ) : m_widget( widget )
{

}

// ** QtView::widget
QWidget* QtView::widget( void )
{
	return m_widget;
}

// ** QtView::bindString
void QtView::bindString( const String& target, const String& uri )
{
	Property<String>::WPtr property = findProperty<String>( uri );

	if( !property.valid() ) {
		return;
	}

	bindString( target, property );
}

// ** QtView::bindString
void QtView::bindString( const String& target, StringProperty::WPtr property )
{
	DC_BREAK_IF( !property.valid() );

	if( QLineEdit* widget = m_widget->findChild<QLineEdit*>( target.c_str() ) ) {
		addBinding( DC_NEW QtLineEditBinding( this, widget, property ) );
	}
	else if( QLabel* widget = m_widget->findChild<QLabel*>( target.c_str() ) ) {
		addBinding( DC_NEW QtLabelBinding( this, widget, property ) );
	}
}

// ** QtView::bindStringList
void QtView::bindStringList( const String& target, const String& uri )
{
	Property<StringArray>::WPtr property = findProperty<StringArray>( uri );

	if( !property.valid() ) {
		return;
	}

	QListWidget* widget = m_widget->findChild<QListWidget*>( target.c_str() );
	addBinding( DC_NEW QtListBoxBinding( this, widget, property ) );
}

// ** QtView::bindState
void QtView::bindState( const String& target, const String& uri )
{
	Property<String>::WPtr property = findProperty<String>( uri );

	if( !property.valid() ) {
		return;
	}

	QStackedWidget* widget = m_widget->findChild<QStackedWidget*>( target.c_str() );
	addBinding( DC_NEW QtStackedWidgetBinding( this, widget, property ) );
}

// ** QtView::bindClick
void QtView::bindClick( const String& target, const String& event )
{
	QPushButton* widget = m_widget->findChild<QPushButton*>( target.c_str() );
	addBinding( DC_NEW QtPushButtonBinding( this, widget, WeakPtr<StringProperty>(), event ) );
}

// ** QtView::bindInteger
void QtView::bindInteger( const String& target, const String& uri )
{

}

// ** QtView::bindEnabled
void QtView::bindEnabled( const String& target, const String& uri )
{
	Property<bool>::WPtr property = findProperty<bool>( uri );

	if( !property.valid() ) {
		return;
	}

	QWidget* widget = m_widget->findChild<QWidget*>( target.c_str() );
	addBinding( DC_NEW QtEnabledBinding( this, widget, property ) );
}

} // namespace mvvm

DC_END_DREEMCHEST
