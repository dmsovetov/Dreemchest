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

#ifndef __Base_Quat_H__
#define __Base_Quat_H__

DC_BEGIN_DREEMCHEST

    /*!
     Quaternion
     */
    class Quat {
    public:

                    //! Constructs a Quat instance.
                    Quat( void );

                    //! Constructs a Quat instance from coordinates.
                    Quat( float x, float y, float z, float w );

					//! Constructs a Quat instance from vector and scalar.
					Quat( const Vec3& v, float w );

					//! Construxts a Quat instance from an array of 4 floats.
					Quat( const float* v );

		//! Returns conjugated quaternion.
		Quat		operator - ( void ) const;

		//! Multiplies two quaternions.
		Quat		operator * ( const Quat& q ) const;

		//! Converts quaternion to an Euler angles vector, where X is bank, Y is heading and Z is attitude.
		Vec3		euler( void ) const;

		//! Rotates point by a quaternion.
		Vec3		rotate( const Vec3& point ) const;

		//! Creates a rotation around axis quaternion.
		static Quat	rotateAroundAxis( float angle, const Vec3& axis );

    public:

        float       x, y, z, w;
    };

    // ** Quat::Quat
    inline Quat::Quat( void ) : x( 0 ), y( 0 ), z( 0 ), w( 1 ) {

    }

    // ** Quat::Quat
    inline Quat::Quat( float x, float y, float z, float w ) : x( x ), y( y ), z( z ), w( w ) {

    }

    // ** Quat::Quat
    inline Quat::Quat( const Vec3& v, float w ) : x( v.x ), y( v.y ), z( v.z ), w( w ) {

    }

    // ** Quat::Quat
    inline Quat::Quat( const float* v ) : x( v[0] ), y( v[1] ), z( v[2] ), w( v[3] ) {

    }

	// ** Quat::operator -
	inline Quat Quat::operator - ( void ) const {
		return Quat( -x, -y, -z, w );
	}

	// ** Quat::operator *
	inline Quat Quat::operator * ( const Quat& q ) const {
		return Quat( w * q.x + x * q.w + y * q.z - z * q.y,
					 w * q.y + y * q.w + z * q.x - x * q.z,
					 w * q.z + z * q.w + x * q.y - y * q.x,
					 w * q.w - x * q.x - y * q.y - z * q.z );
	}

	// ** Quat::euler
	inline Vec3 Quat::euler( void ) const
	{
		float x2 = x * x;
		float y2 = y * y;
		float z2 = z * z;

		return Vec3(  degrees( atan2( 2*x*w - 2*y*z, 1 - 2*x2 - 2*z2 ) )
					, degrees( atan2( 2*y*w - 2*x*z, 1 - 2*y2 - 2*z2 ) )
					, degrees( asin ( 2*x*y + 2*z*w ) ) );
	}

	// ** Quat::rotate
	inline Vec3 Quat::rotate( const Vec3& v ) const
	{
		Quat q = (*this * Quat( v, 0.0f ) * -*this);
		return Vec3( q.x, q.y, q.z );
	}

	// ** Quat::rotateAroundAxis
	inline Quat Quat::rotateAroundAxis( float angle, const Vec3& axis )
	{
		float alpha = radians( angle );
		float sine	= sinf( alpha / 2.0f );

		return Quat( axis * sine, cosf( alpha / 2.0f ) );
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Color_H__ )  */
