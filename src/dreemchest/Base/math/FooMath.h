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

#ifndef __Base_Math_H__
#define __Base_Math_H__

DC_BEGIN_DREEMCHEST

    const float Pi		= 3.1415926535897932f;
	const float Epsilon	= 0.0001f;

	//! Calculates the next power of two of a given number.
	inline unsigned int nextPowerOf2( unsigned int n )
	{
		unsigned count = 0;

		/* First n in the below condition is for the case where n is 0*/
		if( n && !(n & (n - 1)) ) {
			return n;
		}

		while( n != 0 )
		{
			n >>= 1;
			count += 1;
		}
    
		return 1 << count;
	}

    //! Generates a random value in a [0, 1] range.
    inline float rand0to1( void ) {
        static float invRAND_MAX = 1.0f / RAND_MAX;
        return rand() * invRAND_MAX;
    }

    //! Does a linear interpolation between two values.
	template<typename TValue>
    TValue lerp( TValue a, TValue b, f32 scalar )
	{
        return static_cast<TValue>( a * (1.0f - scalar) + b * scalar );
    }

	//! Returns true if three float values are equal.
	inline float equal3( float a, float b, float c, float eps = 0.001f )
	{
		return fabs( a - b ) <= eps && fabs( b - c ) <= eps && fabs( c - a ) <= eps;
	}

	//! Rounds the floating point number to a specified decimal place.
	inline f32 round( f32 a, u32 decimal )
	{
		f32 multipler = powf( 10.0f, ( f32 )decimal );
		return floor( a * multipler ) / multipler;
	}

    //! Returns a minimum value of two.
	template<typename TValue>
    TValue min2( const TValue& a, const TValue& b ) {
        return a < b ? a : b;
    }

    //! Returns a minimum value of three.
	template<typename TValue>
    TValue min3( const TValue& a, const TValue& b, const TValue& c ) {
        return min2( a, min2( b, c ) );
    }

    //! Returns a maximum value of two.
	template<typename TValue>
    TValue max2( const TValue& a, const TValue& b ) {
        return a > b ? a : b;
    }

    //! Returns a maximum value of three.
	template<typename TValue>
    TValue max3( const TValue& a, const TValue& b, const TValue& c ) {
        return max2( a, max2( b, c ) );
    }

	//! Clamps the value to a specified range.
	template<typename TValue>
	TValue clamp( const TValue& value, const TValue& min, const TValue& max ) {
		return min2( max2( value, min ), max );
	}

    //! Converts degrees to radians
    inline float radians( float degrees ) {
        return (degrees) / 180.0f * Pi;
    }

    //! Converts radians to degrees
    inline float degrees( float radians ) {
        return (radians) * 180.0f / Pi;
    }

	//! Returns true if an argument is not a number.
	inline bool isNaN( float value )
	{
	#ifdef WIN32
		return _isnan( value ) ? true : false;
	#else
		return isnan( value );
	#endif
	}

    //! Helper struct to hold a numeric range.
    struct Range {
                Range( f32 min = -FLT_MAX, f32 max = FLT_MAX )
                    : min( min ), max( max ) {}

        f32     min;	//! The minimum range value.
        f32     max;	//! The maximum range value.

		//! Clamps the input value to a range.
		f32		clamp( f32 value ) const { return :: DC_DREEMCHEST_NS clamp( value, min, max ); }

		//! Returns true if the value is inside the range.
		bool	contains( f32 value ) const { return value >= min && value <= max; }
    };

DC_END_DREEMCHEST

#include "Color.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Quat.h"
#include "Matrix4.h"
#include "Bounds.h"
#include "Plane.h"
#include "Graph.h"
#include "FixedPointNumber.h"
#include "Curve.h"

#include "LinearRegression.h"
#include "Vector.h"
#include "Matrix.h"
#include "Samples.h"

#include "Mesh.h"

#endif  /*  !defined( __Base_Math_H__ )  */
