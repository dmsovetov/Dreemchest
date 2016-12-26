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


#ifndef __Examples_H__
#define __Examples_H__

#include "MeshLoader.h"
#include "TextureLoader.h"

//! A namespace that contains declarations for commonly used constant buffer, shader and vertex buffer types.
namespace Examples
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
        Matrix4                     transform;  //!< A camera view matrix.
        Vec3                        position;   //!< A camera position.
        
        static const UniformElement Layout[];   //!< A constant buffer layout.
        
        //! Creates a Camera constant buffer with a camera looking at the specified target.
        static Camera               lookAt(const Vec3& position, const Vec3& target);
    };
    
    //! A constant buffer definition that contains instance info.
    struct Instance
    {
        Matrix4                     transform;          //!< An instance transformation matrix.
        Matrix4                     inverseTranspose;   //!< An inverse transpose matrix.
        
        static const UniformElement Layout[];           //!< A constant buffer layout.
        
        //! Creates an Instance constant buffer with a specified affine transform.
        static Instance              fromTransform(const Matrix4& transform);
    };
    
    // ** Projection::Layout
    const UniformElement Projection::Layout[] =
    {
        { "transform", UniformElement::Matrix4, offsetof(Projection, transform) }
        , { "viewport",  UniformElement::Vec4,    offsetof(Projection, viewport)  }
        , { NULL }
    };
    
    // ** Projection::ortho
    Projection Projection::ortho(s32 left, s32 right, s32 bottom, s32 top, f32 zNear, f32 zFar)
    {
        Projection projection;
        projection.transform = Matrix4::ortho(left, right, bottom, top, zNear, zFar);
        projection.viewport  = Vec4(left, bottom, right - left, top - bottom);
        return projection;
    }
    
    // ** Projection::perspective
    Projection Projection::perspective(f32 fov, s32 width, s32 height, f32 zNear, f32 zFar)
    {
        Projection projection;
        projection.transform = Matrix4::perspective(fov, static_cast<f32>(width) / height, zNear, zFar);
        projection.viewport  = Vec4(0, 0, width, height);
        return projection;
    }
    
    // ** Camera::Layout
    const UniformElement Camera::Layout[] =
    {
        { "transform", UniformElement::Matrix4, offsetof(Camera, transform) }
        , { "position",  UniformElement::Vec3,    offsetof(Camera, position)  }
        , { NULL }
    };
    
    // ** Camera::lookAt
    Camera Camera::lookAt(const Vec3& position, const Vec3& target)
    {
        Camera camera;
        camera.position = position;
        camera.transform = Matrix4::lookAt(position, target, Vec3::axisY());
        return camera;
    }
    
    // ** Instance::Layout
    const UniformElement Instance::Layout[] =
    {
        { "transform",        UniformElement::Matrix4, offsetof(Instance, transform)        }
        , { "inverseTranspose", UniformElement::Matrix4, offsetof(Instance, inverseTranspose) }
        , { NULL }
    };
    
    // ** Instance::fromTransform
    Instance Instance::fromTransform(const Matrix4& transform)
    {
        Instance instance;
        instance.transform = transform;
        instance.inverseTranspose = transform.inversed().transposed();
        return instance;
    }
    
    const String VertexIdentity =
        "void main()                    \n"
        "{                              \n"
        "   gl_Position = gl_Vertex;    \n"
        "}                              \n"
        ;
    
    const String FragmentPink =
        "void main()                                    \n"
        "{                                              \n"
        "   gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);    \n"
        "}                                              \n"
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
    
    const u16 TriangulatedQuadIndices[6] =
    {
        0, 1, 2,
        0, 2, 3,
    };
    
    Rgba CubeFaceColors[] =
    {
          Rgba(1.0f, 0.0f, 0.0f)
        , Rgba(0.0f, 1.0f, 0.0f)
        , Rgba(0.0f, 0.0f, 1.0f)
        , Rgba(1.0f, 1.0f, 0.0f)
        , Rgba(0.0f, 1.0f, 1.0f)
        , Rgba(1.0f, 0.0f, 1.0f)
    };
    
    Vec3 CubeFaceNormals[] =
    {
           Vec3::axisX()
        , -Vec3::axisX()
        ,  Vec3::axisY()
        , -Vec3::axisY()
        ,  Vec3::axisZ()
        , -Vec3::axisZ()
    };
    
    struct MeshStateBlock
    {
        StateBlock      states;
        PrimitiveType   primitives;
        s32             size;
    };
    
    // Creates a rendering states block from a mesh file
    MeshStateBlock createMeshRenderingStates(RenderingContextWPtr renderingContext, RenderFrame& frame, const String& fileName)
    {
        Mesh mesh = objFromFile(fileName);
        NIMBLE_ABORT_IF(!mesh, "failed to load a mesh");
        
        MeshStateBlock meshStates;
        
        VertexFormat vertexFormat = mesh.vertexFormat;
        VertexBuffer_ vertexBuffer = renderingContext->requestVertexBuffer(frame.internBuffer(&mesh.vertices[0], mesh.vertices.size()), mesh.vertices.size());
        InputLayout inputLayout = renderingContext->requestInputLayout(vertexFormat);

        meshStates.states.bindVertexBuffer(vertexBuffer);
        meshStates.states.bindInputLayout(inputLayout);
        meshStates.primitives = mesh.primitives;
        meshStates.size       = mesh.vertices.size() / vertexFormat.vertexSize();
        
        return meshStates;
    }
    
    // Creates an environment map from a texture
    Texture_ createEnvTexture(RenderingContextWPtr renderingContext, RenderFrame& frame, const String& fileName)
    {
        CubeMap cubeMap = cubeFromDds(fileName);
        return renderingContext->requestTextureCube(frame.internBuffer(&cubeMap.pixels[0], cubeMap.pixels.size()), cubeMap.size, cubeMap.mipLevels, cubeMap.format);
    }
    
    //! Creates a fullscreen quad rendering states
    StateBlock createFullscreenRenderingStates(RenderingContextWPtr renderingContext)
    {
        StateBlock states;
        InputLayout   il = renderingContext->requestInputLayout(0);
        VertexBuffer_ vb = renderingContext->requestVertexBuffer(FullscreenQuad, sizeof(FullscreenQuad));
        states.bindVertexBuffer(vb);
        states.bindInputLayout(il);
        states.setDepthState(LessEqual, false);
        return states;
    }
    
} // namespace Examples

#endif  //  __Examples_H__
