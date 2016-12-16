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
    "struct View { mat4 projection; mat4 view; };   \n"
    "struct Instance { mat4 transform; };           \n"
    "uniform View view;                             \n"
    "uniform Instance instance;                     \n"
    "varying vec4 v_color;                          \n"
    "                                               \n"
    "void main()                                    \n"
    "{                                              \n"
    "    v_color     = gl_Color;                    \n"
    "    gl_Position = view.projection * view.view * instance.transform * gl_Vertex;                   \n"
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

struct View
{
    Matrix4 projection;
    Matrix4 view;
    static ConstantBufferLayout s_layout[];
} s_view;

ConstantBufferLayout View::s_layout[] =
{
      { "view.projection", ConstantBufferLayout::Matrix4, offsetof(View, projection) }
    , { "view.view",       ConstantBufferLayout::Matrix4, offsetof(View, view)       }
    , { NULL }
};

struct Instance
{
    Matrix4 transform;
    static ConstantBufferLayout s_layout[];
} s_instance;

ConstantBufferLayout Instance::s_layout[] =
{
      { "instance.transform", ConstantBufferLayout::Matrix4, offsetof(Instance, transform) }
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
        
        s_view.projection = Matrix4::perspective(60.0f, m_window->aspectRatio(), 0.1f, 100.0f);
        s_view.view       = Matrix4::lookAt(Vec3(0.0f, 0.0f, -35.0f), Vec3::zero(), Vec3(0.0f, 1.0f, 0.0f));

        InputLayout inputLayout = m_renderingContext->requestInputLayout(VertexFormat::Position | VertexFormat::Color);
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(s_vertices, sizeof(s_vertices));
        IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(s_indices, sizeof(s_indices));
        ConstantBuffer_ viewConstantBuffer = m_renderingContext->requestConstantBuffer(&s_view, sizeof(s_view), View::s_layout);
        m_instanceConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(s_instance), Instance::s_layout);
        
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);

        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindIndexBuffer(indexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
        m_renderStates.bindProgram(program);
        m_renderStates.bindConstantBuffer(viewConstantBuffer, 0);
    }
    
    inline void mtxRotateXY(float* _result, float _ax, float _ay)
    {
        const float sx = sinf(_ax);
        const float cx = cosf(_ax);
        const float sy = sinf(_ay);
        const float cy = cosf(_ay);
        
        memset(_result, 0, sizeof(float)*16);
        _result[ 0] = cy;
        _result[ 2] = sy;
        _result[ 4] = sx*sy;
        _result[ 5] = cx;
        _result[ 6] = -sx*cy;
        _result[ 8] = -cx*sy;
        _result[ 9] = sx;
        _result[10] = cx*cy;
        _result[15] = 1.0f;
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
                mtxRotateXY(s_instance.transform.m, time + x*0.21f, time + y*0.37f);
                s_instance.transform[12] = -15.0f + float(x)*3.0f;
                s_instance.transform[13] = -15.0f + float(y)*3.0f;
                s_instance.transform[14] = 0.0f;
                
                //s_instance.transform = Matrix4::translation(x, y, 0.0f);
                
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
