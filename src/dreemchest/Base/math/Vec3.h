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

#ifndef __Base_Vec3_H__
#define __Base_Vec3_H__

DC_BEGIN_DREEMCHEST

    /*!
     Euclidean 3-dimensinal vector.
     */
    class Vec3 {
    public:

                    Vec3( void );
                    Vec3( float x, float y, float z );
					Vec3( int x, int y, int z );
                    Vec3( const float* v );
					Vec3( const double* v );

        bool        operator == ( const Vec3& other ) const;
        float&      operator[]( int index );
        float       operator[]( int index ) const;
        Vec3        operator - ( void ) const;
        Vec3        operator + ( float scalar ) const;
        Vec3        operator + ( const Vec3& other ) const;
        Vec3        operator - ( const Vec3& other ) const;
        float       operator * ( const Vec3& other ) const;
        const Vec3& operator += ( const Vec3& other );
        Vec3        operator * ( float scalar ) const;
        Vec3        operator / ( float scalar ) const;
        const Vec3& operator /= ( float scalar );
		const Vec3& operator *= ( float scalar );
        Vec3        operator % ( const Vec3& other ) const;

        //! Normalizes vector.
        float       normalize( void );

        //! Returns a vector length.
        float       length( void ) const;

        //! Returns an ordinal axis.
        Vec3        ordinal( void ) const;

        //! Rotates around an axis.
        static Vec3 rotateAroundAxis( const Vec3& axis, float theta, const Vec3& point );

        //! Returns a random point in sphere.
        static Vec3 randomInSphere( const Vec3& center, float radius );

        //! Returns a random direction.
        static Vec3 randomDirection( void );

        //! Returns a random direction on hemisphere.
        static Vec3 randomHemisphereDirection( const Vec3& normal );

		//! Returns a random cosine weighted direcion on hemisphere.
		static Vec3 randomHemisphereDirectionCosine( const Vec3& normal );

        //! Returns a normalized vector.
        static Vec3 normalize( const Vec3& v );

    public:

        float   x, y, z;
    };

    // ** Vec3::Vec3
    inline Vec3::Vec3( void ) : x( 0.0f ), y( 0.0f ), z( 0.0f )
    {

    }

    // ** Vec3::Vec3
    inline Vec3::Vec3( float x, float y, float z ) : x( x ), y( y ), z( z )
    {

    }

    // ** Vec3::Vec3
    inline Vec3::Vec3( int x, int y, int z ) : x( ( float )x ), y( ( float )y ), z( ( float )z )
    {

    }

    // ** Vec3::Vec3
    inline Vec3::Vec3( const float* v ) : x( v[0] ), y( v[1] ), z( v[2] )
    {

    }

    // ** Vec3::Vec3
    inline Vec3::Vec3( const double* v ) : x( ( float )v[0] ), y( ( float )v[1] ), z( ( float )v[2] )
    {

    }

    // ** Vec3::operator[]
    inline float Vec3::operator[]( int index ) const {
        switch( index ) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        }

        assert( false );
        return 0;
    }

    // ** Vec3::operator[]
    inline float& Vec3::operator[]( int index ) {
        switch( index ) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        }

        assert( false );
        return x;
    }

    // ** Vec3::operator ==
    inline bool Vec3::operator == ( const Vec3& other ) const {
        return x == other.x && y == other.y && z == other.z;
    }

    // ** Vec3::operator *
    inline Vec3 Vec3::operator * ( float scalar ) const {
        return Vec3( x * scalar, y * scalar, z * scalar );
    }

    // ** Vec3::operator /
    inline Vec3 Vec3::operator / ( float scalar ) const {
        return Vec3( x / scalar, y / scalar, z / scalar );
    }

    // ** Vec3::operator /
    inline const Vec3& Vec3::operator /= ( float scalar ) {
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }

    // ** Vec3::operator /
    inline const Vec3& Vec3::operator *= ( float scalar ) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    // ** Vec3::operator *
    inline float Vec3::operator * ( const Vec3& other ) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // ** Vec3::operator +
    inline Vec3 Vec3::operator + ( float scalar ) const {
        return Vec3( x + scalar, y + scalar, z + scalar );
    }

    // ** Vec3::operator +
    inline Vec3 Vec3::operator + ( const Vec3& other ) const {
        return Vec3( x + other.x, y + other.y, z + other.z );
    }

    // ** Vec3::operator +
    inline const Vec3& Vec3::operator += ( const Vec3& other ) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    // ** Vec3::operator -
    inline Vec3 Vec3::operator - ( const Vec3& other ) const {
        return Vec3( x - other.x, y - other.y, z - other.z );
    }

    // ** Vec3::operator %
    inline Vec3 Vec3::operator % ( const Vec3& other ) const {
        return Vec3( y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x );
    }

    inline Vec3 Vec3::operator - ( void ) const {
        return Vec3( -x, -y, -z );
    }

    // ** Vec3::ordinal
    inline Vec3 Vec3::ordinal( void ) const
    {
        float nx = fabs( x );
		float ny = fabs( y );
		float nz = fabs( z );

		if( nx > ny && nx > nz ) return Vec3( 1, 0, 0 );
		if( ny > nx && ny > nz ) return Vec3( 0, 1, 0 );

        return Vec3( 0, 0, 1 );
    }

    // ** Vec3::length
    inline float Vec3::length( void ) const {
        return sqrt( x*x + y*y + z*z ) ;
    }

    // ** Vec3::normalize
    inline float Vec3::normalize( void ) {
        float len = length();
        if( len ) {
            x /= len ; y /= len ; z /= len;
        }
        
        return len;
    }

    // ** Vec3::rotateAroundAxis
    inline Vec3 Vec3::rotateAroundAxis( const Vec3& axis, float theta, const Vec3& point )
    {
        Vec3 rotationAxis = axis;
        rotationAxis.normalize();

        double common_factor = sin( theta * 0.5 );

        double a = cos( theta * 0.5 );
        double b = rotationAxis.x * common_factor;
        double c = rotationAxis.y * common_factor;
        double d = rotationAxis.z * common_factor;

        double mat[9] = {
            a*a+b*b-c*c-d*d, 2*(b*c-a*d),     2*(b*d+a*c),
            2*(b*c+a*d),     a*a-b*b+c*c-d*d, 2*(c*d-a*b),
            2*(b*d-a*c),     2*(c*d+a*b),     a*a-b*b-c*c+d*d
        };

        Vec3 result;

        for(int i = 0; i < 3; i++){
            for( int j = 0; j < 3; j++ ) result[i] += static_cast<float>( mat[i*3+j] * point[j] );
        }

        assert( !isNaN( result.x ) );
        assert( !isNaN( result.y ) );
        assert( !isNaN( result.z ) );

        return result;
    }

    // ** Vec3::randomInSphere
    inline Vec3 Vec3::randomInSphere( const Vec3& center, float radius )
    {
        return center + Vec3::randomDirection() * radius;
    }

    // ** Vec3::randomDirection
    inline Vec3 Vec3::randomDirection( void )
    {
		Vec3 dir;
		f32  len;

		do {
		   dir.x = (rand0to1() * 2.0f - 1.0f);
		   dir.y = (rand0to1() * 2.0f - 1.0f);
		   dir.z = (rand0to1() * 2.0f - 1.0f);
		   len   = dir.length();
		} while( len > 1.0f );

		return dir / len;
    }

    // ** Vec3::randomHemisphereDirection
    inline Vec3 Vec3::randomHemisphereDirection( const Vec3& normal )
    {
        Vec3 dir = randomDirection();

        if( dir * normal < 0 ) {
            dir = -dir;
        }

        return dir;
    }

	// ** Vec3::randomHemisphereDirectionCosine
	inline Vec3 Vec3::randomHemisphereDirectionCosine( const Vec3& normal )
	{
		f32 Xi1 = (f32)rand()/(f32)RAND_MAX;
		f32 Xi2 = (f32)rand()/(f32)RAND_MAX;

		f32  theta = acosf(sqrtf(1.0f-Xi1));
		f32  phi = 2.0f * 3.1415926535897932384626433832795f * Xi2;

		f32 xs = sinf(theta) * cosf(phi);
		f32 ys = cosf(theta);
		f32 zs = sinf(theta) * sinf(phi);

		Vec3 d( xs, ys, zs );
		return d * normal < 0 ? -d : d;

	/*
		Vec3 y = normal;
		Vec3 h = y;
		if (fabs(h.x)<=fabs(h.y) && fabs(h.x)<=fabs(h.z))
			h.x= 1.0;
		else if (fabs(h.y)<=fabs(h.x) && fabs(h.y)<=fabs(h.z))
			h.y= 1.0;
		else
			h.z= 1.0;


		Vec3 x = (h % y); x.normalize();
		Vec3 z = (x % y); y.normalize();

		Vec3 direction = x * xs + y * ys + z * zs;
		direction.normalize();
		return direction;*/
	}

    // ** Vec3::normalize
    inline Vec3 Vec3::normalize( const Vec3& v ) {
        Vec3 result = v;
        result.normalize();
        return result;
    }

	//! Stratified random direction sampler.
	struct StratifiedDirectionSampler {
				//! Constructs StratifiedDirectionSampler instance.
				StratifiedDirectionSampler( s32 samples ) : m_samples( samples ), m_index( 0 )
				{
					m_count	 = ( s32 )sqrtf( ( f32 )samples );
					m_strata = 1.0f / m_count;
				}

		//! Generates next sample on sphere.
		Vec3	randomOnSphere( f32 jitter = 0.0f )
		{
			f32 u = m_strata * (m_index / m_count) + (rand0to1() * 2.0f - 1.0f) * m_strata * jitter;
			f32 v = m_strata * (m_index % m_count) + (rand0to1() * 2.0f - 1.0f) * m_strata * jitter;

			f32 theta0 = 2 * Pi * u;
			f32 theta1 = acos( 1.0f - 2.0f * v );

			f32 s0 = sinf( theta0 );
			f32 s1 = sinf( theta1 );
			f32 c0 = cosf( theta0 );
			f32 c1 = cosf( theta1 );

			m_index++;

			return Vec3( s0 * s1, s1 * c0, c1 );
		}

		//! Generates next sample on hemisphere.
		Vec3	randomOnHemisphere( const Vec3& normal, f32 jitter = 0.0f )
		{
			Vec3 dir = randomOnSphere( jitter );
			return ( dir * normal < 0 ) ?  -dir : dir;
		}

		s32		m_samples;	//!< The total number of samples.
		s32		m_index;	//!< Current index.
		s32		m_count;	//!< The square root of samples.
		f32		m_strata;	//!< The strata size.
	};

	//! Returns the random 3D vector in specified range.
	template<>
	inline Vec3 randomValue<Vec3>( const Vec3& min, const Vec3& max )
	{
		return Vec3(
				  randomValue( min.x, max.x )
				, randomValue( min.y, max.y )
				, randomValue( min.z, max.z ) );
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Vec3_H__ )  */
