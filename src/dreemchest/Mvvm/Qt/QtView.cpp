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

// ** QtView::bind
void QtView::bind( const String& target, StringProperty& property )
{
	View::bind<QtLineEditBinding>( target, property );
}

// ** QtView::bind
void QtView::bindEnabled( const String& target, BoolProperty& property )
{
	View::bind<QtEnabledBinding>( target, property );
}

// ** QtView::bind
void QtView::validateSize( const StringProperty& target, BoolProperty& property, s32 min, s32 max )
{
	View::bind< SizeValidator<String> >( target, &property, min, max );
}

} // namespace mvvm

DC_END_DREEMCHEST
