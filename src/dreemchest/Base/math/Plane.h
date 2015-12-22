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

#ifndef __Base_Plane_H__
#define __Base_Plane_H__

DC_BEGIN_DREEMCHEST

    /*!
     A plane in a 3-dimensinal space.
     */
    class Plane {
    public:

                        //! Constructs a Plane instance.
                        /*!
                         \param normal Plane normal.
                         \param point Point on plane.
                         */
                        Plane( const Vec3& normal = Vec3( 0, 0, 0 ), f32 distance = 0.0f )
                            : m_normal( normal ), m_distance( distance ) {}

                        //! Constructs a Plane instance.
                        /*!
                         \param x Plane normal X coordinate.
						 \param y Plane normal Y coordinate.
						 \param z Plane normal Z coordinate.
                         \param point Point on plane.
                         */
                        Plane( f32 x, f32 y, f32 z, f32 distance = 0.0f )
                            : m_normal( Vec3( x, y, z ) ), m_distance( distance ) {}

		//! Returns true if the plane is valid.
						operator bool ( void ) const;

        //! Projects a point onto this plane.
        Vec3            operator * ( const Vec3& point ) const;

		//! Returns true if the bounding box is behind the plane.
		bool			isBehind( const Bounds& bounds ) const;

		//! Returns true if the sphere is behind the plane.
		bool			isBehind( const Vec3& center, f32 radius ) const;

		//! Normalizes the plane.
		void			normalize( void );

		//! Returns plane normal.
		const Vec3&		normal( void ) const;

		//! Returns plane distance.
		f32				distance( void ) const;

        //! Create plane from point and normal.
        static Plane    calculate( const Vec3& normal, const Vec3& point );

    private:

        //! Plane normal.
        Vec3            m_normal;

        //! Plane distance to origin.
        f32				m_distance;
    };

    // ** Plane::calculate
    inline Plane Plane::calculate( const Vec3& normal, const Vec3& point ) {
        return Plane( normal, -(normal * point) );
    }

	// ** Plane::normalize
	inline void Plane::normalize( void ) {
		f32 len = 1.0f / m_normal.length();

		m_normal   *= len;
		m_distance *= len;
	}

	// ** Plane::isBehind
	inline bool Plane::isBehind( const Bounds& bounds ) const
	{
		Vec3 point = bounds.min();
		const Vec3& max = bounds.max();

		if( m_normal.x >= 0 ) point.x = max.x;
		if( m_normal.y >= 0 ) point.y = max.y;
		if( m_normal.z >= 0 ) point.z = max.z;

		if( (m_normal * point + m_distance) < 0 ) {
			return true;
		}

		return false;
	}

	// ** Plane::isBehind
	inline bool Plane::isBehind( const Vec3& center, f32 radius ) const
	{
		return m_normal * center + m_distance <= -radius;
	}

    // ** Plane::operator bool
    inline Plane::operator bool ( void ) const {
        return m_normal.length() > 0.0f;
    }

    // ** Plane::operator *
    inline Vec3 Plane::operator * ( const Vec3& point ) const {
        f32 distanceToPoint = m_normal * point + m_distance;
        return point - m_normal * distanceToPoint;
    }

	// ** Plane::normal
	inline const Vec3& Plane::normal( void ) const
	{
		return m_normal;
	}

	// ** Plane::distance
	inline f32 Plane::distance( void ) const
	{
		return m_distance;
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Plane_H__ )  */
