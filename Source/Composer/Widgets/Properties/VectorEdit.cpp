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

#include "VectorEdit.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ** AbstractVectorEdit::AbstractVectorEdit
AbstractVectorEdit::AbstractVectorEdit( s32 size, QWidget* parent )
    : QWidget( parent )
    , m_size( size )
{
    // Zero the value
    memset( m_value, 0, sizeof m_value );

    // Create the validator instance
    m_validator = new QDoubleValidator( -FLT_MAX, FLT_MAX, 5, this );

    // Create a horizontal layout
    m_layout = new QHBoxLayout( this );
    m_layout->setMargin( 0 );
    m_layout->setSpacing( 2 );

    // Create vector component widgets
    CString labels[] = { "X", "Y", "Z", "W" };

    for( s32 i = 0; i < 3; i++ ) {
        QLineEdit* edit  = new QLineEdit;
        QLabel*    label = new QLabel( labels[i] );

        edit->setValidator( m_validator );
        connect( edit, SIGNAL(textChanged(const QString&)), this, SLOT(componentTextChanged(const QString&)) );

        m_layout->addWidget( label );
        m_layout->addWidget( edit );

        m_labels.push_back( label );
        m_edits.push_back( edit );
    }

    updateLineEdits();
}

// ** AbstractVectorEdit::value
Variant AbstractVectorEdit::value( void ) const
{
    return toVariant( m_value );
}

// ** AbstractVectorEdit::setValue
void AbstractVectorEdit::setValue( const Variant& value )
{
    fromVariant( value, m_value );
    Q_EMIT valueChanged( value );
    updateLineEdits();
}

// ** AbstractVectorEdit::componentTextChanged
void AbstractVectorEdit::componentTextChanged( const QString& value )
{
    QLineEdit* edit = qobject_cast<QLineEdit*>( sender() );
    s32        idx  = m_edits.indexOf( edit );
    f32        v    = value.toFloat();

    if( m_value[idx] == v ) {
        return;
    }

    m_value[idx] = v;
    setValue( toVariant( m_value ) );
}

// ** AbstractVectorEdit::updateLineEdits
void AbstractVectorEdit::updateLineEdits( void )
{
    for( s32 i = 0, n = m_edits.size(); i < n; i++ ) {
        m_edits[i]->setText( QString::number( m_value[i] ) );
    }
}

} // namespace Ui

DC_END_COMPOSER