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

#include "QuatEdit.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ** QuatEdit::QuatEdit
QuatEdit::QuatEdit( QWidget* parent )
    : AbstractVectorEdit( 3, parent )
{
}

// ** QuatEdit::toVariant
Variant QuatEdit::toVariant( const f32* value ) const
{
    Quat r = Quat::rotateAroundAxis( value[0], Vec3::axisX() ) * Quat::rotateAroundAxis( value[1], Vec3::axisY() ) * Quat::rotateAroundAxis( value[2], Vec3::axisZ() );
    return Variant::fromValue( r );
}

// ** QuatEdit::fromVariant
void QuatEdit::fromVariant( const Variant& input, f32* value )
{
    Quat r = input.as<Quat>();
    value[0] = r.pitch();
    value[1] = r.yaw();
    value[2] = r.roll();
}

} // namespace Ui

DC_END_COMPOSER