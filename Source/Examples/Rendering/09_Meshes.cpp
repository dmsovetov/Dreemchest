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
    "cbuffer Pass     pass     : 0;                     \n"
    "cbuffer Instance instance : 1;                     \n"
    "                                                   \n"
    "varying vec3 v_color;                              \n"
    "                                                   \n"
    "void main()                                        \n"
    "{                                                  \n"
    "   v_color     = gl_Normal * 0.5 + 0.5;            \n"
    "   gl_Position = pass.projection * pass.view * instance.transform * gl_Vertex; \n"
    "}                                                  \n"
;

static String s_fragmentShader =
    "varying vec3 v_color;                              \n"
    "                                                   \n"
    "void main()                                        \n"
    "{                                                  \n"
    "   gl_FragColor = vec4(v_color, 1.0);              \n"
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
    static UniformElement s_layout[];
} s_instance;

UniformElement Instance::s_layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Instance, transform) }
    , { NULL }
};

class Textures : public RenderingApplicationDelegate
{
    StateBlock m_renderStates;
    RenderFrame m_renderFrame;
    MeshLoader::Descriptor mesh;
    ConstantBuffer_ m_instanceConstantBuffer;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800 / 4, 600 / 4))
        {
            application->quit(-1);
        }
        
        // First load a mesh from a file
        mesh = MeshLoader::objFromFile("Assets/Meshes/column.obj");
        
        if (!mesh)
        {
            application->quit(-1);
        }
        
        // Now configure a mesh rendering states
        InputLayout inputLayout = m_renderingContext->requestInputLayout(mesh.format);
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(&mesh.vertices[0], sizeof(MeshLoader::Vertex) * mesh.vertices.size());
        
        m_renderStates.bindInputLayout(inputLayout);
        m_renderStates.bindVertexBuffer(vertexBuffer);

        if (mesh.indices.size())
        {
            IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(&mesh.indices[0], sizeof(u16) * mesh.indices.size());
            m_renderStates.bindIndexBuffer(indexBuffer);
        }
        
        // Configure pass constant buffer
        {
            s_pass.projection = Matrix4::perspective(60.0f, m_window->aspectRatio(), 0.1f, 100.0f);
            s_pass.view       = Matrix4::lookAt(Vec3(0.0f, 5.0f, -5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Pass", Pass::s_layout);
            ConstantBuffer_ constantBuffer = m_renderingContext->requestConstantBuffer(&s_pass, sizeof(s_pass), uniformLayout);
            m_renderStates.bindConstantBuffer(constantBuffer, 0);
        }
        
        // Configure instance constant buffer
        {
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Instance", Instance::s_layout);
            m_instanceConstantBuffer = m_renderingContext->requestConstantBuffer(&s_instance, sizeof(s_instance), uniformLayout);
            m_renderStates.bindConstantBuffer(m_instanceConstantBuffer, 1);
        }
        
        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        m_renderFrame.clear();
        
        CommandBuffer& commands = m_renderFrame.entryPoint();
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Update an instance constant buffer
        s_instance.transform = Matrix4::rotateXY(0.0f, currentTime() * 0.001f);
        commands.uploadConstantBuffer(m_instanceConstantBuffer, &s_instance, sizeof(s_instance));
        
        // Render the mesh
        commands.drawPrimitives(0, Renderer::PrimTriangles, 0, mesh.vertices.size(), &m_renderStates);
    
        m_renderingContext->display(m_renderFrame);
    }
};

dcDeclareApplication(new Textures)
