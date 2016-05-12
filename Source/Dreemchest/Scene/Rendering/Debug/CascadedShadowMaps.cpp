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

// ** CascadedShadowMaps::calculate
void CascadedShadowMaps::calculate( f32 fov, f32 near, f32 far, f32 aspect, f32 lambda, const Matrix4& camera, const Matrix4& light, s32 count )
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

        // Calculate a cascade world space vertices
        calculateWorldSpaceVertices( camera, cascade.worldSpaceVertices, fov, cascade.near, cascade.far, aspect );

        // Calculate a cascade projection matrix
        cascade.transform = calculateViewProjection( light, cascade.worldSpaceVertices );

        // Calculate a cascade bounding box in a world and light spaces
        cascade.worldSpaceBounds = calculateWorldSpaceBounds( cascade );

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

// ** CascadedShadowMaps::calculateWorldSpaceVertices
void CascadedShadowMaps::calculateWorldSpaceVertices( const Matrix4& camera, Vec3 worldSpaceVertices[8], f32 fov, f32 near, f32 far, f32 aspectRatio ) const
{
    // Calculate a tangents from a FOV and aspect ratio
    f32 tanHalfVFOV = tanf( radians( fov * 0.5f ) );
    f32 tanHalfHFOV = tanf( radians( fov * aspectRatio * 0.5f ) );

    // Calculate dimensions of a split far and near faces
    f32 xn = near * tanHalfHFOV;
    f32 xf = far  * tanHalfHFOV;
    f32 yn = near * tanHalfVFOV;
    f32 yf = far  * tanHalfVFOV;

    // Construct a near frustum plane in a world space
    worldSpaceVertices[0] = camera * Vec3( -xn, -yn, -near );
    worldSpaceVertices[1] = camera * Vec3(  xn, -yn, -near );
    worldSpaceVertices[2] = camera * Vec3(  xn,  yn, -near );
    worldSpaceVertices[3] = camera * Vec3( -xn,  yn, -near );

    // Construct a far frustum plane in a world space
    worldSpaceVertices[4] = camera * Vec3( -xf, -yf, -far );
    worldSpaceVertices[5] = camera * Vec3(  xf, -yf, -far );
    worldSpaceVertices[6] = camera * Vec3(  xf,  yf, -far );
    worldSpaceVertices[7] = camera * Vec3( -xf,  yf, -far );
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
Matrix4 CascadedShadowMaps::calculateViewProjection( const Matrix4& light, const Vec3 worldSpaceVertices[8] ) const
{
    Bounds lightSpaceBounds;

    for( s32 i = 0; i < 8; i++ ) {
        // Transform the frustum vertex from world to light space.
        lightSpaceBounds << light.inversed() * worldSpaceVertices[i];
    }

    const Vec3& min = lightSpaceBounds.min();
    const Vec3& max = lightSpaceBounds.max();

    // IMPORTANT: minZ and maxZ are swapped!
    return Matrix4::ortho( min.x, max.x, min.y, max.y, max.z /*+ 50.0f*/, min.z ) * light.inversed();
}

// ** CascadedShadowMaps::calculateWorldSpaceBounds
Bounds CascadedShadowMaps::calculateWorldSpaceBounds( const Cascade& cascade ) const
{
    // Transform vertices to a world space and calculate bounding box
    Bounds result;

    for( s32 i = 0; i < 8; i++ ) {
        // Append a vertex in a light space to split bounding box
        result << cascade.worldSpaceVertices[i];
    }

    return result;
}

} // namespace Scene

DC_END_DREEMCHEST