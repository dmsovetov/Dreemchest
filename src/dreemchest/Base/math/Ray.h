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

#ifndef __DC_Base_Ray_H__
#define __DC_Base_Ray_H__

DC_BEGIN_DREEMCHEST

	//! Ray class.
	class Ray {
	public:

					//! Constructs Ray instance.
					Ray( void );

					//! Constructs Ray instance from origin and direction.
					Ray( const Vec3& origin, const Vec3& direction );

		//! Transforms ray by an affine matrix.
		Ray			operator * ( const Matrix4& matrix ) const;

		//! Return ray origin.
		const Vec3&	origin( void ) const;

		//! Returns ray direction.
		const Vec3& direction( void ) const;

		//! Returns true if the ray intersects bounding box & calculates intersection point.
		bool		intersects( const Bounds& bounds, Vec3* point = NULL, f32* time = NULL ) const;

		//! Returns true if the ray intersects plane & calculates intersection point.
		bool		intersects( const Plane& plane, Vec3* point = NULL, f32* time = NULL ) const;

		//! Returns true if the ray intersects sphere.
		bool		intersects( const Sphere& sphere, Vec3* point = NULL, f32* time = NULL, Vec3* normal = NULL ) const;

	private:

		Vec3		m_origin;		//!< Ray origin.
		Vec3		m_direction;	//!< Ray direction.
	};

	// ** Ray::Ray
	inline Ray::Ray( void )
	{
	
	}

	// ** Ray::Ray
	inline Ray::Ray( const Vec3& origin, const Vec3& direction ) : m_origin( origin ), m_direction( direction )
	{
	
	}

	// ** Ray::operator *
	inline Ray Ray::operator * ( const Matrix4& matrix ) const
	{
		Vec4 origin		= matrix * Vec4( m_origin.x,	m_origin.y,	   m_origin.z,	  1.0f );
		Vec4 direction	= matrix * Vec4( m_direction.x, m_direction.y, m_direction.z, 0.0f );
		return Ray( origin, direction );
	}

	// ** Ray::origin
	inline const Vec3& Ray::origin( void ) const
	{
		return m_origin;
	}

	// ** Ray::direction
	inline const Vec3& Ray::direction( void ) const
	{
		return m_direction;
	}

	// ** Ray::intersects
	inline bool Ray::intersects( const Plane& plane, Vec3* point, f32* time ) const
	{
		f32 t = -(plane.distance() + plane.normal() * m_origin) / (plane.normal() * m_direction);

		if( point ) *point = m_origin + m_direction * t;
		if( time ) *time = t;
		
		return true;
	}

	// ** Ray::intersects
	inline bool Ray::intersects( const Bounds& bounds, Vec3* point, f32* time ) const
	{
		Vec3 rayDelta = m_direction * 999999.0f;
		const Vec3& min = bounds.min();
		const Vec3& max = bounds.max();

		bool inside = true;
		f32 xt, yt, zt;

		if( m_origin.x < min.x) {
			xt = min.x - m_origin.x;

			if( xt > rayDelta.x ) {
				return false;
			}

			xt /= rayDelta.x; 
			inside = false;
		} 
		else if( m_origin.x > max.x ) {
			xt = max.x - m_origin.x;

			if( xt < rayDelta.x ) {
				return false;
			}

			xt /= rayDelta.x;
			inside = false;
		} 
		else {
			xt = -1.0f; 
		}

		if( m_origin.y < min.y ) {
			yt = min.y - m_origin.y;

			if( yt > rayDelta.y ) {
				return false;
			}

			yt /= rayDelta.y;
			inside = false;
		} 
		else if( m_origin.y > max.y ) {
			yt = max.y - m_origin.y;

			if( yt < rayDelta.y ) {
				return false;
			}

			yt /= rayDelta.y;
			inside = false;
		} 
		else {
			yt = -1.0f;
		}

		if( m_origin.z < min.z ) {
			zt = min.z - m_origin.z;

			if( zt > rayDelta.z ) {
				return false;
			}

			zt /= rayDelta.z;
			inside = false;
		} 
		else if( m_origin.z > max.z ) {
			zt = max.z - m_origin.z;

			if( zt < rayDelta.z ) {
				return false;
			}

			zt /= rayDelta.z;
			inside = false;
		} 
		else {
			zt = -1.0f;
		}

		f32 t = xt;

		if( yt > t ) {
			t = yt;
		}

		if( zt > t ) {
			t = zt;
		}

		if( inside ) {
			if( point ) *point = Vec3( m_origin.x + rayDelta.x * t, m_origin.y + rayDelta.y * t, m_origin.z + rayDelta.z * t );
			if( time ) *time = t;

			return true;
		}

		if( t == xt ) {
			f32 y = m_origin.y + rayDelta.y * t;
			f32 z = m_origin.z + rayDelta.z * t;

			if( y < min.y || y > max.y ) {
				return false;
			}
			else if( z < min.z || z > max.z ) {
				return false;
			}
		}
		else if( t == yt ) {
			f32 x = m_origin.x + rayDelta.x * t;
			f32 z = m_origin.z + rayDelta.z * t;

			if( x < min.x || x > max.x ) {
				return false;
			}
			else if( z < min.z || z > max.z ) {
				return false;
			}
		}
		else {
			//	_ASSERTE( t == zt );

			f32 x = m_origin.x + rayDelta.x * t;
			f32 y = m_origin.y + rayDelta.y * t;

			if( x < min.x || x > max.x ) {
				return false;
			}
			else if( y < min.y || y > max.y ) {
				return false;
			}
		}

		if( point ) *point = Vec3( m_origin.x + rayDelta.x * t, m_origin.y + rayDelta.y * t, m_origin.z + rayDelta.z * t );
		if( time ) *time = t;

		return true;
	}

	// ** Ray::intersects
	inline bool Ray::intersects( const Sphere& sphere, Vec3* point, f32* time, Vec3* normal ) const
	{
		const Vec3& center = sphere.center();
		f32			radius = sphere.radius();

		f32 a = m_direction * m_direction;
		f32 b = m_direction * (m_origin * 2.0f - center);
		f32 c = center * center + m_origin * m_origin - 2.0f * (m_origin * center) - radius * radius;
		f32 d = b * b + -4.0f * a * c;

		// No ray intersection
		if( d < 0 ) {
			return false;
		}

		d = sqrtf( d );

		// Ray can intersect the sphere, solve the closer hitpoint
		f32 t = -0.5f * (b + d) / a;

		if( t <= 0.0f ) {
			return false;
		}

		if( time )   *time   = t;
		if( point )  *point  = m_origin + m_direction * t;
		if( normal ) *normal = ((m_origin + m_direction * t) - center) / radius;

		return true;	
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __DC_Base_Ray_H__ )  */
