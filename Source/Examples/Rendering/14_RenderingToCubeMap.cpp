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

static String s_fragmentBackground =
    "cbuffer Projection projection : 0                          \n"

    "uniform samplerCube Texture0;                              \n"

    "void main()                                                \n"
    "{                                                          \n"
    "   vec2 vp  = projection.viewport.zw;                      \n"
    "   vec3 ray = vec3(gl_FragCoord.xy / vp * 2.0 - 1.0, gl_FragCoord.z); \n"
    "   gl_FragColor = textureCube(Texture0, ray);              \n"
    "}                                                          \n"
    ;

static String s_vertexMesh =
    "cbuffer Projection projection : 0;                     \n"
    "cbuffer Camera     camera     : 1;                     \n"
    "cbuffer Instance   instance   : 2;                     \n"

    "varying vec3 v_normal;                                 \n"
    "varying vec3 v_camera;                                 \n"
    "varying vec3 v_vertex;                                 \n"

    "void main()                                            \n"
    "{                                                      \n"
    "   vec4 n   = vec4(gl_Normal, 1.0);                    \n"
    "   v_normal = (instance.inverseTranspose * n).xyz;     \n"
    "   v_vertex = (instance.transform * gl_Vertex).xyz;    \n"
    "   v_camera = camera.position;                         \n"
    "   mat4 mvp = projection.transform                     \n"
    "            * camera.transform                         \n"
    "            * instance.transform                       \n"
    "            ;                                          \n"
    "   gl_Position = mvp * gl_Vertex;                      \n"
    "}                                                      \n"
    ;

static String s_fragmentMesh =
    "uniform samplerCube Texture0;                          \n"
    "uniform samplerCube Texture1;                          \n"

    "varying vec3 v_normal;                                 \n"
    "varying vec3 v_camera;                                 \n"
    "varying vec3 v_vertex;                                 \n"

    "void main()                                            \n"
    "{                                                      \n"
    "   vec3  n = normalize(v_normal);                      \n"
    "   vec3  c = normalize(v_vertex - v_camera);           \n"
    "   vec3  r = reflect(c, n);                            \n"
    "   float f = f_schlick(dot(-c, n), 0.2, 5.0);          \n"

    "   vec4 diffuse    = textureCube(Texture0, n);         \n"
    "   vec4 reflection = textureCube(Texture1, r);         \n"

    "   gl_FragColor = /*mix(diffuse, reflection, f)*/diffuse;         \n"
    "}                                                      \n"
    ;

static String s_fragmentDownsample =
    "uniform samplerCube Texture0;                          \n"
    "cbuffer Convolution convolution   : 0;                 \n"

    "vec3 calculateCubeCoordinates(vec4 fragCoord, vec2 vp, float face)     \n"
    "{                                                                      \n"
    "   vec3 ray = vec3(fragCoord.xy / vp * 2.0 - 1.0, 0.0);                \n"
    "        if(face == 1.0) ray = normalize(vec3(  1, -ray.y,-ray.x));     \n"
    "   else if(face == 0.0) ray = normalize(vec3( -1, -ray.y, ray.x));     \n"
    "   else if(face == 2.0) ray = normalize(vec3(-ray.x,  1,  -ray.y));    \n"
    "   else if(face == 3.0) ray = normalize(vec3(-ray.x, -1,   ray.y));    \n"
    "   else if(face == 5.0) ray = normalize(vec3(ray.x, -ray.y,  1));      \n"
    "   else if(face == 4.0) ray = normalize(vec3(-ray.x, -ray.y, -1));     \n"
    "   return ray;                                                         \n"
    "}                                                                      \n"

    "vec3 hemisphereDirection(vec3 t, vec3 b, vec3 n, float theta, float phi)           \n"
    "{                                                                                  \n"
    "   vec4 angles = vec4(cos(theta), sin(theta), cos(phi), sin(phi));                 \n"
    "   vec3 dir    = t * angles.x * angles.w + b * angles.y * angles.w + n * angles.z; \n"
    "   return normalize(dir);                                                          \n"
    "}                                                                                  \n"

    "#define M_PI 3.1415926535897932384626433832795\n"

    "void main()                                                                        \n"
    "{                                                                                              \n"
    "   vec3 n = calculateCubeCoordinates(gl_FragCoord, convolution.viewport, convolution.face);    \n"
    "   vec3 t = vec3(0.0, 1.0, 0.0);                                                               \n"
    "   vec3 b = cross(n, t);                                                                       \n"
    "   vec4 final = vec4(0.0);                                                                     \n"

    "   for (int i = 0; i < convolution.count; i++)                                         \n"
    "   {                                                                                   \n"
    "       vec2 angle = convolution.rays[i];                                               \n"
    "       vec3 dir = hemisphereDirection(t, b, n, angle.x, angle.y);                      \n"
    "       final += textureCube(Texture0, dir) * cos(angle.y) * sin(angle.y);              \n"
    "   }                                                                                   \n"

    "   gl_FragColor = M_PI * final / float(convolution.count) * convolution.multiplier;    \n"
    "}                                                                                      \n"
    ;

