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

#include "Parameter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

// ** FloatParameter::setData
void FloatParameter::setData( const FloatArray& value, CurveIndex index )
{
    CurveType& target = this->curve( index );

    target.clear();
    for( s32 i = 0, n = ( s32 )value.size(); i < n / 2; i++ ) {
        target.push( value[i * 2 + 0], value[i * 2 + 1] );
    }
}

// ** FloatParameter::setCurve
void FloatParameter::setCurve( const FloatArray& value )
{
    setSamplingMode( SampleCurve );
    setData( value, Lower );
}

// ** FloatParameter::setRandomBetweenCurves
void FloatParameter::setRandomBetweenCurves( const FloatArray& min, const FloatArray& max )
{
    setSamplingMode( SampleRandomBetweenCurves );
    setData( min, Lower );
    setData( max, Upper );
}

// ** FloatParameter::setRandomBetweenConstants
void FloatParameter::setRandomBetweenConstants( f32 min, f32 max )
{
    setSamplingMode( SampleRandomBetweenConstants );
    curve( Lower ).clear();
    curve( Lower ).push( 0.0f, min );
    curve( Upper ).clear();
    curve( Upper ).push( 0.0f, max );
}

// ** Vec3Parameter::setData
void Vec3Parameter::setData( const FloatArray& value, CurveIndex index )
{
    CurveType& target = this->curve( index );

    target.clear();
    for( s32 i = 0, n = ( s32 )value.size(); i < n / 4; i++ ) {
        target.push( value[i * 4 + 0], Vec3( value[i * 4 + 1], value[i * 4 + 2], value[i * 4 + 3] ) );
    }
}

// ** RgbParameter::setData
void RgbParameter::setData( const FloatArray& value, CurveIndex index )
{
    CurveType& target = this->curve( index );

    target.clear();
    for( s32 i = 0, n = ( s32 )value.size(); i < n / 4; i++ ) {
        target.push( value[i * 4 + 0], Rgb( value[i * 4 + 1], value[i * 4 + 2], value[i * 4 + 3] ) );
    }
}

// ** RgbParameter::setCurve
void RgbParameter::setCurve( const FloatArray& value )
{
    setSamplingMode( SampleCurve );
    setData( value, Lower );
}

// ** RgbParameter::setRandomBetweenCurves
void RgbParameter::setRandomBetweenCurves( const FloatArray& min, const FloatArray& max )
{
    setSamplingMode( SampleRandomBetweenCurves );
    setData( min, Lower );
    setData( max, Upper );
}

} // namespace Fx

DC_END_DREEMCHEST
