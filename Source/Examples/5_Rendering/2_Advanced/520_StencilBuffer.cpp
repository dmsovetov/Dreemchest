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

#include <Dreemchest.h>
#include "../Examples.h"

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

static String s_vertexShader =
    "cbuffer Pass     pass     : 0;                         \n"
    "cbuffer Instance instance : 1;                         \n"
    "                                                       \n"
    "varying vec3 v_color;                                  \n"
    "                                                       \n"
    "void main()                                            \n"
    "{                                                      \n"
    "   vec4  normal    = normalize(instance.normal * vec4(gl_Normal, 1.0));    \n"
    "   vec3  light     = normalize(vec3(-1.0, 1.0, -1.0));   \n"
    "   float intensity = max(dot(light, normal.xyz), 0.0);  \n"
    "   v_color         = vec3(intensity);                  \n"
    "   gl_Position     = pass.projection * pass.view * instance.transform * gl_Vertex; \n"
    "}                                                      \n"
;

static String s_fragmentShader =
    "cbuffer Instance instance : 1;                     \n"
    "                                                   \n"
    "varying vec3 v_color;                              \n"
    "                                                   \n"
    "void main()                                        \n"
    "{                                                  \n"
    "   gl_FragColor = vec4(v_color, instance.alpha);   \n"
    "}                                                  \n"
    ;

struct Pass
{
    Matrix4 projection;
    Matrix4 view;
    static UniformElement s_layout[];
} s_pass;

UniformElement Pass::s_layout[] =
{
      { "projection", UniformElement::Matrix4, offsetof(Pass, projection) }
    , { "view",       UniformElement::Matrix4, offsetof(Pass, view)       }
    , { NULL }
};

struct Instance
{
    Matrix4 transform;
    Matrix4 normal;
    f32     alpha;
    static UniformElement s_layout[];
} s_instance;

UniformElement Instance::s_layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Instance, transform) }
    , { "normal",    UniformElement::Matrix4, offsetof(Instance, normal)    }
    , { "alpha",     UniformElement::Float,   offsetof(Instance, alpha)     }
    , { NULL }
};

class StencilBuffer : public RenderingApplicationDelegate
{
    struct Object
    {
        Examples::Mesh mesh;
        StateBlock4     states;
    };
    
    Object      m_platform;
    Object      m_bunny;
    ConstantBuffer_ m_instanceConstantBuffer;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // First initialize objects that will be rendered
        initializeObjectFromMesh("Assets/Meshes/platform.obj", m_platform);
        initializeObjectFromMesh("Assets/Meshes/column.obj", m_bunny);
        
        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        
        // Modify a default state block by adding common resource bindings
        StateBlock& defaults = m_renderingContext->defaultStateBlock();
        defaults.bindProgram(program);
        
        // Configure pass constant buffer
        {
            s_pass.projection = Matrix4::perspective(60.0f, m_window->aspectRatio(), 0.1f, 100.0f);
            s_pass.view       = Matrix4::lookAt(Vec3(0.0f, 2.0f, -3.0f), Vec3(0.0f, 0.6f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Pass", Pass::s_layout);
            ConstantBuffer_ constantBuffer = m_renderingContext->requestConstantBuffer(&s_pass, sizeof(s_pass), uniformLayout);
            
            // And bind it to a default state block
            defaults.bindConstantBuffer(constantBuffer, 0);
        }
        
        // Configure instance constant buffer
        {
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Instance", Instance::s_layout);
            m_instanceConstantBuffer = m_renderingContext->requestConstantBuffer(&s_instance, sizeof(s_instance), uniformLayout);
            
            // And bind it to a default state block
            defaults.bindConstantBuffer(m_instanceConstantBuffer, 1);
        }
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        RenderFrame frame(m_renderingContext->defaultStateBlock());
        
        RenderCommandBuffer& commands = frame.entryPoint();
        
        // In this sample we will use a state stack
        StateStack& states = frame.stateStack();

        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Render the platform
        renderObject(commands, m_platform, Matrix4());
        
        const Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
        const Vec3 scale    = Vec3(0.09f, 0.09f, 0.09f);
        
        // Write a platform to the stencil buffer to create a reflection mask.
        {
            StateScope mask = states.newScope();
            mask->setDepthState(LessEqual, false);
            mask->setColorMask(0);
            mask->setStencilFunction(Always, 1);
            mask->setStencilOp(StencilKeep, StencilKeep, StencilReplace);
            renderObject(commands, m_platform, Matrix4());
        }
        
        // Now render the reflection
        {
            commands.clear(Rgba(), ClearDepth);
            StateScope reflection = states.newScope();
            reflection->setStencilFunction(Equal, 1);
            reflection->setStencilOp(StencilKeep, StencilKeep, StencilKeep);
            reflection->setCullFace(TriangleFaceFront);
            reflection->setBlend(BlendSrcAlpha, BlendInvSrcAlpha);
            renderObject(commands, m_bunny, Matrix4::scale(1.0f, -1.0f, 1.0f) * Matrix4::rotateXY(0.0f, currentTime() * 0.001f) * Matrix4::translation(position) * Matrix4::scale(scale), 0.3f);
        }

        // Finally render the stanford bunny
        renderObject(commands, m_bunny, Matrix4::rotateXY(0.0f, currentTime() * 0.001f) * Matrix4::translation(position) * Matrix4::scale(scale));

        m_renderingContext->display(frame);
    }
    
    void initializeObjectFromMesh(const String& fileName, Object& object)
    {
        object.mesh = Examples::objFromFile(fileName);
        
        VertexFormat vertexFormat  = object.mesh.vertexFormat;
        InputLayout inputLayout    = m_renderingContext->requestInputLayout(vertexFormat);
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(&object.mesh.vertices[0], object.mesh.vertices.size());
        
        object.states.bindInputLayout(inputLayout);
        object.states.bindVertexBuffer(vertexBuffer);
        
        if (object.mesh.indices.size())
        {
            IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(&object.mesh.indices[0], sizeof(u16) * object.mesh.indices.size());
            object.states.bindIndexBuffer(indexBuffer);
        }
    }
    
    void renderObject(RenderCommandBuffer& commands, const Object& object, const Matrix4& transform, f32 alpha = 1.0f)
    {
        s_instance.transform = transform;
        s_instance.alpha = alpha;
        s_instance.normal = transform.inversed().transposed();
        commands.uploadConstantBuffer(m_instanceConstantBuffer, &s_instance, sizeof(s_instance));
        commands.drawPrimitives(0, object.mesh.primitives, 0, object.mesh.vertices.size(), object.states);
    }
};

dcDeclareApplication(new StencilBuffer)