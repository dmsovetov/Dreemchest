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

#ifndef __DC_Composer_VectorEdit_H__
#define __DC_Composer_VectorEdit_H__

#include "../../../Composer.h"

DC_BEGIN_COMPOSER

namespace Ui {

    //! Base class for widgets used for editing vector properties.
    class AbstractVectorEdit : public QWidget {
    
        Q_OBJECT
        Q_PROPERTY( Variant value READ value WRITE setValue NOTIFY valueChanged USER true )

    public:

                            //! Constructs AbstractVectorEdit instance.
                            AbstractVectorEdit( s32 size, QWidget* parent = NULL );

        //! Returns current vector value.
        Variant             value( void ) const;

        //! Sets vector value.
        void                setValue( const Variant& value );

    Q_SIGNALS:

        //! Emitted when the vector value was changed.
        void                valueChanged( const Variant& value );

    private Q_SLOTS:

        //! Updates the vector component value.
        void                componentTextChanged( const QString& value );

    private:

        //! Updates line edits from value.
        void                updateLineEdits( void );

        //! Converts the internal array of floats to a variant value.
        virtual Variant     toVariant( const f32* value ) const = 0;

        //! Convets variant value to an array of floats.
        virtual void        fromVariant( const Variant& input, f32* value ) = 0;

    private:

        QHBoxLayout*        m_layout;       //!< Horizontal box layout.
        QDoubleValidator*   m_validator;    //!< Double validator used by line edits.
        s32                 m_size;         //!< Vector size.
        f32                 m_value[4];     //!< Actual vector value.
        QVector<QLineEdit*> m_edits;        //!< Line edit fields for each vector component.
        QVector<QLabel*>    m_labels;       //!< Labels for each vector component.
    };

    //! Generic type to declare vector property editors.
    template<typename TValue, s32 TSize>
    class GenericVectorEdit : public AbstractVectorEdit {
    public:

                            //! Constructs GenericVectorEdit instance
                            GenericVectorEdit( QWidget* parent = NULL );

    private:

        //! Converts the internal array of floats to a variant value.
        virtual Variant     toVariant( const f32* value ) const DC_DECL_OVERRIDE;

        //! Convets variant value to an array of floats.
        virtual void        fromVariant( const Variant& input, f32* value ) DC_DECL_OVERRIDE;
    };

    // ** GenericVectorEdit::GenericVectorEdit
    template<typename TValue, s32 TSize>
    GenericVectorEdit<TValue, TSize>::GenericVectorEdit( QWidget* parent )
        : AbstractVectorEdit( TSize, parent ) {}

    // ** GenericVectorEdit::toVariant
    template<typename TValue, s32 TSize>
    Variant GenericVectorEdit<TValue, TSize>::toVariant( const f32* value ) const
    {
        return Variant::fromValue( TValue( value ) );
    }

    // ** GenericVectorEdit::fromVariant
    template<typename TValue, s32 TSize>
    void GenericVectorEdit<TValue, TSize>::fromVariant( const Variant& input, f32* value )
    {
        TValue v = input.as<TValue>();
        for( s32 i = 0; i < TSize; i++ ) {
            value[i] = v[i];
        }
    }

    typedef GenericVectorEdit<Vec2, 2> Vec2Edit;
    typedef GenericVectorEdit<Vec3, 3> Vec3Edit;
    typedef GenericVectorEdit<Vec4, 4> Vec4Edit;

} // namespace Ui

DC_END_COMPOSER

#endif	/*	!__DC_Composer_VectorEdit_H__	*/