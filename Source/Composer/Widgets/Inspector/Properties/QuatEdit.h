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

#ifndef __DC_Composer_QuatEdit_H__
#define __DC_Composer_QuatEdit_H__

#include "VectorEdit.h"

DC_BEGIN_COMPOSER

namespace Ui {

    //! Widget to edit string properties.
    class QuatEdit : public AbstractVectorEdit {
    public:

                            //! Constructs QuatEdit instance.
                            QuatEdit( QWidget* parent = NULL );

    private:

        //! Converts the internal array of floats to a variant value.
        virtual Variant     toVariant( const f32* value ) const Q_DECL_OVERRIDE;

        //! Convets variant value to an array of floats.
        virtual void        fromVariant( const Variant& input, f32* value ) Q_DECL_OVERRIDE;
    };

} // namespace Ui

DC_END_COMPOSER

#endif    /*    !__DC_Composer_QuatEdit_H__    */
