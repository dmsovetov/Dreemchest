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

#ifndef __DC_Composer_SpinBoxes_H__
#define __DC_Composer_SpinBoxes_H__

#include "../../../Composer.h"

DC_BEGIN_COMPOSER

namespace Ui {

    //! Widget to edit integer properties.
    class IntegerEdit : public QSpinBox {

        Q_OBJECT
        Q_PROPERTY( Variant value READ value WRITE setValue NOTIFY valueChanged USER true )

    public:

                            //! Constructs IntegerEdit instance.
                            IntegerEdit( s32 min = INT_MIN, s32 max = INT_MAX, s32 step = 1, QWidget* parent = NULL );

        //! Returns current property value as Variant.
        Variant             value( void ) const;

        //! Sets property value from a Variant.
        void                setValue( const Variant& value );

    Q_SIGNALS:

        //! Emitted when the widget value was changed.
        void                valueChanged( const Variant& value );

    private Q_SLOTS:

        //! Handles the spin box change and emits the valueChanged signal.
        void                valueChanged( int value );
    };

    //! Widget to edit float and double properties
    class DoubleEdit : public QDoubleSpinBox {

        Q_OBJECT
        Q_PROPERTY( Variant value READ value WRITE setValue NOTIFY valueChanged USER true )

    public:

                            //! Constructs DoubleEdit instance.
                            DoubleEdit( f64 min = -std::numeric_limits<f64>::max(), f64 max = std::numeric_limits<f64>::max(), f64 step = 1.0, QWidget* parent = NULL );

        //! Returns current property value as Variant.
        Variant             value( void ) const;

        //! Sets property value from a Variant.
        void                setValue( const Variant& value );

    Q_SIGNALS:

        //! Emitted when the widget value was changed.
        void                valueChanged( const Variant& value );

    private Q_SLOTS:

        //! Handles the spin box change and emits the valueChanged signal.
        void                valueChanged( double value );
    };

} // namespace Ui

DC_END_COMPOSER

#endif    /*    !__DC_Composer_SpinBoxes_H__    */
