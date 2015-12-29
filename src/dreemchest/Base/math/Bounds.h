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

#ifndef __Base_Bounds_H__
#define __Base_Bounds_H__

DC_BEGIN_DREEMCHEST

    /*!
     A bounding box class.
     */
    class Bounds {
    public:

        //! Constructs a Bounds instance
        /*!
         \param min A bounds minimum point.
         \param max A bounds maximum point.
         */
        explicit		Bounds( Vec3 min = Vec3( FLT_MAX, FLT_MAX, FLT_MAX ), Vec3 max = Vec3( -FLT_MAX, -FLT_MAX, -FLT_MAX ) );

        //! Adds a new point to a bounding box (extends a bounds if needed).
        Bounds&			operator << ( const Vec3& point );

        //! Adds two bounding volumes.
        const Bounds&   operator += ( const Bounds& other );

		//! Returns a transformed bounding box.
		Bounds			operator * ( const Matrix4& transform ) const;

		//! Scales the bounding box by a specified value.
		Bounds			operator * ( f32 value ) const;

        //! Returns a bounds volume.
        f32				volume( void ) const;

        //! Returns a minimum bound point.
        const Vec3&     min( void ) const;

        //! Returns a maximum bound point.
        const Vec3&     max( void ) const;

		//! Returns the bounding box center point.
		Vec3			center( void ) const;

		//! Returns the bounding box left center point.
		Vec3			leftCenter( void ) const;

		//! Returns the bounding box right center point.
		Vec3			rightCenter( void ) const;

		//! Returns the bounding box top center point.
		Vec3			topCenter( void ) const;

		//! Returns the bounding box bottom center point.
		Vec3			bottomCenter( void ) const;

		//! Returns the bounding box near center point.
		Vec3			nearCenter( void ) const;

		//! Returns the bounding box far center point.
		Vec3			farCenter( void ) const;

        //! Returns bounds width.
        f32				width( void ) const;

        //! Returns bounds height.
        f32				height( void ) const;

        //! Returns bounds depth.
        f32				depth( void ) const;

		//! Returns true if the point is inside the bounding box.
		bool			contains( const Vec3& point ) const;

        //! Returns a random point in bounding box.
        Vec3            randomPointInside( void ) const;

		//! Constructs bounding box from an array of points.
		static Bounds	fromPoints( const Vec3* points, s32 count );

    private:

        //! Lower corner coordinate.
        Vec3            m_min;

        //! Upper corner coordinate.
        Vec3            m_max;
    };

    // ** Bounds::Bounds
    inline Bounds::Bounds( Vec3 min, Vec3 max ) : m_min( min ), m_max( max ) {

    }

    // ** Bounds::randomPointInside
    inline Vec3 Bounds::randomPointInside( void ) const
    {
        return Vec3(  lerp( m_min.x, m_max.x, rand0to1() )
                    , lerp( m_min.y, m_max.y, rand0to1() )
                    , lerp( m_min.z, m_max.z, rand0to1() ) );
    }

    // ** Bounds::volume
    inline f32 Bounds::volume( void ) const {
        return (m_max.x - m_min.x) * (m_max.y - m_min.y) * (m_max.z - m_min.z);
    }

    // ** Bounds::min
    inline const Vec3& Bounds::min( void ) const {
        return m_min;
    }

    // ** Bounds::max
    inline const Vec3& Bounds::max( void ) const {
        return m_max;
    }

	// ** Bounds::center
	inline Vec3 Bounds::center( void ) const {
		return (m_max + m_min) * 0.5f;
	}

	// ** Bounds::leftCenter
	inline Vec3 Bounds::leftCenter( void ) const
	{
		return center() - Vec3::axisX() * width() * 0.5f;
	}

	// ** Bounds::rightCenter
	inline Vec3 Bounds::rightCenter( void ) const
	{
		return center() + Vec3::axisX() * width() * 0.5f;
	}

	// ** Bounds::topCenter
	inline Vec3 Bounds::topCenter( void ) const
	{
		return center() + Vec3::axisY() * height() * 0.5f;
	}

	// ** Bounds::bottomCenter
	inline Vec3 Bounds::bottomCenter( void ) const
	{
		return center() - Vec3::axisY() * height() * 0.5f;
	}

	// ** Bounds::nearCenter
	inline Vec3 Bounds::nearCenter( void ) const
	{
		return center() - Vec3::axisZ() * depth() * 0.5f;
	}

	// ** Bounds::farCenter
	inline Vec3 Bounds::farCenter( void ) const
	{
		return center() + Vec3::axisZ() * depth() * 0.5f;
	}

    // ** Bounds::width
    inline f32 Bounds::width( void ) const {
        return fabsf( m_max.x - m_min.x );
    }

    // ** Bounds::height
    inline f32 Bounds::height( void ) const {
        return fabsf( m_max.y - m_min.y );
    }

    // ** Bounds::depth
    inline f32 Bounds::depth( void ) const {
        return fabsf( m_max.z - m_min.z );
    }

    // ** Bounds::contains
    inline bool Bounds::contains( const Vec3& point ) const
	{
        if( point.x < m_min.x || point.x > m_max.x ) return false;
		if( point.y < m_min.y || point.y > m_max.y ) return false;
		if( point.z < m_min.z || point.z > m_max.z ) return false;

		return true;
    }

    // ** Bounds::operator <<
    inline Bounds& Bounds::operator << ( const Vec3& point ) {
        for( int i = 0; i < 3; i++ ) {
            m_min[i] = min2( m_min[i], point[i] );
            m_max[i] = max2( m_max[i], point[i] );
        }
        return *this;
    }

    // ** Bounds::operator +=
    inline const Bounds& Bounds::operator += ( const Bounds& other ) {
        *this << other.m_min;
        *this << other.m_max;
        return *this;
    }

    // ** Bounds::operator *
    inline Bounds Bounds::operator * ( const Matrix4& transform ) const {
        Vec3 vertices[8] = {
			m_min,
			m_max,
			m_min + Vec3( width(), 0.0f, 0.0f ),
			m_min + Vec3( 0.0f, height(), 0.0f ),
			m_min + Vec3( 0.0f, 0.0f, depth() ),
			m_max - Vec3( width(), 0.0f, 0.0f ),
			m_max - Vec3( 0.0f, height(), 0.0f ),
			m_max - Vec3( 0.0f, 0.0f, depth() ),
		};

		Bounds result;

		for( s32 i = 0; i < 8; i++ ) {
			result << transform * vertices[i];
		}

		return result;
    }

    // ** Bounds::operator *
    inline Bounds Bounds::operator * ( f32 value ) const
	{
		return Bounds( m_min * value, m_max * value );
    }

	// ** Bounds::fromPoints
	inline Bounds Bounds::fromPoints( const Vec3* points, s32 count )
	{
		Bounds result;

		for( s32 i = 0; i < count; i++ ) {
			result << points[i];
		}

		return result;
	}

	//! A 2d bounding rectangle class.
	class Rect {
	public:

						//! Constructs a Rect from corner points.
						Rect( const Vec2& min = Vec2(), const Vec2& max = Vec2() );

						//! Constructs a Rect instance from it's coordinates.
						Rect( f32 x1, f32 y1, f32 x2, f32 y2 );

		//! Returns the upper left bounding rect corner.
		const Vec2&		min( void ) const;

		//! Returns the lower right bounding rect corner.
		const Vec2&		max( void ) const;

		//! Returns the left side of a rectangle.
		f32				left( void ) const;

		//! Returns the right side of a rectangle.
		f32				right( void ) const;

		//! Returns the top side of a rectangle.
		f32				top( void ) const;

		//! Returns the bottom side of a rectangle.
		f32				bottom( void ) const;

		//! Returns rectangle width.
		f32				width( void ) const;

		//! Returns rectangle height.
		f32				height( void ) const;

	private:

		Vec2			m_min;	//!< Upper left bounding rect corner.
		Vec2			m_max;	//!< Lower right bounding rect corner.
	};

	// ** Rect::Rect
	inline Rect::Rect( const Vec2& min, const Vec2& max )
		: m_min( min ), m_max( max )
	{
	}

	// ** Rect::Rect
	inline Rect::Rect( f32 x1, f32 y1, f32 x2, f32 y2 )
		: m_min( x1, y1 ), m_max( x2, y2 )
	{
	}

	// ** Rect::min
	inline const Vec2& Rect::min( void ) const
	{
		return m_min;
	}

	// ** Rect::max
	inline const Vec2& Rect::max( void ) const
	{
		return m_max;
	}

	// ** Rect::left
	inline f32 Rect::left( void ) const
	{
		return m_min.x;
	}

	// ** Rect::right
	inline f32 Rect::right( void ) const
	{
		return m_max.x;
	}

	// ** Rect::top
	inline f32 Rect::top( void ) const
	{
		return m_max.y;
	}

	// ** Rect::bottom
	inline f32 Rect::bottom( void ) const
	{
		return m_min.y;
	}

	// ** Rect::min
	inline f32 Rect::width( void ) const
	{
		return m_max.x - m_min.x;
	}

	// ** Rect::max
	inline f32 Rect::height( void ) const
	{
		return m_max.y - m_min.y;
	}

	//! 3D sphere class.
	class Sphere {
	public:

					//! Constructs Sphere instance.
					Sphere( void )
						: m_center( 0.0f, 0.0f, 0.0f ), m_radius( 0.0f ) {}

					Sphere( const Vec3& center, f32 radius )
						: m_center( center ), m_radius( radius ) {}

					//! Returns true if this sphere has a non-zero positive radius.
					operator bool( void ) const;

		//! Returns sphere center point.
		const Vec3&	center( void ) const;

		//! Sets sphere center point.
		void		setCenter( const Vec3& value );

		//! Returns sphere radius.
		f32			radius( void ) const;

		//! Sets sphere radius.
		void		setRadius( f32 value );

	private:

		Vec3		m_center;	//!< The center point of a sphere.
		f32			m_radius;	//!< The sphere radius.
	};

	// ** Sphere::operator bool
	inline Sphere::operator bool( void ) const
	{
		return m_radius > 0.0f;
	}

	// ** Sphere::center
	inline const Vec3& Sphere::center( void ) const
	{
		return m_center;
	}

	// ** Sphere::setCenter
	inline void Sphere::setCenter( const Vec3& value )
	{
		m_center = value;
	}

	// ** Sphere::radius
	inline f32 Sphere::radius( void ) const
	{
		return m_radius;
	}

	// ** Sphere::setRadius
	inline void Sphere::setRadius( f32 value )
	{
		m_radius = value;
	}

	//! Circle class.
	class Circle {
	public:

					//! Constructs Circle instance.
					Circle( void )
						: m_center( 0.0f, 0.0f ), m_radius( 0.0f ) {}

					Circle( const Vec2& center, f32 radius )
						: m_center( center ), m_radius( radius ) {}

					//! Returns true if this circle has a non-zero positive radius.
					operator bool( void ) const;

		//! Returns circle center point.
		const Vec2&	center( void ) const;

		//! Sets circle center point.
		void		setCenter( const Vec2& value );

		//! Returns circle radius.
		f32			radius( void ) const;

		//! Sets circle radius.
		void		setRadius( f32 value );		

		//! Returns true if the point is inside the circle.
		bool		isInner( const Vec2& point ) const;

		//! Returns true if the point lies on a circumference.
		bool		isCircumference( const Vec2& point, f32 eps ) const;

		//! Returns the nearest tangent line to a point.
		Vec2		nearestTangent( const Vec2& point ) const;

		//! Maps point to a direction vector on a 3D sphere,
		bool		mapToSphere( const Vec2& point, Vec3& direction ) const;

	private:

		Vec2		m_center;	//!< Circle center.
		f32			m_radius;	//!< Circle radius.
	};

	// ** Circle::operator bool
	inline Circle::operator bool( void ) const
	{
		return m_radius > 0.0f;
	}

	// ** Circle::center
	inline const Vec2& Circle::center( void ) const
	{
		return m_center;
	}

	// ** Circle::setCenter
	inline void Circle::setCenter( const Vec2& value )
	{
		m_center = value;
	}

	// ** Circle::radius
	inline f32 Circle::radius( void ) const
	{
		return m_radius;
	}

	// ** Circle::setRadius
	inline void Circle::setRadius( f32 value )
	{
		m_radius = value;
	}

	// ** Circle::isInner
	inline bool Circle::isInner( const Vec2& point ) const
	{
		return (point - m_center).lengthSqr() <= m_radius * m_radius;
	}

	// ** Circle::isCircumference
	inline bool Circle::isCircumference( const Vec2& point, f32 eps ) const
	{
		return fabs( m_radius - (point - m_center).length() ) <= eps;
	}

	// ** Circle::nearestTangen
	inline Vec2 Circle::nearestTangent( const Vec2& point ) const
	{
		Vec2 r = Vec2::normalized( point - center() );
		return r.perp();
	}

	// ** Circle::mapToSphere
	inline bool Circle::mapToSphere( const Vec2& point, Vec3& direction ) const
	{
		// Get the cursor position relative to a center of sphere.
		Vec2 r = (point - m_center);

		// Caclulate the distance from a center to cursor position.
		f32 distance = r.length();

		// Outside of a circle - clamp to a radius
		bool inside = distance <= m_radius;

		if( !inside ) {
			r = Vec2::normalized( r ) * m_radius * 0.99f;
		}

		// Now calculate the Z coordinate of 3D point.
		// Given the sphere equation: X^2 + Y^2 + Z^2 = R^2
		// X, Y, R are known, so just solve for Z:
		// Z = sqrt( R^2 - X^2 - Y^2 ) (positive square root is taken).
		f32 m = m_radius * m_radius - r.x * r.x - r.y * r.y;
		f32 z = sqrtf( m_radius * m_radius - r.x * r.x - r.y * r.y );

		// Construct the final result
		direction = Vec3::normalize( Vec3( r.x, r.y, z ) );

		return inside;
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Bounds_H__ )  */
