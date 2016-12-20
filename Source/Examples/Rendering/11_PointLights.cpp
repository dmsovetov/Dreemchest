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

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

static String s_vertexShader =
    "cbuffer Pass     pass     : 0;                                             \n"
    "cbuffer Instance instance : 1;                                             \n"
    "                                                                           \n"
    "varying vec3 v_wsVertex;                                                   \n"
    "varying vec3 v_wsNormal;                                                   \n"
    "                                                                           \n"
    "void main()                                                                \n"
    "{                                                                          \n"
    "   v_wsNormal  = normalize(instance.normal * vec4(gl_Normal, 1.0)).xyz;    \n"
    "   v_wsVertex  = (instance.transform * gl_Vertex).xyz;                     \n"
    "                                                                           \n"
    "   gl_Position = pass.projection * pass.view * vec4(v_wsVertex, 1.0);      \n"
    "}                                                                          \n"
;

static String s_fragmentShader =
    "cbuffer Pass     pass     : 0;                                             \n"
    "cbuffer Instance instance : 1;                                             \n"
    "cbuffer Light    light    : 2;                                             \n"
    "                                                                           \n"
    "varying vec3 v_wsVertex;                                                   \n"
    "varying vec3 v_wsNormal;                                                   \n"
    "                                                                           \n"
    "vec2 blinn(vec3 light, vec3 normal, vec3 view)                             \n"
    "{                                                                          \n"
    "    float ndotl     = dot(normal, light);                                  \n"
    "    vec3  reflected = light - 2.0 * ndotl * normal;                        \n"
    "    float rdotv     = dot(reflected, view);                                \n"
    "    return vec2(ndotl, rdotv);                                             \n"
    "}                                                                          \n"
    "                                                                           \n"
    "vec2 lit(float ndotl, float rdotv, float m)                                \n"
    "{                                                                          \n"
    "    float diff = max(0.0, ndotl);                                          \n"
    "    float spec = step(0.0, ndotl) * max(0.0, rdotv);                       \n"
    "    return vec2(diff, spec);                                               \n"
    "}                                                                          \n"
    "                                                                           \n"
    "void main()                                                                \n"
    "{                                                                          \n"
    "   vec3  norm  = normalize(v_wsNormal);                                    \n"
    "   vec3  ldir  = normalize(light.position - v_wsVertex);                   \n"
    "   vec3  cdir  = normalize(v_wsVertex - pass.camera);                      \n"
    "   vec2  bln   = blinn(ldir, norm, cdir);                                  \n"
    "   vec2  lc    = lit(bln.x, bln.y, 0.95);                                  \n"
    "   vec3  final = light.color * light.intensity * (lc.x + pow(lc.y, 64.0)); \n"
    "   gl_FragColor = vec4(final, 1.0);                                        \n"
    "}                                                                          \n"
    ;

struct Pass
{
    Matrix4 projection;
    Matrix4 view;
    Vec3    camera;
    static UniformElement s_layout[];
} s_pass;

UniformElement Pass::s_layout[] =
{
      { "projection", UniformElement::Matrix4, offsetof(Pass, projection) }
    , { "view",       UniformElement::Matrix4, offsetof(Pass, view)       }
    , { "camera",     UniformElement::Vec3,    offsetof(Pass, camera)     }
    , { NULL }
};

struct Instance
{
    Matrix4 transform;
    Matrix4 normal;
    static UniformElement s_layout[];
} s_instance;

UniformElement Instance::s_layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Instance, transform) }
    , { "normal",    UniformElement::Matrix4, offsetof(Instance, normal)    }
    , { NULL }
};

struct Light
{
    Vec3    position;
    f32     range;
    Rgb     color;
    f32     intensity;
    static UniformElement s_layout[];
} s_light;

UniformElement Light::s_layout[] =
{
      { "position",  UniformElement::Vec3,  offsetof(Light, position)  }
    , { "range",     UniformElement::Float, offsetof(Light, range)     }
    , { "color",     UniformElement::Vec3,  offsetof(Light, color)     }
    , { "intensity", UniformElement::Float, offsetof(Light, intensity) }
    , { NULL }
};

class PointLights : public RenderingApplicationDelegate
{
    struct Object
    {
        MeshLoader::Descriptor  mesh;
        StateBlock              states;
    };
    
