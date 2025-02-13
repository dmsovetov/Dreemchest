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

#ifndef __DC_Composer_EnumEdit_H__
#define __DC_Composer_EnumEdit_H__

#include "../../../Composer.h"

DC_BEGIN_COMPOSER

namespace Ui {

    //! Widget to edit enumeration properties.
    class EnumEdit : public QComboBox {
    
        Q_OBJECT
        Q_PROPERTY( Variant value READ value WRITE setValue NOTIFY valueChanged USER true )

    public:

                                //! Constructs EnumEdit instance.
                                EnumEdit( const Reflection::Enum* enumeration, QWidget* parent = NULL );

        //! Returns current property value as Variant.
        Variant                 value( void ) const;

        //! Sets property value from a Variant.
        void                    setValue( const Variant& value );

    Q_SIGNALS:

        //! Emitted when the widget value was changed.
        void                    valueChanged( const Variant& value );

    private Q_SLOTS:

        //! Emits the value changed signal with an active enum value converted to Variant.
        void                    currentIndexChanged( const QString& value );

    private:

        const Reflection::Enum* m_enumeration;  //!< Enumeration meta-object.
    };

} // namespace Ui

DC_END_COMPOSER

#endif    /*    !__DC_Composer_StringEdit_H__    */
