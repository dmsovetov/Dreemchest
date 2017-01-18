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
        , { "inverseTranspose", UniformElement::Matrix4, offsetof(Camera, inverseTranspose) }
        , { "rotation", UniformElement::Matrix4, offsetof(Camera, rotation) }
        , { "position",  UniformElement::Vec3,    offsetof(Camera, position)  }
        , { NULL }
    };
    
    // ** Camera::lookAt
    Camera Camera::lookAt(const Vec3& position, const Vec3& target, const Vec3& up)
    {
        Camera camera;
        camera.position = position;
        camera.transform = Matrix4::lookAt(position, target, up);
        camera.inverseTranspose = camera.transform.inversed().transposed();
        return camera;
    }
    
    // ** Camera::fromQuat
    Camera Camera::fromQuat(const Vec3& position, const Quat& quat)
    {
        Camera camera;
        camera.position = position;
        camera.rotation = quat;
        camera.transform = Matrix4(quat) * Matrix4::translation(-position);
        camera.inverseTranspose = camera.transform.inversed().transposed();
        return camera;
    }
    
    // ** Instance::Layout
    const UniformElement Instance::Layout[] =
    {
          { "transform",        UniformElement::Matrix4, offsetof(Instance, transform)        }
        , { "inverseTranspose", UniformElement::Matrix4, offsetof(Instance, inverseTranspose) }
        , { "alpha",            UniformElement::Float,   offsetof(Instance, alpha)            }
        , { NULL }
    };
    
    // ** Instance::fromTransform
    Instance Instance::fromTransform(const Matrix4& transform, f32 alpha)
    {
        Instance instance;
        instance.transform = transform;
        instance.inverseTranspose = transform.inversed().transposed();
        instance.alpha = alpha;
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
    
    const String VertexSkyBox =
        "cbuffer Projection projection : 0;                         \n"
        "cbuffer Camera     camera     : 1;                         \n"
        
        "varying vec3 v_texCoord;                                   \n"
        
        "void main()                                                \n"
        "{                                                          \n"
        "   gl_Position = projection.transform                      \n"
        "               * camera.rotation                           \n"
        "               * gl_Vertex;                                \n"
        "   v_texCoord  = gl_Vertex.xyz;                            \n"
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
    
    RenderItem createSkyBox(RenderingContextWPtr renderingContext, Texture_ texture)
    {
        RenderItem renderItem;
        
        InputLayout   inputLayout  = renderingContext->requestInputLayout(VertexFormat::Position);
        VertexBuffer_ vertexBuffer = renderingContext->requestVertexBuffer(UnitCube, sizeof(UnitCube));
        Program       program      = renderingContext->requestProgram(VertexSkyBox, FragmentSkyBox);
        
        renderItem.primitives = PrimTriangles;
        renderItem.first      = 0;
        renderItem.count      = 36;
        renderItem.indexed    = false;
        renderItem.states.bindVertexBuffer(vertexBuffer);
        renderItem.states.bindInputLayout(inputLayout);
        renderItem.states.bindTexture(texture, 0);
        renderItem.states.bindProgram(program);
        renderItem.states.setDepthState(LessEqual, false);
        
        return renderItem;
    }
    
    RenderItem createRenderItemFromMesh(RenderingContextWPtr renderingContext, const String& fileName)
    {
        Examples::Mesh mesh = Examples::objFromFile(fileName);
        NIMBLE_ABORT_IF(!mesh, "failed to load mesh");
        
        VertexFormat vertexFormat  = mesh.vertexFormat;
        InputLayout inputLayout    = renderingContext->requestInputLayout(vertexFormat);
        VertexBuffer_ vertexBuffer = renderingContext->requestVertexBuffer(&mesh.vertices[0], mesh.vertices.size());
        
        RenderItem renderItem;
        renderItem.primitives = mesh.primitives;
        renderItem.first      = 0;
        renderItem.count      = mesh.vertices.size();
        renderItem.states.bindInputLayout(inputLayout);
        renderItem.states.bindVertexBuffer(vertexBuffer);
        
        if (mesh.indices.size())
        {
            IndexBuffer_ indexBuffer = renderingContext->requestIndexBuffer(&mesh.indices[0], sizeof(u16) * mesh.indices.size());
            renderItem.states.bindIndexBuffer(indexBuffer);
            renderItem.indexed = true;
        }
        else
        {
            renderItem.indexed = false;
        }
        
        return renderItem;
    }
    
    // Creates an environment map from a set of cube map faces.
    Texture_ createEnvFromFiles(RenderingContextWPtr renderingContext, const String& path)
    {
        Image images[6];
        String fileNames[] =
        {
              path + "/posx.tga"
            , path + "/negx.tga"
            , path + "/posy.tga"
            , path + "/negy.tga"
            , path + "/posz.tga"
            , path + "/negz.tga"
        };
        
        Surface pixels;
        for (s32 i = 0; i < 6; i++)
        {
            images[i] = tgaFromFile(fileNames[i]);
            pixels.insert(pixels.end(), images[i].pixels.begin(), images[i].pixels.end());
        }
        
        return renderingContext->requestTextureCube(&pixels[0], images[0].width, 1, images[0].format | TextureTrilinear);
    }
    
    //! Creates a fullscreen quad rendering states
    StateBlock8 createFullscreenRenderingStates(RenderingContextWPtr renderingContext)
    {
        StateBlock8 states;
        InputLayout   il = renderingContext->requestInputLayout(0);
        VertexBuffer_ vb = renderingContext->requestVertexBuffer(FullscreenQuad, sizeof(FullscreenQuad));
        states.bindInputLayout(il);
        states.bindVertexBuffer(vb);
        states.setDepthState(LessEqual, false);
        return states;
    }
    
    //! A rendering application delegate with camera and arc ball rotations.
    class ViewerApplicationDelegate : public RenderingApplicationDelegate
    {
    public:
        
        virtual void handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_ABSTRACT;
        
    protected:
        
        void setCameraPosition(const Vec3& value)
        {
            m_camera.position = value;
        }
        
        virtual bool initialize(s32 width, s32 height) NIMBLE_OVERRIDE
        {
            if (!RenderingApplicationDelegate::initialize(width, height))
            {
                return false;
            }
            
            memset(&m_camera, 0, sizeof(m_camera));

            StateBlock& defaultStates = m_renderingContext->defaultStateBlock();
            
            // Configure projection constant buffer
            {
                Examples::Projection projection = Examples::Projection::perspective(90.0f, m_window->width(), m_window->height(), 0.1f, 100.0f);
                
                UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Projection", Examples::Projection::Layout);
                ConstantBuffer_ constantBuffer = m_renderingContext->requestConstantBuffer(&projection, sizeof(projection), uniformLayout);
                defaultStates.bindConstantBuffer(constantBuffer, 0);
            }
            
            // Configure camera constant buffer
            {
                UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Camera", Examples::Camera::Layout);
                m_camera.constantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Examples::Camera), uniformLayout);
                defaultStates.bindConstantBuffer(m_camera.constantBuffer, 1);
            }
            
            return true;
        }

        virtual void handleTouchBegan(const Platform::Window::TouchBegan& e) NIMBLE_OVERRIDE
        {
            m_camera.active = true;
        }

        virtual void handleTouchEnded(const Platform::Window::TouchEnded& e) NIMBLE_OVERRIDE
        {
            m_camera.active = false;
        }

        virtual void handleTouchMoved(const Platform::Window::TouchMoved& e) NIMBLE_OVERRIDE
        {
            static s32 prevX = -1;
            static s32 prevY = -1;
            
            if (!m_camera.active)
            {
                prevX = -1;
                prevY = -1;
                return;
            }
            
            if (prevX == -1 || prevY == -1)
            {
                prevX = e.x;
                prevY = e.y;
            }
            
            s32 dx = e.x - prevX;
            s32 dy = e.y - prevY;
            
            m_camera.yaw   += dx * 0.25f;
            m_camera.pitch += dy * 0.25f;
            
            prevX = e.x;
            prevY = e.y;
        }
        
        virtual void handleRenderFrame(f32 dt)
        {
            RenderFrame& frame = m_renderingContext->allocateFrame();
            RenderCommandBuffer& commands = frame.entryPoint();
            
            // Update the camera constant buffer
            Quat rotation = Quat::rotateAroundAxis(m_camera.pitch, Vec3::axisX()) * Quat::rotateAroundAxis(m_camera.yaw, Vec3::axisY());
            Examples::Camera camera = Examples::Camera::fromQuat(m_camera.position, rotation);
            commands.uploadConstantBuffer(m_camera.constantBuffer, &camera, sizeof(camera));
            
            handleRenderFrame(frame, frame.stateStack(), commands, dt);
            m_renderingContext->display(frame);
        }
        
    private:

        struct
        {
            f32             yaw;
            f32             pitch;
            bool            active;
            Vec3            position;
            ConstantBuffer_ constantBuffer;
        } m_camera;
    };
    
} // namespace Examples

#endif  //  __Examples_H__