struct Convolution
{
    enum { MaxRays = 32*16, Iterations = 1 };
    Vec3    direction;
    Vec2    viewport;
    Vec2    rays[MaxRays];
    int     count;
    float   face;
    float   multiplier;
    static const UniformElement Layout[];
} s_convolution;

const UniformElement Convolution::Layout[] =
{
      { "direction",  UniformElement::Vec3,    offsetof(Convolution, direction) }
    , { "viewport",   UniformElement::Vec2,    offsetof(Convolution, viewport) }
    , { "face",       UniformElement::Float,   offsetof(Convolution, face) }
    , { "rays",       UniformElement::Vec2,    offsetof(Convolution, rays), Convolution::MaxRays }
    , { "count",      UniformElement::Integer, offsetof(Convolution, count) }
    , { "multiplier", UniformElement::Float,   offsetof(Convolution, multiplier) }
    , { NULL }
};

class RenderingToTexture : public RenderingApplicationDelegate
{
    StateBlock m_renderStates;
    RenderFrame m_renderFrame;
    ConstantBuffer_ m_cameraCBuffer;
    ConstantBuffer_ m_projectionCBuffer;
    ConstantBuffer_ m_instanceCBuffer;
    ConstantBuffer_ m_convolutionCBuffer;
    Examples::MeshStateBlock m_mesh;
    StateBlock m_fullscreenQuad;
    Texture_ m_envmap;
    Program m_downsampleProgram;
    Program m_meshProgram;
    Program m_backgroundProgram;
    
    Examples::Camera s_camera;
    Examples::Projection s_projection;
    Examples::Instance s_instance;
    