    StateBlock  m_renderStates;
    RenderFrame m_renderFrame;
    Object      m_platform;
    Object      m_bunny;
    ConstantBuffer_ m_instanceConstantBuffer;
    ConstantBuffer_ m_passConstantBuffer;
    ConstantBuffer_ m_lightConstantBuffer;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // First initialize objects that will be rendered
        initializeObjectFromMesh("Assets/Meshes/platform.obj", m_platform);
        initializeObjectFromMesh("Assets/Meshes/bunny_decimated.obj", m_bunny);
        
        static const Vec3 s_cameraPos    = Vec3(0.0f, 2.0f, -2.0f);
        static const Vec3 s_cameraTarget = Vec3(0.0f, 0.6f,  0.0f);
        
        // Configure pass constant buffer
        {
            s_pass.projection = Matrix4::perspective(60.0f, m_window->aspectRatio(), 0.1f, 100.0f);
            s_pass.view       = Matrix4::lookAt(s_cameraPos, s_cameraTarget, Vec3(0.0f, 1.0f, 0.0f));
            s_pass.camera     = s_cameraPos;
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Pass", Pass::s_layout);
            m_passConstantBuffer = m_renderingContext->requestConstantBuffer(&s_pass, sizeof(s_pass), uniformLayout);
            m_renderStates.bindConstantBuffer(m_passConstantBuffer, 0);
        }
        
        // Configure instance constant buffer
        {
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Instance", Instance::s_layout);
            m_instanceConstantBuffer = m_renderingContext->requestConstantBuffer(&s_instance, sizeof(s_instance), uniformLayout);
            m_renderStates.bindConstantBuffer(m_instanceConstantBuffer, 1);
        }
        
        // Configure a light constant buffer
        {
            s_light.position  = Vec3(-1.0f, 1.0f, -1.0f);
            s_light.color     = Rgb(1.0f, 1.0f, 1.0f);
            s_light.intensity = 1.0f;
            s_light.range     = 1.0f;
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Light", Light::s_layout);
            m_lightConstantBuffer = m_renderingContext->requestConstantBuffer(&s_light, sizeof(s_light), uniformLayout);
        }
        
        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
        m_renderStates.setColorMask(ColorMaskAll);
        m_renderStates.setDepthState(LessEqual, true);
        m_renderStates.disableStencilTest();
        m_renderStates.disableBlending();
        m_renderStates.setCullFace(TriangleFaceBack);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        m_renderFrame.clear();
        
        CommandBuffer& commands = m_renderFrame.entryPoint();
        
        // In this sample we will use a state stack
        StateStack& states = m_renderFrame.stateStack();
        
        // Push the default state
        StateScope defaultScope = states.push(&m_renderStates);
        
        // Light pass scope
        StateScope lightScope = states.newScope();
        lightScope->bindConstantBuffer(m_lightConstantBuffer, 2);
        
        f32 time = currentTime() * 0.001f;
        
        // Update light parameters
        {
            f32 st           = sinf(time);
            f32 ct           = cosf(time);
            s_light.color    = Rgb(st, 1.0f, ct);
            commands.uploadConstantBuffer(m_lightConstantBuffer, &s_light, sizeof(s_light));
        }
        
        // Update pass parameters
        {
            commands.uploadConstantBuffer(m_passConstantBuffer, &s_pass, sizeof(s_pass));
        }
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);

        // Render the platform
        renderObject(states, commands, m_platform, Matrix4());
        
        const Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
        const Vec3 scale    = Vec3(0.9f, 0.9f, 0.9f);
        
        // Finally render the stanford bunny
        renderObject(states, commands, m_bunny, Matrix4::rotateXY(0.0f, time * 0.5f) * Matrix4::translation(position) * Matrix4::scale(scale));

        m_renderingContext->display(m_renderFrame);
    }
    
    void initializeObjectFromMesh(const String& fileName, Object& object)
    {
        object.mesh = MeshLoader::objFromFile(fileName);
        
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
    
    void renderObject(StateStack& states, CommandBuffer& commands, const Object& object, const Matrix4& transform)
    {
        StateScope instanceState = states.push(&object.states);
        s_instance.transform = transform;
        s_instance.normal = transform.inversed().transposed();
        commands.uploadConstantBuffer(m_instanceConstantBuffer, m_renderFrame.internBuffer(&s_instance, sizeof(s_instance)), sizeof(s_instance));
        commands.drawPrimitives(0, object.mesh.primitives, 0, object.mesh.vertices.size(), states);
    }
};

dcDeclareApplication(new PointLights)
