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

// A colored vertex data structure.
struct PosColorVertex
{
    f32 position[3];
    u32 color;
};

static PosColorVertex s_vertices[] =
{
    { { -1.0f,  1.0f,  1.0f }, 0xff000000 },
    { {  1.0f,  1.0f,  1.0f }, 0xff0000ff },
    { { -1.0f, -1.0f,  1.0f }, 0xff00ff00 },
    { {  1.0f, -1.0f,  1.0f }, 0xff00ffff },
    { { -1.0f,  1.0f, -1.0f }, 0xffff0000 },
    { {  1.0f,  1.0f, -1.0f }, 0xffff00ff },
    { { -1.0f, -1.0f, -1.0f }, 0xffffff00 },
    { {  1.0f, -1.0f, -1.0f }, 0xffffffff },
};

static const u16 s_indices[] =
{
    0, 1, 2,
    1, 3, 2,
    4, 6, 5,
    5, 6, 7,
    0, 2, 4,
    4, 2, 6,
    1, 5, 3,
    5, 7, 3,
    0, 4, 1,
    4, 5, 1,
    2, 3, 6,
    6, 3, 7,
};

static String s_vertexShader =
    "cbuffer Pass pass : 0;                         \n"
    "cbuffer Instance instance : 1;                 \n"
    "varying vec4 v_color;                          \n"
    "                                               \n"
    "void main()                                    \n"
    "{                                              \n"
    "    v_color     = gl_Color;                    \n"
    "    gl_Position = pass.projection * pass.view * instance.transform * gl_Vertex;    \n"
    "}                                              \n"
    ;

static String s_fragmentShader =
    "varying vec4 v_color;                          \n"
    "                                               \n"
    "void main()                                    \n"
    "{                                              \n"
    "    gl_FragColor = v_color;                    \n"
    "}                                              \n"
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
    static UniformElement s_layout[];
} s_instance;

UniformElement Instance::s_layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Instance, transform) }
    , { NULL }
};

class Cubes : public RenderingApplicationDelegate
{
    StateBlock m_renderStates;
    RenderFrame m_renderFrame;
    ConstantBuffer_ m_instanceConstantBuffer;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        s_pass.projection = Matrix4::perspective(60.0f, m_window->aspectRatio(), 0.1f, 100.0f);
        s_pass.view       = Matrix4::lookAt(Vec3(0.0f, 0.0f, -35.0f), Vec3::zero(), Vec3(0.0f, 1.0f, 0.0f));

        InputLayout inputLayout = m_renderingContext->requestInputLayout(VertexFormat::Position | VertexFormat::Color);
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(s_vertices, sizeof(s_vertices));
        IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(s_indices, sizeof(s_indices));
        UniformLayout viewUniformLayout = m_renderingContext->requestUniformLayout("Pass", Pass::s_layout);
        ConstantBuffer_ viewConstantBuffer = m_renderingContext->requestConstantBuffer(&s_pass, sizeof(s_pass), viewUniformLayout);
        UniformLayout instanceUniformLayout = m_renderingContext->requestUniformLayout("Instance", Instance::s_layout);
        m_instanceConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(s_instance), instanceUniformLayout);
        
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);

        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindIndexBuffer(indexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
        m_renderStates.bindProgram(program);
        m_renderStates.bindConstantBuffer(viewConstantBuffer, 0);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        m_renderFrame.clear();
        
        CommandBuffer& commands = m_renderFrame.entryPoint();
        StateStack& stateStack = m_renderFrame.stateStack();
        
        StateScope global = stateStack.push(&m_renderStates);

        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        f32 time = currentTime() * 0.001f;
        
        for (s32 y = 0; y < 11; ++y)
        {
            for (s32 x = 0; x < 11; ++x)
            {
                s_instance.transform = Matrix4::translation(x * 3.0f - 15.0f, y * 3.0f - 15.0f, 0.0f) *
                                       Matrix4::rotateXY(time + x*0.21f, time + y*0.37f);

                StateScope instance = stateStack.newScope();
                instance->bindConstantBuffer(m_instanceConstantBuffer, 1);
                commands.uploadConstantBuffer(m_instanceConstantBuffer, m_renderFrame.internBuffer(&s_instance, sizeof(s_instance)), sizeof(s_instance));
                commands.drawIndexed(0, Renderer::PrimTriangles, 0, sizeof(s_indices) / sizeof(u16), stateStack);
            }
        }
        
        m_renderingContext->display(m_renderFrame);
    }
};

dcDeclareApplication(new Cubes)
