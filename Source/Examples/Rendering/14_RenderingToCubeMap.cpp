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

// Now let's define a vertex ubershader with F_Transform, F_TexCoord and
// F_NormalAsColor feature options.
static String s_vertexShader =
    "cbuffer Transform transform : 0;                           \n"
    "                                                           \n"
    "varying vec3 v_normal;                                     \n"
    "                                                           \n"
    "void main()                                                \n"
    "{                                                          \n"
    "   mat4 vp = transform.projection * transform.view;        \n"
    "   gl_Position = vp * transform.instance * gl_Vertex;      \n"
    "   v_normal = gl_Normal;                                   \n"
    "}                                                          \n"
    ;

// And the same for a fragment shader
static String s_fragmentShader =
    "uniform samplerCube Texture0;                              \n"
    "                                                           \n"
    "varying vec3 v_normal;                                     \n"
    "                                                           \n"
    "void main()                                                \n"
    "{                                                          \n"
    "   gl_FragColor = textureCube(Texture0, v_normal);         \n"
    "}                                                          \n"
    ;

struct Transform
{
    Matrix4 projection;
    Matrix4 view;
    Matrix4 instance;
    static UniformElement s_layout[];
} s_transform;

UniformElement Transform::s_layout[] =
{
      { "projection", UniformElement::Matrix4, offsetof(Transform, projection) }
    , { "view",       UniformElement::Matrix4, offsetof(Transform, view)       }
    , { "instance",   UniformElement::Matrix4, offsetof(Transform, instance)   }
    , { NULL }
};

class RenderingToTexture : public RenderingApplicationDelegate
{
    StateBlock m_renderStates;
    RenderFrame m_renderFrame;
    MeshLoader::Descriptor m_mesh;
    StateBlock m_meshStates;
    ConstantBuffer_ m_transformCBuffer;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800/4, 600/4))
        {
            application->quit(-1);
        }

        // Load mesh from a file
        m_mesh = MeshLoader::objFromFile("Assets/Meshes/bunny_decimated.obj");
        NIMBLE_ABORT_IF(!m_mesh, "failed to load mesh");
        
        {
            s_transform.projection = Matrix4::perspective(60.0f, m_window->aspectRatio(), 0.1f, 100.0f);
            s_transform.view       = Matrix4::lookAt(Vec3(0.0f, 2.0f, -2.0f), Vec3(0.0f, 0.5f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
            
            VertexFormat vertexFormat = m_mesh.vertexFormat;
            VertexBuffer_ vb = m_renderingContext->requestVertexBuffer(&m_mesh.vertices[0], m_mesh.vertices.size());
            InputLayout il = m_renderingContext->requestInputLayout(vertexFormat);
            UniformLayout layout = m_renderingContext->requestUniformLayout("Transform", Transform::s_layout);
            m_transformCBuffer = m_renderingContext->requestConstantBuffer(&s_transform, sizeof(s_transform), layout);
            m_meshStates.bindVertexBuffer(vb);
            m_meshStates.bindInputLayout(il);
            m_meshStates.bindConstantBuffer(m_transformCBuffer, 0);
        }
        
        // Create a program that consists from a vertex and fragment shaders.
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        m_renderFrame.clear();
        
        StateStack&    stateStack = m_renderFrame.stateStack();
        CommandBuffer& commands  = m_renderFrame.entryPoint();
        
        StateScope defaults = stateStack.push(&m_renderStates);
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Allocate a transient cube map texture
        TransientTexture env = commands.acquireTextureCube(256, PixelRgb8);
        
        // Now render to a cube map
        {
            Rgba colors[] =
            {
                  Rgba(1.0f, 0.0f, 0.0f)
                , Rgba(0.0f, 1.0f, 0.0f)
                , Rgba(0.0f, 0.0f, 1.0f)
                , Rgba(0.0f, 1.0f, 1.0f)
                , Rgba(1.0f, 0.0f, 1.0f)
                , Rgba(1.0f, 1.0f, 0.0f)
            };
            
            for (s32 i = 0; i < 6; i++)
            {
                CommandBuffer& renderToCubeMap = commands.renderToCubeMap(m_renderFrame, env, i);
                renderToCubeMap.clear(colors[i], ClearAll);
            }
        }

        StateScope instanceStates = stateStack.push(&m_meshStates);
        s_transform.instance = Matrix4::rotateXY(0.0f, currentTime() * 0.001f);
        StateScope envStates = stateStack.newScope();
        envStates->bindTexture(env, 0);
        commands.uploadConstantBuffer(m_transformCBuffer, &s_transform, sizeof(s_transform));
        commands.drawPrimitives(0, m_mesh.primitives, 0, m_mesh.vertices.size(), stateStack);
        
        commands.releaseTexture(env);

        m_renderingContext->display(m_renderFrame);
    }
};

dcDeclareApplication(new RenderingToTexture)
