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

#include "CascadedShadowMaps.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** CascadedShadowMaps::CascadedShadowMaps
CascadedShadowMaps::CascadedShadowMaps( void )
    : m_textureSize( 0 )
{
}

// ** CascadedShadowMaps::CascadedShadowMaps
CascadedShadowMaps::CascadedShadowMaps( const Matrix4& camera, const Matrix4& light, s32 textureSize )
    : m_textureSize( textureSize )
    , m_camera( camera )
    , m_light( light )
{
}

// ** CascadedShadowMaps::light
const Matrix4& CascadedShadowMaps::light( void ) const
{
    return m_light;
}

// ** CascadedShadowMaps::calculate
void CascadedShadowMaps::calculate( f32 fov, f32 near, f32 far, f32 aspect, f32 lambda, s32 count )
{
    // Resize cascaded shadow maps
    resize( count );

    // Setup all cascades
    for( s32 i = 0, n = cascadeCount(); i < n; i++ ) {
        // Get a cascade by index
        Cascade& cascade = m_cascades[i];

        // Calculate near and far planes for this cascade
        f32 k	= (i + 1) / static_cast<f32>( count );
		f32 CLi	= near * powf( far / near, k );
		f32 CUi	= near + (far - near) * k;

        cascade.near = (i == 0) ? near : m_cascades[i - 1].far;
        cascade.far  = lerp( CUi, CLi, lambda );

        // Calculate a cascade bounding box in a world and light spaces
        cascade.worldSpaceBounds = calculateWorldSpaceBounds( fov, cascade.near, cascade.far, aspect );

        // Calculate a cascade projection matrix
        cascade.transform = calculateViewProjection( cascade.worldSpaceBounds );

        // Calculate light space vertices
        calculateLightSpaceVertices( cascade.transform, cascade.lightSpaceVertices );
    }
}

// ** CascadedShadowMaps::cascadeCount
s32 CascadedShadowMaps::cascadeCount( void ) const
{
    return static_cast<s32>( m_cascades.size() );
}

// ** CascadedShadowMaps::cascadeAt
const CascadedShadowMaps::Cascade& CascadedShadowMaps::cascadeAt( s32 index ) const
{
    DC_ABORT_IF( index < 0 || index >= cascadeCount(), "index is out of range" );
    return m_cascades[index];
}

// ** CascadedShadowMaps::resize
void CascadedShadowMaps::resize( s32 count )
{
    m_cascades.resize( count );
    memset( &m_cascades[0], 0, sizeof( Cascade ) * cascadeCount() );
}

// ** CascadedShadowMaps::calculateWorldSpaceBounds
CascadedShadowMaps::BoundingVolume CascadedShadowMaps::calculateWorldSpaceBounds( f32 fov, f32 near, f32 far, f32 aspectRatio ) const
{
    // Calculate a tangents from a FOV and aspect ratio
    f32 tanHalfVFOV = tanf( radians( fov * 0.5f ) );
    f32 tanHalfHFOV = tanf( radians( fov * aspectRatio * 0.5f ) );

    // Calculate dimensions of a split far and near faces
    f32 xn = near * tanHalfHFOV;
    f32 xf = far  * tanHalfHFOV;
    f32 yn = near * tanHalfVFOV;
    f32 yf = far  * tanHalfVFOV;

    Vec3 localSpaceVertices[8];

    // Construct a near frustum plane in a local space
    localSpaceVertices[0] = Vec3( -xn, -yn, -near );
    localSpaceVertices[1] = Vec3(  xn, -yn, -near );
    localSpaceVertices[2] = Vec3(  xn,  yn, -near );
    localSpaceVertices[3] = Vec3( -xn,  yn, -near );

    // Construct a far frustum plane in a local space
    localSpaceVertices[4] = Vec3( -xf, -yf, -far );
    localSpaceVertices[5] = Vec3(  xf, -yf, -far );
    localSpaceVertices[6] = Vec3(  xf,  yf, -far );
    localSpaceVertices[7] = Vec3( -xf,  yf, -far );

    return BoundingVolume::fromPoints( localSpaceVertices, 8 ) * m_camera;
}

// ** CascadedShadowMaps::calculateLightSpaceVertices
void CascadedShadowMaps::calculateLightSpaceVertices( const Matrix4& viewProjection, Vec3 lightSpaceVertices[8] ) const
{
    // A set of clip space vertices
    Vec4 clipSpaceVertices[] = {
          { -1, -1, -1 }
        , {  1, -1, -1 }
        , {  1,  1, -1 }
        , { -1,  1, -1 }
        , { -1, -1,  1 }
        , {  1, -1,  1 }
        , {  1,  1,  1 }
        , { -1,  1,  1 }
    };

    // Transform each clip space vertex back to a world space
    for( s32 i = 0; i < 8; i++ ) {
        Vec4 point = viewProjection.inversed() * clipSpaceVertices[i];
        lightSpaceVertices[i] = Vec3( point ) / point.w;
    }
}

// ** CascadedShadowMaps::calculateViewProjection
Matrix4 CascadedShadowMaps::calculateViewProjection( const BoundingVolume& worldSpaceBounds ) const
{
    // Compute an inverse of a light matrix
    Matrix4 inverseLight = m_light.inversed();

    // Transform a world space bounds to a light space
#if DEV_CSM_BOUNDING_SPHERES
    Bounds lightSpaceBounds = Bounds::fromSphere( inverseLight * worldSpaceBounds.center(), worldSpaceBounds.radius() );
#else
    Bounds lightSpaceBounds = worldSpaceBounds * inverseLight;
#endif  /*  #if DEV_CSM_BOUNDING_SPHERES    */

    // Calculate a projection matrix based on a light-space cascade bounding box
    const Vec3& min = lightSpaceBounds.min();
    const Vec3& max = lightSpaceBounds.max();

    // IMPORTANT: minZ and maxZ are swapped!
    Matrix4 projection = Matrix4::ortho( min.x, max.x, min.y, max.y, max.z + 50.0f, min.z );

    // Fix the sub-texel jittering
    projection = fixSubTexel( projection * inverseLight, projection );

    // Return a final view-projection matrix for a cascade
    return projection * inverseLight;
}

// ** CascadedShadowMaps::fixSubTexel
Matrix4 CascadedShadowMaps::fixSubTexel( const Matrix4& viewProjection, const Matrix4& projection ) const
{
    // Transform an origin to a light projection space
	Vec3 zero = viewProjection * Vec3::zero();

	f32 sh = m_textureSize * 0.5f;
	f32 tx = zero.x * sh;
	f32 ty = zero.y * sh;

	f32 dx = ( floor( tx ) - tx ) / sh;
	f32 dy = ( floor( ty ) - ty ) / sh;

    // Compute the final projection matrix
	return Matrix4::translation( dx, dy, 0.0f ) * projection;
}

} // namespace Scene

DC_END_DREEMCHEST