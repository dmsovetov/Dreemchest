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


#ifndef __Framework_H__
#define __Framework_H__

#include "ApplicationDelegate.h"
#include "MeshLoader.h"
#include "TextureLoader.h"

namespace Framework
{
    DC_USE_DREEMCHEST
    using namespace Renderer;
    
    //! A constant buffer definition that contains a projection info.
    struct Projection
    {
        Matrix4                     transform;  //!< A pass projection matrix.
        Vec4                        viewport;   //!< A viewport info.
        
        static const UniformElement Layout[];   //!< A constant buffer layout.
        
        //! Creates a Projection constant buffer with ortho projection.
        static Projection           ortho(s32 left, s32 right, s32 bottom, s32 top, f32 zNear, f32 zFar);
        
        //! Creates a Projection constant buffer with perpsective projection.
        static Projection           perspective(f32 fov, s32 width, s32 height, f32 zNear, f32 zFar);
    };
    
    //! A constant buffer definition that contains camera info.
    struct Camera
    {
        Matrix4                     transform;          //!< A camera view matrix.
        Matrix4                     inverseTranspose;   //!< An inverse transpose matrix.
        Matrix4                     rotation;           //!< A camera rotation matrix.
        Vec3                        position;           //!< A camera position.
        
        static const UniformElement Layout[];   //!< A constant buffer layout.
        
        //! Creates a Camera constant buffer with a camera looking at the specified target.
        static Camera               lookAt(const Vec3& position, const Vec3& target, const Vec3& up = Vec3::axisY());
        
        //! Creates a Camera constant buffer from a quaternion and a position.
        static Camera               fromQuat(const Vec3& position, const Quat& quat);
    };
    
    //! A constant buffer definition that contains instance info.
    struct Instance
    {
        Matrix4                     transform;          //!< An instance transformation matrix.
        Matrix4                     inverseTranspose;   //!< An inverse transpose matrix.
        f32                         alpha;              //!< An instance alpha.
        
        static const UniformElement Layout[];           //!< A constant buffer layout.
        
        //! Creates an Instance constant buffer with a specified affine transform.
        static Instance              fromTransform(const Matrix4& transform, f32 alpha = 1.0f);
    };
    
    // ** Projection::ortho
    inline Projection Projection::ortho(s32 left, s32 right, s32 bottom, s32 top, f32 zNear, f32 zFar)
    {
        Projection projection;
        projection.transform = Matrix4::ortho(left, right, bottom, top, zNear, zFar);
        projection.viewport  = Vec4(left, bottom, right - left, top - bottom);
        return projection;
    }
    
    // ** Projection::perspective
    inline Projection Projection::perspective(f32 fov, s32 width, s32 height, f32 zNear, f32 zFar)
    {
        Projection projection;
        projection.transform = Matrix4::perspective(fov, static_cast<f32>(width) / height, zNear, zFar);
        projection.viewport  = Vec4(0, 0, width, height);
        return projection;
    }
    
    // ** Camera::lookAt
    inline Camera Camera::lookAt(const Vec3& position, const Vec3& target, const Vec3& up)
    {
        Camera camera;
        camera.position = position;
        camera.transform = Matrix4::lookAt(position, target, up);
        camera.inverseTranspose = camera.transform.inversed().transposed();
        return camera;
    }
    
    // ** Camera::fromQuat
    inline Camera Camera::fromQuat(const Vec3& position, const Quat& quat)
    {
        Camera camera;
        camera.position = position;
        camera.rotation = quat;
        camera.transform = Matrix4(quat) * Matrix4::translation(-position);
        camera.inverseTranspose = camera.transform.inversed().transposed();
        return camera;
    }
    
    // ** Instance::fromTransform
    inline Instance Instance::fromTransform(const Matrix4& transform, f32 alpha)
    {
        Instance instance;
        instance.transform = transform;
        instance.inverseTranspose = transform.inversed().transposed();
        instance.alpha = alpha;
        return instance;
    }
    