    Vec2 m_rays[Convolution::Iterations][Convolution::MaxRays];
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800 / 4, 600 / 4))
        {
            application->quit(-1);
        }

        // Load mesh from a file
        m_mesh = Examples::createMeshRenderingStates(m_renderingContext, m_renderFrame, "Assets/Meshes/bunny.obj");
        
        //m_envmap = Examples::createEnvTexture(m_renderingContext, m_renderFrame, "Assets/Textures/coast2.dds");
        
        m_envmap = Examples::createEnvFromFiles(m_renderingContext, m_renderFrame, "Assets/Textures/Environments/Arches_E_PineTree_128");
        
        // Projection cbuffer
        {
            s_projection = Examples::Projection::perspective(60.0f, m_window->width(), m_window->height(), 0.1f, 100.0f);
            
            UniformLayout layout = m_renderingContext->requestUniformLayout("Projection", Examples::Projection::Layout);
            m_projectionCBuffer = m_renderingContext->requestConstantBuffer(&s_projection, sizeof(s_projection), layout);
        }
        
        // Camera cbuffer
        {
            s_camera = Examples::Camera::lookAt(Vec3(0.0f, 2.0f, -2.0f), Vec3(0.0f, 0.5f, 0.0f));
            
            UniformLayout layout = m_renderingContext->requestUniformLayout("Camera", Examples::Camera::Layout);
            m_cameraCBuffer = m_renderingContext->requestConstantBuffer(&s_camera, sizeof(s_camera), layout);
        }
        
        // Camera cbuffer
        {
            UniformLayout layout = m_renderingContext->requestUniformLayout("Instance", Examples::Instance::Layout);
            m_instanceCBuffer = m_renderingContext->requestConstantBuffer(&s_instance, sizeof(s_instance), layout);
        }
        
        // Convolution cbuffer
        {
            UniformLayout layout = m_renderingContext->requestUniformLayout("Convolution", Convolution::Layout);
            m_convolutionCBuffer = m_renderingContext->requestConstantBuffer(&s_convolution, sizeof(s_convolution), layout);
        }
        
        m_fullscreenQuad = Examples::createFullscreenRenderingStates(m_renderingContext);
        
        // Create a program that consists from a vertex and fragment shaders.
        m_renderStates.disableBlending();
        m_renderStates.setDepthState(LessEqual, true);
        m_renderStates.bindConstantBuffer(m_projectionCBuffer, 0);
        
        m_meshProgram = m_renderingContext->requestProgram(s_vertexMesh, s_fragmentMesh);
        
        for (s32 i = 0; i < Convolution::Iterations; i++)
        {
            for (s32 j = 0; j < Convolution::MaxRays; j++)
            {
                m_rays[i][j] = Vec2(rand0to1() * 6.283f, rand0to1() * 1.57);
            }
        }
        
        m_downsampleProgram = m_renderingContext->requestProgram(Examples::VertexIdentity, s_fragmentDownsample);
        m_backgroundProgram = m_renderingContext->requestProgram(Examples::VertexIdentity, s_fragmentBackground);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        m_renderFrame.clear();
        
        StateStack&    stateStack = m_renderFrame.stateStack();
        CommandBuffer& commands  = m_renderFrame.entryPoint();
        
        StateScope defaults = stateStack.push(&m_renderStates);
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        const s32 envTextureSize = 128;
        
        // Allocate a transient cube map texture
        TransientTexture env = commands.acquireTextureCube(envTextureSize, PixelRgba32F);
        
        // Now render to a cube map
        for (s32 j = 0; j < Convolution::Iterations; j++)
        {
            StateScope downsamplePass = stateStack.newScope();
            downsamplePass->bindProgram(m_downsampleProgram);
            downsamplePass->bindTexture(m_envmap, 0);
            downsamplePass->bindConstantBuffer(m_convolutionCBuffer, 0);
            downsamplePass->setBlend(BlendOne, BlendOne);
            
            StateScope quadStates = stateStack.push(&m_fullscreenQuad);
            
            for (s32 i = 0; i < 6; i++)
            {
                s_convolution.viewport   = Vec2(envTextureSize, envTextureSize);
                s_convolution.face       = i;
                s_convolution.count      = Convolution::MaxRays;
                s_convolution.multiplier = 1.0f / Convolution::Iterations;
                
                for (s32 r = 0; r < Convolution::MaxRays; r++)
                {
                    s_convolution.rays[r] = m_rays[j][r];
                }
                
                CommandBuffer& renderToCubeMap = commands.renderToCubeMap(m_renderFrame, env, i);
                if (j == 0)
                {
                    renderToCubeMap.clear(Rgba(0.0f, 0.0f, 0.0f, 0.0f), ClearAll);
                }
                renderToCubeMap.uploadConstantBuffer(m_convolutionCBuffer, m_renderFrame.internBuffer(&s_convolution, sizeof(s_convolution)), sizeof(s_convolution));
                renderToCubeMap.drawPrimitives(0, PrimQuads, 0, 4, stateStack);
            }
        }

        {
            s_projection.viewport = Vec4(0, 0, (f32)m_window->width(), (f32)m_window->height());
            StateScope backgroundPass = stateStack.push(&m_fullscreenQuad);
            StateScope backgroundProgram = stateStack.newScope();
            backgroundProgram->bindProgram(m_backgroundProgram);
            backgroundProgram->bindTexture(m_envmap, 0);
            commands.uploadConstantBuffer(m_projectionCBuffer, &s_projection, sizeof(s_projection));
            commands.drawPrimitives(1, PrimQuads, 0, 4, stateStack);
        }
        
        StateScope meshPass = stateStack.newScope();
        meshPass->bindConstantBuffer(m_cameraCBuffer, 1);
        meshPass->bindConstantBuffer(m_instanceCBuffer, 2);
        meshPass->bindProgram(m_meshProgram);
        meshPass->bindTexture(env, 0);
        meshPass->bindTexture(m_envmap, 1);
        
        StateScope meshStates = stateStack.push(&m_mesh.states);
        s_instance = Examples::Instance::fromTransform(Matrix4::rotateXY(0.0f, currentTime() * 0.0003f) * Matrix4::rotateXY(currentTime() * 0.0003f, 0.0));
        commands.uploadConstantBuffer(m_instanceCBuffer, &s_instance, sizeof(s_instance));
        commands.drawPrimitives(0, m_mesh.primitives, 0, m_mesh.size, stateStack);
        
        commands.releaseTexture(env);
        
        m_renderingContext->display(m_renderFrame);
    }
};

dcDeclareApplication(new RenderingToTexture)
