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

// A textured vertex data structure.
struct TexturedVertex
{
    f32 position[3];
    f32 uv[2];
};

static TexturedVertex s_vertices[] =
{
    { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
    { {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
    { {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
    { { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } },
};

static u16 s_indices[] =
{
    0, 1, 2,
    0, 2, 3,
};

// First define user options to control shader program behaviour
enum UserOptions
{
    F_NormalAsColor = BIT(0)
};

// Now declare the pipeline feature layout that maps from pipeline state to shader options.
static PipelineFeature s_features[] =
{
    // This item tells a rendering context that 'F_Transform' option
    // should be passed to a shader program each time some contant buffer
    // is bound to a unit with index 0
      { "F_Transform",     PipelineFeature::constantBuffer(0) }
    
    // 'F_TexCoord' is passed to a shader if an input layout has a texture
    // coordinate #0 stream.
    , { "F_TexCoord",      PipelineFeature::vertexAttribute(VertexTexCoord0) }
    
    // 'F_NormalAsColor' option should be passed to a shader program each time
    // the 'F_NormalAsColor' bit flag was set by user.
    , { "F_NormalAsColor", PipelineFeature::user(F_NormalAsColor) }
    
      // A sentinel item
    , { NULL }
};


// Now let's define a vertex ubershader with F_Transform, F_TexCoord and
// F_NormalAsColor feature options.
static String s_vertexShader =
    "cbuffer Transform transform : 0;                           \n"
    "                                                           \n"
    "varying vec2 v_texCoord;                                   \n"
    "varying vec3 v_color;                                      \n"
    "                                                           \n"
    "void main()                                                \n"
    "{                                                          \n"
    "#if defined(F_Transform)                                   \n" // When F_Transform option is set
    "   mat4 vp = transform.projection * transform.view;        \n" // we should transform an input vertex
    "   gl_Position = vp * transform.instance * gl_Vertex;      \n" // with a model view projection matrix,
    "#else                                                      \n" // otherwise we just pass an input to
    "   gl_Position = gl_Vertex;                                \n" // an output.
    "#endif // F_ViewProjection                                 \n"
    "                                                           \n"
    "#if defined(F_TexCoord)                                    \n" // Pass texture coordinates to a fragment
    "   v_texCoord  = gl_MultiTexCoord0.xy;                     \n" // shader if F_TexCoord is set.
    "#endif // F_TexCoord                                       \n"
    "                                                           \n"
    "#if defined(F_NormalAsColor)                               \n" // Pack the vertex normal to a [0, 1]
    "   v_color = gl_Normal * 0.5 + 0.5;                        \n" // range an output it as a diffuse color.
    "#endif // F_NormalAsColor                                  \n"
    "}                                                          \n"
    ;

// And the same for a fragment shader
static String s_fragmentShader =
    "uniform sampler2D Texture0;                                \n"
    "                                                           \n"
    "varying vec2 v_texCoord;                                   \n"
    "varying vec3 v_color;                                      \n"
    "                                                           \n"
    "void main()                                                \n"
    "{                                                          \n"
    "#if defined(F_TexCoord)                                    \n"
    "   vec3  color = texture2D(Texture0, v_texCoord).rgb;      \n" // When F_TexCoord option is set sample a 2D
    "   gl_FragColor = vec4(1.0 - color, 1.0);                  \n" // texture, then invert it and pass to an output.
    "#elif defined(F_NormalAsColor)                             \n"
    "   gl_FragColor = vec4(v_color, 1.0);                      \n" // Use a color value that was passed from a
    "#else                                                      \n" // vertex shader if F_NormalAsColor is set,
    "   gl_FragColor = vec4(1.0);                               \n" // otherwise use a constant fragment color.
    "#endif // F_TexCoord                                       \n"
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
    StateBlock8 m_renderStates;
    Examples::Mesh m_mesh;
    StateBlock4 m_meshStates;
    ConstantBuffer_ m_transformCBuffer;
    
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
            m_meshStates.enableFeatures(F_NormalAsColor);
        }
        
        // Create a program that consists from a vertex and fragment shaders.
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
        
        // Finally configure mesh rendering states as before
        InputLayout inputLayout = m_renderingContext->requestInputLayout(VertexFormat::Position | VertexFormat::TexCoord0);
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(s_vertices, sizeof(s_vertices));
        IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(s_indices, sizeof(s_indices));
        FeatureLayout features = m_renderingContext->requestPipelineFeatureLayout(s_features);

        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindIndexBuffer(indexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
        m_renderStates.bindFeatureLayout(features);
    }
 
    virtual void handleRenderFrame(f32 dt) NIMBLE_OVERRIDE
    {
        static f32 s_time = 0.0f;
        s_time += dt;
        
        RenderFrame& frame = m_renderingContext->allocateFrame();
        
        StateStack&          stateStack = frame.stateStack();
        RenderCommandBuffer& commands   = frame.entryPoint();
        
        StateScope defaults = stateStack.push(&m_renderStates);
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Acquire a transient render target
        TransientTexture renderTarget = commands.acquireTexture2D(256, 256, PixelRgb8);

        // Now render an object to a texture target
        {
            // Construct a nested command buffer to render to texture
            RenderCommandBuffer& renderToTarget = commands.renderToTexture(renderTarget, TextureD16);

            StateScope instanceStates = stateStack.push(&m_meshStates);
            s_transform.instance = Matrix4::rotateXY(0.0f, s_time);
            renderToTarget.clear(Rgba(0.0f, 1.0f, 0.0f), ClearAll);
            renderToTarget.uploadConstantBuffer(m_transformCBuffer, &s_transform, sizeof(s_transform));
            renderToTarget.drawPrimitives(0, m_mesh.primitives, 0, m_mesh.vertices.size());
        }
        
        // Render a 2D quad with a rendered texture
        {
            StateScope quadStates = stateStack.newScope();
            quadStates->bindTexture(renderTarget, 0);
            commands.drawIndexed(0, Renderer::PrimTriangles, 0, 6);
        }
        
        // Always release transient render targets
        commands.releaseTexture(renderTarget);
    
        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new RenderingToTexture)
