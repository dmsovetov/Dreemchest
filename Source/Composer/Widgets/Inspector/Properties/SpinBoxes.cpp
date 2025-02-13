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

#include "SpinBoxes.h"

DC_BEGIN_COMPOSER

namespace Ui {

// --------------------------------------------------- IntegerEdit --------------------------------------------------- //

// ** IntegerEdit::IntegerEdit
IntegerEdit::IntegerEdit( s32 min, s32 max, s32 step, QWidget* parent )
    : QSpinBox( parent )
{
    setMinimum( min );
    setMaximum( max );
    setSingleStep( step );
    connect( this, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)) );
}

// ** IntegerEdit::value
Variant IntegerEdit::value( void ) const
{
    return Variant::fromValue( QSpinBox::value() );
}

// ** IntegerEdit::setValue
void IntegerEdit::setValue( const Variant& value )
{
    QSpinBox::setValue( value.as<s32>() );
}

// ** IntegerEdit::valueChanged
void IntegerEdit::valueChanged( int value )
{
    Q_EMIT valueChanged( this->value() );
}

// --------------------------------------------------- DoubleEdit --------------------------------------------------- //

// ** DoubleEdit::DoubleEdit
DoubleEdit::DoubleEdit( f64 min, f64 max, f64 step, QWidget* parent )
    : QDoubleSpinBox( parent )
{
    setMinimum( min );
    setMaximum( max );
    setSingleStep( step );
    connect( this, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)) );
}

// ** DoubleEdit::value
Variant DoubleEdit::value( void ) const
{
    return Variant::fromValue( QDoubleSpinBox::value() );
}

// ** DoubleEdit::setValue
void DoubleEdit::setValue( const Variant& value )
{
    QDoubleSpinBox::setValue( value.as<f64>() );
}

// ** DoubleEdit::valueChanged
void DoubleEdit::valueChanged( double value )
{
    Q_EMIT valueChanged( this->value() );
}

} // namespace Ui

DC_END_COMPOSER
