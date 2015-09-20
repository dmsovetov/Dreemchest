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

#include "Scene.h"
#include "PlaneClipper.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** PlaneClipper::PlaneClipper
PlaneClipper::PlaneClipper( u8 id ) : m_id( id )
{

}

// ** PlaneClipper::id
u8 PlaneClipper::id( void ) const
{
	return m_id;
}

// ** PlaneClipper::add
void PlaneClipper::add( const Plane& plane )
{
	m_planes.push_back( plane );
}

// ** PlaneClipper::addFrustum
void PlaneClipper::addFrustum( const Matrix4& matrix )
{
	const f32 *m = matrix.m;
	Plane planes[6];

	planes[0] = Plane( m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12] );
	planes[1] = Plane( m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12] );

	planes[2] = Plane( m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13] );
	planes[3] = Plane( m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13] );

	planes[4] = Plane( m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14] );
	planes[5] = Plane( m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14] );

	for( s32 i = 0; i < 6; i++ ) {
		planes[i].normalize();
		add( planes[i] );
	}
}

// ** PlaneClipper::inside
bool PlaneClipper::inside( const Bounds& bounds ) const
{
	for( u32 i = 0, n = ( u32 )m_planes.size(); i < n; i++ ) {
		if( m_planes[i].isBehind( bounds ) ) {
			return false;
		}
	}

	return true;
}

// ** PlaneClipper::inside
bool PlaneClipper::inside( const Vec3& center, f32 radius ) const
{
	for( u32 i = 0, n = ( u32 )m_planes.size(); i < n; i++ ) {
		if( m_planes[i].isBehind( center, radius ) ) {
			return false;
		}
	}

	return true;
}

// ** PlaneClipper::createFromFrustum
PlaneClipper PlaneClipper::createFromFrustum( const Matrix4& viewProjection, u8 id )
{
	PlaneClipper clipper( id );
	clipper.addFrustum( viewProjection );
	return clipper;
}

} // namespace Scene

DC_END_DREEMCHEST