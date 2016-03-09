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

#ifndef __DC_Scene_PlaneClipper_H__
#define __DC_Scene_PlaneClipper_H__

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Checks if an object is inside the set of planes.
	class PlaneClipper {
	public:

							//! Constructs the PlaneClipper instance.
							PlaneClipper( u8 id = 0 );

		//! Adds a new clipping plane.
		void				add( const Plane& plane );

		//! Adds planes from a camera frustum.
		void				setAsFrustum( const Matrix4& viewProjection );

		//! Adds planes from a box.
		void				setAsBox( const Vec3& center, f32 radius );

		//! Returns plane clipper id.
		u8					id( void ) const;

		//! Returns true if bounding box is inside the clipping planes.
		bool				inside( const Bounds& bounds ) const;

		//! Returns true if bounding sphere is inside the clipping planes.
		bool				inside( const Vec3& center, f32 radius ) const;

		//! Creates the plane clipper from a camera frustum.
		static PlaneClipper	createFromFrustum( const Matrix4& viewProjection, u8 id = 0 );

		//! Creates the plane clipper from a box.
		static PlaneClipper	createFromBox( const Vec3& center, f32 radius, u8 id = 0 );

	private:

		u8					m_id;		//!< Clipper identifier.
		Array<Plane>		m_planes;	//!< Clipping planes.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_PlaneClipper_H__    */