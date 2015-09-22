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

#ifndef ___Base_Matrix4_H__
#define ___Base_Matrix4_H__

DC_BEGIN_DREEMCHEST

    class Vec3;
	class Vec4;
    class Quat;

    /*!
     Affine transform matrix.
     */
    class Matrix4 {
    public:

                        //! Constructs an identity matrix.
                        Matrix4( void );

                        //! Constructs a matrix.
                        Matrix4( float t11, float t12, float t13, float t14,
                                 float t21, float t22, float t23, float t24,
                                 float t31, float t32, float t33, float t34,
                                 float t41, float t42, float t43, float t44 );

                        //! Constructs a rotation matrix from quaternion
                        Matrix4( const Quat& quat );

        float           operator[] ( int index ) const;
        float&          operator[] ( int index );

		//! Multiplies two 4x4 matrices.
        Matrix4         operator * ( const Matrix4& other ) const;

		//! Multiplies a matrix by a 3-component vector.
        Vec3            operator * ( const Vec3& v ) const;
		
		//! Multiplies a matrix by a homogeneous coordinate vector.
		Vec4            operator * ( const Vec4& v ) const;

		//! Returns the matrix row by index.
		Vec4			row( int index ) const;

        //! Calculate the inverse matrix.
        Matrix4         inversed( void ) const;

		//! Calculate the transpose matrix.
		Matrix4			transposed( void ) const;

		//! Sets the matrix row by index.
		void			setRow( int index, const Vec4& value );

		//! Returns the matrix column by index.
		Vec4			column( int index ) const;

		//! Sets the matrix column by index.
		void			setColumn( int index, const Vec4& value );

        //! Rotates vector.
        Vec3            rotate( const Vec3& v ) const;

        //! Constructs a translation transform matrix.
        static Matrix4  translation( float x, float y, float z );

        //! Constructs a translation transform matrix.
        static Matrix4  translation( const Vec3& position );

        //! Constructs a scale transform matrix.
        static Matrix4  scale( float x, float y, float z );

        //! Constructs a scale transform matrix.
        static Matrix4  scale( const Vec3& scale );

        //! Creates a perspective projection matrix.
        static Matrix4  perspective( float fov, float aspect, float zNear, float zFar );

        //! Creates a left-handed perspective projection matrix.
        static Matrix4  perspectiveLeft( float fov, float aspect, float zNear, float zFar );

        //! Creates a ortho projection matrix.
        static Matrix4  ortho( float left, float right, float bottom, float top, float zNear, float zFar );

        //! Creates a look at view matrix.
        static Matrix4  lookAt( const Vec3& position, const Vec3& target, const Vec3& up );

        //! Creates a left-handed look at view matrix.
        static Matrix4  lookAtLeft( const Vec3& position, const Vec3& target, const Vec3& up );

		//! Creates a view matrix from basis vectors.
		static Matrix4	view( const Vec3& position, const Vec3& direction, const Vec3& up, const Vec3& right );

		//! Creates a left-handed view matrix from basis vectors.
		static Matrix4	viewLeft( const Vec3& position, const Vec3& direction, const Vec3& up, const Vec3& right );

        //! Calculates an affine transform matrix from components.
        static Matrix4  affineTransform( const Vec3& position, const Quat& rotation, const Vec3& scale );

    public:

        float           m[16];
    };

    // ** Matrix4::Matrix4
    inline Matrix4::Matrix4( void )
    {
        m[0 ] = 1.0f; m[1 ] = 0.0f; m[2 ] = 0.0f; m[3 ] = 0.0f;
        m[4 ] = 0.0f; m[5 ] = 1.0f; m[6 ] = 0.0f; m[7 ] = 0.0f;
        m[8 ] = 0.0f; m[9 ] = 0.0f; m[10] = 1.0f; m[11] = 0.0f;
        m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
    }

    // ** Matrix4::Matrix4
    inline Matrix4::Matrix4( float t11, float t12, float t13, float t14,
                             float t21, float t22, float t23, float t24,
                             float t31, float t32, float t33, float t34,
                             float t41, float t42, float t43, float t44 ) {
        m[ 0] = t11; m[ 1] = t12; m[ 2] = t13; m[ 3] = t14;
        m[ 4] = t21; m[ 5] = t22; m[ 6] = t23; m[ 7] = t24;
        m[ 8] = t31; m[ 9] = t32; m[10] = t33; m[11] = t34;
        m[12] = t41; m[13] = t42; m[14] = t43; m[15] = t44;
    }

    // ** Matrix4::Matrix4
    inline Matrix4::Matrix4( const Quat& quat )
    {
        float x = quat.x;
        float y = quat.y;
        float z = quat.z;
        float w = quat.w;
		float x2 = x * x;
		float y2 = y * y;
		float z2 = z * z;

        m[0]  = 1.0f - 2.0f * (y2 + z2);
        m[1]  = 2.0f * (x * y + z * w);
        m[2]  = 2.0f * (x * z - y * w);
        m[3]  = 0.0f;

        m[4]  = 2.0f * (x * y - z * w);
        m[5]  = 1.0f - 2.0f * (x2 + z2);
        m[6]  = 2.0f * (z * y + x * w);
        m[7]  = 0.0f;

        m[8]  = 2.0f * (x * z + y * w);
        m[9]  = 2.0f * (y * z - x * w);
        m[10] = 1.0f - 2.0f * (x2 + y2);
        m[11] = 0.0f;

        m[12] = 0;
        m[13] = 0;
        m[14] = 0;
        m[15] = 1.0f;
    }

    // ** Matrix4::operator *
    inline Vec3 Matrix4::operator * ( const Vec3& v ) const {
        Vec3 r;

        r.x = v.x * m[0] + v.y * m[4] + v.z * m[8] + m[12];
        r.y = v.x * m[1] + v.y * m[5] + v.z * m[9] + m[13];
        r.z = v.x * m[2] + v.y * m[6] + v.z * m[10]+ m[14];

        return r;
    }

    // ** Matrix4::operator *
    inline Vec4 Matrix4::operator * ( const Vec4& v ) const {
        Vec4 r;

        r.x = v.x * m[0] + v.y * m[4] + v.z * m[8] + v.w * m[12];
        r.y = v.x * m[1] + v.y * m[5] + v.z * m[9] + v.w * m[13];
        r.z = v.x * m[2] + v.y * m[6] + v.z * m[10]+ v.w * m[14];

        return r;
    }

    // ** Matrix4::operator *
    inline Matrix4 Matrix4::operator * ( const Matrix4& other ) const
    {
        return Matrix4(
                       m[0]*other[0]  + m[4]*other[1]  + m[8]*other[2]   + m[12]*other[3],
                       m[1]*other[0]  + m[5]*other[1]  + m[9]*other[2]   + m[13]*other[3],
                       m[2]*other[0]  + m[6]*other[1]  + m[10]*other[2]  + m[14]*other[3],
                       m[3]*other[0]  + m[7]*other[1]  + m[11]*other[2]  + m[15]*other[3],
                       m[0]*other[4]  + m[4]*other[5]  + m[8]*other[6]   + m[12]*other[7],
                       m[1]*other[4]  + m[5]*other[5]  + m[9]*other[6]   + m[13]*other[7],
                       m[2]*other[4]  + m[6]*other[5]  + m[10]*other[6]  + m[14]*other[7],
                       m[3]*other[4]  + m[7]*other[5]  + m[11]*other[6]  + m[15]*other[7],
                       m[0]*other[8]  + m[4]*other[9]  + m[8]*other[10]  + m[12]*other[11],
                       m[1]*other[8]  + m[5]*other[9]  + m[9]*other[10]  + m[13]*other[11],
                       m[2]*other[8]  + m[6]*other[9]  + m[10]*other[10] + m[14]*other[11],
                       m[3]*other[8]  + m[7]*other[9]  + m[11]*other[10] + m[15]*other[11],
                       m[0]*other[12] + m[4]*other[13] + m[8]*other[14]  + m[12]*other[15],
                       m[1]*other[12] + m[5]*other[13] + m[9]*other[14]  + m[13]*other[15],
                       m[2]*other[12] + m[6]*other[13] + m[10]*other[14] + m[14]*other[15],
                       m[3]*other[12] + m[7]*other[13] + m[11]*other[14] + m[15]*other[15] );
    }

    // ** Matrix4::operator[]
    inline float Matrix4::operator[]( int index ) const
    {
        assert( index >= 0 && index < 16 );
        return m[index];
    }

    // ** Matrix4::operator[]
    inline float& Matrix4::operator[]( int index )
    {
        assert( index >= 0 && index < 16 );
        return m[index];
    }

    // ** Matrix4::inversed
	inline Matrix4 Matrix4::inversed( void ) const
    {
		Matrix4 result;
		float d12, d13, d23, d24, d34, d41;

		d12 = m[2]  * m[7]  - m[3]  * m[6];
		d13 = m[2]  * m[11] - m[3]  * m[10];
		d23 = m[6]  * m[11] - m[7]  * m[10];
		d24 = m[6]  * m[15] - m[7]  * m[14];
		d34 = m[10] * m[15] - m[11] * m[14];
		d41 = m[14] * m[3]  - m[15] * m[2];

		result[0] =   m[5] * d34 - m[9] * d24 + m[13] * d23;
		result[1] = -(m[1] * d34 + m[9] * d41 + m[13] * d13);
		result[2] =   m[1] * d24 + m[5] * d41 + m[13] * d12;
		result[3] = -(m[1] * d23 - m[5] * d13 + m[9]  * d12);

		float determinant = m[0] * result[0] +
							m[4] * result[1] +
							m[8] * result[2] +
							m[12] * result[3];

		if( determinant == 0.0 ) {
			return Matrix4();
		}

		float invDeterminant = 1.0f / determinant;
			
		result[0] *= invDeterminant;
		result[1] *= invDeterminant;
		result[2] *= invDeterminant;
		result[3] *= invDeterminant;

		result[4] = -(m[4] * d34 - m[8] * d24 + m[12] * d23) * invDeterminant;
		result[5] =   m[0] * d34 + m[8] * d41 + m[12] * d13  * invDeterminant;
		result[6] = -(m[0] * d24 + m[4] * d41 + m[12] * d12) * invDeterminant;
		result[7] =   m[0] * d23 - m[4] * d13 + m[8]  * d12  * invDeterminant;

		d12 = m[0]  * m[5]  - m[1]  * m[12];
		d13 = m[0]  * m[9]  - m[1]  * m[8];
		d23 = m[4]  * m[9]  - m[5]  * m[8];
		d24 = m[4]  * m[13] - m[5]  * m[12];
		d34 = m[8]  * m[13] - m[9]  * m[12];
		d41 = m[12] * m[1]  - m[13] * m[0];

		result[8]  =   m[7] * d34 - m[11] * d24 + m[15] * d23  * invDeterminant;
		result[9]  = -(m[3] * d34 + m[11] * d41 + m[15] * d13) * invDeterminant;
		result[10] =   m[3] * d24 + m[7]  * d41 + m[15] * d12  * invDeterminant;
		result[11] = -(m[3] * d23 - m[7]  * d13 + m[11] * d12) * invDeterminant;
		result[12] = -(m[6] * d34 - m[10] * d24 + m[14] * d23) * invDeterminant;
		result[13] =   m[2] * d34 + m[10] * d41 + m[14] * d13  * invDeterminant;
		result[14] = -(m[2] * d24 + m[6]  * d41 + m[14] * d12) * invDeterminant;
		result[15] =   m[2] * d23 - m[6]  * d13 + m[10] * d12  * invDeterminant;

        return result;
	}

	// ** Matrix4::transposed
	inline Matrix4 Matrix4::transposed( void ) const
	{
		Matrix4 result;

		for( s32 i = 0; i < 4; i++ ) {
			result.setRow( i, column( i ) );
		}

		return result;
	}

	// ** Matrix4::row
	inline Vec4 Matrix4::row( int index ) const
	{
		assert( index >= 0 && index < 4 );
		return Vec4( m[index * 4 + 0], m[index * 4 + 1], m[index * 4 + 2], m[index * 4 + 3] );
	}

	// ** Matrix4::setRow
	inline void Matrix4::setRow( int index, const Vec4& value )
	{
		assert( index >= 0 && index < 4 );
		m[index * 4 + 0] = value.x;
		m[index * 4 + 1] = value.y;
		m[index * 4 + 2] = value.z;
		m[index * 4 + 3] = value.w;
	}

	// ** Matrix4::column
	inline Vec4 Matrix4::column( int index ) const
	{
		assert( index >= 0 && index < 4 );
		return Vec4( m[index + 0], m[index + 4], m[index + 8], m[index + 12] );
	}

	// ** Matrix4::setColumn
	inline void Matrix4::setColumn( int index, const Vec4& value )
	{
		assert( index >= 0 && index < 4 );
		m[index +  0] = value.x;
		m[index +  4] = value.y;
		m[index +  8] = value.z;
		m[index + 12] = value.w;
	}

    // ** Matrix4::rotate
    inline Vec3 Matrix4::rotate( const Vec3& v ) const
    {
        Vec3 r;

        r.x = v.x * m[0] + v.y * m[4] + v.z * m[8];
        r.y = v.x * m[1] + v.y * m[5] + v.z * m[9];
        r.z = v.x * m[2] + v.y * m[6] + v.z * m[10];

        return r;
    }

    // ** Matrix4::translation
    inline Matrix4 Matrix4::translation( float x, float y, float z )
    {
        Matrix4 m;
        m[12] = x; m[13] = y; m[14] = z;
        return m;
    }

    // ** Matrix4::translation
    inline Matrix4 Matrix4::translation( const Vec3& position )
    {
        Matrix4 m;
        m[12] = position.x; m[13] = position.y; m[14] = position.z;
        return m;
    }

    // ** Matrix4::affineTransform
    inline Matrix4 Matrix4::affineTransform( const Vec3& position, const Quat& rotation, const Vec3& scale )
    {
        return Matrix4::translation( position ) * rotation * Matrix4::scale( scale );
    }

    // ** Matrix4::scale
    inline Matrix4 Matrix4::scale( float x, float y, float z )
    {
        Matrix4 m;

        m[0 ] = x;    m[1 ] = 0.0f; m[2 ] = 0.0f; m[3 ] = 0.0f;
        m[4 ] = 0.0f; m[5 ] = y;    m[6 ] = 0.0f; m[7 ] = 0.0f;
        m[8 ] = 0.0f; m[9 ] = 0.0f; m[10] = z;    m[11] = 0.0f;
        m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;

        return m;
    }

    // ** Matrix4::scale
    inline Matrix4 Matrix4::scale( const Vec3& scale )
    {
        Matrix4 m;

        m[0 ] = scale.x;    m[1 ] = 0.0f;       m[2 ] = 0.0f;       m[3 ] = 0.0f;
        m[4 ] = 0.0f;       m[5 ] = scale.y;    m[6 ] = 0.0f;       m[7 ] = 0.0f;
        m[8 ] = 0.0f;       m[9 ] = 0.0f;       m[10] = scale.z;    m[11] = 0.0f;
        m[12] = 0.0f;       m[13] = 0.0f;       m[14] = 0.0f;       m[15] = 1.0f;

        return m;
    }

    // ** Matrix4::perspective
    inline Matrix4 Matrix4::perspective( float fov, float aspect, float zNear, float zFar )
    {
        float yScale = cosf( radians( fov * 0.5f ) ) / sinf( radians( fov * 0.5f ) );
        float xScale = yScale / aspect;

        return Matrix4(	xScale,     0,          0,							  0,
                        0,			yScale,     0,							  0,
                        0,			0,			zFar/(zNear-zFar),			 -1,
                        0,			0,			zNear*zFar/(zNear-zFar),      0 );
    }

    // ** Matrix4::perspectiveLeft
    inline Matrix4 Matrix4::perspectiveLeft( float fov, float aspect, float zNear, float zFar )
    {
        float yScale = cosf( radians( fov * 0.5f ) ) / sinf( radians( fov * 0.5f ) );
        float xScale = yScale / aspect;

        return Matrix4(	xScale,     0,          0,							  0,
                        0,			yScale,     0,							  0,
                        0,			0,			zFar/(zFar-zNear),			  1,
                        0,			0,		   -zNear*zFar/(zFar-zNear),      0 );
    }

    // ** Matrix4::ortho
    inline Matrix4 Matrix4::ortho( float left, float right, float bottom, float top, float zNear, float zFar )
    {
        float tx = -(right + left)   / (right - left);
        float ty = -(top   + bottom) / (top   - bottom);
        float tz = -(zFar  + zNear)  / (zFar  - zNear);

        return Matrix4( 2.0f / (right - left),  0.0f,					 0.0f,					0.0f,
                        0.0f,					2.0f / (top - bottom),	 0.0f,					0.0f,
                        0.0f,					0.0f,					-2.0f / (zFar - zNear), 0.0f,
                        tx,                     ty,						 tz,					1.0f );
    }

    // ** Matrix4::lookAt
    inline Matrix4 Matrix4::lookAt( const Vec3& position, const Vec3& target, const Vec3& up )
    {
        Vec3 d = Vec3::normalize( target - position );
        Vec3 r = Vec3::normalize( d % up );
        Vec3 u = Vec3::normalize( r % d );
        
        return view( position, d, u, r );
    }

    // ** Matrix4::lookAt
    inline Matrix4 Matrix4::lookAtLeft( const Vec3& position, const Vec3& target, const Vec3& up )
    {
        Vec3 d = Vec3::normalize( target - position );
        Vec3 r = Vec3::normalize( up % d );
        Vec3 u = Vec3::normalize( d % r );
        
        return viewLeft( position, d, u, r );
    }

    // ** Matrix4::view
    inline Matrix4 Matrix4::view( const Vec3& position, const Vec3& direction, const Vec3& up, const Vec3& right )
    {
        const Vec3& d = direction;
        const Vec3& r = right;
        const Vec3& u = up;

        return Matrix4(  r.x,				 u.x,				-d.x,				0.0f,
                         r.y,				 u.y,				-d.y,				0.0f,
                         r.z,				 u.z,				-d.z,				0.0f,
                        -(position * r),	-(position * u),	-(position * -d),	1.0f );
    }

    // ** Matrix4::viewLeft
    inline Matrix4 Matrix4::viewLeft( const Vec3& position, const Vec3& direction, const Vec3& up, const Vec3& right )
    {
        const Vec3& d = direction;
        const Vec3& r = right;
        const Vec3& u = up;
        
        return Matrix4(  r.x,				 u.x,				d.x,				0.0f,
                         r.y,				 u.y,				d.y,				0.0f,
                         r.z,				 u.z,				d.z,				0.0f,
                        -(position * r),	-(position * u),	-(position * d),	1.0f );
    }

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Matrix4_H__ )  */