    const String VertexIdentity =
        "attribute vec4 a_position;     \n"
        "void main()                    \n"
        "{                              \n"
        "   gl_Position = a_position;   \n"
        "}                              \n"
        ;
    
    const String FragmentPink =
        "void main()                                    \n"
        "{                                              \n"
        "   gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);    \n"
        "}                                              \n"
        ;
    
    const String VertexSkyBox =
        "cbuffer Projection projection : 0;                         \n"
        "cbuffer Camera     camera     : 1;                         \n"
    
        "attribute vec4 a_position;                                 \n"
    
        "varying vec3 v_texCoord;                                   \n"
        
        "void main()                                                \n"
        "{                                                          \n"
        "   gl_Position = projection.transform                      \n"
        "               * camera.rotation                           \n"
        "               * a_position;                               \n"
        "   v_texCoord  = a_position.xyz;                           \n"
        "}                                                          \n"
        ;
        
    const String FragmentSkyBox =
        "uniform samplerCube Texture0;                              \n"
        
        "varying vec3 v_texCoord;                                   \n"
        
        "void main()                                                \n"
        "{                                                          \n"
        "   gl_FragColor = textureCube(Texture0, v_texCoord);       \n"
        "}                                                          \n"
        ;
    
    const String VertexFixedTransform =
        "cbuffer Projection projection : 0;                                         \n"
        "cbuffer Camera     camera     : 1;                                         \n"
        "cbuffer Instance   instance   : 2;                                         \n"
    
        "attribute vec4 a_position;                                                 \n"
    
        "void main()                                                                \n"
        "{                                                                          \n"
        "   gl_Position = projection.transform                                      \n"
        "               * camera.transform                                          \n"
        "               * instance.transform                                        \n"
        "               * a_position                                                \n"
        "               ;                                                           \n"
        "}                                                                          \n"
        ;
    
    const String VertexDebug =
        "cbuffer Projection projection : 0;                                         \n"
        "cbuffer Camera     camera     : 1;                                         \n"
    
        "attribute vec4 a_position;                                                 \n"
    
        "varying vec4 v_color;                                                      \n"
    
        "void main()                                                                \n"
        "{                                                                          \n"
        "   v_color     = gl_Color;                                                 \n"
        "   gl_Position = projection.transform                                      \n"
        "               * camera.transform                                          \n"
        "               * a_position                                                \n"
        "               ;                                                           \n"
        "}                                                                          \n"
        ;
    
    const String FragmentDebug =
        "varying vec4 v_color;                                                      \n"
        
        "void main()                                                                \n"
        "{                                                                          \n"
        "   gl_FragColor = v_color;                                                 \n"
        "}                                                                          \n"
        ;
    
    const f32 Triangle[9] =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };
    
    const f32 FullscreenQuad[12] =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    };
    
    const f32 QuatterScreenQuad[12] =
    {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
    };
    
    const f32 TexturedQuad[20] =
    {
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
    };
    
    const f32 UnitCube[] =
    {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    const u16 TriangulatedQuadIndices[6] =
    {
        0, 1, 2,
        0, 2, 3,
    };
    
    static Rgba CubeFaceColors[] =
    {
          Rgba(1.0f, 0.0f, 0.0f)
        , Rgba(0.0f, 1.0f, 0.0f)
        , Rgba(0.0f, 0.0f, 1.0f)
        , Rgba(1.0f, 1.0f, 0.0f)
        , Rgba(0.0f, 1.0f, 1.0f)
        , Rgba(1.0f, 0.0f, 1.0f)
    };
    
    static Vec3 CubeFaceNormals[] =
    {
           Vec3::axisX()
        , -Vec3::axisX()
        ,  Vec3::axisY()
        , -Vec3::axisY()
        ,  Vec3::axisZ()
        , -Vec3::axisZ()
    };
    
} // namespace Framework

#endif  /*  !__Framework_H__    */
