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
#include "Examples.h"

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

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

struct Viewport
{
    f32 width;
    f32 height;
    static UniformElement s_layout[];
} s_viewport;

UniformElement Viewport::s_layout[] =
{
      { "width",  UniformElement::Float, offsetof(Viewport, width)  }
    , { "height", UniformElement::Float, offsetof(Viewport, height) }
    , { NULL }
};

static Vec3 s_vertices[] =
{
      Vec3(-1.0f, -1.0f, 0.0f)
    , Vec3( 1.0f, -1.0f, 0.0f)
    , Vec3( 1.0f,  1.0f, 0.0f)
    , Vec3(-1.0f,  1.0f, 0.0f)
};

static String s_vertexDownsample =
    "void main()                      \n"
    "{                                \n"
    "   gl_Position = gl_Vertex;      \n"
    "}                                \n"
    ;

static String s_fragmentDownsample =
    "cbuffer Viewport viewport : 0;         \n"

    "uniform samplerCube Texture0;          \n"

    "void main()                                                                        \n"
    "{                                                                                  \n"
    "   vec2 vp        = vec2(viewport.width, viewport.height);                         \n"
    "   vec3 ray       = vec3(gl_FragCoord.xy / vp * 2.0 - 1.0, -gl_FragCoord.z * 2.0); \n"
    "   vec3 direction = normalize(ray);                                                \n"
    "   gl_FragColor   = textureCube(Texture0, direction);                              \n"
    "}\n"
    ;

class RenderingToTexture : public RenderingApplicationDelegate
{
    StateBlock m_renderStates;
    StateBlock  m_fullscreenQuadStates;
    RenderFrame m_renderFrame;
    Examples::Mesh m_mesh;
    StateBlock m_meshStates;
    ConstantBuffer_ m_transformCBuffer;
    ConstantBuffer_ m_viewportCBuffer;
    Texture_ m_envmap;
    Program m_downsampleProgram;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }

        // Load mesh from a file
        m_mesh = Examples::objFromFile("Assets/Meshes/bunny_decimated.obj");
        NIMBLE_ABORT_IF(!m_mesh, "failed to load mesh");
        
        static Examples::CubeMap cubeMap = Examples::cubeFromDds("Assets/Textures/coast2.dds");
        m_envmap = m_renderingContext->requestTextureCube(&cubeMap.pixels[0], cubeMap.size, cubeMap.mipLevels, cubeMap.format);
        
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
        
        {
            UniformLayout layout = m_renderingContext->requestUniformLayout("Viewport", Viewport::s_layout);
            m_viewportCBuffer = m_renderingContext->requestConstantBuffer(&s_viewport, sizeof(s_viewport), layout);
        }
        
        // Cobfigure quad rendering state block
        {
            InputLayout   il = m_renderingContext->requestInputLayout(0);
            VertexBuffer_ vb = m_renderingContext->requestVertexBuffer(s_vertices, sizeof(s_vertices));
            m_fullscreenQuadStates.bindVertexBuffer(vb);
            m_fullscreenQuadStates.bindInputLayout(il);
            m_fullscreenQuadStates.setDepthState(LessEqual, false);
        }
        
        // Create a program that consists from a vertex and fragment shaders.
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
        
        m_downsampleProgram = m_renderingContext->requestProgram(s_vertexDownsample, s_fragmentDownsample);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        m_renderFrame.clear();
        
        StateStack&    stateStack = m_renderFrame.stateStack();
        CommandBuffer& commands  = m_renderFrame.entryPoint();
        
        StateScope defaults = stateStack.push(&m_renderStates);
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Allocate a transient cube map texture
        const s32 envTextureSize = 128;
        TransientTexture env = commands.acquireTextureCube(envTextureSize, PixelRgb8);
        
        // Now render to a cube map
        {
            static Rgba s_colors[] =
            {
                  Rgba(1.0f, 0.0f, 0.0f)
                , Rgba(0.0f, 1.0f, 0.0f)
                , Rgba(0.0f, 0.0f, 1.0f)
                , Rgba(1.0f, 1.0f, 0.0f)
                , Rgba(0.0f, 1.0f, 1.0f)
                , Rgba(1.0f, 0.0f, 1.0f)
            };
            
            StateScope fullscreenQuadScope = stateStack.push(&m_fullscreenQuadStates);
            
            StateScope downsampleScope = stateStack.newScope();
            downsampleScope->bindTexture(m_envmap, 0);
            downsampleScope->bindProgram(m_downsampleProgram);
            downsampleScope->bindConstantBuffer(m_viewportCBuffer, 0);
            
            s_viewport.width = envTextureSize;
            s_viewport.height = envTextureSize;
            commands.uploadConstantBuffer(m_viewportCBuffer, &s_viewport, sizeof(s_viewport));
            
            for (s32 i = 0; i < 6; i++)
            {
                CommandBuffer& renderToCubeMap = commands.renderToCubeMap(m_renderFrame, env, i);
                renderToCubeMap.clear(s_colors[i], ClearAll);
                //renderToCubeMap.drawPrimitives(0, PrimQuads, 0, 4, stateStack);
            }
        }
        
        // Render quad
        {
            StateScope quadScope = stateStack.push(&m_fullscreenQuadStates);
            StateScope scope = stateStack.newScope();
            scope->bindTexture(env, 0);
            scope->bindProgram(m_downsampleProgram);
            scope->bindConstantBuffer(m_transformCBuffer, 0);
            
            s_viewport.width = m_window->width();
            s_viewport.height = m_window->height();
            commands.uploadConstantBuffer(m_viewportCBuffer, &s_viewport, sizeof(s_viewport));
            commands.drawPrimitives(0, PrimQuads, 0, 4, stateStack);
        }

        commands.releaseTexture(env);

        m_renderingContext->display(m_renderFrame);
    }
};

dcDeclareApplication(new RenderingToTexture)
