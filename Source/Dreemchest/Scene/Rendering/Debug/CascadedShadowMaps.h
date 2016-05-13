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

#ifndef __DC_Scene_Rendering_CascadedShadowMaps_H__
#define __DC_Scene_Rendering_CascadedShadowMaps_H__

#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! An utilitary class to setup cascaded shadow maps before rendering.
    class CascadedShadowMaps {
    public:

        //! A helper struct to store a single split info.
        struct Cascade {
            f32                 near;                   //!< The near cascade plane.
            f32                 far;                    //!< The far cascade plane.
            Vec3                worldSpaceVertices[8];  //!< World space vertices of a cascade frustum.
            Vec3                lightSpaceVertices[8];  //!< Light space vertices reprojected back to the world space.
            Bounds              worldSpaceBounds;       //!< A world space bounding box of the cascade.
            Matrix4             transform;              //!< A view-projection matrix that is used to render a shadowmap for a cascade.
        };

        //! A container type to store shadow map cascades.
        typedef Array<Cascade>  Cascades;

                                //! Constructs an empty CascadedShadowMaps instance.
                                CascadedShadowMaps( void );

                                //! Constructs a CascadedShadowMaps instance.
                                CascadedShadowMaps( const Matrix4& camera, const Matrix4& light, s32 textureSize );

        //! Returns an active light world transform.
        const Matrix4&          light( void ) const;

        //! Calculates a specified number of splits.
        void                    calculate( f32 fov, f32 near, f32 far, f32 aspect, f32 lambda, s32 count );

        //! Returns a total number of splits.
        s32                     cascadeCount( void ) const;

        //! Returns an array of split distances.
        const Cascade&          cascadeAt( s32 index ) const;

    private:

        //! Fixes a sub-texel jitter in a cascade final matrix.
        Matrix4                 fixSubTexel( const Matrix4& viewProjection, const Matrix4& projection ) const;

        //! Resizes a cascaded shadow maps.
        void                    resize( s32 count );

        //! Calculates world space vertices for a cascade.
        void                    calculateWorldSpaceVertices( Vec3 worldSpaceVertices[8], f32 fov, f32 near, f32 far, f32 aspectRatio ) const;

        //! Calculates light space vertices for a cascade.
        void                    calculateLightSpaceVertices( const Matrix4& viewProjection, Vec3 lightSpaceVertices[8] ) const;

        //! Calculates a view-projection matrix from a set of cascade world space vertices.
        Matrix4                 calculateViewProjection( const Vec3 worldSpaceVertices[8] ) const;

        //! Calculates a split world space bounding box.
        Bounds                  calculateWorldSpaceBounds( const Cascade& cascade ) const;

    private:

        Cascades                m_cascades;     //!< An array of shadow cascades.
        s32                     m_textureSize;  //!< A texture size of a single cascade shadowmap.
        Matrix4                 m_camera;       //!< A camera transform.
        Matrix4                 m_light;        //!< A light transform.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_CascadedShadowMaps_H__    */