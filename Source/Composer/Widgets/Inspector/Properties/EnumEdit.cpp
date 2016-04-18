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

#include "EnumEdit.h"
#include "../../../Models/EnumerationModel.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ** EnumEdit::EnumEdit
EnumEdit::EnumEdit( const Reflection::Enum* enumeration, QWidget* parent )
    : QComboBox( parent )
    , m_enumeration( enumeration )
{
    setModel( new EnumeraionModel( enumeration ) );
    connect( this, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(currentIndexChanged(const QString&)) );
}

// ** EnumEdit::value
Variant EnumEdit::value( void ) const
{
    return m_enumeration->valueFromString( currentText().toStdString() );
}

// ** EnumEdit::setValue
void EnumEdit::setValue( const Variant& value )
{
    setCurrentIndex( value.as<s32>() );
}

// ** EnumEdit::currentIndexChanged
void EnumEdit::currentIndexChanged( const QString& value )
{
    Q_EMIT valueChanged( this->value() );
}

} // namespace Ui

DC_END_COMPOSER